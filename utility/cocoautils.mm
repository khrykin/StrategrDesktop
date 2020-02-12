#include "cocoautils.h"

NSImage *NSMakeImageResized(NSImage *anImage, NSSize newSize) {
    NSImage *sourceImage = anImage;

    if (![sourceImage isValid]) {
        NSLog(@"Invalid Image");
    } else {
        NSImage *smallImage = [[NSImage alloc] initWithSize:newSize];
        [smallImage lockFocus];
        [sourceImage setSize:newSize];
        [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
        [sourceImage drawAtPoint:NSZeroPoint
                        fromRect:CGRectMake(0,
                                            0,
                                            newSize.width,
                                            newSize.height)
                       operation:NSCompositingOperationCopy fraction:1.0];
        [smallImage unlockFocus];

        return smallImage;
    }

    return nil;
}