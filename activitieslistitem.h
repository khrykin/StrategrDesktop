#ifndef ACTIVITIESLISTITEM_H
#define ACTIVITIESLISTITEM_H

#include <QWidget>
#include <QLabel>

class ActivitiesListItem : public QWidget
{
    Q_OBJECT
public:
    explicit ActivitiesListItem(QString title = "", QWidget *parent = nullptr);

    QString title() const;
    void setTitle(const QString &title);

signals:
    void selected();
public slots:

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void paintEvent(QPaintEvent *) override;
    QString _title;

    QLabel *label;
    bool _isClicked = false;
};

#endif // ACTIVITIESLISTITEM_H
