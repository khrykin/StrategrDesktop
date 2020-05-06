//
//  CalendarImportView.m
//  Strategr
//
//  Created by Dmitry Khrykin on 02/05/2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <CoreImage/CoreImage.h>

#import "SGCalendarImportView.h"
#import "SGCalendarImportExportViewSubclass.h"
#import "SGColoredCheckBoxView.h"

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

@interface SGCalendarImportView ()

@property(nonatomic, weak) NSDatePicker *datePicker;
@property(nonatomic, weak) NSTableView *tableView;
@property(nonatomic, weak) NSTableColumn *checkmarksColumn;
@property(nonatomic, weak) NSTableColumn *calendarsColumn;
@property(nonatomic, weak) NSButton *overrideCheckbox;

@end

@implementation SGCalendarImportView

- (void)setup {
    [super setup];

    self.okButton.title = @"Import";

    NSDatePicker *datePicker = [[NSDatePicker alloc] init];
    datePicker.datePickerMode = NSDatePickerModeSingle;
    datePicker.datePickerStyle = NSDatePickerStyleClockAndCalendar;
    datePicker.datePickerElements = NSDatePickerElementFlagYearMonth |
                                    NSDatePickerElementFlagYearMonthDay;
    datePicker.dateValue = [NSDate date];

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

    NSButton *overrideCheckbox = [NSButton checkboxWithTitle:@"Override current strategy"
                                                      target:self
                                                      action:nil];

    self.overrideCheckbox = overrideCheckbox;

    [self.bottomView addArrangedSubview:overrideCheckbox];
}

- (void)checkBoxClicked:(SGColoredCheckBoxView *)checkBoxView {
    NSInteger calendarIndex = [self.tableView rowForView:checkBoxView];

    [self setCalendarSelectedAtIndex:(NSUInteger) calendarIndex
                            selected:(BOOL) checkBoxView.state];

    self.headerCheckbox.state = NSControlStateValueMixed;
    [self.tableView.headerView setNeedsDisplay:YES];
}

- (void)setCalendars:(NSArray<EKCalendar *> *)calendars {
    _calendars = calendars;

    [self.tableView reloadData];
}

- (NSDate *)date {
    return self.datePicker.dateValue;
}

- (void)setSelectedCalendarsIdentifiers:(NSMutableArray<NSString *> *_Nullable)selectedCalendarsIdentifiers {
    if (selectedCalendarsIdentifiers)
        _selectedCalendarsIdentifiers = selectedCalendarsIdentifiers;
    else
        _selectedCalendarsIdentifiers = nil;

    [self updateCheckBoxes];
}

- (void)updateCheckBoxes {
    BOOL everythingIsSelected = NO;
    BOOL nothingIsSelected = NO;

    BOOL everythingIsSelectedSet = NO;
    BOOL nothingIsSelectedSet = NO;

    for (NSUInteger calendarIndex = 0; calendarIndex < self.tableView.numberOfRows; calendarIndex++) {
        EKCalendar *calendar = self.calendars[calendarIndex];
        NSButton *checkbox = [self checkboxAtIndex:calendarIndex];

        BOOL isSelected = [self isCalendarSelected:calendar];
        checkbox.state = isSelected ? NSControlStateValueOn : NSControlStateValueOff;

        if (!everythingIsSelectedSet) {
            everythingIsSelected = isSelected;
            everythingIsSelectedSet = !isSelected;
        }

        if (!nothingIsSelectedSet) {
            nothingIsSelected = !isSelected;
            nothingIsSelectedSet = isSelected;
        }
    }

    if (everythingIsSelected) {
        self.headerCheckbox.state = NSControlStateValueOn;
    } else if (nothingIsSelected) {
        self.headerCheckbox.state = NSControlStateValueOff;
    } else {
        self.headerCheckbox.state = NSControlStateValueMixed;
    }
}

- (NSButtonCell *)headerCheckbox {
    SGCheckboxHeaderCell *headerCell = [self.checkmarksColumn headerCell];
    return headerCell.checkbox;
}

- (NSButton *)checkboxAtIndex:(NSUInteger)index {
    return [self.tableView viewAtColumn:0 row:index makeIfNecessary:NO];
}

- (BOOL)isCalendarSelected:(EKCalendar *)calendar {
    if (self.selectedCalendarsIdentifiers == nil) {
        return YES;
    }

    NSPredicate *sameIdentifierPredicate
            = [NSPredicate predicateWithBlock:^BOOL(NSString *calendarIdentifier, NSDictionary *_) {
                return [calendarIdentifier isEqualToString:calendar.calendarIdentifier];
            }];

    NSArray<NSString *> *filtered
            = [self.selectedCalendarsIdentifiers filteredArrayUsingPredicate:sameIdentifierPredicate];

    return filtered.count != 0;
}

- (void)setCalendarSelectedAtIndex:(NSUInteger)calendarIndex selected:(BOOL)wantToSelect {
    EKCalendar *calendar = self.calendars[calendarIndex];
    BOOL calendarAlreadySelected = [self isCalendarSelected:calendar];

    if (!_selectedCalendarsIdentifiers)
        _selectedCalendarsIdentifiers = [[NSMutableArray alloc] init];

    if (wantToSelect && !calendarAlreadySelected) {
        [self.selectedCalendarsIdentifiers addObject:calendar.calendarIdentifier];
    } else if (calendarAlreadySelected) {
        [self.selectedCalendarsIdentifiers removeObject:calendar.calendarIdentifier];
    }
}

@end

@implementation SGCalendarImportView (NSTableViewDataSource)

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return self.calendars ? self.calendars.count : 0;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    return [[NSNumber alloc] initWithInt:(int) row];
}

@end

@implementation SGCalendarImportView (NSTableViewDelegate)

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSTableCellView *cell = [[NSTableCellView alloc] init];

    EKCalendar *calendar = self.calendars[(NSUInteger) row];
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
        SGColoredCheckBoxView *checkbox = [[SGColoredCheckBoxView alloc]
                initWithTarget:self
                        action:@selector(checkBoxClicked:)];

        checkbox.color = calendar.color;
        checkbox.state = [self isCalendarSelected:calendar]
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

    SGCheckboxHeaderCell *headerCell = [tableColumn headerCell];
    if (headerCell.checkbox.state == NSControlStateValueMixed) {
        headerCell.checkbox.state = NSControlStateValueOn;
    }

    headerCell.checkbox.state = !headerCell.checkbox.state;

    if (headerCell.checkbox.state == NSControlStateValueOn) {
        _selectedCalendarsIdentifiers = nil;
    } else {
        _selectedCalendarsIdentifiers = [[NSMutableArray alloc] init];
    }

    for (NSUInteger i = 0; i < tableView.numberOfRows; i++) {
        NSButton *checkbox = [self checkboxAtIndex:i];
        checkbox.state = headerCell.checkbox.state;
    }
}

- (void)setOptionsMask:(SGCalendarImportOptions)optionsMask {
    self.overrideCheckbox.state = (optionsMask & SGCalendarImportOptionsOverwrite) == SGCalendarImportOptionsOverwrite;
}

- (SGCalendarImportOptions)optionsMask {
    SGCalendarImportOptions optionsMask = 0;

    if (self.overrideCheckbox.state == NSControlStateValueOn)
        optionsMask |= SGCalendarImportOptionsOverwrite;

    return optionsMask;
}

@end
