//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "STGCalendarExportView.h"
#import "STGCalendarImportExportViewSubclass.h"

@interface STGCalendarExportView ()

@property(nonatomic, weak) NSButton *overwriteCheckbox;
@property(nonatomic, weak) NSButton *notificationsCheckbox;
@property(nonatomic, weak) NSButton *specificCalendarCheckbox;
@property(nonatomic, weak) NSTextField *calendarNameTextField;
@property(nonatomic, weak) NSDatePicker *datePicker;

@end

@implementation STGCalendarExportView

- (instancetype)init {
    self.viewModel = [[STGCalendarExportViewModel alloc] init];
    self.viewModel.delegate = self;

    self = [super init];

    return self;
}

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

    NSButton *overwriteCheckbox = [NSButton buttonWithTitle:STGCalendarExportViewModelTitleOverwrite
                                                     target:self
                                                     action:@selector(checkboxClicked:)];

    overwriteCheckbox.state = self.viewModel.overwriteIsOn;
    [overwriteCheckbox setButtonType:NSButtonTypeSwitch];
    [checksView addArrangedSubview:overwriteCheckbox];
    self.overwriteCheckbox = overwriteCheckbox;

    NSButton *notificationsCheckbox = [NSButton buttonWithTitle:STGCalendarExportViewModelTitleIncludeNotifications
                                                         target:self
                                                         action:@selector(checkboxClicked:)];

    notificationsCheckbox.state = self.viewModel.includeNotificationsIsOn;
    [notificationsCheckbox setButtonType:NSButtonTypeSwitch];
    [checksView addArrangedSubview:notificationsCheckbox];
    self.notificationsCheckbox = notificationsCheckbox;

    NSButton *specificCalendarCheckbox = [NSButton buttonWithTitle:STGCalendarExportViewModelTitleUseSpecificCalendar
                                                            target:self
                                                            action:@selector(checkboxClicked:)];

    specificCalendarCheckbox.state = self.viewModel.useSpecificCalendarIsOn;
    [specificCalendarCheckbox setButtonType:NSButtonTypeSwitch];
    [checksView addArrangedSubview:specificCalendarCheckbox];
    self.specificCalendarCheckbox = specificCalendarCheckbox;

    NSTextField *calendarNameTextField = [NSTextField textFieldWithString:self.viewModel.settings.calendarName];
    calendarNameTextField.bezeled = YES;
    calendarNameTextField.bezelStyle = NSTextFieldRoundedBezel;
    calendarNameTextField.placeholderString = @"Calendar Name";
    calendarNameTextField.target = self;
    calendarNameTextField.action = @selector(calendarNameChanged:);

    [checksView addArrangedSubview:calendarNameTextField];
    self.calendarNameTextField = calendarNameTextField;

    [self.rightView addArrangedSubview:checksView];

    NSDatePicker *datePicker = [[NSDatePicker alloc] init];
    datePicker.datePickerMode = NSDatePickerModeSingle;
    datePicker.datePickerStyle = NSDatePickerStyleClockAndCalendar;
    datePicker.datePickerElements = NSDatePickerElementFlagYearMonth |
                                    NSDatePickerElementFlagYearMonthDay;
    datePicker.dateValue = self.viewModel.settings.date;
    datePicker.target = self;
    datePicker.action = @selector(dateChanged:);

    self.datePicker = datePicker;

    [self.leftView addArrangedSubview:datePicker];

    [self setCalendarNameEditorVisible:self.viewModel.useSpecificCalendarIsOn];
}

- (void)checkboxClicked:(NSButton *)target {
    if (target == self.overwriteCheckbox) {
        [self.viewModel didToggleOverwrite];
    } else if (target == self.notificationsCheckbox) {
        [self.viewModel didToggleIncludeNotifications];
    } else if (target == self.specificCalendarCheckbox) {
        [self.viewModel didToggleSpecificCalendar];
    }
}

- (void)dateChanged:(NSDatePicker *)datePicker {
    [self.viewModel didChangeDate:datePicker.dateValue];
}

- (void)calendarNameChanged:(NSTextField *)textField {
    [self.viewModel didChangeCalendarName:textField.stringValue];
}

- (void)setCalendarNameEditorVisible:(BOOL)isVisible {
    NSString *checkboxTitle = self.specificCalendarCheckbox.title;

    if (isVisible) {
        self.calendarNameTextField.hidden = NO;
        self.specificCalendarCheckbox.title = [checkboxTitle stringByAppendingString:@":"];
    } else {
        self.calendarNameTextField.hidden = YES;
        self.specificCalendarCheckbox.title = [checkboxTitle stringByReplacingOccurrencesOfString:@":"
                                                                                       withString:@""];
    }
}

@end

@implementation STGCalendarExportView (SGCalendarExportViewModelDelegate)

- (void)viewModel:(STGCalendarExportViewModel *)viewModel wantChangeSpecificCalendarEditorVisibility:(BOOL)isVisible {
    [self setCalendarNameEditorVisible:isVisible];
}

@end

