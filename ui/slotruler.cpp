#include "slotruler.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
#include "applicationsettings.h"

SlotRuler::SlotRuler(QStringList labels, int cellHeight, QWidget *parent)
        : _labels(std::move(labels)), _cellHeight(cellHeight), QWidget(parent) {
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, cellHeight / 2, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    // TODO: Adjust to label size
    setFixedWidth(ApplicationSettings::defaultRulerWidth);

    // TODO: Extract to global stylesheet
    setStyleSheet("SlotRuler {"
                  "border-right: 1px solid #e4e4e4;"
                  "}");

    updateList();
}

QStringList SlotRuler::labels() const { return _labels; }

void SlotRuler::setLabels(const QStringList &labels) {
    _labels = labels;
    updateList();
}

void SlotRuler::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

int SlotRuler::cellHeight() const { return _cellHeight; }

void SlotRuler::setCellHeight(int cellHeight) {
    _cellHeight = cellHeight;
    updateList();
}

int SlotRuler::numberOfItems() {
    return labels().count();
}

QVBoxLayout *SlotRuler::listLayout() {
    return dynamic_cast<QVBoxLayout *>(layout());
}

void SlotRuler::reuseItemAtIndex(int index, QLabel *itemWidget) {
    itemWidget->setText(labels()[index]);
    itemWidget->setFixedHeight(cellHeight());
}

QLabel *SlotRuler::makeNewItemAtIndex(int index) {
    auto cell = new QLabel();
    cell->setAlignment(Qt::AlignCenter);
    cell->setFixedHeight(cellHeight());
    
    // TODO extract to global stylesheet
    cell->setStyleSheet("font-size: 10pt;"
                        "font-weight: bold;"
                        "color: #777");
    return cell;
}
