#include "stacklayout.h"

StackLayout::StackLayout()
    :QLayout()
{}

StackLayout::StackLayout(QWidget *parent) :
    QLayout(parent)
{
}

StackLayout::~StackLayout(){
    QLayoutItem *item;
    foreach (item, itemList){
        delete item;
    }
}

void StackLayout::addItem ( QLayoutItem * item ){
    itemList.append(item);
}

int StackLayout::count () const{
    return itemList.count();
}

Qt::Orientations StackLayout::expandingDirections () const{
    Qt::Orientations result = 0;
    QLayoutItem *item;
    foreach (item, itemList){
        result = result | item->expandingDirections();
    }
    return result;
}

bool StackLayout::hasHeightForWidth () const{
    QLayoutItem *item;
    foreach (item, itemList){
        if (item->hasHeightForWidth())
            return true;
    }
    return false;
}

int StackLayout::heightForWidth ( int w ) const{
    int result = 0;
    QLayoutItem *item;
    foreach (item, itemList){
        if (item->hasHeightForWidth())
            result = qMax(result, item->heightForWidth(w));
    }
    return result;
}

QLayoutItem * StackLayout::itemAt ( int index ) const{
    if (index<itemList.count())
        return itemList[index];
    return 0;
}

bool StackLayout::isEmpty () const{
    QLayoutItem *item;
    foreach (item, itemList){
        if (!item->isEmpty())
            return false;
    }
    return true;
}

QSize StackLayout::maximumSize () const{
    QSize result=QLayout::maximumSize();
    QLayoutItem *item;
    foreach (item, itemList){
        result = result.boundedTo(item->maximumSize());
    }
    return result;
}

int StackLayout::minimumHeightForWidth ( int w ) const{
    int result = 0;
    QLayoutItem *item;
    foreach (item, itemList){
        if (item->hasHeightForWidth())
            result = qMax(result, item->minimumHeightForWidth(w));
    }
    return result;
}

QSize StackLayout::minimumSize () const{
    QSize result=QLayout::minimumSize();
    QLayoutItem *item;
    foreach (item, itemList){
        result = result.expandedTo(item->minimumSize());
    }
    return result;
}

void StackLayout::setGeometry ( const QRect & r ){
    QLayoutItem *item;
    foreach (item, itemList){
        item->setGeometry(r);
    }
}

QSize StackLayout::sizeHint () const{
    QSize result=QSize(0,0);
    QLayoutItem *item;
    foreach (item, itemList){
        result = result.expandedTo(item->sizeHint());
    }
    return result;
}

QLayoutItem * StackLayout::takeAt ( int index ){
    if (index < itemList.count())
        return itemList.takeAt(index);
    return 0;
}
