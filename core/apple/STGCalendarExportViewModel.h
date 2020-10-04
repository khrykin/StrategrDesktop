//
//  STGCalendarExportViewModel.h
//  StrategrKit
//
//  Created by Dmitry Khrykin on 11.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class STGCalendarExportViewModel;

NS_SWIFT_NAME(CalendarExportViewModelDelegate)
@protocol STGCalendarExportViewModelDelegate <NSObject>

@optional
- (void)viewModel:(STGCalendarExportViewModel *)viewModel wantChangeSpecificCalendarEditorVisibility:(BOOL)isVisible;

@end

@class STGCalendarExporterSettings;

NS_SWIFT_NAME(CalendarExportViewModel)
@interface STGCalendarExportViewModel : NSObject

@property(nonatomic, weak) id<STGCalendarExportViewModelDelegate> delegate;
@property(nonatomic, strong) STGCalendarExporterSettings *settings;

@property(nonatomic, readonly) BOOL overwriteIsOn;
@property(nonatomic, readonly) BOOL includeNotificationsIsOn;
@property(nonatomic, readonly) BOOL useSpecificCalendarIsOn;

- (void)didChangeDate:(NSDate *)date;
- (void)didToggleOverwrite;
- (void)didToggleIncludeNotifications;
- (void)didToggleSpecificCalendar;
- (void)didChangeCalendarName:(NSString *)calendarName;

@end

NS_SWIFT_NAME(CalendarExportViewModel.Title)
typedef NSString *STGCalendarExportViewModelTitle NS_STRING_ENUM;
static STGCalendarExportViewModelTitle const STGCalendarExportViewModelTitleOverwrite = @"Overwrtie previous events";
static STGCalendarExportViewModelTitle const STGCalendarExportViewModelTitleIncludeNotifications = @"Include notifications";
static STGCalendarExportViewModelTitle const STGCalendarExportViewModelTitleUseSpecificCalendar = @"Export to specific calendar";

NS_ASSUME_NONNULL_END
