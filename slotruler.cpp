#include "slotruler.h"
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

SlotRuler::SlotRuler(int cellHeight, QWidget *parent)
    : QWidget(parent), _cellHeight(cellHeight) {
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, cellHeight / 2, 0, 0);
  layout->setSpacing(0);

  setFixedWidth(defaultRulerWidth);

  setStyleSheet("SlotRuler {"
                "border-right: 1px solid #aaa;"
                "}");

  setLayout(layout);
  updateUI();
}

QVector<QString> SlotRuler::labels() const { return _labels; }

void SlotRuler::setLabels(QVector<QString> labels) {
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
  auto indexOfExtraCell = 0; // labels().count();
  if (!!layout()->count()) {
    while (layout()->itemAt(indexOfExtraCell) != nullptr) {
      if (layout()->itemAt(indexOfExtraCell)->widget() != nullptr) {
        layout()->itemAt(indexOfExtraCell)->widget()->hide();
      }
      layout()->removeItem(layout()->itemAt(indexOfExtraCell));
    }
  }

  for (int i = 0; i < labels().count(); i++) {
    QLabel *cell;
    if (layout()->itemAt(i) == nullptr) {
      cell = new QLabel();
      cell->setAlignment(Qt::AlignCenter);
      cell->setFixedHeight(cellHeight());
      cell->setStyleSheet("font-size: 10pt;"
                          "font-weight: bold;"
                          "color: #777");
      layout()->addWidget(cell);
    } else {
      cell = dynamic_cast<QLabel *>(layout()->itemAt(i)->widget());
    }

    if (cell != nullptr) {
      cell->setText(labels()[i]);
    }
  }

  auto *mainLayout = static_cast<QVBoxLayout *>(layout());
  mainLayout->addStretch();
}

int SlotRuler::cellHeight() const { return _cellHeight; }

void SlotRuler::setCellHeight(int cellHeight) {
  _cellHeight = cellHeight;
  updateUI();
}
