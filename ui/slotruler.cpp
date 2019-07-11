#include "slotruler.h"
#include <QDebug>
#include <QLabel>
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

    setFixedWidth(ApplicationSettings::defaultRulerWidth);

    // TODO: Extract to global stylesheet
    setStyleSheet("SlotRuler {"
                  "border-right: 1px solid #e4e4e4;"
                  "}");

    updateUI();
}

QStringList SlotRuler::labels() const { return _labels; }

void SlotRuler::setLabels(const QStringList &labels) {
    _labels = labels;
    updateUI();
}

void SlotRuler::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SlotRuler::updateUI() {
    removeExtraCells();

    for (auto cellIndex = 0; cellIndex < labels().count(); cellIndex++) {
        reuseCellAtIndex(cellIndex);
    }

    auto *mainLayout = dynamic_cast<QVBoxLayout *>(layout());
    if (mainLayout) {
        mainLayout->addStretch();
    }
}

void SlotRuler::reuseCellAtIndex(int cellIndex) const {
    QLabel *cell;
    if (layout()->itemAt(cellIndex) == nullptr) {
        cell = new QLabel();
        cell->setAlignment(Qt::AlignCenter);
        cell->setFixedHeight(cellHeight());
        // TODO extract to global stylesheet
        cell->setStyleSheet("font-size: 10pt;"
                            "font-weight: bold;"
                            "color: #777");

        layout()->addWidget(cell);
    } else {
        cell = dynamic_cast<QLabel *>(layout()->itemAt(cellIndex)->widget());
    }

    if (cell) {
        cell->setText(labels()[cellIndex]);
    }
}

void SlotRuler::removeExtraCells() const {
    auto indexOfExtraCell = 0;
    if (layout()->count() != 0) {
        while (layout()->itemAt(indexOfExtraCell) != nullptr) {
            if (layout()->itemAt(indexOfExtraCell)->widget() != nullptr) {
                layout()->itemAt(indexOfExtraCell)->widget()->hide();
            }
            layout()->removeItem(layout()->itemAt(indexOfExtraCell));
        }
    }
}

int SlotRuler::cellHeight() const { return _cellHeight; }

void SlotRuler::setCellHeight(int cellHeight) {
    _cellHeight = cellHeight;
    updateUI();
}
