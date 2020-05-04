//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_OPTIONSVIEW_H
#define STRATEGR_OPTIONSVIEW_H

#import "SGCalendarExporter.h"
#import "SGCalendarImportExportView.h"

@interface SGCalendarExportView : SGCalendarImportExportView

@property(nonatomic, readonly) NSDate *date;
@property(nonatomic, weak) NSString *calendarName;
@property(nonatomic) SGCalendarExportOptions optionsMask;

- (NSString *)description;

@end

#endif //STRATEGR_OPTIONSVIEW_H
