//
//  SGCalendarExportViewModel.h
//  StrategrKit
//
//  Created by Dmitry Khrykin on 11.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SGCalendarExportViewModel;

NS_SWIFT_NAME(CalendarExportViewModelDelegate)
@protocol SGCalendarExportViewModelDelegate <NSObject>

@optional
- (void)viewModel:(SGCalendarExportViewModel *)viewModel wantChangeSpecificCalendarEditorVisibility:(BOOL)isVisible;

@end

@class SGCalendarExporterSettings;

NS_SWIFT_NAME(CalendarExportViewModel)
@interface SGCalendarExportViewModel : NSObject

@property(nonatomic, weak) id <SGCalendarExportViewModelDelegate> delegate;
@property(nonatomic, strong) SGCalendarExporterSettings *settings;

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
typedef NSString *SGCalendarExportViewModelTitle NS_STRING_ENUM;
static SGCalendarExportViewModelTitle const SGCalendarExportViewModelTitleOverwrite = @"Overwrtie previous events";
static SGCalendarExportViewModelTitle const SGCalendarExportViewModelTitleIncludeNotifications = @"Include notifications";
static SGCalendarExportViewModelTitle const SGCalendarExportViewModelTitleUseSpecificCalendar = @"Export to specific calendar";

NS_ASSUME_NONNULL_END
