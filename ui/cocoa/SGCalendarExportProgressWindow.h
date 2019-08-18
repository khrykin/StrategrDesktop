//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
#define STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H

#import <AppKit/AppKit.h>
#import "SGCalendarManager.h"
#import "SGProgressWindow.h"

@interface SGCalendarExportProgressWindow : SGProgressWindow <SGCalendarManagerDelegate>
@property(nonatomic) unsigned numberOfEvents;
@property(nonatomic) unsigned currentEventIndex;
@end

#endif //STRATEGR_SGCALENDAREXPORTPROGRESSWINDOW_H
