#include <QSettings>

#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

#include "notifierbackend.h"

void NotifierBackend::sendMessage(const QString &title, const QString &message) {
    @autoreleasepool {
        if (@available(macOS 10.14, *)) {
            UNMutableNotificationContent *content = [[[UNMutableNotificationContent alloc] init] autorelease];
            content.title = title.toNSString();
            content.body = message.toNSString();
            content.sound = [UNNotificationSound defaultSound];

            UNTimeIntervalNotificationTrigger *trigger
                    = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:0.1
                                                                         repeats:NO];

            UNNotificationRequest *request
                    = [UNNotificationRequest requestWithIdentifier:[NSUUID UUID].UUIDString
                                                           content:content
                                                           trigger:trigger];

            [[UNUserNotificationCenter currentNotificationCenter] addNotificationRequest:request
                                                                   withCompletionHandler:nil];
        } else {
            NSUserNotification *notification = [[[NSUserNotification alloc] init] autorelease];
            notification.title = title.toNSString();
            notification.informativeText = message.toNSString();
            notification.soundName = NSUserNotificationDefaultSoundName;

            NSUserNotificationCenter *userNotificationCenter =
                    [NSUserNotificationCenter defaultUserNotificationCenter];

            [userNotificationCenter deliverNotification:notification];
        }
    }
}
