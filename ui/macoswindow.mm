//
// Created by Dmitry Khrykin on 2019-07-26.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "hicpp-use-auto"

#import <AppKit/AppKit.h>

#import "third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h"

#include <QtMac>

#include "macoswindow.h"
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

NSWindow *NSWindowFromQWindow(const MainWindow *window) {
    auto *nsView = reinterpret_cast<NSView *>(window->winId());
    return nsView.window;
}

@interface Toolbar : NSToolbar
@property(retain) id retainedDelegate;
@end

@implementation Toolbar
- (void)setDelegate:(id)delegate {
    [super setDelegate:delegate];
    self.retainedDelegate = delegate;
}

- (void)dealloc {
    [self.retainedDelegate release];
    [super dealloc];
}
@end

@interface ToolbarDelegate : NSObject <NSToolbarDelegate>

@property(readonly) unsigned int currentPage;
@property MainWindow *qWindow;
@property(assign) NSToolbar *toolbar;

- (void)setPage:(unsigned int)index;
@end

@implementation ToolbarDelegate
- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
     itemForItemIdentifier:(NSString *)itemIdentifier
 willBeInsertedIntoToolbar:(BOOL)flag {
    NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];

    if ([itemIdentifier isEqual:TAAdaptiveSpaceItemIdentifier]) {
        return [[[TAAdaptiveSpaceItem alloc]
                initWithItemIdentifier:(NSString *) TAAdaptiveSpaceItemIdentifier] autorelease];
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

    NSSegmentedControl *segmentedControl
            = [NSSegmentedControl segmentedControlWithImages:images
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

        NSString *qtWindowTitle = _qWindow->windowTitle().toNSString();
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
    _qWindow->scene()->showActivities();
}

- (void)openSettings:(id)sender {
    _qWindow->scene()->showStrategySettings();
}

- (void)openSessions:(id)sender {
    _qWindow->scene()->showSessions();
}

- (void)openNewActivityMenu:(id)sender {
    _qWindow->scene()->showNewActivityMenu();
}

- (void)setPage:(unsigned int)index {
    _currentPage = index;

    for (NSToolbarItem *_ in _toolbar.items) {
        [_toolbar removeItemAtIndex:0];
    }

    for (NSString *itemIdentifier in toolbarPages[index]) {
        [_toolbar insertItemWithItemIdentifier:itemIdentifier atIndex:0];
    }

    for (NSToolbarItem *item in _toolbar.items) {
        if ([item.itemIdentifier isEqualToString:(NSString *) TAAdaptiveSpaceItemIdentifier]) {
            TAAdaptiveSpaceItem *adaptiveSpaceItem = (TAAdaptiveSpaceItem *) item;
            [adaptiveSpaceItem updateWidth];
        }
    }
}

@end

void MacOSWindow::setup(MainWindow *window) {
    @autoreleasepool {
        NSWindow *nsWindow = NSWindowFromQWindow(window);
        nsWindow.titleVisibility = NSWindowTitleHidden;

        [nsWindow.contentView setWantsLayer:YES];

        // Generate unique identifier to ensure that every window
        // has it's own toolbar
        NSString *toolbarIdentifier = makeToolbarIdentifier(window);

        Toolbar *toolbar = [[[Toolbar alloc] initWithIdentifier:toolbarIdentifier] autorelease];
        toolbar.allowsUserCustomization = NO;
        toolbar.displayMode = NSToolbarDisplayModeIconOnly;

        ToolbarDelegate *toolbarDelegate = [[[ToolbarDelegate alloc] init] autorelease];
        toolbarDelegate.toolbar = toolbar;
        toolbarDelegate.qWindow = window;

        toolbar.delegate = toolbarDelegate;
        nsWindow.toolbar = toolbar;
        [toolbarDelegate setPage:0];

        auto path = window->fsIOManager.fileInfo().filePath();
        [nsWindow setRepresentedFilename:path.toNSString()];
    }
}

NSString *MacOSWindow::makeToolbarIdentifier(const MainWindow *window) {
    auto integerPointer = reinterpret_cast<uintptr_t>(window);
    return QString::number(integerPointer).toNSString();
}

void MacOSWindow::pageChange(MainWindow *window, int pageIndex) {
    @autoreleasepool {
        auto *toolbarDelegate = (ToolbarDelegate *) NSWindowFromQWindow(window).toolbar.delegate;
        [toolbarDelegate setPage:(unsigned int) pageIndex];
    }
}

void MacOSWindow::updateWindowTitle(MainWindow *window) {
    @autoreleasepool {
        NSWindow *nativeWindow = NSWindowFromQWindow(window);
        auto *toolbarDelegate = (ToolbarDelegate *) nativeWindow.toolbar.delegate;
        [toolbarDelegate setPage:toolbarDelegate.currentPage];
    }
}

QPixmap MacOSWindow::resizeCursor() {
    NSCursor *cursor = [NSCursor resizeUpDownCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:nil
                                                    context:nil
                                                      hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QPixmap MacOSWindow::closedHandCursor() {
    NSCursor *cursor = [NSCursor closedHandCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:nil
                                                    context:nil
                                                      hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QPixmap MacOSWindow::openHandCursor() {
    NSCursor *cursor = [NSCursor openHandCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:nil
                                                    context:nil
                                                      hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QRect MacOSWindow::geometry(MainWindow *window) {
    // What has_only 16? I've no idea...
    auto yDifference = 16;
    // What has_only 22? God only knows.
    auto heightDifference = 22;

    auto qRect = QRect(window->geometry().x(),
                       window->geometry().y() - yDifference,
                       window->geometry().width(),
                       window->geometry().height() - heightDifference);

    return qRect;
}


#pragma clang diagnostic pop