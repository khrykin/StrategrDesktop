//
//  SGCalendarExportViewModel.m
//  StrategrKit
//
//  Created by Dmitry Khrykin on 11.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import "SGCalendarExportViewModel.h"
#import "SGCalendarExporter.h"

@interface SGCalendarExportViewModel ()

@end

@implementation SGCalendarExportViewModel

- (instancetype)init {
    if (self = [super init]) {
        self.settings = [SGCalendarExporter defaultSettings];
    }

    return self;
}

- (BOOL)overwriteIsOn {
    return (self.settings.optionsMask & SGCalendarExportOptionsOverwrite) == SGCalendarExportOptionsOverwrite;
}

- (BOOL)includeNotificationsIsOn {
    return (self.settings.optionsMask & SGCalendarExportOptionsIncludeNotifications) ==
           SGCalendarExportOptionsIncludeNotifications;
}

- (BOOL)useSpecificCalendarIsOn {
    return (self.settings.optionsMask & SGCalendarExportOptionsUseSpecificCalendar) ==
           SGCalendarExportOptionsUseSpecificCalendar;
}

- (void)didChangeDate:(NSDate *)date {
    self.settings.date = date;
}

- (void)didToggleOverwrite {
    self.settings.optionsMask ^= SGCalendarExportOptionsOverwrite;
}

- (void)didToggleIncludeNotifications {
    self.settings.optionsMask ^= SGCalendarExportOptionsIncludeNotifications;
}

- (void)didToggleSpecificCalendar {
    self.settings.optionsMask ^= SGCalendarExportOptionsUseSpecificCalendar;

    [self.delegate viewModel:self wantChangeSpecificCalendarEditorVisibility:self.useSpecificCalendarIsOn];
}

- (void)didChangeCalendarName:(NSString *)calendarName {
    self.settings.calendarName = calendarName;
}

@end
