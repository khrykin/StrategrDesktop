//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "STGCalendarExportViewController.h"

@implementation STGCalendarExportViewController

- (void)optionsViewWantsPerform {
    [[NSApplication sharedApplication] stopModalWithCode:NSModalResponseOK];
    [self.view.window close];
}

- (void)optionsViewWantsCancel {
    [[NSApplication sharedApplication] stopModalWithCode:NSModalResponseCancel];
    [self.view.window close];
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    if ([sender isEqual:self.view.window]) {
        [[NSApplication sharedApplication] stopModal];
    }

    return YES;
}

@end
