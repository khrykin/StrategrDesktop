#include "alert.h"
#include <QAbstractButton>
#include <QMessageBox>
#include <QTimer>

#import <AppKit/NSAlert.h>
#import <AppKit/NSImage.h>

int Alert::showAskToSave(const QString &title, const QString &message) {
    NSModalResponse result;

    @autoreleasepool {
        QMessageBox messageBox;
        messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:
                   messageBox.button(QMessageBox::Save)->text().toNSString()];
        [alert addButtonWithTitle:
                   messageBox.button(QMessageBox::Cancel)->text().toNSString()];
        [alert addButtonWithTitle:
                   messageBox.button(QMessageBox::Discard)->text().toNSString()];

        [alert setMessageText:title.toNSString()];
        [alert setInformativeText:message.toNSString()];
        [alert setAlertStyle:NSAlertStyleWarning];

        result = [alert runModal];
    }

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

int Alert::showWarning(QWidget *,
                       const QString &title,
                       const QString &message) {
    NSString *nsTitle = title.toNSString();
    NSString *nsMessage = message.toNSString();

    dispatch_async(dispatch_get_main_queue(), ^{
      @autoreleasepool {
          NSAlert *alert = [[NSAlert alloc] init];
          alert.alertStyle = NSAlertStyleCritical;
          alert.icon = [NSImage imageNamed:NSImageNameCaution];

          [alert setMessageText:nsTitle];
          [alert setInformativeText:nsMessage];
          [alert setAlertStyle:NSAlertStyleWarning];

          [alert runModal];
      }
    });

    return QMessageBox::Ok;
}
