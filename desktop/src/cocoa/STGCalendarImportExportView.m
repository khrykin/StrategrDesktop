//
//  CalendarImportExportView.m
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import "STGCalendarImportExportView.h"
#import "STGCalendarImportExportViewSubclass.h"

@implementation STGCalendarImportExportView

- (instancetype)init {
    if (self == [super init]) {
        [self setup];
    }

    return self;
}

- (void)setup {
    [self addButtonViews];
    [self addBoxView];
}

- (void)addBoxView {
    NSBox *box = [[NSBox alloc] init];
    self.box = box;

    box.titlePosition = NSNoTitle;
    box.translatesAutoresizingMaskIntoConstraints = NO;

    [self addSubview:box];
    [self addConstraints:@[
        [box.topAnchor constraintEqualToAnchor:self.topAnchor
                                      constant:20],
        [box.leadingAnchor constraintEqualToAnchor:self.leadingAnchor
                                          constant:20],
        [box.trailingAnchor constraintEqualToAnchor:self.trailingAnchor
                                           constant:-20],
        [box.bottomAnchor constraintEqualToAnchor:self.buttonsView.topAnchor
                                         constant:-8],
    ]];

    NSStackView *boxStackView = [[NSStackView alloc] init];
    self.mainStackView = boxStackView;

    boxStackView.spacing = 8;
    boxStackView.alignment = NSLayoutAttributeLeft;
    boxStackView.orientation = NSUserInterfaceLayoutOrientationVertical;
    boxStackView.translatesAutoresizingMaskIntoConstraints = NO;
    //    boxStackView.wantsLayer = YES;
    //    boxStackView.layer.borderColor = [[NSColor orangeColor] CGColor];
    //    boxStackView.layer.borderWidth = 1;

    [box addSubview:boxStackView];
    [box addConstraints:@[
        [boxStackView.leadingAnchor constraintEqualToAnchor:box.leadingAnchor
                                                   constant:20],
        [boxStackView.trailingAnchor constraintEqualToAnchor:box.trailingAnchor
                                                    constant:-20],
        [boxStackView.topAnchor constraintEqualToAnchor:box.topAnchor
                                               constant:20],
        [boxStackView.bottomAnchor constraintEqualToAnchor:box.bottomAnchor
                                                  constant:-20],
    ]];

    NSStackView *middleStackView = [[NSStackView alloc] init];

    middleStackView.spacing = 20;
    middleStackView.alignment = NSLayoutAttributeCenterY;
    middleStackView.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    //    middleStackView.wantsLayer = YES;
    //    middleStackView.layer.borderColor = [[NSColor blackColor] CGColor];
    //    middleStackView.layer.borderWidth = 1;

    [boxStackView addArrangedSubview:middleStackView];

    NSStackView *leftView = [[NSStackView alloc] init];
    self.leftView = leftView;

    leftView.spacing = 8;
    leftView.alignment = NSLayoutAttributeCenterY;
    leftView.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    //    leftView.wantsLayer = YES;
    //    leftView.layer.borderColor = [[NSColor yellowColor] CGColor];
    //    leftView.layer.borderWidth = 1;

    [leftView setHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];

    [middleStackView addArrangedSubview:leftView];

    NSStackView *rightView = [[NSStackView alloc] init];
    self.rightView = rightView;

    rightView.spacing = 8;
    rightView.alignment = NSLayoutAttributeCenterY | NSLayoutAttributeLeading;
    rightView.orientation = NSUserInterfaceLayoutOrientationVertical;
    //    rightView.wantsLayer = YES;
    //    rightView.layer.borderColor = [[NSColor blueColor] CGColor];
    //    rightView.layer.borderWidth = 1;

    [middleStackView addArrangedSubview:rightView];

    NSStackView *bottomView = [[NSStackView alloc] init];
    self.bottomView = bottomView;

    bottomView.spacing = 8;
    bottomView.alignment = NSLayoutAttributeCenterY | NSLayoutAttributeLeading;
    bottomView.orientation = NSUserInterfaceLayoutOrientationVertical;
    bottomView.translatesAutoresizingMaskIntoConstraints = false;
    //    bottomView.wantsLayer = YES;
    //    bottomView.layer.borderColor = [[NSColor greenColor] CGColor];
    //    bottomView.layer.borderWidth = 1;

    [boxStackView addArrangedSubview:bottomView];
}

- (void)addButtonViews {
    NSStackView *buttonsView = [[NSStackView alloc] initWithFrame:self.bounds];
    buttonsView.spacing = 8;
    buttonsView.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    buttonsView.translatesAutoresizingMaskIntoConstraints = NO;

    self.buttonsView = buttonsView;

    NSButton *okButton = [NSButton buttonWithTitle:@"Ok"
                                            target:self.delegate
                                            action:@selector(optionsViewWantsPerform)];
    self.okButton = okButton;

    okButton.keyEquivalent = @"\r";
    [okButton setNeedsDisplay:YES];

    NSButton *cancelButton = [NSButton buttonWithTitle:@"Cancel"
                                                target:self.delegate
                                                action:@selector(optionsViewWantsCancel)];
    self.cancelButton = cancelButton;

    [self addConstraints:@[
        [buttonsView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor
                                                  constant:20],
        [buttonsView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor
                                                   constant:-20],
        [buttonsView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor
                                                 constant:-8],
        [buttonsView.heightAnchor constraintEqualToAnchor:okButton.heightAnchor
                                               multiplier:1
                                                 constant:20]
    ]];


    [buttonsView addView:cancelButton inGravity:NSStackViewGravityTrailing];
    [buttonsView addView:okButton inGravity:NSStackViewGravityTrailing];

    [self addSubview:buttonsView];
}

@end
