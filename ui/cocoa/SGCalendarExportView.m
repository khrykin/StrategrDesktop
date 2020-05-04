//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "SGCalendarExportView.h"
#import "SGCalendarImportExportViewSubclass.h"

@interface SGCalendarExportView ()

@property(nonatomic, weak) NSButton *overwriteCheckbox;
@property(nonatomic, weak) NSButton *notificationsCheckbox;
@property(nonatomic, weak) NSButton *specificCalendarCheckbox;
@property(nonatomic, weak) NSTextField *calendarNameTextField;
@property(nonatomic, weak) NSDatePicker *datePicker;

@end

@implementation SGCalendarExportView

- (void)setup {
    [super setup];

    self.bottomView.hidden = YES;

    self.okButton.title = @"Export";

    NSStackView *checksView = [[NSStackView alloc] init];

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

    [self.rightView addArrangedSubview:checksView];

    NSDatePicker *datePicker = [[NSDatePicker alloc] init];
    datePicker.datePickerMode = NSDatePickerModeSingle;
    datePicker.datePickerStyle = NSDatePickerStyleClockAndCalendar;
    datePicker.datePickerElements = NSDatePickerElementFlagYearMonth |
                                    NSDatePickerElementFlagYearMonthDay;
    datePicker.dateValue = [NSDate date];

    self.datePicker = datePicker;

    [self.leftView addArrangedSubview:datePicker];
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

