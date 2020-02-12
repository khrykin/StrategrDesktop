//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "SGOptionsViewController.h"

@implementation SGOptionsWindowViewController
- (void)loadView {
    NSRect initialFrame = NSMakeRect(0, 0, 400, 200);

    SGCalendarExportOptionsView *view = [[SGCalendarExportOptionsView alloc] initWithFrame:initialFrame];
    view.delegate = self;

    [self setView:view];
}

- (void)optionsViewWantsExport {
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

- (SGCalendarExportOptionsView *)view {
    return (SGCalendarExportOptionsView *)
            [super view];
}

@end
