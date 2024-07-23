#include "report_render.hpp"
#include "item/watermark_item.hpp"
#include "page_render_factory.hpp"
#include "abstract_render.hpp"
#include "item/col_header_table.hpp"
#define  DEFAULT_SPACING  15

namespace ReportDesign
{
void inserDeltaYMap(QMap<qreal, qreal> &deltaYMap, qreal y, qreal deltaY)
{

}

// 当多个相同y（等高元件）存在时，以最大的那个deltaY为准
qreal equalYDeltaY(QMap<qreal, qreal> &deltaYMap, qreal y, qreal deltaY)
{
    return 0;
}

report_render::report_render()
{

}

report_render::~report_render()
{

}

page_item::Ptr report_render::findPage(ReportPages &pages, page_item *page)
{
    return nullptr;
}

void report_render::renderPage(ReportPages &pages, PrintDataMap &data)
{
    // page页渲染，包含数据自适应，自动分页等逻辑
    if (data.isEmpty() || pages.isEmpty())
    {
        return;
    }
}

bool report_render::emptyPage(page_item::Ptr page)
{
    return false;
}

int report_render::pageSpliceY(page_item::Ptr page)
{
    // 拿到page进行拼接的位置
    QList<base_design_item *> itemList = page->childBaseItems();
    moveValidItem(itemList);
   return 0;
}

void report_render::renderSinglePage(QList<page_item::Ptr> &newPageList, page_item::Ptr page, QList<base_design_item *> &posTable, PrintDataMap &data)
{
    // 判断newPageList的最后一个（最新一页），是否有越界控件，有则向下走，没有则停止递归。
    if ((!newPageList.isEmpty() && !isOverPage(page)) || posTable.isEmpty())
    {
        return;
    }
}

QList<QList<base_design_item *>> report_render::genPosTable(const ReportPages &pages)
{
    QList<QList<base_design_item *>> posTable;
    return posTable;
}

void report_render::addDeltaY(QList<base_design_item *> itemList, base_design_item *item, int deltaY)
{
    moveValidItem(itemList);
}

bool report_render::isOverPage(page_item::Ptr page)
{
    return false;
}

QList<base_design_item *> report_render::separateOverPageItems(page_item::Ptr page)
{
    QList<base_design_item *> overItems;
    return overItems;
}

page_item::Ptr report_render::createMasterMaskPage(page_item::Ptr page, const PrintDataMap &PrintData)
{
    // copy此page
    page_item::Ptr result = page_item::Ptr(dynamic_cast<page_item *>(page->cloneItem(page->itemMode())));
    return result;
}

QRect report_render::getBoundingRect(QVector<base_design_item *> &items)
{
    qreal minX = std::numeric_limits<qreal>::max();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal maxY = std::numeric_limits<qreal>::min();

    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY)).toRect();
}

QRect report_render::getBoundingRect(QList<base_design_item *> &items)
{
    qreal minX = std::numeric_limits<qreal>::max();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal maxY = std::numeric_limits<qreal>::min();

    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY)).toRect();
}

// 模板顶部（页眉）矩形区域
QRect report_render::masterMaskTop(page_item *page)
{
    //  masterMaskItem is empty
    return QRect();
}

// 模板底部（页脚）矩形区域
QRect report_render::masterMaskBottom(page_item *page)
{
    //  masterMaskItem is empty
    return QRect();
}

QRect report_render::masterMaskTop(page_item::Ptr page)
{
    //  masterMaskItem is empty
    return QRect();
}

QRect report_render::masterMaskBottom(page_item::Ptr page)
{
    //  masterMaskItem is empty
    return QRect();
}

// 表格翻页时，分离表格
table_item *report_render::separateTableItem(QRectF edgeRect, table_item *item, bool flag, int &deltaY)
{
    return nullptr;
}

void report_render::moveValidItem(QList<base_design_item *> &itemList)
{

}

bool report_render::itemIsValid(base_design_item *item)
{
    return false;
}

void report_render::addDeltaY(QMap<qreal, qreal> deltaYMap, base_design_item *item)
{

}
}
