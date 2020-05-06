//
//  CalendarImportView.h
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <EventKit/EventKit.h>

#import "SGCalendarImportExportView.h"
#import "SGCalendarImporter.h"

NS_ASSUME_NONNULL_BEGIN

@interface SGCalendarImportView : SGCalendarImportExportView <NSTableViewDataSource, NSTableViewDelegate>

@property(nonatomic) SGCalendarImportOptions optionsMask;
@property(nonatomic, strong) NSArray<EKCalendar *> *calendars;
@property(nonatomic, strong) NSMutableArray<NSString *> *_Nullable selectedCalendarsIdentifiers;
@property(readonly) NSDate *date;

@end

NS_ASSUME_NONNULL_END
