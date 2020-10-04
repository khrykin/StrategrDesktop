#import <AppKit/AppKit.h>

#include <QtMacExtras>

#include "applicationicon.h"
#include "cocoautils.h"
#include "utils.h"

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
