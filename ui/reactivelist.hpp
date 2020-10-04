//
// Created by Dmitry Khrykin on 2019-07-12.
//

#ifndef STRATEGR_REACTIVELIST_HPP
#define STRATEGR_REACTIVELIST_HPP

#include <QVBoxLayout>

template<class ItemWidget>
class ReactiveList {
public:
    virtual ~ReactiveList() = default;

protected:
    virtual int numberOfItems() = 0;
    virtual QVBoxLayout *listLayout() = 0;
    virtual void reuseItemAtIndex(int index, ItemWidget *itemWidget) = 0;
    virtual ItemWidget *makeNewItemAtIndex(int index) = 0;

    virtual void updateList() {
        if (!lastLayoutItem() || !lastLayoutItem()->spacerItem()) {
            listLayout()->addStretch();
        }

        for (int index = 0; index < numberOfItems(); index++) {
            renderItemAtIndex(index);
        }

        removeExtraRows();
    }

protected:
    ItemWidget *listItemWidgetAtIndex(int index) {
        return qobject_cast<ItemWidget *>(listLayout()->itemAt(index)->widget());
    }

private:
    void renderItemAtIndex(int index) {
        auto *currentItem = listLayout()->itemAt(index);
        ItemWidget *itemWidget = nullptr;

        if (currentItem) {
            if (currentItem->spacerItem()) {
                itemWidget = makeNewItemAtIndex(index);
                listLayout()->insertWidget(index, itemWidget);

                auto spacer = listLayout()->takeAt(index + 1);
                listLayout()->addItem(spacer);
            } else {
                itemWidget = dynamic_cast<ItemWidget *>(currentItem->widget());
                reuseItemAtIndex(index, itemWidget);
            }
        }

        if (!itemWidget) {
            itemWidget = makeNewItemAtIndex(index);
            listLayout()->insertWidget(index, itemWidget);
        }

        if (!itemWidget->isVisible()) {
            itemWidget->show();
        }
    }

    void removeExtraRows() {
        for (auto hideAtIndex = numberOfItems();
             hideAtIndex < listLayout()->count();
             hideAtIndex++) {
            auto itemToHide = listLayout()->itemAt(hideAtIndex);
            if (itemToHide->widget() && !itemToHide->widget()->isHidden()) {
                itemToHide->widget()->hide();
            }
        }
    };

    QLayoutItem *lastLayoutItem() {
        return listLayout()->itemAt(listLayout()->count() - 1);
    }
};


#endif//STRATEGR_REACTIVELIST_HPP
