//
// Created by Dmitry Khrykin on 2020-02-27.
//

#import <AppKit/AppKit.h>
#include <QtMacExtras>

#include "colorprovider.h"

QColor ColorProvider::controlColor() {
    NSColor *nsColor;
    if (@available(macOS 10.14, *)) {
        nsColor = [[NSColor controlAccentColor] colorUsingColorSpace:[NSColorSpace deviceRGBColorSpace]];
    } else {
        nsColor = [[NSColor linkColor] colorUsingColorSpace:[NSColorSpace deviceRGBColorSpace]];
    }

    return QColor(static_cast<int>(nsColor.redComponent * 255),
                  static_cast<int>(nsColor.greenComponent * 255),
                  static_cast<int>(nsColor.blueComponent * 255),
                  static_cast<int>(nsColor.alphaComponent * 255));
}