#ifndef GROUPSLIST_H
#define GROUPSLIST_H

#include <QAction>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "applicationsettings.h"
#include "colorprovider.h"
#include "cursorprovider.h"
#include "dataproviderwidget.h"
#include "reactivelist.hpp"
#include "selection.h"
#include "selectionwidget.h"
#include "sessionwidget.h"
#include "slotruler.h"

class SlotsWidget : public DataProviderWidget,
                    public ReactiveList<SessionWidget>,
                    private CursorProvider,
                    private ColorProvider {
    Q_OBJECT
public:
    explicit SlotsWidget(QWidget *parent = nullptr);

    QAction *setActivityAction = nullptr;
    QAction *deleteActivityAction = nullptr;
    QAction *clearSelectionAction = nullptr;
    QAction *shiftSlotsBelowAction = nullptr;
    QAction *selectAllAction = nullptr;

    void reloadStrategy();

private:
    QVBoxLayout *slotsLayout = nullptr;
    QWidget *slotsWidget = nullptr;
    SelectionWidget *selectionWidget = nullptr;

    void setupActions();
    void layoutChildWidgets();

    int topLineThickness();
    void updateSlotsLayoutContentMargins();

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, SessionWidget *itemWidget) override;
    SessionWidget *makeNewItemAtIndex(int index) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void enterEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif// GROUPSLIST_H
