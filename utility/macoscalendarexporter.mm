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
                                           time_t dateSecsFromEpoch) {
    @autoreleasepool {
        EKAuthorizationStatus authorizationStatus = [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];
        EKEventStore *store = [[[EKEventStore alloc] init] autorelease];
        if (authorizationStatus != EKAuthorizationStatusAuthorized) {
            [store requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError *error) {
                if (!granted) {
                    showAccessDeniedAlert();
                    return;
                }

                exportStrategyUnauthorized(store,
                                           strategy,
                                           options,
                                           dateSecsFromEpoch);
            }];
        } else {
            exportStrategyUnauthorized(store,
                                       strategy,
                                       options,
                                       dateSecsFromEpoch);
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

void MacOSCalendarExporter::exportStrategyUnauthorized(EKEventStore *store,
                                                       const stg::strategy &strategy,
                                                       Options options,
                                                       time_t dateSecsFromEpoch) {
    SGCalendarExportProgressWindow *progressWindow = [[SGCalendarExportProgressWindow alloc] init];
    progressWindow.title = @"Exporting Strategy...";
    [progressWindow makeKeyAndOrderFront:nil];

    void (^progressBlock)(void);
    progressBlock = ^{
        // TODO refactor this to make an ObjC method -->

        NSDate *date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];
        SGCalendarManager *calendarManager = [[[SGCalendarManager alloc] initWithStore:store] autorelease];
        auto nonEmptySessions = strategy.sessions().get_non_empty();

        progressWindow.numberOfEvents = static_cast<unsigned>(nonEmptySessions.size());
        calendarManager.delegate = progressWindow;

        if (optionEnabled(options, OverridePreviousEvents)) {
            [calendarManager removeAllEventsForDate:date];
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

        // --> TODO refactor this to make an ObjC method

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

    NSColor *color = [NSColor colorWithHexColorString:
            [NSString stringWithUTF8String:strategy.activity->color().c_str()]];

    NSString *title = [NSString stringWithUTF8String:strategy.activity->name().c_str()];

    EKCalendar *calendar = [calendarManager findOrCreateCalendarWithTitle:title
                                                                 andColor:color];

    [calendarManager createEventForCalendar:calendar
                                       date:date
                               beginMinutes:strategy.begin_time()
                             duraionMinutes:strategy.duration()
                       includeNotifications:optionEnabled(options, IncludeNotifications)];
}

bool MacOSCalendarExporter::optionEnabled(Options optionsMask,
                                          Options setting) {
    return (optionsMask & setting) == setting;
}

MacOSCalendarExporter::OptionsWindowResult
MacOSCalendarExporter::showOptionsAlert(Options initialOptions) {
    @autoreleasepool {
        SGOptionsWindowViewController *optionsWindowViewController
                = [[[SGOptionsWindowViewController alloc] init] autorelease];
        SGCalendarExportOptionsView *optionsView = (SGCalendarExportOptionsView *) optionsWindowViewController.view;
        optionsView.optionsMask = initialOptions;

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
        NSModalResponse response;
        for (;;) {
            response = [app runModalSession:session];
            if (response != NSModalResponseContinue) {
                if (response == NSModalResponseOK) {
                    optionsMask = reinterpret_cast<Options>(optionsView.optionsMask);
                    date = optionsView.date;
                }

                break;
            }
        }

        [app endModalSession:session];

        return OptionsWindowResult(reinterpret_cast<Response &&>(response),
                                   reinterpret_cast<Options &&>(optionsMask),
                                   static_cast<time_t &&>(date.timeIntervalSince1970));
    }
}
