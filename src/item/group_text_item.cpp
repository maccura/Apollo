#include "group_text_item.hpp"
#include "design_elements_factory.hpp"
#include "data_manager/device_variant_manager.hpp"
#include "page_scene.hpp"
#define DATA_COLUM_CORLOR QColor(0xed, 0xee, 0xf0)
// 表格控件数据列数默认值（不包含表头）
#define DEFAULT_COLUMN_NUM      6
// 表格控件数据行数默认值（不包含表头）
#define DEFAULT_ROW_NUM         3

// 表格控件列宽默认值
#define DEFAULT_COLUMN_WIDTH    150
// 表格控件行高默认值
#define DEFAULT_ROW_HEIGHT      50

namespace
{

ReportDesign::base_design_item *createTableItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::group_text_item(owner, parent);
}
bool registred = ReportDesign::DesignElementsFactory::instance().registerCreator(ReportDesign::GROUP_TEXT_ITEM_TYPES,
                 ReportDesign::ItemAttribs(QObject::tr("Group Text Item"),
                                           ReportDesign::GROUP_TEXT_ITEM_TYPES),
                 createTableItem);
}

namespace ReportDesign
{
group_text_item::group_text_item(QObject *owner /*= 0*/, QGraphicsItem *parent /*= 0*/):
    table_base_item(ReportDesign::GROUP_TEXT_ITEM_TYPES, owner, parent)
{
    setDataColumnCount(DEFAULT_COLUMN_NUM);
    setDataRowCount(DEFAULT_ROW_NUM);
    setRowHeight(DEFAULT_ROW_HEIGHT);
    setColumnWidth(DEFAULT_COLUMN_WIDTH);
}

cellContent group_text_item::contentData(int row, int column)
{
    // 显示变量
    if (column % 2)
    {
        variant_info variant = getTableVariant(QPair<int, int>(row, column));
        return variant.m_name;
    }
    else
    {
        QMutexLocker lock(&m_printDataMapMutex);
        variant_info variant = getTableVariant(QPair<int, int>(row, column - 1));
        if (m_printDataMap.contains(variant.m_ID))
        {
            QString str =  m_printDataMap.value(variant.m_ID);
            return cellContent(m_printDataMap.value(variant.m_ID));
        }
        return cellContent(QString(""));
    }
}

base_design_item *group_text_item::createSameTypeItem(QObject *owner/*=0*/, QGraphicsItem *parent/*=0*/)
{
    return new ReportDesign::group_text_item(owner, parent);
}

void group_text_item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (selectCellIndex().first != 0 &&
        selectCellIndex().second % 2)
    {
        table_base_item::mouseDoubleClickEvent(event);
    }
}

QWidget *group_text_item::defaultEditor()
{
    base_homogeneous_variant *GroupVariant = nullptr;
    base_homogeneous_variant *variant = nullptr;
    if (groupVariant() != variant_info())
    {
        GroupVariant =  device_variant_manager::instance().
                        HomogeneousVariant(page()->pageItem()->module(), VariantType::Group);
    }

    if (GroupVariant != nullptr)
    {
        variant = GroupVariant->typeChildVariants(groupVariant().m_ID, VariantType::Text);
    }
    else
    {
        variant = device_variant_manager::instance().
                  HomogeneousVariant(page()->pageItem()->module(), VariantType::Text);
    }

    variable_edit_dialog *dlg = new variable_edit_dialog(false, variant, getTableVariant(selectCellIndex()));
    connect(dlg, SIGNAL(variable(variant_info)), this, SLOT(slotSetVariant(variant_info)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    return dlg;
}

void group_text_item::fillPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget/* = Q_NULLPTR*/)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (itemMode() & DesignMode)
    {
        paintRect(painter);
    }
}

void group_text_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget/* = Q_NULLPTR*/)
{
    table_base_item::paint(painter, option, widget);
    drawMasterMarkColor(painter);
}

void group_text_item::paintRect(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    for (int i = 1; i <= dataColumnCount() ; ++i)
    {
        // 双数列为数据列，不允许选择变量和输入文本，绘制一个灰色框
        if (i % 2 == 0)
        {
            for (int j = 1; j <= dataRowCount(); ++j)
            {
                painter->fillRect(cellRect(j, i), QBrush(DATA_COLUM_CORLOR));
            }
        }
    }
    painter->restore();
}

void group_text_item::reloadVariantName(const VariantNameMap &variantName)
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
            // 只加载该控件保存的变量对应的打印数据
            for (auto it = variantMap.begin(); it != variantMap.end(); ++it)
            {
                if (!it.value().m_nameChange)
                {
                    if (groupMap.contains(it.value().m_ID))
                    {
                        variant_info var = groupMap.value(it.value().m_ID).value<variant_info>();
                        it.value().m_name = var.m_name;
                    }
                }
            }
        }
    }
    else
    {
        // 只加载该控件保存的变量对应的打印数据
        for (auto it = variantMap.begin(); it != variantMap.end(); ++it)
        {
            if (!it.value().m_nameChange)
            {
                VariantNameMap::const_iterator varint = variantName.find(it.value().m_ID);
                if (varint != variantName.end())
                {
                    it.value().m_name = (*varint).value<variant_info>().m_name;
                }
            }
        }
    }
    setTableVariant(variantMap);
}

bool group_text_item::printDataIsEmpty()
{
    return m_printDataMap.isEmpty();
}

void group_text_item::renderPrintData(const PrintDataMap &PrintData)
{
    variant_info GroupVariant = groupVariant();
    QMutexLocker lock(&m_printDataMapMutex);
    QMap<QPair<int, int>, variant_info> variantMap = tableVariant();
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (PrintData.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupMap = PrintData.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            // 只加载该控件保存的变量对应的打印数据
            for (auto it : variantMap)
            {
                if (groupMap.contains(it.m_ID))
                {
                    QVector<QString> valueArr = groupMap.value(it.m_ID).value<QVector<QString>>();
                    if (valueArr.count() > groupDataIndex())
                    {
                        m_printDataMap.insert(it.m_ID, valueArr[groupDataIndex()]);
                    }
                }
            }
        }
    }
    else // 非repeat元件
    {
        for (auto it : variantMap)
        {
            PrintDataMap::const_iterator varint = PrintData.find(it.m_ID);
            if (varint != PrintData.end())
            {
                m_printDataMap.insert(it.m_ID, (*varint).value<QString>());
            }
        }
    }
}

void group_text_item::clearPrintData()
{
    QMutexLocker lock(&m_printDataMapMutex);
    m_printDataMap.clear();
}

bool group_text_item::variantIsEmpty()
{
    return tableVariant().isEmpty();
}

void group_text_item::clearVariant()
{
    // 清除组变量
    setGroupVariant(variant_info());

    // 清除表格中的变量
    setTableVariant(QMap<QPair<int, int>, variant_info>());
}
}
