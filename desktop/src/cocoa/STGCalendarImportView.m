//
//  CalendarImportView.m
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <CoreImage/CoreImage.h>

#import "STGCalendarImportExportViewSubclass.h"
#import "STGCalendarImportView.h"
#import "STGColoredCheckBoxView.h"

@interface SGCheckboxHeaderCell : NSTableHeaderCell

@property(nonatomic, strong) NSButtonCell *checkbox;

@end

@implementation SGCheckboxHeaderCell

- (instancetype)init {
    if (self = [super init]) {
        NSButtonCell *checkbox = [[NSButtonCell alloc] init];
        [checkbox setButtonType:NSButtonTypeSwitch];
        checkbox.state = NSControlStateValueOn;
        checkbox.title = @"";
        checkbox.allowsMixedState = YES;

        self.checkbox = checkbox;
    }

    return self;
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
    [super drawWithFrame:cellFrame inView:controlView];
    [self.checkbox drawWithFrame:CGRectOffset(cellFrame, 2.5, 0) inView:controlView];
}

@end

@interface STGCalendarImportView ()

@property(nonatomic, readwrite) STGCalendarImportViewModel *viewModel;

@property(nonatomic, weak) NSDatePicker *datePicker;
@property(nonatomic, weak) NSTableView *tableView;
@property(nonatomic, weak) NSTableColumn *checkmarksColumn;
@property(nonatomic, weak) NSTableColumn *calendarsColumn;
@property(nonatomic, weak) NSButton *overrideCheckbox;

@end

@implementation STGCalendarImportView

- (instancetype)initWithFrame:(NSRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.viewModel = [[STGCalendarImportViewModel alloc] init];
        self.viewModel.delegate = self;
    }

    return self;
}

- (void)setup {
    [super setup];

    self.okButton.title = @"Import";

    NSDatePicker *datePicker = [[NSDatePicker alloc] init];
    datePicker.datePickerMode = NSDatePickerModeSingle;
    datePicker.datePickerStyle = NSDatePickerStyleClockAndCalendar;
    datePicker.datePickerElements = NSDatePickerElementFlagYearMonth |
                                    NSDatePickerElementFlagYearMonthDay;
    datePicker.dateValue = [NSDate date];
    datePicker.target = self;
    datePicker.action = @selector(datePickerChanged:);

    self.datePicker = datePicker;

    [self.leftView addArrangedSubview:datePicker];

    NSTableView *calendarsTableView = [[NSTableView alloc] init];
    self.tableView = calendarsTableView;

    calendarsTableView.dataSource = self;
    calendarsTableView.delegate = self;
    calendarsTableView.focusRingType = NSFocusRingTypeNone;
    calendarsTableView.allowsColumnResizing = NO;
    calendarsTableView.allowsColumnReordering = NO;
    calendarsTableView.allowsTypeSelect = NO;
    calendarsTableView.selectionHighlightStyle = NSTableViewSelectionHighlightStyleNone;
    calendarsTableView.headerView = [[NSTableHeaderView alloc] init];

    NSTableColumn *checkmarksColumn = [[NSTableColumn alloc] init];
    self.checkmarksColumn = checkmarksColumn;

    checkmarksColumn.headerCell = [[SGCheckboxHeaderCell alloc] init];
    checkmarksColumn.title = @"";
    checkmarksColumn.width = 21.5;
    [calendarsTableView addTableColumn:checkmarksColumn];

    NSTableColumn *calendarsColumn = [[NSTableColumn alloc] init];
    self.calendarsColumn = calendarsColumn;

    calendarsColumn.title = @"Calendar";
    [calendarsTableView addTableColumn:calendarsColumn];

    NSScrollView *scrollView = [[NSScrollView alloc] init];
    scrollView.documentView = calendarsTableView;
    scrollView.drawsBackground = YES;
    scrollView.borderType = NSBezelBorder;

    [self.rightView addArrangedSubview:scrollView];
    [self addConstraints:@[
        [scrollView.widthAnchor constraintEqualToConstant:250],
        [scrollView.heightAnchor constraintEqualToAnchor:datePicker.heightAnchor],
        [datePicker.topAnchor constraintEqualToAnchor:scrollView.topAnchor]
    ]];

    NSButton *overrideCheckbox = [NSButton checkboxWithTitle:STGCalendarImportViewModelTitleOverwrite
                                                      target:self
                                                      action:@selector(overwriteCheckboxClicked:)];

    overrideCheckbox.state = (self.viewModel.settings.optionsMask & STGCalendarImportOptionsOverwrite) ==
                             STGCalendarImportOptionsOverwrite;

    self.overrideCheckbox = overrideCheckbox;

    [self.bottomView addArrangedSubview:overrideCheckbox];
}

- (void)overwriteCheckboxClicked:(NSButton *)checkbox {
    [self.viewModel didToggleOverwrite];
}

- (void)datePickerChanged:(NSDatePicker *)datePicker {
    [self.viewModel didChangeDate:datePicker.dateValue];
}

- (void)calendarCheckboxClicked:(STGColoredCheckBoxView *)checkBoxView {
    [self.viewModel didToggleCalendarAt:checkBoxView.calendarIndex];

    EKCalendar *calendar = self.viewModel.calendars[(NSUInteger) checkBoxView.calendarIndex];
    checkBoxView.state = [self.viewModel isCalendarSelected:calendar];

    self.headerCheckbox.state = self.viewModel.selectedAllCheckboxState;
    [self.tableView.headerView setNeedsDisplay:YES];
}

- (NSButtonCell *)headerCheckbox {
    SGCheckboxHeaderCell *headerCell = [self.checkmarksColumn headerCell];
    return headerCell.checkbox;
}

@end

@implementation STGCalendarImportView (NSTableViewDataSource)

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return self.viewModel.calendars ? self.viewModel.calendars.count : 0;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    return [[NSNumber alloc] initWithInt:(int) row];
}

@end

@implementation STGCalendarImportView (NSTableViewDelegate)

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSTableCellView *cell = [[NSTableCellView alloc] init];

    EKCalendar *calendar = self.viewModel.calendars[(NSUInteger) row];
    NSTextField *textField = [NSTextField textFieldWithString:calendar.title];

    if (tableColumn == self.calendarsColumn) {
        textField.editable = NO;
        textField.selectable = NO;
        textField.drawsBackground = NO;
        textField.bezeled = NO;
        textField.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        textField.frame = cell.bounds;

        [cell addSubview:textField];
    } else if (tableColumn == self.checkmarksColumn) {
        STGColoredCheckBoxView *checkbox = [[STGColoredCheckBoxView alloc] initWithTarget:self
                                                                                   action:@selector(calendarCheckboxClicked:)];

        checkbox.calendarIndex = row;
        checkbox.color = calendar.color;
        checkbox.state = [self.viewModel isCalendarSelected:calendar]
                             ? NSControlStateValueOn
                             : NSControlStateValueOff;

        return checkbox;
    }

    return cell;
}

- (NSTableRowView *)tableView:(NSTableView *)tableView rowViewForRow:(NSInteger)row {
    return [[NSTableRowView alloc] init];
}

- (void)tableView:(NSTableView *)tableView didClickTableColumn:(NSTableColumn *)tableColumn {
    if (tableColumn != self.checkmarksColumn) {
        return;
    }

    [self.viewModel didToggleSelectedAll];

    self.headerCheckbox.state = self.viewModel.selectedAllCheckboxState;

    [self.tableView reloadData];
}

@end

@implementation STGCalendarImportView (SGCalendarImportViewModelDelegate)

- (void)viewModel:(STGCalendarImportViewModel *)viewModel didChangeCalendars:(NSArray<EKCalendar *> *)calendars {
    self.headerCheckbox.state = self.viewModel.selectedAllCheckboxState;

    [self.tableView reloadData];
}

@end
