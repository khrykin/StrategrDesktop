//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
#define STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H

#import <AppKit/AppKit.h>

#import <strategr/apple/STGCalendarExporter.h>
#import <strategr/apple/STGCalendarManager.h>

#import "STGProgressWindow.h"

@interface STGCalendarExportProgressWindow : STGProgressWindow <STGCalendarExporterDelegate>
@end

#endif//STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
