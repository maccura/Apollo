#include "item_synchronization.hpp"
#include <QGraphicsScene>
namespace ReportDesign
{
item_synchronization::item_synchronization(QObject *parent): QObject(parent)
{

}

QList<base_design_item *> item_synchronization::itemSynchronization(base_design_item *item, const ReportPages &pageList)
{
    // （1）如果传入的item不是模板控件，则说明是用户把item取消了母版属性，则其他页的相同item直接删除
    // （2）如果传入的item是模板控件，则开始克隆item. 克隆个数为pages.count -1。
    // （3）全部克隆item都需要connect(item的synchronization、destroyed)，
    //          synchronization信号需要新添加，在propertyChanged和变量改变的场景中触发,返回到report_manager中去完成。
    // （4）找到每一个page当中masterMarkID 和masterMarkItemID相同的item，删掉他们并把克隆的item添加进去,如果没找到则直接添加克隆item。
    QList<base_design_item *> itemList;
    for (int i = 0; i < pageList.count(); ++i)
    {
        if (item->parentItem() != pageList[i].data())
        {
            deleteSameMastermarkIDItem(item, pageList[i]);
            if (item->isMasterMask())
            {
                base_design_item *newBaseItem = item->cloneItem(item->itemMode());
                if (newBaseItem)
                {
                    newBaseItem->setParent(pageList[i].data());
                    newBaseItem->setParentItem(pageList[i].data());
                    itemList.push_back(newBaseItem);
                }
            }
        }
    }
    return itemList;
}

void item_synchronization::synchronizationItemDestroyed(base_design_item *item, const ReportPages &pageList)
{
    // 找到每一个page当中的masterMarkID 和masterMarkItemID相同的item,并删掉他们,item本身已经被删除，就不能再次delete
    for (page_item::Ptr page : pageList)
    {
        deleteSameMastermarkIDItem(item, page);
    }
}

void item_synchronization::deleteSameMastermarkIDItem(base_design_item *item, const page_item::Ptr page)
{
    for (base_design_item *pageItem : page->childBaseItems())
    {
        if (pageItem->masterMarkItemID() == item->masterMarkItemID()
            && item != pageItem)
        {
            pageItem->setParentItem(nullptr);
            pageItem->setParent(nullptr);
            pageItem->deleteLater();
        }
    }
}

}
