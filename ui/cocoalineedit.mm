//
// Created by Dmitry Khrykin on 2019-08-05.
//

#import <AppKit/AppKit.h>

#include "cocoalineedit.h"

#include <QString>

NSTextField *NSTextFieldFromCococaView(NSView *view) {
    return (NSTextField *) view;
}

CocoaLineEdit::CocoaLineEdit(QWidget *parent) :
        QMacCocoaViewContainer(nil, parent) {
    @autoreleasepool {
        // Create the NSSearchField, set it on the QCocoaViewContainer.
        NSTextField *textField = [[NSTextField alloc] init];
        textField.maximumNumberOfLines = 1;
        textField.font = [NSFont boldSystemFontOfSize:18];
        textField.bordered = NO;
        textField.bezeled = NO;
        textField.drawsBackground = NO;
        textField.focusRingType = NSFocusRingTypeNone;

        setCocoaView(textField);
        [textField release];
    }
}

void CocoaLineEdit::setText(const QString &text) {
    NSTextField *textField = NSTextFieldFromCococaView(cocoaView());
    textField.stringValue = text.toNSString();
}

QString CocoaLineEdit::text() {
    NSTextField *textField = NSTextFieldFromCococaView(cocoaView());
    return QString::fromNSString(textField.stringValue);
}
