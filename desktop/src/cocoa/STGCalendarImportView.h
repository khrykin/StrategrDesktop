//
//  CalendarImportView.h
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <EventKit/EventKit.h>

#import <strategr/apple/STGCalendarImportViewModel.h>
#import <strategr/apple/STGCalendarImporter.h>

#import "STGCalendarImportExportView.h"

NS_ASSUME_NONNULL_BEGIN

@interface STGCalendarImportView : STGCalendarImportExportView <NSTableViewDataSource, NSTableViewDelegate, STGCalendarImportViewModelDelegate>

@property(nonatomic, readonly) STGCalendarImportViewModel *viewModel;

@end

NS_ASSUME_NONNULL_END
