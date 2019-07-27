//
// Created by Dmitry Khrykin on 2019-07-26.
//

#import <AppKit/AppKit.h>

#include "macoswindow.h"
#include "mainwindow.h"

#include <QtMac>

#include <QDebug>

const NSString *ToolbarItemActivitiesIdentifier = @"Activities";
const NSString *ToolbarItemStrategyTitleIdentifier = @"Title:Strategy";
const NSString *ToolbarItemNewActivityIdentifier = @"New";
const NSString *ToolbarItemActivitiesTitleIdentifier = @"Title:Activities";
const NSString *ToolbarItemBackButtonIdentifier = @"Back";
const NSString *ToolbarItemSettingsIdentifier = @"Settings";

const NSArray *sessionsPage = @[ToolbarItemActivitiesIdentifier,
                                NSToolbarFlexibleSpaceItemIdentifier,
                                ToolbarItemStrategyTitleIdentifier,
                                NSToolbarFlexibleSpaceItemIdentifier];

const NSArray *activitiesPage = @[ToolbarItemNewActivityIdentifier,
                                  NSToolbarFlexibleSpaceItemIdentifier,
                                  ToolbarItemActivitiesTitleIdentifier,
                                  ToolbarItemBackButtonIdentifier];

const NSArray *toolbarPages = @[sessionsPage, activitiesPage];

@interface ToolbarDelegate : NSObject <NSToolbarDelegate>

@property(readonly) unsigned int currentPage;
@property MainWindow *qWindow;
@property(retain) NSToolbar *toolbar;

- (void)setPage:(unsigned int)index;

@end

@implementation ToolbarDelegate
- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
     itemForItemIdentifier:(NSString *)itemIdentifier
 willBeInsertedIntoToolbar:(BOOL)flag {
    NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];

    if ([itemIdentifier isEqual:ToolbarItemActivitiesTitleIdentifier] ||
        [itemIdentifier isEqual:ToolbarItemStrategyTitleIdentifier]) {
        item.view = [self makeTextFieldForItemIdentifier:itemIdentifier];;
        return item;
    }

    item.view = [self makeButtonForItemIdentifier:itemIdentifier];;
    item.target = self;
    item.action = [self getActionSelectorForItemIdentifier:itemIdentifier];

    return item;
}

- (NSTextField *)makeTextFieldForItemIdentifier:(NSString *)itemIdentifier {
    NSTextField *textField = [NSTextField labelWithString:@""];
    [textField setFont:[NSFont boldSystemFontOfSize:13]];

    if ([itemIdentifier isEqual:ToolbarItemActivitiesTitleIdentifier]) {
        textField.stringValue = @"Activities";
    } else {
        textField.stringValue = _qWindow->windowTitle().toNSString();
    }

    textField.editable = NO;
    textField.selectable = NO;
    textField.textColor = [NSColor secondaryLabelColor];

    return textField;
}

- (NSButton *)makeButtonForItemIdentifier:(NSString *)itemIdentifier {
    NSButton *btn = [NSButton buttonWithTitle:itemIdentifier
                                       target:self
                                       action:@selector(openActivities:)];
    [btn setBezelStyle:NSBezelStyleTexturedRounded];

    if ([itemIdentifier isEqual:ToolbarItemSettingsIdentifier]) {
        btn.image = [NSImage imageNamed:NSImageNameActionTemplate];
    } else if ([itemIdentifier isEqual:ToolbarItemBackButtonIdentifier]) {
        btn.image = [NSImage imageNamed:NSImageNameGoBackTemplate];
        btn.bordered = NO;
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

-
(NSArray *)toolbarSelectableItemIdentifiers:
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

-
(void)setPage:(unsigned int)index {
    _currentPage = index;

    for (NSToolbarItem *_ in _toolbar.items) {
        [_toolbar removeItemAtIndex:0];
    }


    _toolbar.centeredItemIdentifier = index
                                      ? (NSString *) ToolbarItemActivitiesTitleIdentifier
                                      : (NSString *) ToolbarItemStrategyTitleIdentifier;

    for (NSString *itemIdentifier in toolbarPages[index]) {
        [_toolbar insertItemWithItemIdentifier:itemIdentifier atIndex:0];
    }
}

@end

NSWindow *nsWindowFromQWindow(const MainWindow *window) {
    auto *nsView = reinterpret_cast<NSView *>(window->winId());
    NSWindow *nsWindow = nsView.window;
    return nsWindow;


}

void MacOSWindow::setup(MainWindow *window) {
    @autoreleasepool {
        NSWindow *nsWindow = nsWindowFromQWindow(window);
        nsWindow.titleVisibility = NSWindowTitleHidden;

        [nsWindow.contentView setWantsLayer:YES];

        // Generate unique identifier to ensure that every window
        // has it's own toolbar
        auto integerPointer = reinterpret_cast<std::uintptr_t>(window);
        auto toolbarIdentifier = QString::number(integerPointer).toNSString();

        NSToolbar *toolbar = [[[NSToolbar alloc] initWithIdentifier:toolbarIdentifier] autorelease];
        toolbar.allowsUserCustomization = NO;
        toolbar.displayMode = NSToolbarDisplayModeIconOnly;

        // We must call teardown() function to release toolbarDelegate
        ToolbarDelegate *toolbarDelegate = [[ToolbarDelegate alloc] init];
        toolbarDelegate.toolbar = toolbar;
        toolbarDelegate.qWindow = window;


        toolbar.delegate = toolbarDelegate;
        nsWindow.toolbar = toolbar;

        [toolbarDelegate setPage:0];

        auto path = window->fsIOManager.fileInfo().filePath();
        [nsWindow setRepresentedFilename:path.toNSString()];
    }
}


void MacOSWindow::teardown(MainWindow *window) {
    @autoreleasepool {
        auto *toolbarDelegate = (ToolbarDelegate *) nsWindowFromQWindow(window).toolbar.delegate;
        [toolbarDelegate release];
    }
}

void MacOSWindow::pageChange(MainWindow *window, int pageIndex) {
    @autoreleasepool {
        auto *toolbarDelegate = (ToolbarDelegate *) nsWindowFromQWindow(window).toolbar.delegate;
        [toolbarDelegate setPage:(unsigned int) pageIndex];
    }
}

void MacOSWindow::updateWindowTitle(MainWindow *window) {
    @autoreleasepool {
        auto *toolbarDelegate = (ToolbarDelegate *) nsWindowFromQWindow(window).toolbar.delegate;
        [toolbarDelegate setPage:toolbarDelegate.currentPage];
    }
}

QPixmap MacOSWindow::resizeCursor() {
    NSCursor *cursor = [NSCursor resizeUpDownCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:NULL context:nil hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QPixmap MacOSWindow::closedHandCursor() {
    NSCursor *cursor = [NSCursor closedHandCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:NULL context:nil hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QPixmap MacOSWindow::openHandCursor() {
    NSCursor *cursor = [NSCursor openHandCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:NULL context:nil hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QPixmap MacOSWindow::dragCopyCursor() {
    NSCursor *cursor = [NSCursor dragCopyCursor];
    CGImageRef cgRef = [cursor.image CGImageForProposedRect:NULL context:nil hints:nil];
    return QtMac::fromCGImageRef(cgRef);
}

QRect MacOSWindow::geometry(MainWindow *window) {
    CGRect frame = nsWindowFromQWindow(window).frame;
    return QRect(static_cast<int>(frame.origin.x),
                 static_cast<int>(frame.origin.y),
                 static_cast<int>(frame.size.width),
                 static_cast<int>(frame.size.height));
}

