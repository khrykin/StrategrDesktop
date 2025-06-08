#import <AppKit/AppKit.h>

#include <QImage>
#include <QPixmap>

#include "applicationicon.h"
#include "cocoautils.h"
#include "utils.h"

QPixmap ApplicationIcon::defaultIcon() {
    @autoreleasepool {
        NSImage *appIcon = [[NSApplication sharedApplication] applicationIconImage];
        NSImage *resizedIcon = NSMakeImageResized(appIcon, NSMakeSize(size, size));

        // Convert NSImage to PNG data
        NSData *pngData = [resizedIcon TIFFRepresentation];
        NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithData:pngData];
        NSData *pngData2 = [bitmapRep representationUsingType:NSBitmapImageFileTypePNG properties:@{}];
        
        // Create QImage from PNG data
        QImage image;
        image.loadFromData(static_cast<const uchar*>(pngData2.bytes), pngData2.length, "PNG");
        
        QPixmap pixmap = QPixmap::fromImage(image);
        pixmap.setDevicePixelRatio(devicePixelRatio());

        return pixmap;
    }
}
