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
    void setLength(int length);

    void grow();
    void shrink();

    bool isSelected() const;
    void setIsSelected(bool isSelected);

    void setSlotHeight(int height);

    void selectSlotAtIndex(int slotIndex);
    void deselectSlotAtIndex(int slotIndex);
    void deselectAllSlots();

    bool hasSelection();

private:
    void paintEvent(QPaintEvent *) override;
    void updateStyleSheet();
    int _number;
    int _length = 1;

    bool _isSelected = false;

    QString _title;

    QLabel *label;
    QLabel *titleLabel;
    QWidget *selectionWidget;
    QMap<int, QWidget *> selectionSlots;
};

#endif // SLOT_H
