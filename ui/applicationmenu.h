//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef STRATEGR_APPLICATIONMENU_H
#define STRATEGR_APPLICATIONMENU_H

#include <QMenuBar>
#include <QMenu>

class MainWindow;

class ApplicationMenu : public QMenuBar {
Q_OBJECT
public:
    explicit ApplicationMenu(MainWindow *window);;

    void updateRecentFilesActions();
private:
    MainWindow *window;

    QMenu *fileMenu = new QMenu(tr("File"));
    QMenu *editMenu = new QMenu(tr("Edit"));
    QMenu *viewMenu = new QMenu(tr("View"));

    QMenu *recentMenu = new QMenu(tr("Open Recent"));

    QVector<QAction *> recentFileActions;

    void setupRecentMenu();
    void setupFileMenu();
    void setupViewMenu();
    void setupEditMenu();

    void createRecentFilesActions();
};


#endif //STRATEGR_APPLICATIONMENU_H
