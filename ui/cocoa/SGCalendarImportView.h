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
#import "SGCalendarImportViewModel.h"

NS_ASSUME_NONNULL_BEGIN

@interface SGCalendarImportView : SGCalendarImportExportView <NSTableViewDataSource, NSTableViewDelegate, SGCalendarImportViewModelDelegate>

@property(nonatomic, readonly) SGCalendarImportViewModel *viewModel;

@end

NS_ASSUME_NONNULL_END
