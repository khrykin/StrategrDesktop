//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "SGCalendarExportProgressWindow.h"

@implementation SGCalendarExportProgressWindow
- (instancetype)init {
    if (self = [super init]) {
        self.numberOfEvents = 0;
        self.currentEventIndex = 0;
        self.progressIndicatior.minValue = 0.0;
        self.progressIndicatior.maxValue = 1.0;
    }

    return self;
}

- (unsigned)calendarManagerNumberOfEvents {
    return self.numberOfEvents;
}

- (unsigned)calendarManagerIndexOfEvent:(EKEvent *)event {
    return self.currentEventIndex;
}

- (void)calendarManagerProgressChanged:(double)doubleValue {
    if (self.progressIndicatior.indeterminate) {
        self.progressIndicatior.indeterminate = NO;
    }

    NSLog(@"Progress %f", doubleValue);

    self.progressIndicatior.doubleValue = doubleValue;
    if (doubleValue == 1.0) {
        [self close];
    }
}

@end