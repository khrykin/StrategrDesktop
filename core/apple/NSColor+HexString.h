//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_NSCOLOR_HEXSTRING_H
#define STRATEGR_NSCOLOR_HEXSTRING_H

#import <CoreGraphics/CGColor.h>
#import <Foundation/Foundation.h>
#import <TargetConditionals.h>

#ifdef __cplusplus
extern "C" {
#endif

CGColorRef CGColorCreateWithHexColorString(NSString *inColorString);
NSString *CGColorToHexColorString(CGColorRef color);

#ifdef __cplusplus
}
#endif

#if !TARGET_OS_IOS

#import <AppKit/NSColor.h>

@interface NSColor (HexString)
+ (NSColor *)colorWithHexColorString:(NSString *)inColorString;
@end

#endif

#endif //STRATEGR_NSCOLOR_HEXSTRING_H
