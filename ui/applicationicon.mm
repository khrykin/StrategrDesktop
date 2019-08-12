
#include "applicationicon.h"

#import <AppKit/AppKit.h>
#import <QtMac>

#include "utils.h"
#include "cocoautils.h"

QPixmap ApplicationIcon::defaultIcon() {
    @autoreleasepool {
        NSImage *appIcon = [[NSApplication sharedApplication] applicationIconImage];
        NSImage *resizedIcon = NSMakeImageResized(appIcon, NSMakeSize(size, size));

        CGImageRef cgRef = [resizedIcon CGImageForProposedRect:NULL
                                                       context:nil
                                                         hints:nil];

        auto pixmap = QtMac::fromCGImageRef(cgRef);
        pixmap.setDevicePixelRatio(devicePixelRatio());

        return pixmap;
    }
}

