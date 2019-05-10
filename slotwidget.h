#ifndef SLOT_H
#define SLOT_H

#include <QWidget>
#include <QLabel>

class SlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SlotWidget(QWidget *parent = nullptr);
    int number() const;
    void setNumber(int number);

    QString title() const;
    void setTitle(QString title);

    int length() const;
    void setLength(int length);

    void grow();
    void shrink();

signals:

public slots:
private:
    void paintEvent(QPaintEvent *) override;
    int _number;
    int _length = 1;

    QString _title;

    QLabel *label;
    QLabel *titleLabel;
};

#endif // SLOT_H
