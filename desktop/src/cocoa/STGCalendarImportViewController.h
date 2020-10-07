//
// Created by Dmitry Khrykin on 03/05/2020.
//

#ifndef STRATEGR_SGCALENDARIMPORTVIEWCONTROLLER_H
#define STRATEGR_SGCALENDARIMPORTVIEWCONTROLLER_H

#import "STGCalendarImportExportView.h"
#import <AppKit/AppKit.h>

@interface STGCalendarImportViewController : NSViewController <NSWindowDelegate, SGCalendarImportExportViewDelegate>
- (void)optionsViewWantsCancel;
- (void)optionsViewWantsPerform;
@end

#endif//STRATEGR_SGCALENDARIMPORTVIEWCONTROLLER_H
