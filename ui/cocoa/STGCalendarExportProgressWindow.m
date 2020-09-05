//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "STGCalendarExportProgressWindow.h"

@implementation STGCalendarExportProgressWindow
- (instancetype)init {
    if (self = [super init]) {
        self.progressIndicatior.minValue = 0.0;
        self.progressIndicatior.maxValue = 1.0;
    }

    return self;
}

- (void)calendarExporterProgressChanged:(double)doubleValue {
    if (self.progressIndicatior.indeterminate) {
        self.progressIndicatior.indeterminate = NO;
    }

    self.progressIndicatior.doubleValue = doubleValue;

    if (doubleValue == 1.0) {
        [self close];
    }
}

@end