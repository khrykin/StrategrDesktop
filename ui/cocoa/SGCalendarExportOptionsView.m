//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "SGCalendarExportOptionsView.h"

@interface SGCalendarExportOptionsView ()
@property(nonatomic, assign) NSStackView *stackView;
@property(nonatomic, assign) NSButton *overwriteCheckbox;
@property(nonatomic, assign) NSButton *notificationsCheckbox;
@property(nonatomic, assign) NSDatePicker *datePicker;
@end

@implementation SGCalendarExportOptionsView
- (instancetype)init {
    if (self = [super init]) {
        [self setup];
    }

    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        [self setup];
    }

    return self;
}

- (void)addStackView {
    NSStackView *stackView = [[NSStackView alloc] initWithFrame:self.bounds];
    stackView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    stackView.edgeInsets = NSEdgeInsetsMake(20, 20, 20, 20);
    stackView.spacing = 10;
    stackView.orientation = NSUserInterfaceLayoutOrientationVertical;
    stackView.distribution = NSStackViewDistributionFill;

    self.stackView = stackView;

    [self addSubview:stackView];
    [stackView release];
}

- (void)addButtonViews {
    NSStackView *buttonsView = [[NSStackView alloc] initWithFrame:self.bounds];
    buttonsView.spacing = 8;
    buttonsView.orientation = NSUserInterfaceLayoutOrientationHorizontal;

    [self.stackView addConstraint:
            [buttonsView.widthAnchor constraintEqualToAnchor:self.stackView.widthAnchor
                                                  multiplier:1.0
                                                    constant:-self.stackView.edgeInsets.left -
                                                             self.stackView.edgeInsets.right]];

    NSButton *okButton = [NSButton buttonWithTitle:@"Export"
                                            target:self.delegate
                                            action:@selector(optionsViewWantsExport)];
    okButton.keyEquivalent = @"\r";
    [okButton setNeedsDisplay:YES];

    NSButton *cancelButton = [NSButton buttonWithTitle:@"Cancel"
                                                target:self.delegate
                                                action:@selector(optionsViewWantsCancel)];

    [buttonsView addView:cancelButton inGravity:NSStackViewGravityTrailing];
    [buttonsView addView:okButton inGravity:NSStackViewGravityTrailing];

    [self.stackView addArrangedSubview:buttonsView];
    [buttonsView release];
}

- (void)addBoxView {
    NSBox *box = [[NSBox alloc] init];
    box.titlePosition = NSNoTitle;

    NSStackView *leftView = [[NSStackView alloc] init];
    leftView.spacing = 8;
    leftView.alignment = NSLayoutAttributeLeft;
    leftView.orientation = NSUserInterfaceLayoutOrientationVertical;

    NSStackView *rightView = [[NSStackView alloc] init];
    rightView.spacing = 8;
    rightView.alignment = NSLayoutAttributeLeft;
    rightView.orientation = NSUserInterfaceLayoutOrientationVertical;

    NSStackView *boxContainerView = [[NSStackView alloc] init];
    boxContainerView.spacing = 8;
    boxContainerView.edgeInsets = NSEdgeInsetsMake(8, 0, 8, 8);

    boxContainerView.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    boxContainerView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    NSStackView *checksView = [[NSStackView alloc] init];
    checksView.spacing = 8;
    checksView.alignment = NSLayoutAttributeLeft;
    checksView.orientation = NSUserInterfaceLayoutOrientationVertical;

    self.overwriteCheckbox = [NSButton buttonWithTitle:@"Overwrite previous events"
                                                target:nil
                                                action:nil];

    self.overwriteCheckbox.state = NSControlStateValueOn;
    [self.overwriteCheckbox setButtonType:NSButtonTypeSwitch];
    [checksView addArrangedSubview:self.overwriteCheckbox];

    self.notificationsCheckbox = [NSButton buttonWithTitle:@"Include notifications"
                                                    target:nil
                                                    action:nil];

    self.notificationsCheckbox.state = NSControlStateValueOn;
    [self.notificationsCheckbox setButtonType:NSButtonTypeSwitch];
    [checksView addArrangedSubview:self.notificationsCheckbox];

    [rightView addArrangedSubview:checksView];
    [checksView release];

    NSDatePicker *datePicker = [[NSDatePicker alloc] init];
    datePicker.datePickerMode = NSDatePickerModeSingle;
    datePicker.datePickerStyle = NSDatePickerStyleClockAndCalendar;
    datePicker.datePickerElements = NSDatePickerElementFlagYearMonth |
                                    NSDatePickerElementFlagYearMonthDay;
    datePicker.dateValue = [NSDate date];
    datePicker.bordered = NO;

    self.datePicker = datePicker;

    [leftView addArrangedSubview:datePicker];
    [datePicker release];

    [self.stackView addConstraint:
            [box.widthAnchor constraintEqualToAnchor:self.stackView.widthAnchor
                                          multiplier:1.0
                                            constant:-self.stackView.edgeInsets.left -
                                                     self.stackView.edgeInsets.right]];


    [boxContainerView addArrangedSubview:leftView];
    [leftView release];

    [boxContainerView addArrangedSubview:rightView];
    [rightView release];

    [box addSubview:boxContainerView];
    [boxContainerView release];

    [self.stackView addArrangedSubview:box];
    [box release];
}

- (void)setup {
    [self addStackView];
    [self addBoxView];
    [self addButtonViews];
}

- (void)setOptionsMask:(unsigned)optionsMask {
    self.overwriteCheckbox.state
            = (optionsMask & SGCalendarExportOptionsOverwriteIsOn) == SGCalendarExportOptionsOverwriteIsOn;

    self.notificationsCheckbox.state
            = (optionsMask & SGCalendarExportOptionsNotificationsIsOn) == SGCalendarExportOptionsNotificationsIsOn;
}

- (unsigned)optionsMask {
    SGCalendarExportOptions mask = (SGCalendarExportOptions) 0;

    if (self.overwriteCheckbox.state == NSControlStateValueOn) {
        mask = mask | SGCalendarExportOptionsOverwriteIsOn;
    }

    if (self.notificationsCheckbox.state == NSControlStateValueOn) {
        mask = mask | SGCalendarExportOptionsNotificationsIsOn;
    }

    return mask;
}

- (NSDate *)date {
    return self.datePicker.dateValue;
}

- (NSString *)description {
    NSString *overwriteIsOn =
            (self.optionsMask & SGCalendarExportOptionsOverwriteIsOn) == SGCalendarExportOptionsOverwriteIsOn
            ? @"YES"
            : @"NO";
    NSString *notificationsIsOn =
            (self.optionsMask & SGCalendarExportOptionsNotificationsIsOn) == SGCalendarExportOptionsNotificationsIsOn
            ? @"YES"
            : @"NO";

    return [NSString stringWithFormat:@"SGCalendarExportOptionsView(overwriteIsOn=%@, notificationsIsOn=%@, date=%@)",
                                      overwriteIsOn,
                                      notificationsIsOn,
                                      self.date.description];
}


@end

