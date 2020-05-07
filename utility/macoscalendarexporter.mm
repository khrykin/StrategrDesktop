//
// Created by Dmitry Khrykin on 2019-08-15.
//

#import <EventKit/EventKit.h>
#import <AppKit/AppKit.h>

#import "cocoa/SGCalendarExportViewController.h"
#import "cocoa/SGCalendarImportViewController.h"
#import "cocoa/SGCalendarImportView.h"
#import "cocoa/SGCalendarExportView.h"
#import "cocoa/SGCalendarExportProgressWindow.h"

#import "SGCalendarExporter.h"
#import "SGCalendarImporter.h"

#include "macoscalendarexporter.h"

void MacOSCalendarExporter::exportStrategy(const stg::strategy &strategy,
                                           Options options,
                                           time_t dateSecsFromEpoch,
                                           const std::string &calTitle) {
    // Obj-C block can't capture a C++ function parameter so we copy it here
    NSString *calendarTitle = calTitle.empty() ? nil : [NSString stringWithUTF8String:calTitle.c_str()];
    [SGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store) {
            return showAccessDeniedAlert();
        }

        SGCalendarExportProgressWindow *progressWindow = [[SGCalendarExportProgressWindow alloc] init];
        progressWindow.title = @"Exporting Strategy...";

        // window will be released automatically by ARC
        progressWindow.releasedWhenClosed = NO;
        [progressWindow makeKeyAndOrderFront:nil];

        SGCalendarExporterSettings *settings = [[SGCalendarExporterSettings alloc] init];
        settings.optionsMask = static_cast<SGCalendarExportOptions>(options);
        settings.date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];
        settings.calendarName = calendarTitle;

        SGCalendarExporter *exporter = [[SGCalendarExporter alloc] initWithStrategyPtr:(void *) &strategy
                                                                            eventStore:store
                                                                              settings:settings];
        exporter.delegate = progressWindow;

        [exporter export:^{
            [SGCalendarManager launchCalendarApp];
        }];
    }];
}

void MacOSCalendarExporter::showAccessDeniedAlert() {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"macOS denies Strategr access to Calendar";
    alert.informativeText = @"Change access settings in "
                            "System Preferences > Privacy > Calendar.";
    alert.icon = [NSImage imageNamed:NSImageNameCaution];

    [alert addButtonWithTitle:@"Open System Preferences"];
    [alert addButtonWithTitle:@"Cancel"];

    NSModalResponse modalResponse = [alert runModal];
    if (modalResponse == NSAlertFirstButtonReturn) {
        NSURL *prefsURL = [NSURL URLWithString:@"x-apple.systempreferences:"
                                               "com.apple.preference.security"
                                               "?Privacy_Calendars"];
        [[NSWorkspace sharedWorkspace] openURL:prefsURL];
    }
}

MacOSCalendarExporter::ExportOptionsWindowResult
MacOSCalendarExporter::showExportOptionsWindow(Options initialOptions, const std::string &initialCalendarName) {
    SGCalendarExportViewController *optionsWindowViewController
            = [[SGCalendarExportViewController alloc] init];
    SGCalendarExportView *optionsView = [[SGCalendarExportView alloc] init];
    optionsWindowViewController.view = optionsView;

    optionsView.optionsMask = static_cast<SGCalendarExportOptions>(initialOptions);

    if (!initialCalendarName.empty())
        optionsView.calendarName = [NSString stringWithUTF8String:initialCalendarName.c_str()];

    NSPanel *window = [NSPanel windowWithContentViewController:optionsWindowViewController];

    window.delegate = optionsWindowViewController;
    window.title = @"Export Strategy to Calendar";
    window.styleMask |= NSWindowStyleMaskUtilityWindow;
    window.styleMask &= ~(NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);

    NSApplication *app = [NSApplication sharedApplication];
    NSModalSession session = [app beginModalSessionForWindow:window];

    Options optionsMask = 0;
    NSDate *date = [NSDate date];
    std::string calendarName;
    NSModalResponse response = NSModalResponseCancel;
    for (;;) {
        response = [app runModalSession:session];
        if (response != NSModalResponseContinue) {
            if (response == NSModalResponseOK) {
                optionsMask = reinterpret_cast<Options>(optionsView.optionsMask);
                date = optionsView.date;
                if (optionsView.calendarName)
                    calendarName = optionsView.calendarName.UTF8String;

            }

            break;
        }
    }

    [app endModalSession:session];

    return ExportOptionsWindowResult{static_cast<Response>(response),
                                     optionsMask,
                                     static_cast<time_t>(date.timeIntervalSince1970),
                                     calendarName};
}

MacOSCalendarExporter::ImportOptionsWindowResult
MacOSCalendarExporter::showImportOptionsWindow(MacOSCalendarExporter::Options initialOptions,
                                               std::unique_ptr<std::vector<std::string>> initialCalendarsIdentifiers) {
    SGCalendarImportViewController *optionsWindowViewController
            = [[SGCalendarImportViewController alloc] init];
    SGCalendarImportView *optionsView = [[SGCalendarImportView alloc] init];
    optionsWindowViewController.view = optionsView;
    optionsView.optionsMask = static_cast<SGCalendarImportOptions>(initialOptions);

    NSPanel *window = [NSPanel windowWithContentViewController:optionsWindowViewController];

    window.delegate = optionsWindowViewController;
    window.title = @"Import Strategy from Calendar";
    window.styleMask |= NSWindowStyleMaskUtilityWindow;
    window.styleMask &= ~(NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);

    NSApplication *app = [NSApplication sharedApplication];
    NSModalSession session = [app beginModalSessionForWindow:window];

    NSMutableArray<NSString *> *nsCalendarsIdentifiers = nil;

    if (initialCalendarsIdentifiers) {
        nsCalendarsIdentifiers = [[NSMutableArray alloc] init];
        for (auto &identifier: *initialCalendarsIdentifiers) {
            [nsCalendarsIdentifiers addObject:[NSString stringWithUTF8String:identifier.c_str()]];
        }

    }

    NSLog(@"initialCalendarsIdentifiers: %@", nsCalendarsIdentifiers);

    [SGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store) {
            return showAccessDeniedAlert();
        }

        NSArray *calendars = [store calendarsForEntityType:EKEntityTypeEvent];
        optionsView.calendars = calendars;
        optionsView.selectedCalendarsIdentifiers = nsCalendarsIdentifiers;
    }];

    Options optionsMask = 0;
    NSDate *date = [NSDate date];
    NSModalResponse response = NSModalResponseCancel;
    std::unique_ptr<std::vector<std::string>> outputCalendarsIdentifiers = nullptr;

    for (;;) {
        response = [app runModalSession:session];
        if (response != NSModalResponseContinue) {
            if (response == NSModalResponseOK) {
                optionsMask = reinterpret_cast<Options>(optionsView.optionsMask);
                date = optionsView.date;

                NSLog(@"optionsView.selectedCalendarsIdentifiers: %@", optionsView.selectedCalendarsIdentifiers);

                if (optionsView.selectedCalendarsIdentifiers) {
                    outputCalendarsIdentifiers = std::make_unique<std::vector<std::string>>();
                    for (NSString *calendarIdentifier in optionsView.selectedCalendarsIdentifiers) {
                        outputCalendarsIdentifiers->push_back(calendarIdentifier.UTF8String);
                    }
                }
            }

            break;
        }
    }

    [app endModalSession:session];

    return ImportOptionsWindowResult{static_cast<Response>(response),
                                     optionsMask,
                                     static_cast<time_t>(date.timeIntervalSince1970),
                                     std::move(outputCalendarsIdentifiers)};
}

void MacOSCalendarExporter::importStrategy(stg::strategy &strategy,
                                           MacOSCalendarExporter::Options options,
                                           time_t dateSecsFromEpoch,
                                           std::unique_ptr<std::vector<std::string>> initialCalendarsIdentifiers) {
    NSMutableArray *calendarsIdentifiers = nil;
    if (initialCalendarsIdentifiers) {
        calendarsIdentifiers = [[NSMutableArray alloc] init];
        for (auto &identifier : *initialCalendarsIdentifiers) {
            [calendarsIdentifiers addObject:[NSString stringWithUTF8String:identifier.c_str()]];
        }
    }

    [SGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store)
            return showAccessDeniedAlert();

        SGCalendarImporterSettings *settings = [[SGCalendarImporterSettings alloc] init];
        settings.optionsMask = static_cast<SGCalendarImportOptions>(options);
        settings.date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];
        settings.calendarsIdentifiers = calendarsIdentifiers;

        SGCalendarImporter *calendarImporter = [[SGCalendarImporter alloc] initWithStrategyPtr:(void *) &strategy
                                                                                    eventStore:store
                                                                                      settings:settings];
        [calendarImporter import];
    }];
}
