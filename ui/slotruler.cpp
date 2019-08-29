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
    font.setPixelSize(10);
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

void SlotRuler::reuseItemAtIndex(int index, ColoredLabel *itemWidget) {
    itemWidget->setText(labels()[index].label);
    itemWidget->setFixedHeight(cellHeight());
    itemWidget->setStyleSheet(makeStyleSheetForLabelIndex(index));
    itemWidget->setDynamicColor(labelColorGetterAtIndex(index));
}

SlotRuler::ColorGetter SlotRuler::labelColorGetterAtIndex(int index) {
    auto colorGetter = this->isIntegerHourAtIndex(index)
                       ? &SlotRuler::secondaryTextColor
                       : &SlotRuler::tertiaryTextColor;
    return colorGetter;
}

ColoredLabel *SlotRuler::makeNewItemAtIndex(int index) {
    auto label = new ColoredLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setFixedHeight(cellHeight());
    label->setStyleSheet(makeStyleSheetForLabelIndex(index));
    label->setDynamicColor(labelColorGetterAtIndex(index));
    label->setText(labels()[index].label);
    return label;
}

QString SlotRuler::makeStyleSheetForLabelIndex(int index) const {
    auto fontSize = isIntegerHourAtIndex(index) ? 10 : 9;

    return QString("font-size: %1px;"
                   "font-weight: bold;")
            .arg(QString::number(fontSize));
}

bool SlotRuler::isIntegerHourAtIndex(int index) const {
    auto &timeLabel = labels()[index];
    auto isIntegerHour = timeLabel.time % 60 == 0;
    return isIntegerHour;
}

