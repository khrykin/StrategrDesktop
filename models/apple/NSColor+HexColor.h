//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_NSCOLOR_HEXCOLOR_H
#define STRATEGR_NSCOLOR_HEXCOLOR_H

#import <AppKit/NSColor.h>

@interface NSColor (HexColor)
+ (NSColor *)colorWithHexColorString:(NSString *)inColorString;
@end

#endif //STRATEGR_NSCOLOR_HEXCOLOR_H
