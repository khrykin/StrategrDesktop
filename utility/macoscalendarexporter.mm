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

void MacOSCalendarExporter::exportStrategy(const Strategy &strategy,
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
                                                       const Strategy &strategy,
                                                       Options options,
                                                       time_t dateSecsFromEpoch) {
    SGCalendarExportProgressWindow *progressWindow = [[SGCalendarExportProgressWindow alloc] init];
    progressWindow.title = @"Exporting Strategy...";
    [progressWindow makeKeyAndOrderFront:nil];

    void (^progressBlock)(void);
    progressBlock = ^{
        NSDate *date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];
        SGCalendarManager *calendarManager = [[[SGCalendarManager alloc] initWithStore:store] autorelease];
        progressWindow.numberOfEvents = static_cast<unsigned>(strategy.activitySessions().size());
        calendarManager.delegate = progressWindow;

        if (optionEnabled(options, OverridePreviousEvents)) {
            [calendarManager removeAllEventsForDate:date];
        }

        for (auto &session : strategy.activitySessions()) {
            if (!session.activity) {
                continue;
            }

            progressWindow.currentEventIndex
                    = static_cast<unsigned >(&session -
                                             &strategy.activitySessions()[0]);
            exportSession(session, calendarManager, options, dateSecsFromEpoch);

        }

        [SGCalendarManager launchCalendarApp];
    };

    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    dispatch_async(queue, progressBlock);
}

void
MacOSCalendarExporter::exportSession(const ActivitySession &strategy,
                                     SGCalendarManager *calendarManager,
                                     Options options,
                                     time_t dateSecsFromEpoch) {
    if (!strategy.activity) {
        return;
    }

    NSDate *date = [NSDate dateWithTimeIntervalSince1970:dateSecsFromEpoch];

    NSColor *color = [NSColor colorWithHexColorString:
            [NSString stringWithCString:strategy.activity->color().c_str()
                               encoding:[NSString defaultCStringEncoding]]];

    NSString *title = [NSString stringWithCString:strategy.activity->name().c_str()
                                         encoding:[NSString defaultCStringEncoding]];

    EKCalendar *calendar = [calendarManager findOrCreateCalendarWithTitle:title
                                                                 andColor:color];

    [calendarManager createEventForCalendar:calendar
                                       date:date
                               beginMinutes:strategy.beginTime()
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
