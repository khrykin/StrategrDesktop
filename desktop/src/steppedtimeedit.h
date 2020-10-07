#ifndef STEPPEDTIMEEDIT_H
#define STEPPEDTIMEEDIT_H

#include <QTimeEdit>

class SteppedTimeEdit : public QTimeEdit {
    using QTimeEdit::QTimeEdit;

    void stepBy(int steps) override {
        if (currentSection() == MinuteSection) {
            setTime(time().addMSecs(steps * minuteStepSize * 60 * 1000));
            return;
        }

        QTimeEdit::stepBy(steps);
    }

public:
    int minuteStepSize = 1;
};

#endif// STEPPEDTIMEEDIT_H
