//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_OPTIONSVIEW_H
#define STRATEGR_OPTIONSVIEW_H

#import <strategr/apple/STGCalendarExportViewModel.h>
#import <strategr/apple/STGCalendarExporter.h>

#import "STGCalendarImportExportView.h"

NS_ASSUME_NONNULL_BEGIN

@interface STGCalendarExportView : STGCalendarImportExportView <STGCalendarExportViewModelDelegate>

@property(nonatomic) STGCalendarExportViewModel *viewModel;

@end

NS_ASSUME_NONNULL_END

#endif//STRATEGR_OPTIONSVIEW_H
