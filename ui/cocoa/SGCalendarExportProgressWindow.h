//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
#define STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H

#import <AppKit/AppKit.h>

#import "core/apple/SGCalendarExporter.h"

#import "SGCalendarManager.h"
#import "SGProgressWindow.h"

@interface SGCalendarExportProgressWindow : SGProgressWindow <SGCalendarExporterDelegate>
@end

#endif //STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
