#include "notifierbackend.h"

#ifdef Q_OS_MAC
#import <Foundation/Foundation.h>

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
#endif

NotifierBackend::NotifierBackend(QSystemTrayIcon *trayIcon, QObject *parent)
    : QObject(parent), trayIcon(trayIcon) {}

#ifdef Q_OS_MAC
void NotifierBackend::sendMessage(QString title, QString message) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  NSUserNotification *notification = [[NSUserNotification alloc] init];
  notification.title = title.toNSString();
  notification.informativeText = message.toNSString();
  notification.soundName = NSUserNotificationDefaultSoundName;
  NSUserNotificationCenter *nc =
      [NSUserNotificationCenter defaultUserNotificationCenter];
  NotificationDelegate *delegate = [[NotificationDelegate alloc] init];
  nc.delegate = delegate;
  [nc deliverNotification:notification];
  [notification autorelease];
  [pool release];
}
#else
void NotifierBackend::sendMessage(QString title, QString message) {
  trayIcon->showMessage(title, message, QIcon(), 10000);
}
#endif
