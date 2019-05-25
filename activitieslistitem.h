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
    void wantToDelete();
    void wantToEdit();

public slots:

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *) override;

    void wantDeleteActivity();
    void wantEditActivity();

    QString _title;

    QAction *editActivityAction;
    QAction *deleteActivityAction;

    QLabel *label;
    bool _isClicked = false;
};

#endif // ACTIVITIESLISTITEM_H
