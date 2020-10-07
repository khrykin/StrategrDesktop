//
// Created by Dmitry Khrykin on 2019-07-26.
//

#import <AppKit/AppKit.h>

#import "cocoa/STGToolbar.h"

#include <QtMac>

#include "macoswindow.h"
#include "mainscene.h"
#include "mainwindow.h"

NSWindow *NSWindowFromQWindow(const MainWindow *window) {
    auto *nsView = reinterpret_cast<NSView *>(window->winId());
    return nsView.window;
}

void MacOSWindow::setup(MainWindow *window) {
    @autoreleasepool {
        NSWindow *nsWindow = NSWindowFromQWindow(window);
        nsWindow.titleVisibility = NSWindowTitleHidden;
        nsWindow.styleMask |= NSWindowStyleMaskFullSizeContentView;
        [nsWindow.contentView setWantsLayer:YES];

        // Generate unique identifier to ensure that every window
        // has it's own toolbar
        NSString *toolbarIdentifier = makeToolbarIdentifier(window);

        STGToolbar *toolbar = [[[STGToolbar alloc] initWithIdentifier:toolbarIdentifier] autorelease];
        toolbar.allowsUserCustomization = NO;
        toolbar.displayMode = NSToolbarDisplayModeIconOnly;
        toolbar->qWindow = window;

        nsWindow.toolbar = toolbar;
        [toolbar setPage:0];

        auto path = window->fsIOManager.fileInfo().filePath();
        [nsWindow setRepresentedFilename:path.toNSString()];
    }
}

double MacOSWindow::toolbarHeight(MainWindow *window) {
    NSWindow *nativeWindow = NSWindowFromQWindow(window);
    return nativeWindow.frame.size.height - nativeWindow.contentLayoutRect.size.height;
}

NSString *MacOSWindow::makeToolbarIdentifier(const MainWindow *window) {
    auto integerPointer = reinterpret_cast<uintptr_t>(window);
    return QString::number(integerPointer).toNSString();
}

void MacOSWindow::pageDidChanged(MainWindow *window, int pageIndex) {
    auto *toolbar = (STGToolbar *) NSWindowFromQWindow(window).toolbar;
    [toolbar setPage:(unsigned int) pageIndex];
}

void MacOSWindow::updateWindowTitle(MainWindow *window) {
    auto *toolbar = (STGToolbar *) NSWindowFromQWindow(window).toolbar;
    [toolbar setPage:toolbar.currentPage];
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

QRect MacOSWindow::adjustedGeometry(MainWindow *window) {
    // NB! This method now does nothing. May be removed.
    auto qRect = QRect(window->geometry().x(),
                       window->geometry().y(),
                       window->geometry().width(),
                       window->geometry().height());

    return qRect;
}

bool MacOSWindow::hasSFSymbol() {
    if (@available(macOS 10.15, *))
        return true;

    return false;
}
