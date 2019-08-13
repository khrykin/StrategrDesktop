#include "notifierbackend.h"
#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

#include <QDebug>

@interface NotificationDelegate : NSObject <NSUserNotificationCenterDelegate>
- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center
     shouldPresentNotification:(NSUserNotification *)notification;
@end

@implementation NotificationDelegate
- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center
     shouldPresentNotification:(NSUserNotification *)notification {
    return YES;
};
@end


NotifierBackend::NotifierBackend(QSystemTrayIcon *trayIcon, QObject *parent)
        : QObject(parent), trayIcon(trayIcon) {
}

void NotifierBackend::sendMessage(const QString &title, const QString &message) {
    @autoreleasepool {
        NSUserNotification *notification = [[[NSUserNotification alloc] init] autorelease];
        notification.title = title.toNSString();
        notification.informativeText = message.toNSString();
        notification.soundName = NSUserNotificationDefaultSoundName;

        NSUserNotificationCenter *nc =
                [NSUserNotificationCenter defaultUserNotificationCenter];

        if (!nc.delegate) {
            NotificationDelegate *delegate = [[[NotificationDelegate alloc] init] autorelease];
            nc.delegate = delegate;
        }

        [nc deliverNotification:notification];
    }
}

