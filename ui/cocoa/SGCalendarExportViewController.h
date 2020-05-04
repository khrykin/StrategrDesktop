//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDAREXPORTVIEWCONTROLLER_H
#define STRATEGR_SGCALENDAREXPORTVIEWCONTROLLER_H

#import <AppKit/AppKit.h>
#import "SGCalendarImportExportView.h"

@interface SGCalendarExportViewController : NSViewController <NSWindowDelegate, SGCalendarImportExportViewDelegate>

- (void)optionsViewWantsCancel;
- (void)optionsViewWantsPerform;

@end

#endif //STRATEGR_SGCALENDAREXPORTVIEWCONTROLLER_H
