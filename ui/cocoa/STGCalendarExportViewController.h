//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDAREXPORTVIEWCONTROLLER_H
#define STRATEGR_SGCALENDAREXPORTVIEWCONTROLLER_H

#import "STGCalendarImportExportView.h"
#import <AppKit/AppKit.h>

@interface STGCalendarExportViewController : NSViewController <NSWindowDelegate, SGCalendarImportExportViewDelegate>

- (void)optionsViewWantsCancel;
- (void)optionsViewWantsPerform;

@end

#endif//STRATEGR_SGCALENDAREXPORTVIEWCONTROLLER_H
