#ifndef STACKLAYOUT_H
#define STACKLAYOUT_H

#include <QLayout>

class StackLayout : public QLayout
{
    Q_OBJECT
public:
    StackLayout();
    explicit StackLayout(QWidget *parent);
    ~StackLayout();

    void addItem ( QLayoutItem * item );
    int count () const;
    Qt::Orientations expandingDirections () const;
    bool hasHeightForWidth () const;
    int heightForWidth ( int w ) const;
    QLayoutItem * itemAt ( int index ) const;
    bool isEmpty () const;
    QSize maximumSize () const;
    int minimumHeightForWidth ( int w ) const;
    QSize minimumSize () const;
    void setGeometry ( const QRect & r );
    QSize sizeHint () const;
    QLayoutItem * takeAt ( int index );

private:
     QList<QLayoutItem *> itemList;

};

#endif // STACKLAYOUT_H
