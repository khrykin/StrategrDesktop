//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_OPTIONSVIEWDELEGATE_H
#define STRATEGR_OPTIONSVIEWDELEGATE_H

#import <AppKit/AppKit.h>

@protocol SGCalendarExportOptionsViewDelegate
- (void)optionsViewWantsCancel;
- (void)optionsViewWantsExport;
@end

typedef NS_OPTIONS(unsigned, SGCalendarExportOptions) {
    SGCalendarExportOptionsOverwriteIsOn = 1,
    SGCalendarExportOptionsNotificationsIsOn
};

@interface SGCalendarExportOptionsView : NSView
@property(nonatomic, assign) id <SGCalendarExportOptionsViewDelegate> delegate;
@property(nonatomic, readonly) NSDate *date;
@property(nonatomic) unsigned optionsMask;

- (NSString *)description;
@end

#endif //STRATEGR_OPTIONSVIEWDELEGATE_H
