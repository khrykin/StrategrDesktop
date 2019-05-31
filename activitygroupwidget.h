#ifndef SLOT_H
#define SLOT_H

#include <QWidget>
#include <QLabel>
#include <QMap>

class ActivityGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActivityGroupWidget(QWidget *parent = nullptr);
    int number() const;
    void setNumber(int number);

    QString title() const;
    void setTitle(QString title);

    int length() const;
    void setLength(unsigned int length);

    bool isSelected() const;
    void setIsSelected(bool isSelected);

    void setSlotHeight(int height);

    void selectSlotAtIndex(int slotIndex);
    void deselectSlotAtIndex(int slotIndex);
    void deselectAllSlots();

    bool hasSelection();

    QMap<int, QWidget *> selectionSlots() const;

    int slotDuration() const;
    void setSlotDuration(int slotDuration);

private:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;

    void updateStyleSheet();
    int _number;
    int _length = 1;
    int _slotDuration;

    bool _isSelected = false;

    QString _title;

    QLabel *label;
    QLabel *titleLabel;
    QWidget *selectionWidget;
    QMap<int, QWidget *> _selectionSlots;

    QWidget *mainWidget;
};

#endif // SLOT_H
