//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGOPTIONSVIEWCONTROLLER_H
#define STRATEGR_SGOPTIONSVIEWCONTROLLER_H

#import <AppKit/AppKit.h>
#import "SGCalendarExportOptionsView.h"

@interface SGOptionsWindowViewController : NSViewController <NSWindowDelegate, SGCalendarExportOptionsViewDelegate>
- (void)optionsViewWantsCancel;
- (void)optionsViewWantsExport;
@end

#endif //STRATEGR_SGOPTIONSVIEWCONTROLLER_H
