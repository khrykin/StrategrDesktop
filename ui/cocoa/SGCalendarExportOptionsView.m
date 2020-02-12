//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "SGCalendarExportOptionsView.h"

@interface SGCalendarExportOptionsView ()
@property(nonatomic, weak) NSStackView *stackView;
@property(nonatomic, weak) NSStackView *checksView;
@property(nonatomic, weak) NSStackView *rightView;

@property(nonatomic, weak) NSButton *overwriteCheckbox;
@property(nonatomic, weak) NSButton *notificationsCheckbox;
@property(nonatomic, weak) NSButton *specificCalendarCheckbox;
@property(nonatomic, weak) NSTextField *calendarNameTextField;

@property(nonatomic, weak) NSDatePicker *datePicker;
@end

@implementation SGCalendarExportOptionsView
- (instancetype)init {
    self = [self initWithFrame:NSZeroRect];

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
}

- (void)addButtonViews {
    @autoreleasepool {
        NSStackView *buttonsView = [[NSStackView alloc] initWithFrame:self.bounds];
        buttonsView.spacing = 8;
        buttonsView.orientation = NSUserInterfaceLayoutOrientationHorizontal;

        [self.stackView addConstraint:
                [buttonsView.widthAnchor constraintEqualToAnchor:self.stackView.widthAnchor
                                                      multiplier:1.0
                                                        constant:-self.stackView.edgeInsets.left
                                                                 - self.stackView.edgeInsets.right]];

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
    }
}

- (void)addBoxView {
    @autoreleasepool {
        NSBox *box = [[NSBox alloc] init];
        box.titlePosition = NSNoTitle;

        NSStackView *leftView = [[NSStackView alloc] init];
        leftView.spacing = 8;
        leftView.alignment = NSLayoutAttributeLeft;
        leftView.orientation = NSUserInterfaceLayoutOrientationVertical;

        NSStackView *rightView = [[NSStackView alloc] init];
        self.rightView = rightView;
        rightView.spacing = 8;
        rightView.alignment = NSLayoutAttributeLeft;
        rightView.orientation = NSUserInterfaceLayoutOrientationVertical;

        NSStackView *boxContainerView = [[NSStackView alloc] init];
        boxContainerView.spacing = 8;
        boxContainerView.edgeInsets = NSEdgeInsetsMake(8, 0, 8, 8);

        boxContainerView.orientation = NSUserInterfaceLayoutOrientationHorizontal;
        boxContainerView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        NSStackView *checksView = [[NSStackView alloc] init];
        self.checksView = checksView;

        checksView.spacing = 8;
        checksView.alignment = NSLayoutAttributeLeft;
        checksView.orientation = NSUserInterfaceLayoutOrientationVertical;
        [checksView setHuggingPriority:NSLayoutPriorityDefaultHigh
                        forOrientation:NSLayoutConstraintOrientationVertical];

        NSButton *overwriteCheckbox = [NSButton buttonWithTitle:@"Overwrite previous events"
                                                         target:nil
                                                         action:nil];

        overwriteCheckbox.state = NSControlStateValueOn;
        [overwriteCheckbox setButtonType:NSButtonTypeSwitch];
        [checksView addArrangedSubview:overwriteCheckbox];
        self.overwriteCheckbox = overwriteCheckbox;

        NSButton *notificationsCheckbox = [NSButton buttonWithTitle:@"Include notifications"
                                                             target:nil
                                                             action:nil];

        notificationsCheckbox.state = NSControlStateValueOn;
        [notificationsCheckbox setButtonType:NSButtonTypeSwitch];
        [checksView addArrangedSubview:notificationsCheckbox];
        self.notificationsCheckbox = notificationsCheckbox;

        NSButton *specificCalendarCheckbox = [NSButton buttonWithTitle:@"Export to the specific calendar"
                                                                target:self
                                                                action:@selector(specificCalendarCheckboxClilcked:)];

        specificCalendarCheckbox.state = NSControlStateValueOff;
        [specificCalendarCheckbox setButtonType:NSButtonTypeSwitch];
        [checksView addArrangedSubview:specificCalendarCheckbox];
        self.specificCalendarCheckbox = specificCalendarCheckbox;

        NSTextField *calendarNameTextField = [NSTextField textFieldWithString:@"Strategr"];
        calendarNameTextField.bezeled = YES;
        calendarNameTextField.bezelStyle = NSTextFieldRoundedBezel;
        calendarNameTextField.placeholderString = @"Calendar Name";
        calendarNameTextField.hidden = YES;

        [checksView addArrangedSubview:calendarNameTextField];
        self.calendarNameTextField = calendarNameTextField;

        [rightView addArrangedSubview:checksView];

        NSDatePicker *datePicker = [[NSDatePicker alloc] init];
        datePicker.datePickerMode = NSDatePickerModeSingle;
        datePicker.datePickerStyle = NSDatePickerStyleClockAndCalendar;
        datePicker.datePickerElements = NSDatePickerElementFlagYearMonth |
                                        NSDatePickerElementFlagYearMonthDay;
        datePicker.dateValue = [NSDate date];
        datePicker.bordered = NO;

        self.datePicker = datePicker;

        [leftView addArrangedSubview:datePicker];

        [self.stackView addConstraint:
                [box.widthAnchor constraintEqualToAnchor:self.stackView.widthAnchor
                                              multiplier:1.0
                                                constant:-self.stackView.edgeInsets.left -
                                                         self.stackView.edgeInsets.right]];


        [boxContainerView addArrangedSubview:leftView];
        [boxContainerView addArrangedSubview:rightView];
        [box addSubview:boxContainerView];
        [self.stackView addArrangedSubview:box];
    }
}

- (void)setup {
    [self addStackView];
    [self addBoxView];
    [self addButtonViews];
}

- (void)setOptionsMask:(SGCalendarExportOptions)optionsMask {
    self.overwriteCheckbox.state =
            (optionsMask & SGCalendarExportOptionsOverwriteIsOn) == SGCalendarExportOptionsOverwriteIsOn;

    self.notificationsCheckbox.state =
            (optionsMask & SGCalendarExportOptionsNotificationsIsOn) == SGCalendarExportOptionsNotificationsIsOn;

    BOOL specificCalendarOptionIsOn =
            (optionsMask & SGCalendarExportOptionsSpecificCalendarIsOn) == SGCalendarExportOptionsSpecificCalendarIsOn;

    self.specificCalendarCheckbox.state = specificCalendarOptionIsOn;
    self.calendarNameTextField.hidden = !specificCalendarOptionIsOn;
}

- (SGCalendarExportOptions)optionsMask {
    SGCalendarExportOptions mask = (SGCalendarExportOptions) 0;

    if (self.overwriteCheckbox.state == NSControlStateValueOn) {
        mask = mask | SGCalendarExportOptionsOverwriteIsOn;
    }

    if (self.notificationsCheckbox.state == NSControlStateValueOn) {
        mask = mask | SGCalendarExportOptionsNotificationsIsOn;
    }


    if (self.specificCalendarCheckbox.state == NSControlStateValueOn) {
        mask = mask | SGCalendarExportOptionsSpecificCalendarIsOn;
    }

    return mask;
}

- (void)specificCalendarCheckboxClilcked:(NSButton *)target {
    NSString *title = target.title;

    switch (target.state) {
        case NSControlStateValueOn:
            self.calendarNameTextField.hidden = NO;
            target.title = [title stringByAppendingString:@":"];
            break;

        case NSControlStateValueOff:
            self.calendarNameTextField.hidden = YES;
            target.title = [title stringByReplacingOccurrencesOfString:@":" withString:@""];

            break;
        default:
            break;
    }
}

- (NSDate *)date {
    return self.datePicker.dateValue;
}

- (NSString *)calendarName {
    switch (self.specificCalendarCheckbox.state) {
        case NSControlStateValueOn:
            return self.calendarNameTextField.stringValue;
        case NSControlStateValueOff:
            return nil;
        default:
            return nil;
    }
}

- (void)setCalendarName:(NSString *)calendarName {
    self.calendarNameTextField.stringValue = calendarName;
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

    NSString *specificCalendarIsOn =
            (self.optionsMask & SGCalendarExportOptionsSpecificCalendarIsOn) ==
            SGCalendarExportOptionsSpecificCalendarIsOn
            ? @"YES"
            : @"NO";

    return [NSString stringWithFormat:@"SGCalendarExportOptionsView(overwriteIsOn=%@, notificationsIsOn=%@, specificCalendarIsOn=%@, date=%@)",
                                      overwriteIsOn,
                                      notificationsIsOn,
                                      specificCalendarIsOn,
                                      self.date.description];
}


@end

