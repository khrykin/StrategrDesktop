//
//  STGCalendarExportViewModel.m
//  StrategrKit
//
//  Created by Dmitry Khrykin on 11.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import "STGCalendarExportViewModel.h"
#import "STGCalendarExporter.h"

@interface STGCalendarExportViewModel ()

@end

@implementation STGCalendarExportViewModel

- (instancetype)init {
    if (self = [super init]) {
        self.settings = [STGCalendarExporter defaultSettings];
    }

    return self;
}

- (BOOL)overwriteIsOn {
    return (self.settings.optionsMask & STGCalendarExportOptionsOverwrite) == STGCalendarExportOptionsOverwrite;
}

- (BOOL)includeNotificationsIsOn {
    return (self.settings.optionsMask & STGCalendarExportOptionsIncludeNotifications) ==
           STGCalendarExportOptionsIncludeNotifications;
}

- (BOOL)useSpecificCalendarIsOn {
    return (self.settings.optionsMask & STGCalendarExportOptionsUseSpecificCalendar) ==
           STGCalendarExportOptionsUseSpecificCalendar;
}

- (void)didChangeDate:(NSDate *)date {
    self.settings.date = date;
}

- (void)didToggleOverwrite {
    self.settings.optionsMask ^= STGCalendarExportOptionsOverwrite;
}

- (void)didToggleIncludeNotifications {
    self.settings.optionsMask ^= STGCalendarExportOptionsIncludeNotifications;
}

- (void)didToggleSpecificCalendar {
    self.settings.optionsMask ^= STGCalendarExportOptionsUseSpecificCalendar;

    [self.delegate viewModel:self wantChangeSpecificCalendarEditorVisibility:self.useSpecificCalendarIsOn];
}

- (void)didChangeCalendarName:(NSString *)calendarName {
    self.settings.calendarName = calendarName;
}

@end
