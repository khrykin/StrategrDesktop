//
// Created by Dmitry Khrykin on 2019-07-26.
//

#import <AppKit/AppKit.h>

#import "cocoa/STGToolbar.h"

#include <QImage>
#include <QPixmap>

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

        // Generate a unique identifier to ensure that every window has its own
        // toolbar
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
    NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithData:[cursor.image TIFFRepresentation]];
    NSData *pngData = [bitmapRep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];

    QImage image;
    image.loadFromData(static_cast<const uchar *>(pngData.bytes), pngData.length, "PNG");
    return QPixmap::fromImage(image);
}

QPixmap MacOSWindow::closedHandCursor() {
    NSCursor *cursor = [NSCursor closedHandCursor];
    NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithData:[cursor.image TIFFRepresentation]];
    NSData *pngData = [bitmapRep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];

    QImage image;
    image.loadFromData(static_cast<const uchar *>(pngData.bytes), pngData.length, "PNG");
    return QPixmap::fromImage(image);
}

QPixmap MacOSWindow::openHandCursor() {
    NSCursor *cursor = [NSCursor openHandCursor];
    NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithData:[cursor.image TIFFRepresentation]];
    NSData *pngData = [bitmapRep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];

    QImage image;
    image.loadFromData(static_cast<const uchar *>(pngData.bytes), pngData.length, "PNG");
    return QPixmap::fromImage(image);
}

bool MacOSWindow::hasSFSymbol() {
    if (@available(macOS 10.15, *))
        return true;

    return false;
}
