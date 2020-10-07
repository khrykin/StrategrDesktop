//
//  CalendarImportExportView.h
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SGCalendarImportExportViewDelegate <NSObject>

- (void)optionsViewWantsCancel;
- (void)optionsViewWantsPerform;

@end

@interface STGCalendarImportExportView : NSView

@property(nonatomic, weak) id<SGCalendarImportExportViewDelegate> delegate;

@end


NS_ASSUME_NONNULL_END
