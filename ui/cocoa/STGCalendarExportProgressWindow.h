//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
#define STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H

#import <AppKit/AppKit.h>

#import "core/apple/STGCalendarExporter.h"

#import "STGCalendarManager.h"
#import "STGProgressWindow.h"

@interface STGCalendarExportProgressWindow : STGProgressWindow <STGCalendarExporterDelegate>
@end

#endif //STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
