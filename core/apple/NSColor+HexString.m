//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "NSColor+HexString.h"

CGColorRef CGColorCreateWithHexColorString(NSString *inColorString) {
    unsigned colorCode = 0;
    unsigned char redByte, greenByte, blueByte;

    if (nil != inColorString) {
        NSString *sanitizedString = [inColorString stringByReplacingOccurrencesOfString:@"#"
                                                                             withString:@""];
        NSScanner *scanner = [NSScanner scannerWithString:sanitizedString];
        (void) [scanner scanHexInt:&colorCode]; // ignore error
    }

    redByte = (unsigned char) (colorCode >> 16u);
    greenByte = (unsigned char) (colorCode >> 8u);
    blueByte = (unsigned char) (colorCode);

    CGFloat components[4] = {(CGFloat) redByte / 0xff,
                             (CGFloat) greenByte / 0xff,
                             (CGFloat) blueByte / 0xff,
                             1.0};

    return CGColorCreate(CGColorSpaceCreateDeviceRGB(), components);
}

NSString *CGColorToHexColorString(CGColorRef color) {
    const CGFloat *components = CGColorGetComponents(color);
    return [[NSString stringWithFormat:@"#%02lX%02lX%02lX",
                                       lroundf((float) components[0] * 255),
                                       lroundf((float) components[1] * 255),
                                       lroundf((float) components[2] * 255)]
            lowercaseString];
}

#if !TARGET_OS_IOS

@implementation NSColor (HexString)
+ (NSColor *)colorWithHexColorString:(NSString *)inColorString {
    return [NSColor colorWithCGColor:CGColorCreateWithHexColorString(inColorString)];
}
@end

#endif
