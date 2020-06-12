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

void MacOSCalendarExporter::exportStrategy(const stg::strategy &strategy) {
    SGCalendarExportView *optionsView = [[SGCalendarExportView alloc] init];

    SGCalendarExportViewController *optionsWindowViewController = [[SGCalendarExportViewController alloc] init];
    optionsWindowViewController.view = optionsView;

    NSPanel *window = [NSPanel windowWithContentViewController:optionsWindowViewController];
    window.delegate = optionsWindowViewController;
    window.title = @"Export Strategy to Calendar";
    window.styleMask |= NSWindowStyleMaskUtilityWindow;
    window.styleMask &= ~(NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);

    NSApplication *app = [NSApplication sharedApplication];
    NSModalSession session = [app beginModalSessionForWindow:window];

    NSModalResponse response = NSModalResponseCancel;
    for (;;) {
        response = [app runModalSession:session];
        if (response != NSModalResponseContinue) {
            break;
        }
    }

    [app endModalSession:session];

    if (response == NSModalResponseOK) {
        [SGCalendarExporter saveDefaultSettings:optionsView.viewModel.settings];

        SGCalendarExportProgressWindow *progressWindow = [[SGCalendarExportProgressWindow alloc] init];
        progressWindow.title = @"Exporting Strategy...";

        // window will be released automatically by ARC
        progressWindow.releasedWhenClosed = NO;
        [progressWindow makeKeyAndOrderFront:nil];

        [SGCalendarExporter exportFromStrategyPtr:(void *) &strategy
                                             date:optionsView.viewModel.settings.date
                                         delegate:progressWindow
                                completionHandler:^(BOOL accessGranted) {
                                    if (!accessGranted) {
                                        showAccessDeniedAlert();
                                        return;
                                    }
                                }];
    }
}

void MacOSCalendarExporter::importStrategy(stg::strategy &strategy) {
    SGCalendarImportView *optionsView = [[SGCalendarImportView alloc] init];

    SGCalendarImportViewController *optionsWindowViewController = [[SGCalendarImportViewController alloc] init];
    optionsWindowViewController.view = optionsView;

    NSPanel *window = [NSPanel windowWithContentViewController:optionsWindowViewController];
    window.delegate = optionsWindowViewController;
    window.title = @"Import Strategy from Calendar";
    window.styleMask |= NSWindowStyleMaskUtilityWindow;
    window.styleMask &= ~(NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);

    NSApplication *app = [NSApplication sharedApplication];
    NSModalSession session = [app beginModalSessionForWindow:window];

    [SGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store) {
            return showAccessDeniedAlert();
        }

        NSArray *calendars = [store calendarsForEntityType:EKEntityTypeEvent];
        optionsView.viewModel.calendars = calendars;
    }];

    NSModalResponse response = NSModalResponseCancel;

    for (;;) {
        response = [app runModalSession:session];
        if (response != NSModalResponseContinue) {
            break;
        }
    }

    [app endModalSession:session];

    if (response == NSModalResponseOK) {
        auto *strategyPtr = (void *) &strategy;
        [SGCalendarImporter saveDefaultSettings:optionsView.viewModel.settings];
        [SGCalendarImporter importToStrategyPtr:strategyPtr
                                           date:optionsView.viewModel.settings.date
                              completionHandler:nil];
    }
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