#include "col_header_table.hpp"
#include "design_elements_factory.hpp"
#include "page_item.hpp"
#define    REPEAT_OFFSET   20
#define DEFAULT_ROW_NUM    3
#define DEFAULT_COLUMN_NUM 3
namespace
{

ReportDesign::base_design_item *createColHeaderTableItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::col_header_table(owner, parent);
}

bool colHeaderTableItemRegistred = ReportDesign::DesignElementsFactory::instance().registerCreator(ReportDesign::COL_HEADER_TABLE_ITEM_TYPE,
                                   ReportDesign::ItemAttribs(QObject::tr("colHeaderTable Item"),
                                                             ReportDesign::COL_HEADER_TABLE_ITEM_TYPE), createColHeaderTableItem);
}

namespace ReportDesign
{
col_header_table::col_header_table(QObject *owner, QGraphicsItem *parent):
    table_item(ReportDesign::COL_HEADER_TABLE_ITEM_TYPE, owner, parent)
{
    setDataRowCount(DEFAULT_ROW_NUM);
    setDataColumnCount(DEFAULT_COLUMN_NUM);
}

cellContent col_header_table::contentData(int row, int column)
{
    QMutexLocker lock(&m_printDataMapMutex);
    variant_info variant = getTableVariant(QPair<int, int>(row, 1));
    if (column == 1)
    {
        return cellContent(variant.m_name);
    }
    else
    {
        if (m_printDataMap.contains(variant.m_ID))
        {
            QVector<QString> printData = m_printDataMap.value(variant.m_ID);
            // 因为第一行已经显示了变量（数据表头）。所以实际是从第二行开始显示数据
            if (column - 2 < printData.count() && column - 2 >= 0)
            {
                return cellContent(printData[column - 2]);
            }
        }
        return cellContent("");
    }
}

base_design_item *col_header_table::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new ReportDesign::col_header_table(owner, parent);
}

void col_header_table::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (selectCellIndex().second == 1 && selectCellIndex().first != 0)
    {
        table_base_item::mouseDoubleClickEvent(event);
    }
}

QMap<qreal, qreal> col_header_table::renderDataAdaptive()
{
    QMap<qreal, qreal> deltaYMap;
    int totalDeltaY = 0;
    if (autoFill())
    {
        // 根据渲染数据m_printDataMap 做控件的尺寸适配,表格包括行数和每行的行高
        // 根据数据设置计算需要增加的table个数
        int newTableNum = 0;
        int yu = 0;
        if (!m_printDataMap.isEmpty())
        {
            int dataCount = m_printDataMap.begin()->count();
            newTableNum = dataCount / availableColCount();
            yu = dataCount % availableColCount();
            if (yu == 0)
            {
                --newTableNum;
            }
            // 新增newTableNum个表格,返回新增的deltaY
            addNewTableItem(newTableNum, deltaYMap);
        }
        else
        {
            // 新增newTableNum个表格,返回新增的deltaY
            addNewTableItem(0, deltaYMap);
        }
    }
    else
    {
        totalDeltaY = contentAdapt();
        deltaYMap[pos().y()] = totalDeltaY;
    }
    update();

    // 返回适配后控件高度变化的deltaY
    return deltaYMap;
}

int col_header_table::contentAdapt()
{
    int totalDeltaY = 0;

    // 这个地方之所以要这么做是因为，行高只会存行当中最大的行高，如果用户手动去拉很大（比如表格拉下面一行作为布局），就会存储下用户手动调整的那个行高。
    // 先设置为等高
    QRect orgRect = rect().toRect();
    setRowHeight(rowHeight());
    // 新增行后Y的增加量
    totalDeltaY = rect().toRect().height() - orgRect.height();


    QRect pageItemShowRect;
    page_item *ptr = dynamic_cast<page_item *>(parentItem());
    if (ptr)
    {
        pageItemShowRect =  ptr->itemShowRect();
    }
    // 每一个单元格做数据的尺寸适配，自动换行，增加行高
    for (int i = 0; i <= dataColumnCount(); ++i)
    {
        for (int j = 0; j <= dataRowCount(); ++j)
        {
            cellContent data = contentData(j, i);
            QRect item = cellRect(j, i);
            content_property property = cellContentProperty(j, i);
            QFont font = transformToSceneFont(property.font());
            // 包含也是相交
            if (rect().intersects(item))
            {
                QRect interRect = rect().intersected(item).toRect();
                QRect adpRect ;
                adpRect = adpContent(font, interRect, data.content(), property.alignment());
                // TODO: 最大行高判断，超过最大行高则设置为最大行高，最大行高的设计见doc/变长设计.drawio
                int maxHeight = maxRowHeight(j, pageItemShowRect);
                if (adpRect.height() > maxHeight && ptr != nullptr)
                {
                    adpRect.setHeight(maxHeight);
                }

                int deltaY = adpRect.height() -  interRect.height();
                // 保持最大的行高度
                if (deltaY > 0)
                {
                    totalDeltaY += deltaY;
                    resizeRowHight(deltaY, j);
                    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
                }
            }
        }
    }
    return totalDeltaY;
}

void col_header_table::setAutoFillImpl(int dataRow, int dataCol, bool autoFill)
{
    //setDataColumnCount(dataCol);
    setDataRowCount(dataRow);
}

int col_header_table::addNewTableItem(int num, QMap<qreal, qreal> &deltaYMap)
{
    // 本身的内容自适应deltaY;
    int deltaY = contentAdapt();
    if (num == 0)
    {
        deltaYMap[pos().y()] = deltaY;
        return deltaY;
    }

    col_header_table *preTableItem = this;
    int preTableItemCol = preTableItem->availableColCount();
    QMap<QString, QVector<QString>> orgPrintDataMap = m_printDataMap;
    for (int i = 0; i < num; ++i)
    {
        base_design_item *newItem = preTableItem->cloneItem(preTableItem->itemMode(), preTableItem->parent(), preTableItem->parentItem());
        col_header_table *newTableItem = dynamic_cast<col_header_table *>(newItem);
        if (newTableItem)
        {
            // 设置位置
            newTableItem->setPos(preTableItem->pos().x(), preTableItem->pos().y() + preTableItem->rect().height() + REPEAT_OFFSET);

            // TODO 把数据进行分割
            QMap<QString, QVector<QString>> newPrintDataMap;
            // 根据row分离m_printDataMap（打印数据）
            for (QString key : orgPrintDataMap.keys())
            {
                QVector<QString> dataVec = orgPrintDataMap.value(key);
                newPrintDataMap.insert(key, dataVec.mid((i + 1) * preTableItemCol, preTableItemCol));
            }

            // 设置打印数据
            newTableItem->setPrintDataMap(newPrintDataMap);
            // 打印数据自适应,
            newTableItem->contentAdapt();
            deltaY += newTableItem->height() + REPEAT_OFFSET ;
            preTableItem = newTableItem;
        }
    }
    deltaYMap[pos().y()] = deltaY;
    return deltaY;
}

table_item *col_header_table::separateTable(int row, int pixel, int &deltaY)
{
    // 从给定的row，向下截断表格，并把截断的内容作为新的表格控件返回
    // 克隆当前表格控件（分离表格）
    page_item *page = dynamic_cast<page_item *>(parentItem());
    base_design_item *newItem = cloneItem(page->itemMode(), page, page);
    col_header_table *newTable = dynamic_cast<col_header_table *>(newItem);
    if (!newTable)
    {
        return nullptr;
    }

    // 新建表格设置分割后的打印数据
    newTable->setPrintDataMap(m_printDataMap);

    int totalNUm = dataRowCount();
    int leaveNum = dataRowCount() - row + 1;
    int deleteNum = totalNUm - leaveNum;
    for (int i = 0; i < deleteNum; ++i)
    {
        // 每次都删除第一个数据行
        newTable->deleteRow(1);
    }

    newTable->setDataRowCount(dataRowCount() - row + 1);

    // 因为是新建的表格控件，所以需要数据自适应一次
    //newTable->renderDataAdaptive();

    // 新建表格的尺寸变长了（表头和截断行的长度）会导致原表格后的控件相对位置发生变化，所以这里保证相对于原表格的相对位置不变，需要计算出新增Y值偏差
    deltaY = newTable->rect().height() - pixelToBottomDistance(pixel);

    // 当前表格删除row行下面的行
    deleteBelowRow(row);

    // 返回分离表格
    return newTable;
}

void col_header_table::setAutoFill(bool autoFill)
{
    if (autoFill == m_autoFill)
    {
        return;
    }
    notify("font", m_autoFill, autoFill);
    m_autoFill = autoFill;
    update();
}

int col_header_table::maxRowHeight(int row, QRect pageItemShowRect)
{
    // 此方案为临时方案，后续需要解决单行数据多长导致的无限递归问题。
    const int MaxRowHeight = 10;
    int tableHeight = cellRect(0, 0).height();
    return pageItemShowRect.height() - tableHeight - MaxRowHeight;
}


}
