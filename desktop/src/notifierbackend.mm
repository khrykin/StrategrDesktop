#if !defined(STG_UI_TEST)

#include <QSettings>

#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

#include "notifierbackend.h"


NotifierBackend::NotifierBackend() {
    @autoreleasepool {
        if (@available(macOS 10.14, *)) {
            UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
            UNAuthorizationOptions options = UNAuthorizationOptionAlert |
                                             UNAuthorizationOptionBadge |
                                             UNAuthorizationOptionSound;

            [center requestAuthorizationWithOptions:options completionHandler:^(BOOL granted, NSError *error) {
                if (granted) {
                    NSLog(@"Notifications granted");
                }

                if (error) {
                    NSLog(@"Can't authorize notifications: %@\n%@",
                          error.localizedDescription,
                          error.localizedFailureReason);
                }
            }];
        }
    }
}

void NotifierBackend::sendMessage(const QString &title, const QString &message) const {
    if (@available(macOS 10.14, *)) {
        UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
        content.title = title.toNSString();
        content.body = message.toNSString();
        content.sound = [UNNotificationSound defaultSound];

        UNNotificationRequest *request
                = [UNNotificationRequest requestWithIdentifier:[NSUUID UUID].UUIDString
                                                       content:content
                                                       trigger:nil];

        UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
        [center addNotificationRequest:request withCompletionHandler:^(NSError *__nullable error) {
            if (error) {
                NSLog(@"Can't send notification: %@",
                      error.localizedDescription);
            }
        }];
    } else {
        NSUserNotification *notification = [[NSUserNotification alloc] init];
        notification.title = title.toNSString();
        notification.informativeText = message.toNSString();
        notification.soundName = NSUserNotificationDefaultSoundName;

        NSUserNotificationCenter *userNotificationCenter =
                [NSUserNotificationCenter defaultUserNotificationCenter];

        [userNotificationCenter deliverNotification:notification];
    }
}

#endif