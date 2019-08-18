//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "NSColor+HexColor.h"

@implementation NSColor (HexColor)
+ (NSColor *)colorWithHexColorString:(NSString *)inColorString {
    NSColor *result = nil;
    unsigned colorCode = 0;
    unsigned char redByte, greenByte, blueByte;

    if (nil != inColorString) {
        NSString *sanitizedString = [inColorString stringByReplacingOccurrencesOfString:@"#"
                                                                             withString:@""];
        NSScanner *scanner = [NSScanner scannerWithString:sanitizedString];
        (void) [scanner scanHexInt:&colorCode]; // ignore error
    }

    redByte = (unsigned char) (colorCode >> 16);
    greenByte = (unsigned char) (colorCode >> 8);
    blueByte = (unsigned char) (colorCode);

    result = [NSColor colorWithCalibratedRed:(CGFloat) redByte / 0xff
                                       green:(CGFloat) greenByte / 0xff
                                        blue:(CGFloat) blueByte / 0xff
                                       alpha:1.0];
    return result;
}
@end