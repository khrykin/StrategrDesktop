//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import "SGCalendarImportViewController.h"

@implementation SGCalendarImportViewController

- (void)optionsViewWantsPerform {
    [[NSApplication sharedApplication] stopModalWithCode:NSModalResponseOK];
    [self.view.window close];
}

- (void)optionsViewWantsCancel {
    [[NSApplication sharedApplication] stopModalWithCode:NSModalResponseCancel];
    [self.view.window close];
}

@end
