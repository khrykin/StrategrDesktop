//
//  STGCalendarImportViewModel.m
//  StrategrKit
//
//  Created by Dmitry Khrykin on 10.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <EventKit/EKCalendar.h>

#import "STGCalendarImportViewModel.h"
#import "STGCalendarImporter.h"

@interface STGCalendarImportViewModel ()

@property(nonatomic, strong) NSDate *date;
@property(nonatomic, strong) NSMutableArray<NSString *> *_Nullable selectedCalendarsIdentifiers;
@property(nonatomic) STGCalendarImportOptions optionsMask;

@end

@implementation STGCalendarImportViewModel

- (instancetype)init {
    if (self = [super init]) {
        STGCalendarImporterSettings *defaultSettings = [STGCalendarImporter defaultSettings];

        self.calendars = [[NSArray alloc] init];
        self.date = [[NSDate alloc] init];
        self.selectedCalendarsIdentifiers = defaultSettings.calendarsIdentifiers
                                                ? [[NSMutableArray alloc] initWithArray:defaultSettings.calendarsIdentifiers]
                                                : nil;

        self.optionsMask = defaultSettings.optionsMask;
    }

    return self;
}

- (STGCalendarImporterSettings *)settings {
    STGCalendarImporterSettings *settings = [[STGCalendarImporterSettings alloc] init];
    settings.date = self.date;
    settings.optionsMask = self.optionsMask;
    settings.calendarsIdentifiers = self.selectedCalendarsIdentifiers;

    return settings;
}

- (STGCheckboxState)selectedAllCheckboxState {
    if (self.selectedCalendarsIdentifiers == nil) {
        return STGCheckboxStateOn;
    } else if (self.selectedCalendarsIdentifiers.count == 0) {
        return STGCheckboxStateOff;
    }

    return STGCheckboxStateMixed;
}

- (BOOL)isCalendarSelected:(EKCalendar *)calendar {
    if (self.selectedCalendarsIdentifiers == nil) {
        return YES;
    }

    return [self.selectedCalendarsIdentifiers containsObject:calendar.calendarIdentifier];
}

- (BOOL)isNoCalendarSelected {
    return self.selectedCalendarsIdentifiers != nil && self.selectedCalendarsIdentifiers.count == 0;
}

- (void)setCalendars:(NSArray<EKCalendar *> *)calendars {
    NSSortDescriptor *sort = [NSSortDescriptor sortDescriptorWithKey:@"title" ascending:YES];
    NSArray<EKCalendar *> *sortedCalendars = [calendars sortedArrayUsingDescriptors:@[sort]];

    _calendars = sortedCalendars;

    [self cleanupCalendarIdentifiers];
    [self.delegate viewModel:self didChangeCalendars:self.calendars];
}

- (void)cleanupCalendarIdentifiers {
    for (NSString *calendarIdentifier in self.selectedCalendarsIdentifiers) {
        NSPredicate *sameIdentifierPredicate = [NSPredicate predicateWithFormat:@"calendarIdentifier = %@", calendarIdentifier];

        NSArray<EKCalendar *> *filteredCalendars = [self.calendars filteredArrayUsingPredicate:sameIdentifierPredicate];
        if (!filteredCalendars.count) {
            [self.selectedCalendarsIdentifiers removeObject:calendarIdentifier];
        }
    }
}

- (void)didToggleCalendarAt:(NSInteger)calendarIndex {
    EKCalendar *calendar = self.calendars[calendarIndex];

    if (self.selectedCalendarsIdentifiers == nil) {
        self.selectedCalendarsIdentifiers = [[NSMutableArray alloc] init];

        for (EKCalendar *cal in self.calendars) {
            if (cal.calendarIdentifier == calendar.calendarIdentifier) {
                continue;
            }

            [self.selectedCalendarsIdentifiers addObject:cal.calendarIdentifier];
        }
    } else {
        if ([self.selectedCalendarsIdentifiers containsObject:calendar.calendarIdentifier]) {
            [self.selectedCalendarsIdentifiers removeObject:calendar.calendarIdentifier];
        } else {
            [self.selectedCalendarsIdentifiers addObject:calendar.calendarIdentifier];
        }
    }

    if (self.selectedCalendarsIdentifiers.count == self.calendars.count) {
        self.selectedCalendarsIdentifiers = nil;
    }
}

- (void)didToggleSelectedAll {
    if (self.selectedAllCheckboxState == STGCheckboxStateOn) {
        self.selectedCalendarsIdentifiers = [[NSMutableArray alloc] init];
    } else {
        self.selectedCalendarsIdentifiers = nil;
    }
}

- (void)didToggleOverwrite {
    self.optionsMask ^= STGCalendarImportOptionsOverwrite;
}

- (void)didChangeDate:(NSDate *)date {
    self.date = date;
}

@end
