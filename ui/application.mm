//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include <Foundation/Foundation.h>
#include <UserNotifications/UNUserNotificationCenter.h>

#include "application.h"

@interface CocoaDelegate : NSObject <UNUserNotificationCenterDelegate>

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler API_AVAILABLE(
        macosx(10.14));

@end

@implementation CocoaDelegate

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler {
    UNNotificationPresentationOptions options = UNNotificationPresentationOptionSound
                                                | UNNotificationPresentationOptionBadge;
    completionHandler(options);
}

@end

CocoaDelegate *Application::cocoaDelegate = nullptr;

void Application::setupCocoaDelegate() {
    cocoaDelegate = [[CocoaDelegate alloc] init];

    if (@available(macOS 10.14, *)) {
        [UNUserNotificationCenter currentNotificationCenter].delegate = cocoaDelegate;
    }
}

void Application::releaseCocoaDelegate() {
    [cocoaDelegate release];
};
