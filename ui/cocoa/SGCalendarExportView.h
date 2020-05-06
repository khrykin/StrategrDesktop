//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_OPTIONSVIEW_H
#define STRATEGR_OPTIONSVIEW_H

#import "SGCalendarExporter.h"
#import "SGCalendarImportExportView.h"

NS_ASSUME_NONNULL_BEGIN

@interface SGCalendarExportView : SGCalendarImportExportView

@property(nonatomic, readonly) NSDate *date;
@property(nonatomic, weak) NSString *_Nullable calendarName;
@property(nonatomic) SGCalendarExportOptions optionsMask;

- (NSString *)description;

@end

NS_ASSUME_NONNULL_END

#endif //STRATEGR_OPTIONSVIEW_H
