#include "table_item.hpp"
#include "design_elements_factory.hpp"
#include "mask_dialog/variable_edit_dialog.hpp"
#include "page_scene.hpp"
#include <chrono>
#define UNDERLINE_WIDTH  2
// 表格控件数据列数默认值（不包含表头）
#define DEFAULT_COLUMN_NUM      2
// 表格控件数据行数默认值（不包含表头）
#define DEFAULT_ROW_NUM         2

// 表格控件列宽默认值
#define DEFAULT_COLUMN_WIDTH    150
// 表格控件行高默认值
#define DEFAULT_ROW_HEIGHT      50

#define EMPTY_DATA_ROW_COUNT    1
// 关联表头颜色
#define CONNECT_TABLE_HEADER_CORLOR  QColor(0xed, 0xff, 0xf0)

namespace
{

ReportDesign::base_design_item *createTableItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::table_item(owner, parent);
}
bool registred = ReportDesign::DesignElementsFactory::instance().registerCreator(ReportDesign::TABLE_ITEM_TYPE,
                 ReportDesign::ItemAttribs(QObject::tr("Table Item"),
                                           ReportDesign::TABLE_ITEM_TYPE),
                 createTableItem);
}

namespace ReportDesign
{
table_item::table_item(QObject *owner /*= 0*/, QGraphicsItem *parent /*= 0*/):
    table_base_item(ReportDesign::TABLE_ITEM_TYPE, owner, parent),
    m_autoFill(false), m_underLine(false), m_hideTable(true), m_tableItemID(-1),
    m_connectTableItemID(-1)

{
    setDataColumnCount(DEFAULT_COLUMN_NUM);
    setDataRowCount(DEFAULT_ROW_NUM);
    setRowHeight(DEFAULT_ROW_HEIGHT);
    setColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_tableItemID = genTableItemID();
    m_designHeaderColor = TABLE_HEADER_CORLOR;
}

table_item::table_item(QString storageTypeName, QObject *owner, QGraphicsItem *parent):
    table_base_item(storageTypeName, owner, parent),
    m_autoFill(false), m_underLine(false), m_hideTable(true), m_tableItemID(-1),
    m_connectTableItemID(-1)
{
    setDataColumnCount(DEFAULT_COLUMN_NUM);
    setDataRowCount(DEFAULT_ROW_NUM);
    setRowHeight(DEFAULT_ROW_HEIGHT);
    setColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_tableItemID = genTableItemID();
    m_designHeaderColor = TABLE_HEADER_CORLOR;
}

int table_item::genTableItemID()
{
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    // 返回时间戳作为ID，确保为正整数
    return static_cast<int>(timestamp);
}

int table_item::availableRowCount()
{
    // 如果有表头（变量显示的那一行）则需要再减1,目前默认是保留表头的
    return dataRowCount() - 1;
}

int table_item::availableColCount()
{
    // 如果有表头（变量显示的那一行）则需要再减1,目前默认是保留表头的
    return dataColumnCount() - 1;
}

cellContent table_item::contentData(int row, int column)
{
    QMutexLocker lock(&m_printDataMapMutex);
    variant_info variant = getTableVariant(QPair<int, int>(1, column));
    if (row == 1)
    {
        return cellContent(variant.m_name);
    }
    else
    {
        if (m_printDataMap.contains(variant.m_ID))
        {
            QVector<QString> printData = m_printDataMap.value(variant.m_ID);
            // 因为第一行已经显示了变量（数据表头）。所以实际是从第二行开始显示数据
            if (row - 2 < printData.count() && row - 2 >= 0)
            {
                return cellContent(printData[row - 2]);
            }
        }
        return cellContent("");
    }
}

base_design_item *table_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new ReportDesign::table_item(owner, parent);
}

bool table_item::autoFill()
{
    return m_autoFill;
}

void table_item::setAutoFill(bool autoFill)
{
    if (autoFill == m_autoFill)
    {
        return;
    }
    notify("font", m_autoFill, autoFill);
    m_autoFill = autoFill;
    SetAutoFillAction(autoFill);
    update();
}

void table_item::SetAutoFillAction(bool AutoFill)
{
    base_design_item *page_item = dynamic_cast<base_design_item *>(parentItem());
    if (page_item)
    {
        if (page_item->itemMode() & DesignMode)
        {
            if (m_autoFill)
            {
                setDataRowCount(2);
            }
        }
    }
}

bool table_item::hideTable()
{
    return m_hideTable;
}

void table_item::setHideTable(bool hideTable)
{
    m_hideTable = hideTable;
}

bool table_item::underLine()
{
    return m_underLine;
}

void table_item::setUnderLine(bool underLine)
{
    m_underLine = underLine;
    update();
}

void table_item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (selectCellIndex().first == 1 && selectCellIndex().second != 0)
    {
        table_base_item::mouseDoubleClickEvent(event);
    }
}

QWidget *table_item::defaultEditor()
{
    // 关联表格控件不能进行变量的编辑
    if (m_connectTableItemID != -1)
    {
        return nullptr;
    }

    base_homogeneous_variant *GroupVariant = nullptr;
    base_homogeneous_variant *variant = nullptr;
    if (groupVariant() != variant_info())
    {
        GroupVariant =  device_variant_manager::instance().
                        HomogeneousVariant(page()->pageItem()->module(), VariantType::Group);
    }

    if (GroupVariant != nullptr)
    {
        variant = GroupVariant->typeChildVariants(groupVariant().m_ID, VariantType::Table);
    }
    else
    {
        variant = device_variant_manager::instance().
                  HomogeneousVariant(page()->pageItem()->module(), VariantType::Table);
    }

    variable_edit_dialog *dlg = new variable_edit_dialog(true, variant, getTableVariant(selectCellIndex()), m_tableVariant);
    connect(dlg, SIGNAL(tableVariable(variant_info, variant_info)), this, SLOT(slotSetTableVariant(variant_info, variant_info)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    return dlg;
}

void table_item::slotSetTableVariant(variant_info itemVariant, variant_info tableVariant)
{
    slotSetVariant(itemVariant);
    if (!tableVariant.m_ID.isEmpty())
    {
        m_tableVariant = tableVariant;
    }
}

void table_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget/* = Q_NULLPTR*/)
{
    table_base_item::paint(painter, option, widget);
    drawUnderLine(painter);
    drawMasterMarkColor(painter);
}

void table_item::drawUnderLine(QPainter *painter)
{
    if (dataRowCount() >= 1 &&
        dataColumnCount() >= 1 &&
        m_underLine)
    {
        painter->save();
        QPen pen;
        pen.setColor(Qt::black);
        pen.setStyle(Qt::SolidLine);
        pen.setWidthF(UNDERLINE_WIDTH); //To draw with point precision (By default: 2px = 1 pt)
        painter->setPen(pen);
        painter->drawLine(cellRect(1, 1).bottomLeft(), cellRect(1, dataColumnCount()).bottomRight());
        painter->restore();
    }
}

void table_item::reloadVariantName(const VariantNameMap &variantName)
{
    // repeat
    variant_info GroupVariant = groupVariant();
    QMap<QPair<int, int>, variant_info> variantMap = tableVariant();
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (variantName.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupMap = variantName.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            if (!groupMap.contains(m_tableVariant.m_ID))
            {
                return;
            }

            QMap<QString, variant_info>  variantNameMap = groupMap.value(m_tableVariant.m_ID).value<QMap<QString, variant_info>>();
            // 只加载该控件保存的变量对应的打印数据
            for (auto it = variantMap.begin(); it != variantMap.end(); ++it)
            {
                if (!it.value().m_nameChange)
                {
                    if (variantNameMap.contains(it.value().m_ID))
                    {
                        variant_info var = variantNameMap.value(it.value().m_ID);
                        it.value().m_name = var.m_name;
                    }
                }
            }
        }
    }
    else
    {
        // 找表
        QMap<QString, variant_info>  variantNameMap;
        VariantNameMap::const_iterator varint = variantName.find(m_tableVariant.m_ID);
        if (varint == variantName.end())
        {
            return;
        }
        variantNameMap = varint.value().value<QMap<QString, variant_info>>();
        for (auto it = variantMap.begin(); it != variantMap.end(); ++it)
        {
            if (!it.value().m_nameChange)
            {
                auto varintName = variantNameMap.find(it.value().m_ID);
                if (varintName != variantNameMap.end())
                {
                    it.value().m_name = varintName.value().m_name;
                }
            }
        }
    }

    setTableVariant(variantMap);
}

void table_item::renderPrintData(const PrintDataMap &PrintData)
{
    variant_info GroupVariant = groupVariant();
    QString tableVariantID;
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (PrintData.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupTableMap = PrintData.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            // 只加载该控件保存的变量对应的打印数据
            QMutexLocker lock(&m_printDataMapMutex);
            tableVariantID = m_tableVariant.m_ID;
            if (groupTableMap.contains(tableVariantID))
            {
                QVector<QMap<QString, QVector<QString>>> valueArr = groupTableMap.value(tableVariantID).value<QVector<QMap<QString, QVector<QString>>>>();
                if (valueArr.count() > groupDataIndex())
                {
                    m_printDataMap = valueArr[groupDataIndex()];
                }
            }
        }
    }
    else
    {
        QMutexLocker lock(&m_printDataMapMutex);
        tableVariantID = m_tableVariant.m_ID;
        // 关联表格
        if (m_connectTableItemID != -1)
        {
            tableVariantID = m_tableVariant.m_ID + TABELITEMUUSTR + QString::number(m_tableItemID);
        }

        // 找表
        PrintDataMap::const_iterator varint = PrintData.find(tableVariantID);
        if (varint != PrintData.end())
        {
            m_printDataMap = (*varint).value<QMap<QString, QVector<QString>>>();
        }
    }
}

bool table_item::printDataIsEmpty()
{
    return m_printDataMap.isEmpty();
}

QRect table_item::adpContent(QFont font, QRect rect, QString content, Qt::Alignment Alignment)
{
    QFontMetrics Fm(font);
    QRect limt;
    limt.setWidth(rect.width() - DEFAULT_ROW_OFFSET);
    int h = Fm.boundingRect(limt, Alignment | Qt::TextWrapAnywhere, content).height();
    return QRect(rect.x(), rect.y(), rect.width(), h).adjusted(0, 0, 0, DEFAULT_ROW_OFFSET); // 行间距增加
    //return QRect(rect.x(), rect.y(), rect.width(), h); // 行间距增加
}

int table_item::maxRowHeight(int row, QRect pageItemShowRect)
{
    // 此方案为临时方案，后续需要解决单行数据多长导致的无限递归问题。
    const int MaxRowHeight = 10;
    // 表头行，最大行高 = 单页显示区域矩形高度
    if (row <= 1)
    {
        return pageItemShowRect.height() - MaxRowHeight; // 10px的固定间距
    }
    else//非表头行，最大行高 = 单页显示区域矩形高度 -  表头高度
    {
        // 获取表头高度
        int tableHeight = cellRect(0, 0).height();
        tableHeight += cellRect(1, 1).height();
        return pageItemShowRect.height() - tableHeight - MaxRowHeight;
    }
}

QMap<qreal, qreal>  table_item::renderDataAdaptive()
{
    QMap<qreal, qreal> deltaYMap;
    int totalDeltaY = 0;
    // 根据渲染数据m_printDataMap 做控件的尺寸适配,表格包括行数和每行的行高
    QRect orgRect = rect().toRect();
    setRowHeight(rowHeight());
    if (autoFill())
    {
        // 根据数据设置对应行数
        if (!m_printDataMap.isEmpty())
        {
            // 这个地方会先把所有的行高等高为整表行高设置的高度
            setDataRowCount(m_printDataMap.begin()->count() +  1);
        }
        else
        {
            setDataRowCount(EMPTY_DATA_ROW_COUNT);
        }
    }
    // 新增行后Y的增加量
    totalDeltaY = rect().toRect().height() - orgRect.height();

    // 获取显示区域矩形，控件高度不超过此区域，否则会无线递归导致崩溃
    QRect pageItemShowRect;
    page_item *ptr = dynamic_cast<page_item *>(parentItem());
    if (ptr)
    {
        pageItemShowRect =  ptr->itemShowRect();
    }

    // 每一个单元格做数据的尺寸适配，自动换行，增加行高
    for (int j = 0; j <= dataRowCount(); ++j)
    {
        for (int i = 0; i <= dataColumnCount(); ++i)
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
                if (adpRect.height() > maxHeight)
                {
                    adpRect.setHeight(maxHeight);
                }

                int deltaY = adpRect.height() -  interRect.height();
                if (deltaY > 0)
                {
                    totalDeltaY += deltaY;
                    resizeRowHight(deltaY, j);
                    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
                }
            }
        }
    }
    update();
    deltaYMap[pos().y()] = totalDeltaY;
    return deltaYMap;
}

void table_item::clearPrintData()
{
    QMutexLocker lock(&m_printDataMapMutex);
    m_printDataMap.clear();
}

bool table_item::variantIsEmpty()
{
    return tableVariant().isEmpty();
}

bool table_item::isConnectTableItem()
{
    return m_connectTableItemID != -1;
}

int table_item::connectTableItemID()
{
    return m_connectTableItemID;
}

void table_item::setconnectTableItemID(int ID)
{
    // 为被关联控件时
    if (ID == m_tableItemID)
    {
        m_designHeaderColor = generateRandomLightColor();
        setHeaderBrush(QBrush(m_designHeaderColor));
    }
    m_connectTableItemID = ID;
}

int table_item::pixelToRow(int pixel)
{
    return  table_base_item::pixelToRow(pixel);
}

int table_item::pixelToBottomDistance(int pixel)
{
    QPointF pixlePoint(2, pixel);
    for (int i = 0; i <= dataRowCount(); ++i)
    {
        for (int j = 0; j <= dataColumnCount(); ++j)
        {
            if (cellRect(i, j).contains(pixlePoint.toPoint()))
            {
                return cellRect(dataRowCount(), j).bottom() - pixel;
            }
        }
    }
    return -1;
}

void table_item::deleteBelowRow(int row)
{
    //    int count = dataRowCount() + 1 - row;
    //    for (int i = 0; i < count; ++i)
    //    {
    //        table_base_item::deleteRow(row);
    //    }
    deleteRowRange(row, dataRowCount());
}

void table_item::deleteRangeRow(int begin, int end)
{
    //    int count = end - begin;
    //    for (int i = 0; i < count; ++i)
    //    {
    //        table_base_item::deleteRow(begin);
    //    }
    deleteRowRange(begin, end);
}

table_item *table_item::separateTable(int row, int pixel, int &deltaY)
{
    // 从给定的row，向下截断表格，并把截断的内容作为新的表格控件返回
    // 克隆当前表格控件（分离表格）
    page_item *page = dynamic_cast<page_item *>(parentItem());
    base_design_item *newItem = cloneItem(page->itemMode(), page, page);
    table_item *newTable = dynamic_cast<table_item *>(newItem);
    if (!newTable)
    {
        return nullptr;
    }

    QMap<QString, QVector<QString>> newPrintDataMap;

    // 根据row分离m_printDataMap（打印数据）
    for (QString key : m_printDataMap.keys())
    {
        QVector<QString> dataVec = m_printDataMap.value(key);
        // row - 2. 参考contentData函数实现
        newPrintDataMap.insert(key, dataVec.mid(row - 2, -1));
    }

    // 新建表格设置分割后的打印数据
    newTable->setPrintDataMap(newPrintDataMap);

    // 数据的行数 + 表头行,这里的表格控件已经经过了自适应,行数已经自动适配了。这里不要用打印数据的个数去设置新表格的行数
    //newTable->setDataRowCount(dataRowCount() - row + 2);

    // 因为setDataRowCount会重置行高，等于整表的行高，所以需要重新渲染（优化项）
    //newTable->renderDataAdaptive();

    // 删除新控件和原控件在page中重叠的部分,第row行在新控件中显示
    newTable->deleteRangeRow(2, row - 1);

    // 新建表格的尺寸变长了（表头和截断行的长度）会导致原表格后的控件相对位置发生变化，所以这里保证相对于原表格的相对位置不变，需要计算出新增Y值偏差
    deltaY = newTable->rect().height() - pixelToBottomDistance(pixel);

    // 当前表格删除row行下面的行
    deleteBelowRow(row);

    // 返回分离表格
    return newTable;
}

void table_item::setPrintDataMap(QMap<QString, QVector<QString>> printDataMap)
{
    QMutexLocker lock(&m_printDataMapMutex);
    m_printDataMap = printDataMap;
}

void table_item::clearVariant()
{
    // 清除组变量
    setGroupVariant(variant_info());

    // 清除表格变量ID
    setTableVariantID(variant_info());

    // 清除表格中的变量
    setTableVariant(QMap<QPair<int, int>, variant_info>());
}

}
