//
// Created by Dmitry Khrykin on 2019-08-15.
//

#import <EventKit/EventKit.h>
#import <AppKit/AppKit.h>

#import "cocoa/STGCalendarExportViewController.h"
#import "cocoa/STGCalendarImportViewController.h"
#import "cocoa/STGCalendarImportView.h"
#import "cocoa/STGCalendarExportView.h"
#import "cocoa/STGCalendarExportProgressWindow.h"

#import "STGCalendarExporter.h"
#import "STGCalendarImporter.h"

#include "macoscalendarexporter.h"

void MacOSCalendarExporter::exportStrategy(const stg::strategy &strategy) {
    STGCalendarExportView *optionsView = [[STGCalendarExportView alloc] init];

    STGCalendarExportViewController *optionsWindowViewController = [[STGCalendarExportViewController alloc] init];
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
        [STGCalendarExporter saveDefaultSettings:optionsView.viewModel.settings];

        STGCalendarExportProgressWindow *progressWindow = [[STGCalendarExportProgressWindow alloc] init];
        progressWindow.title = @"Exporting Strategy...";

        // window will be released automatically by ARC
        progressWindow.releasedWhenClosed = NO;
        [progressWindow makeKeyAndOrderFront:nil];

        [STGCalendarExporter exportFromStrategyPtr:&strategy
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
    STGCalendarImportView *optionsView = [[STGCalendarImportView alloc] init];

    STGCalendarImportViewController *optionsWindowViewController = [[STGCalendarImportViewController alloc] init];
    optionsWindowViewController.view = optionsView;

    NSPanel *window = [NSPanel windowWithContentViewController:optionsWindowViewController];
    window.delegate = optionsWindowViewController;
    window.title = @"Import Strategy from Calendar";
    window.styleMask |= NSWindowStyleMaskUtilityWindow;
    window.styleMask &= ~(NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);

    NSApplication *app = [NSApplication sharedApplication];
    NSModalSession session = [app beginModalSessionForWindow:window];

    [STGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
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
        [STGCalendarImporter saveDefaultSettings:optionsView.viewModel.settings];
        [STGCalendarImporter importToStrategyPtr:&strategy
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