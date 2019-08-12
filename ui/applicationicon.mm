
#include "applicationicon.h"

#import <AppKit/AppKit.h>
#import <QtMac>

#include "utils.h"

NSImage *NSMakeImageResized(NSImage *anImage, NSSize newSize) {
    NSImage *sourceImage = anImage;
    [sourceImage setScalesWhenResized:YES];

    if (![sourceImage isValid]) {
        NSLog(@"Invalid Image");
    } else {
        NSImage *smallImage = [[[NSImage alloc] initWithSize:newSize] autorelease];
        [smallImage lockFocus];
        [sourceImage setSize:newSize];
        [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
        [sourceImage drawAtPoint:NSZeroPoint
                        fromRect:CGRectMake(0,
                                            0,
                                            newSize.width,
                                            newSize.height)
                       operation:NSCompositeCopy fraction:1.0];
        [smallImage unlockFocus];

        return smallImage;
    }

    return nil;
}

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

