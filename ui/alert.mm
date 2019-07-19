#include "alert.h"
#include <QMessageBox>
#include <QAbstractButton>
#include <AppKit/NSAlert.h>

int Alert::showAskToSave(const QString &title, const QString &message) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    QMessageBox messageBox;
    messageBox.setStandardButtons(QMessageBox::Save
                                  | QMessageBox::Discard
                                  | QMessageBox::Cancel);

    NSAlert *alert = [[[NSAlert alloc] init] autorelease];
    [alert addButtonWithTitle:
            messageBox.button(QMessageBox::Save)->text()
                    .toNSString()];
    [alert addButtonWithTitle:
            messageBox.button(QMessageBox::Cancel)->text()
                    .toNSString()];
    [alert addButtonWithTitle:
            messageBox.button(QMessageBox::Discard)->text()
                    .toNSString()];

    [alert setMessageText:title.toNSString()];
    [alert setInformativeText:message.toNSString()];
    [alert setAlertStyle:NSAlertStyleWarning];

    NSModalResponse result = [alert runModal];

    [pool drain];

    switch (result) {
        case NSAlertFirstButtonReturn:
            return QMessageBox::Save;
        case NSAlertSecondButtonReturn:
            return QMessageBox::Cancel;
        case NSAlertThirdButtonReturn:
            return QMessageBox::Discard;
        default:
            return QMessageBox::Save;
    }
}

int Alert::showWarning(QWidget *parent,
                       const QString &title,
                       const QString &message) {
    QMessageBox::warning(parent, title, message);
}
