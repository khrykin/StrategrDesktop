//
// Created by Dmitry Khrykin on 2019-08-15.
//

#import <Foundation/Foundation.h>
#import <UserNotifications/UNUserNotificationCenter.h>
#import <AppKit/NSDocumentController.h>

#include "application.h"

@interface CocoaDelegate : NSObject <UNUserNotificationCenterDelegate>

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler API_AVAILABLE(
        macosx(10.14));

+ (void)registerRecentFile:(NSString *)filePath;

@end

@implementation CocoaDelegate

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler {
    UNNotificationPresentationOptions options = UNNotificationPresentationOptionSound
                                                | UNNotificationPresentationOptionBadge;
    completionHandler(options);
}

+ (void)registerRecentFile:(NSString *)filePath {
    [[NSDocumentController sharedDocumentController]
            noteNewRecentDocumentURL:[NSURL fileURLWithPath:filePath]];
}

@end

CocoaDelegate *Application::cocoaDelegate = nullptr;

void Application::registerCocoaRecentFile(const QString &filePath) {
    [CocoaDelegate registerRecentFile:filePath.toNSString()];
}

void Application::setupCocoaDelegate() {
    cocoaDelegate = [[CocoaDelegate alloc] init];

    if (@available(macOS 10.14, *)) {
        [UNUserNotificationCenter currentNotificationCenter].delegate = cocoaDelegate;
    }
}

void Application::releaseCocoaDelegate() {
    [cocoaDelegate release];
};
