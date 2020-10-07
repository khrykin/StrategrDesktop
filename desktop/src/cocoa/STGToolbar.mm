//
// Created by Dmitry Khrykin on 25.05.2020.
//

#import "STGToolbar.h"
#import "third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h"

#include "mainscene.h"
#include "mainwindow.h"

const NSString *ToolbarItemActivitiesIdentifier = @"Activities";
const NSString *ToolbarItemStrategyTitleIdentifier = @"Title:Strategy";
const NSString *ToolbarItemNewActivityIdentifier = @"New";
const NSString *ToolbarItemActivitiesTitleIdentifier = @"Title:Activities";
const NSString *ToolbarItemBackButtonIdentifier = @"Back";
const NSString *ToolbarItemSettingsIdentifier = @"Settings";
const NSString *TAAdaptiveSpaceItemIdentifier = @"TAAdaptiveSpaceItem";


const NSArray *sessionsPage = @[ToolbarItemActivitiesIdentifier,
                                NSToolbarFlexibleSpaceItemIdentifier,
                                ToolbarItemStrategyTitleIdentifier,
                                TAAdaptiveSpaceItemIdentifier];

const NSArray *activitiesPage = @[ToolbarItemNewActivityIdentifier,
                                  NSToolbarFlexibleSpaceItemIdentifier,
                                  ToolbarItemActivitiesTitleIdentifier,
                                  TAAdaptiveSpaceItemIdentifier,
                                  ToolbarItemBackButtonIdentifier];

const NSArray *toolbarPages = @[sessionsPage, activitiesPage];

@implementation STGToolbar
- (instancetype)initWithIdentifier:(NSToolbarIdentifier)identifier {
    if (self = [super initWithIdentifier:identifier]) {
        self.delegate = self;
    }

    return self;
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
        itemForItemIdentifier:(NSString *)itemIdentifier
    willBeInsertedIntoToolbar:(BOOL)flag {
    NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier];

    if ([itemIdentifier isEqual:TAAdaptiveSpaceItemIdentifier]) {
        return [[TAAdaptiveSpaceItem alloc] initWithItemIdentifier:(NSString *) TAAdaptiveSpaceItemIdentifier];
    }

    if ([itemIdentifier isEqual:ToolbarItemActivitiesTitleIdentifier] ||
        [itemIdentifier isEqual:ToolbarItemStrategyTitleIdentifier]) {
        NSTextField *textField = [self makeTextFieldForItemIdentifier:itemIdentifier];
        item.view = textField;

        return item;
    }

    if ([itemIdentifier isEqual:ToolbarItemBackButtonIdentifier]) {
        NSSegmentedControl *segmentedControl = [self makeBackButton];
        item.view = segmentedControl;

        return item;
    }

    item.view = [self makeButtonForItemIdentifier:itemIdentifier];
    item.target = self;
    item.action = [self getActionSelectorForItemIdentifier:itemIdentifier];

    return item;
}

- (NSSegmentedControl *)makeBackButton {
    NSArray<NSImage *> *images = @[[NSImage imageNamed:NSImageNameGoBackTemplate]];

    NSSegmentedControl *segmentedControl = [NSSegmentedControl segmentedControlWithImages:images
                                                                             trackingMode:NSSegmentSwitchTrackingMomentary
                                                                                   target:self
                                                                                   action:@selector(openSessions:)];

    segmentedControl.segmentStyle = NSSegmentStyleSeparated;
    return segmentedControl;
}

- (NSTextField *)makeTextFieldForItemIdentifier:(NSString *)itemIdentifier {
    NSTextField *textField = [NSTextField labelWithString:@""];
    [textField setFont:[NSFont boldSystemFontOfSize:13]];

    if ([itemIdentifier isEqual:ToolbarItemActivitiesTitleIdentifier]) {
        textField.stringValue = @"Activities";
    } else {
        // qtWindowTitle includes "[*]" at the end.
        // see QWidget::windowModified()

        NSString *qtWindowTitle = qWindow->windowTitle().toNSString();
        NSError *error = nil;
        NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"\\[\\*\\]$"
                                                                               options:NSRegularExpressionCaseInsensitive
                                                                                 error:&error];

        NSString *windowTitle = [regex stringByReplacingMatchesInString:qtWindowTitle
                                                                options:0
                                                                  range:NSMakeRange(0, [qtWindowTitle length])
                                                           withTemplate:@""];

        textField.stringValue = windowTitle;
    }

    textField.editable = NO;
    textField.selectable = NO;
    textField.textColor = [NSColor secondaryLabelColor];

    textField.cell.truncatesLastVisibleLine = YES;
    textField.cell.lineBreakMode = NSLineBreakByTruncatingMiddle;

    [textField setContentCompressionResistancePriority:0.1
                                        forOrientation:NSLayoutConstraintOrientationHorizontal];

    return textField;
}

- (NSButton *)makeButtonForItemIdentifier:(NSString *)itemIdentifier {
    NSButton *btn = [NSButton buttonWithTitle:itemIdentifier
                                       target:self
                                       action:@selector(openActivities:)];
    [btn setBezelStyle:NSBezelStyleTexturedRounded];

    if ([itemIdentifier isEqual:ToolbarItemSettingsIdentifier]) {
        btn.image = [NSImage imageNamed:NSImageNameActionTemplate];
    } else if ([itemIdentifier isEqual:ToolbarItemNewActivityIdentifier]) {
        btn.image = [NSImage imageNamed:NSImageNameAddTemplate];
    } else if ([itemIdentifier isEqual:ToolbarItemActivitiesIdentifier]) {
        btn.image = [NSImage imageNamed:NSImageNameListViewTemplate];
    }

    return btn;
}

- (SEL)getActionSelectorForItemIdentifier:(NSString *)itemIdentifier {
    SEL action;
    if ([itemIdentifier isEqual:ToolbarItemActivitiesIdentifier]) {
        action = @selector(openActivities:);
    } else if ([itemIdentifier isEqual:ToolbarItemSettingsIdentifier]) {
        action = @selector(openSettings:);
    } else if ([itemIdentifier isEqual:ToolbarItemBackButtonIdentifier]) {
        action = @selector(openSessions:);
    } else if ([itemIdentifier isEqual:ToolbarItemNewActivityIdentifier]) {
        action = @selector(openNewActivityMenu:);
    }

    return action;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar {
    return @[];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar {
    return [self toolbarDefaultItemIdentifiers:toolbar];
}

- (NSArray *)toolbarSelectableItemIdentifiers:
    (NSToolbar *)toolbar {
    return [self toolbarDefaultItemIdentifiers:toolbar];
}

- (void)openActivities:(id)sender {
    qWindow->scene()->actionCenter().show_activities();
}

- (void)openSettings:(id)sender {
    qWindow->scene()->showStrategySettings();
}

- (void)openSessions:(id)sender {
    qWindow->scene()->actionCenter().show_sessions();
}

- (void)openNewActivityMenu:(id)sender {
    qWindow->scene()->showNewActivityMenu();
}

- (void)setPage:(unsigned int)index {
    _currentPage = index;

    for (NSToolbarItem *_ in self.items) {
        [self removeItemAtIndex:0];
    }

    for (NSString *itemIdentifier in toolbarPages[index]) {
        [self insertItemWithItemIdentifier:itemIdentifier atIndex:0];
    }

    for (NSToolbarItem *item in self.items) {
        if ([item.itemIdentifier isEqualToString:(NSString *) TAAdaptiveSpaceItemIdentifier]) {
            TAAdaptiveSpaceItem *adaptiveSpaceItem = (TAAdaptiveSpaceItem *) item;
            [adaptiveSpaceItem updateWidth];
        }
    }
}

@end