//
//  SGColoredCheckBoxView.h
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface STGColoredCheckBoxView : NSView <CALayerDelegate>

@property(nonatomic) NSInteger calendarIndex;
@property(nonatomic, strong) NSColor *color;
@property(nonatomic) NSControlStateValue state;
@property(weak) id target;
@property SEL action;

- (instancetype)initWithTarget:(id)target action:(SEL)action;

@end

NS_ASSUME_NONNULL_END
