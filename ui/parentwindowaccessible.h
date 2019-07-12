//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_PARENTWINDOWACCESSIBLE_H
#define STRATEGR_PARENTWINDOWACCESSIBLE_H

#include <QObject>

class MainWindow;
template<class Widget>
class ParentWindowAccessible {
protected:
    MainWindow *parentWindow() {
        auto *instance = static_cast<Widget *>(this);
        auto parentWidget = instance->parentWidget();
        while (parentWidget) {
            auto mainWindow = qobject_cast<MainWindow *>(parentWidget);

            if (mainWindow) {
                return mainWindow;
            }

            parentWidget = parentWidget->parentWidget();
        }

        return nullptr;
    }
};


#endif //STRATEGR_PARENTWINDOWACCESSIBLE_H
