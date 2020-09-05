//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import "STGCalendarImportViewController.h"
#import "STGCalendarImportView.h"

@implementation STGCalendarImportViewController

- (void)optionsViewWantsPerform {
    STGCalendarImportView *importView = (STGCalendarImportView *) self.view;
    if (importView.viewModel.isNoCalendarSelected) {
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = @"Can't import from Calendar";
        alert.informativeText = @"Please, selelect at least one calendar from the list";

        [alert beginSheetModalForWindow:self.view.window completionHandler:nil];

        return;
    }

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
