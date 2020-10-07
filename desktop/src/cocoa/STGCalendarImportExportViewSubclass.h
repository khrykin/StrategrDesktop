//
//  SGCalendarImportExportSubclass.h
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import "STGCalendarImportExportView.h"

NS_ASSUME_NONNULL_BEGIN

@interface STGCalendarImportExportView ()

@property(nonatomic, weak) NSBox *box;
@property(nonatomic, weak) NSView *buttonsView;
@property(nonatomic, weak) NSButton *okButton;
@property(nonatomic, weak) NSButton *cancelButton;
@property(nonatomic, weak) NSStackView *mainStackView;
@property(nonatomic, weak) NSStackView *leftView;
@property(nonatomic, weak) NSStackView *rightView;
@property(nonatomic, weak) NSStackView *bottomView;

- (void)setup;

@end


NS_ASSUME_NONNULL_END
