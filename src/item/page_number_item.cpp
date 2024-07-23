#include "page_number_item.hpp"
#include "design_elements_factory.hpp"
#include "report_manager.hpp"
// 表格控件列宽默认值
#define DEFAULT_WIDTH     100
// 表格控件行高默认值
#define DEFAULT_HEIGHT    50
namespace
{

ReportDesign::base_design_item *createPageNumberItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::page_number_item(owner, parent);
}
bool registred = ReportDesign::DesignElementsFactory::instance().registerCreator(ReportDesign::PAGENUMBER_ITEM_TYPE, ReportDesign::ItemAttribs(QObject::tr("Shape Item"), ReportDesign::PAGENUMBER_ITEM_TYPE), createPageNumberItem);

}

namespace ReportDesign
{
page_number_item::page_number_item(QObject *owner /*= 0*/, QGraphicsItem *parent /*= 0*/):
    TextItem(PAGENUMBER_ITEM_TYPE, owner, parent)
{
    setSize(QSize(150, 50));
    setAlignment(Qt::AlignCenter);
}

void page_number_item::updatePageNumber()
{
    if (itemMode() & DesignMode)
    {
        QObject *parent = scene()->parent();
        report_manager *report = dynamic_cast<report_manager *>(parent);
        if (report)
        {
            ReportPages pageList = report->editePages();
            int currentNum = -1;
            for (int i = 0; i < pageList.count(); ++i)
            {
                if (pageList[i].data() == parentItem())
                {
                    currentNum = i + 1;
                }
            }
            setPageNumber(currentNum, pageList.count());
        }
    }
}

void page_number_item::setPageNumber(int current, int total)
{
    if (current < 0)
    {
        return;
    }
    variant_info variant;
    variant.m_name = QString("%1/%2").arg(current).arg(total);
    setVariant(variant);
}

void page_number_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    updatePageNumber();
    TextItem::paint(painter, nullptr, nullptr);
    drawMasterMarkColor(painter);
}

void page_number_item::renderPrintData(const PrintDataMap &PrintData)
{

}

void page_number_item::clearPrintData()
{

}

QWidget *page_number_item::defaultEditor()
{
    return nullptr;
}

base_design_item *page_number_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new page_number_item(owner, parent);
}

}
