//
// Created by Dmitry Khrykin on 03/05/2020.
//

#ifndef STRATEGR_SGCALENDARIMPORTVIEWCONTROLLER_H
#define STRATEGR_SGCALENDARIMPORTVIEWCONTROLLER_H

#import <AppKit/AppKit.h>
#import "STGCalendarImportExportView.h"

@interface STGCalendarImportViewController : NSViewController <NSWindowDelegate, SGCalendarImportExportViewDelegate>
- (void)optionsViewWantsCancel;
- (void)optionsViewWantsPerform;
@end

#endif //STRATEGR_SGCALENDARIMPORTVIEWCONTROLLER_H
