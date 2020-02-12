//
// Created by Dmitry Khrykin on 2019-08-15.
//

#import <EventKit/EventKit.h>
#import <AppKit/AppKit.h>

#import "cocoa/SGOptionsViewController.h"
#import "cocoa/SGCalendarExportProgressWindow.h"
#import "SGCalendarExporter.h"

#include "macoscalendarexporter.h"

void MacOSCalendarExporter::exportStrategy(const stg::strategy &strategy,
                                           Options options,
                                           time_t dateSecsFromEpoch,
                                           const std::string &calTitle) {
    // Obj-C block can't capture a C++ function parameter so we copy it here
    NSString *calendarTitle = [NSString stringWithCString:calTitle.c_str()
                                                 encoding:NSUTF8StringEncoding];
    [SGCalendarExporter requestExport:^(EKEventStore *store) {
        if (!store) {
            return showAccessDeniedAlert();
        }

        SGCalendarExportProgressWindow *progressWindow = [[SGCalendarExportProgressWindow alloc] init];
        progressWindow.title = @"Exporting Strategy...";

        // window will be released automatically by ARC
        progressWindow.releasedWhenClosed = NO;
        [progressWindow makeKeyAndOrderFront:nil];

        auto strategyPtr = (void *) &strategy;

        SGCalendarExporterSettings *settings = [[SGCalendarExporterSettings alloc] init];
        settings.optionsMask = static_cast<SGCalendarExportOptions>(options);
        settings.date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];
        settings.calendarName = calendarTitle;

        SGCalendarExporter *exporter = [[SGCalendarExporter alloc] initWithStrategyPtr:strategyPtr
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

MacOSCalendarExporter::OptionsWindowResult
MacOSCalendarExporter::showOptionsAlert(Options initialOptions, const std::string &initialCalendarName) {
    SGOptionsWindowViewController *optionsWindowViewController
            = [[SGOptionsWindowViewController alloc] init];
    SGCalendarExportOptionsView *optionsView = (SGCalendarExportOptionsView *) optionsWindowViewController.view;
    optionsView.optionsMask = static_cast<SGCalendarExportOptions>(initialOptions);

    if (!initialCalendarName.empty())
        optionsView.calendarName = [NSString stringWithCString:initialCalendarName.c_str()
                                                      encoding:NSUTF8StringEncoding];

    NSPanel *window = [NSPanel windowWithContentViewController:optionsWindowViewController];

    window.delegate = optionsWindowViewController;
    window.title = @"Export Strategy to Calendar";
    window.styleMask |= NSWindowStyleMaskUtilityWindow;
    window.styleMask = window.styleMask & ~(NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);

    [window setContentSize:window.contentView.frame.size];

    NSApplication *app = [NSApplication sharedApplication];
    NSModalSession session = [app beginModalSessionForWindow:window];

    Options optionsMask = 0;
    NSDate *date = [NSDate date];
    std::string calendarName;
    NSModalResponse response;
    for (;;) {
        response = [app runModalSession:session];
        if (response != NSModalResponseContinue) {
            if (response == NSModalResponseOK) {
                optionsMask = reinterpret_cast<Options>(optionsView.optionsMask);
                date = optionsView.date;
                if (optionsView.calendarName)
                    calendarName = [optionsView.calendarName cStringUsingEncoding:NSUTF8StringEncoding];
            }

            break;
        }
    }

    [app endModalSession:session];

    return OptionsWindowResult(reinterpret_cast<Response &&>(response),
                               reinterpret_cast<Options &&>(optionsMask),
                               static_cast<time_t &&>(date.timeIntervalSince1970),
                               calendarName);
}
