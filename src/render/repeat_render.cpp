#include "repeat_render.hpp"
#include "page_render_factory.hpp"
#include "report_render.hpp"
#define REPEAT_FIXED_SPACING 25
namespace
{
ReportDesign::abstract_render *RepeatRenderCreator()
{
    return new ReportDesign::RepeatRender();
}
bool ret = ReportDesign::PageRenderFactor::instance().registerRender(ReportDesign::REPEAT_RENDER, RepeatRenderCreator);
}

namespace ReportDesign
{

RepeatRender::RepeatRender()
{

}

/*
 *   1.读取数据中repeat变量key.
 *   2.找到对应page中添加了此变量的组（repeat组）,无需管y的顺序。
 *   3.根据数据块个数新增对应数量的控件。
 *   for()
 *   {
 *       4.加上固定间距
 *       5.计算出新增repeat控件的deltaY,
 *       deltaY =  rect() + 固定间距
 *       处于后面的控件都加上对应的deltaY
 *   }
 */
void RepeatRender::pageRender(ReportPages &pages, PrintDataMap &data)
{
    m_printData = data;
    m_newPages = pages;
    pages = m_newPages;
}

int RepeatRender::repeatCount(QMap<QString, QVariant> &gourpMap)
{
    if (gourpMap.isEmpty())
    {
        return 0;
    }
    return 0;
}

// 返回增加后的deltaY
void RepeatRender::copyGroupItemAtBottom(int repeatNum, QList<base_design_item *> &groupItems, page_item::Ptr page)
{
    if (groupItems.isEmpty())
    {
        return;
    }
}

void RepeatRender::insertFrontPage(page_item::Ptr posPage, page_item::Ptr insertPage)
{

}

/* 传入的overItems 需要根据pos().y 升序排序，也就是front()为最上方的元件 */
page_item::Ptr RepeatRender::moveToNewPage(QList<base_design_item *> &overItems, page_item::Ptr beCopiedPage, PrintDataMap &PrintData)
{
    page_item::Ptr newPage = nullptr;
    return newPage;
}

// 位于groupItems下方的item全部增加deltaY;
void RepeatRender::AddDeltaY(QList<base_design_item *> &groupItems, int deltaY, page_item::Ptr page)
{
    if (groupItems.isEmpty())
    {
        return;
    }
}

QList<base_design_item *> RepeatRender::copyItems(QList<base_design_item *> &orgItems, int groupDataIndex)
{
    QList<base_design_item *> copyItemList;
    return copyItemList;
}

QList<base_design_item *> RepeatRender::belowTheYItems(int Y, page_item::Ptr page)
{
    QList<base_design_item *> belowList;
    return belowList;
}

}
