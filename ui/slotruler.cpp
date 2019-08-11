#include "slotruler.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
#include "applicationsettings.h"
#include "utils.h"

SlotRuler::SlotRuler(QVector<TimeLabel> labels,
                     int cellHeight,
                     QWidget *parent)
        : _labels(std::move(labels)),
          _cellHeight(cellHeight),
          QWidget(parent) {
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(ApplicationSettings::defaultPadding, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    setFixedWidth(calculateLabelWidth() + ApplicationSettings::defaultPadding);

    updateList();
}

int SlotRuler::calculateLabelWidth() const {
    auto font = QFont();
    font.setPointSize(10);
    font.setBold(true);

    return QFontMetrics(font)
            .horizontalAdvance(QStringForMinutes(0));
}

QVector<TimeLabel> SlotRuler::labels() const {
    return _labels;
}

void SlotRuler::setLabels(const QVector<TimeLabel> &labels) {
    if (_labels != labels) {
        _labels = labels;
        updateList();
    }
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
    itemWidget->setText(labels()[index].label);
    itemWidget->setFixedHeight(cellHeight());
    itemWidget->setStyleSheet(makeStyleSheetForLabelIndex(index));
}

QLabel *SlotRuler::makeNewItemAtIndex(int index) {
    auto label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setFixedHeight(cellHeight());
    label->setStyleSheet(makeStyleSheetForLabelIndex(index));
    label->setText(labels()[index].label);
    return label;
}

QString SlotRuler::makeStyleSheetForLabelIndex(int index) const {
    auto &timeLabel = labels()[index];

    auto isIntegerHour = timeLabel.time % 60 == 0;
    auto color = isIntegerHour ? "#777" : "#aaa";
    auto fontSize = isIntegerHour ? 10 : 9;

    return QString("font-size: %1pt;"
                   "font-weight: bold;"
                   "color: %2")
            .arg(QString::number(fontSize))
            .arg(color);
}

