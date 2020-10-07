//
// Created by Dmitry Khrykin on 2019-08-15.
//

#import <AppKit/NSDocumentController.h>
#import <Foundation/Foundation.h>
#import <Sparkle/Sparkle.h>
#import <UserNotifications/UNUserNotificationCenter.h>

#include "application.h"

@interface CocoaDelegate : NSObject <UNUserNotificationCenterDelegate>

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler API_AVAILABLE(
                                   macosx(10.14));

+ (void)registerRecentFile:(NSString *)filePath;
+ (void)clearRecentFiles;

@end

@implementation CocoaDelegate

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler {
    UNNotificationPresentationOptions options = UNNotificationPresentationOptionSound | UNNotificationPresentationOptionBadge;
    completionHandler(options);
}

+ (void)registerRecentFile:(NSString *)filePath {
    [[NSDocumentController sharedDocumentController]
        noteNewRecentDocumentURL:[NSURL fileURLWithPath:filePath]];
}

+ (void)clearRecentFiles {
    [[NSDocumentController sharedDocumentController] clearRecentDocuments:nil];
}

@end

CocoaDelegate *Application::cocoaDelegate = nullptr;


void Application::registerCocoaRecentFile(const QString &filePath) {
    [CocoaDelegate registerRecentFile:filePath.toNSString()];
}

void Application::clearCocoaRecentFiles() {
    [CocoaDelegate clearRecentFiles];
}

void Application::setupCocoaDelegate() {
    cocoaDelegate = [[CocoaDelegate alloc] init];
    [SUUpdater.sharedUpdater retain];

#ifndef STG_UI_TEST
    if (@available(macOS 10.14, *)) {
        [UNUserNotificationCenter currentNotificationCenter].delegate = cocoaDelegate;
    }
#endif
}

void Application::releaseCocoaDelegate() {
    [cocoaDelegate release];
    [SUUpdater.sharedUpdater release];
};

void Application::checkForUpdates() {
    [SUUpdater.sharedUpdater checkForUpdates:nil];
}
