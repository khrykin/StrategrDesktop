//
// Created by Dmitry Khrykin on 2019-08-15.
//

#import <EventKit/EventKit.h>
#import <AppKit/AppKit.h>
#import <qlogging.h>

#import "cocoa/SGOptionsViewController.h"
#import "cocoa/SGCalendarExportProgressWindow.h"

#import "NSColor+HexColor.h"
#import "SGCalendarManager.h"

#include "macoscalendarexporter.h"

#include <QDebug>

void MacOSCalendarExporter::exportStrategy(const stg::strategy &strategy,
                                           Options options,
                                           time_t dateSecsFromEpoch,
                                           const std::string &calTitle) {

    // Obj-C block seem to can't capture wrapping C++ function's object parameters,
    // so we copy it here
    std::string calendarTitle = calTitle;
    @autoreleasepool {
        EKAuthorizationStatus authorizationStatus = [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];
        EKEventStore *store = [[[EKEventStore alloc] init] autorelease];
        if (authorizationStatus != EKAuthorizationStatusAuthorized) {
            [store requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError *error) {
                if (!granted) {
                    showAccessDeniedAlert();
                    return;
                }

                dispatch_async(dispatch_get_main_queue(), ^{
                    exportStrategyUnauthorized(store,
                                               strategy,
                                               options,
                                               dateSecsFromEpoch,
                                               calendarTitle);
                });
            }];
        } else {
            exportStrategyUnauthorized(store,
                                       strategy,
                                       options,
                                       dateSecsFromEpoch,
                                       calendarTitle);
        }
    }
}

void MacOSCalendarExporter::showAccessDeniedAlert() {
    dispatch_async(dispatch_get_main_queue(), ^{
        @autoreleasepool {
            NSAlert *alert = [[[NSAlert alloc] init] autorelease];
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
    });
}

void
MacOSCalendarExporter::exportStrategyUnauthorized(EKEventStore *store, const stg::strategy &strategy, Options options,
                                                  time_t dateSecsFromEpoch, const std::string &calTitle) {
    SGCalendarExportProgressWindow *progressWindow = [[SGCalendarExportProgressWindow alloc] init];
    progressWindow.title = @"Exporting Strategy...";
    [progressWindow makeKeyAndOrderFront:nil];

    // Obj-C block seem to can't capture wrapping C++ function's object parameters,
    // so we copy it here
    std::string calendarTitle = calTitle;
    void (^progressBlock)(void);
    progressBlock = ^{
        // TODO refactor this to make it an ObjC method -->

        NSDate *date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];
        SGCalendarManager *calendarManager = [[[SGCalendarManager alloc] initWithStore:store] autorelease];
        auto nonEmptySessions = strategy.sessions().get_non_empty();

        progressWindow.numberOfEvents = static_cast<unsigned>(nonEmptySessions.size());
        calendarManager.delegate = progressWindow;

        if (optionEnabled(options, OverridePreviousEvents)) {
            [calendarManager removeAllEventsForDate:date];
        }

        if (optionEnabled(options, ExportToSpecificCalendar)) {
            calendarManager.calendarName = [NSString stringWithUTF8String:calendarTitle.c_str()];
        }

        if (nonEmptySessions.empty()) {
            [progressWindow close];
        } else {
            for (auto &session : nonEmptySessions) {
                if (!session.activity) {
                    continue;
                }

                progressWindow.currentEventIndex
                        = static_cast<unsigned >(&session - &nonEmptySessions[0]);

                exportSession(session, calendarManager, options, dateSecsFromEpoch);
            }
        }

        // --> TODO refactor this to it make an ObjC method

        [SGCalendarManager launchCalendarApp];
    };

    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    dispatch_async(queue, progressBlock);
}

void
MacOSCalendarExporter::exportSession(const stg::session &strategy,
                                     SGCalendarManager *calendarManager,
                                     Options options,
                                     time_t dateSecsFromEpoch) {
    if (!strategy.activity) {
        return;
    }

    NSDate *date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];

    NSColor *color = nil;
    NSString *calendarTitle = calendarManager.calendarName;

    if (!calendarTitle) {
        calendarTitle = [NSString stringWithUTF8String:strategy.activity->name().c_str()];
        color = [NSColor colorWithHexColorString:
                [NSString stringWithUTF8String:strategy.activity->color().c_str()]];
    }

    EKCalendar *calendar = [calendarManager findOrCreateCalendarWithTitle:calendarTitle
                                                                 andColor:color];

    NSString *eventTitle = calendarManager.calendarName != nil
                           ? [NSString stringWithUTF8String:strategy.activity->name().c_str()]
                           : calendarTitle;

    [calendarManager createEventForCalendar:calendar
                                       date:date
                                      title:eventTitle
                               beginMinutes:strategy.begin_time()
                             duraionMinutes:strategy.duration()
                       includeNotifications:optionEnabled(
                               options, IncludeNotifications)];
}

bool MacOSCalendarExporter::optionEnabled(Options optionsMask,
                                          Options setting) {
    return (optionsMask & setting) == setting;
}

MacOSCalendarExporter::OptionsWindowResult
MacOSCalendarExporter::showOptionsAlert(Options initialOptions, const std::string &initialCalendarName) {
    @autoreleasepool {
        SGOptionsWindowViewController *optionsWindowViewController
                = [[[SGOptionsWindowViewController alloc] init] autorelease];
        SGCalendarExportOptionsView *optionsView = (SGCalendarExportOptionsView *) optionsWindowViewController.view;
        optionsView.optionsMask = initialOptions;
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
        std::string calendarName = "";
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
}
