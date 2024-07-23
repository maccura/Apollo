#include "table_base_item.hpp"

//// 表格控件数据列数默认值（不包含表头）
//#define DEFAULT_COLUMN_NUM      6
//// 表格控件数据行数默认值（不包含表头）
//#define DEFAULT_ROW_NUM         1
//// 表格控件列宽默认值
//#define DEFAULT_COLUMN_WIDTH    80
//// 表格控件行高默认值
//#define DEFAULT_ROW_HEIGHT      80

// 表格列宽最小值
#define MIN_COLUMN_WIDTH   40
// 表格行高最小值
#define MIN_ROW_HEIGHT     40

// 行列选择框颜色
#define SELECT_CORLOR   QColor(0x05,0x66,0x08)

// 行列选择框线条粗细
#define SELECT_LINE_WIDTH   4

namespace ReportDesign
{
int s = qRegisterMetaType<QList<column_property>>("QList<column_property>");
int resRowProperty = qRegisterMetaType<QList<row_property>>("QList<row_property>");
int resContentProperty = qRegisterMetaType<content_property>("content_property");
int resQMapContentProperty = qRegisterMetaType<QMap<QPair<int, int>, content_property>>("QMap<QPair<int,int>,content_property>");
int resQMapVariantProperty = qRegisterMetaType<QMap<QPair<int, int>, variant_info>>("QMap<QPair<int,int>,variant_info>");
int resRowDeltaProperty = qRegisterMetaType<QList<int>>("QList<int>");

table_base_item::table_base_item(QString storageTypeName, QObject *owner /*= 0*/, QGraphicsItem *parent /*= 0*/):
    base_design_item(storageTypeName, owner, parent), m_columnWidth(MIN_COLUMN_WIDTH), m_rowHeight(MIN_ROW_HEIGHT),
    m_tableContentProperty(base_design_item::font(), Qt::AlignLeft | Qt::AlignTop, Qt::black), m_showGrid(true), m_showHeader(true),
    contentPropertyTimeCount(0)
{
    init();
}

void table_base_item::init()
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    m_buttonDownCellIndex.first = -1;
    m_buttonDownCellIndex.second = -1;
    m_headerBrush = QBrush(TABLE_HEADER_CORLOR);
}

bool table_base_item::showGrid()
{
    return m_showGrid;
}

void table_base_item::setShowGrid(bool showGrid)
{
    m_showGrid = showGrid;
    update();
}

bool table_base_item::showHeader()
{
    return m_showHeader;
}

void table_base_item::setShowHeader(bool showHeader)
{
    if (m_showHeader == showHeader)
    {
        return;
    }

    bool old = m_showHeader;
    m_showHeader = showHeader;
    notify("showHeader", old, showHeader);

    {
        QRect HEndItem = cellRect(0, dataColumnCount());
        QMutexLocker columnPropertyLocker(&m_columnPropertyMutex);
        m_columnProperty.back().setWidth(rect().right() - HEndItem.x());
    }

    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    update();
}

QFont table_base_item::font()
{
    return cellContentProperty(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second).font();
}

void table_base_item::setFont(QFont newFont)
{
    if (!compareFonts(newFont, font()))
    {
        //TODO 这里撤销的时候没传m_buttonDownCellIndex的信息，有可能撤销的时候m_buttonDownCellIndex已经改变了，存在问题。
        QFont oldFont = font();
        content_property property(newFont, alignment(), fontColor());
        setCellContentProperty(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second, property);
        notify("font", oldFont, newFont);
    }
    update();
}


QColor table_base_item::fontColor()
{
    return cellContentProperty(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second).fontColor();
}

void table_base_item::setFontColor(QColor color)
{
    if (color != fontColor())
    {
        //TODO 这里撤销的时候没传m_buttonDownCellIndex的信息，有可能撤销的时候m_buttonDownCellIndex已经改变了，存在问题。
        QColor old = fontColor();
        content_property property(font(), alignment(), color);
        setCellContentProperty(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second, property);
        notify("fontColor", old, color);
    }
    update();
}

void table_base_item::setAlignment(Qt::Alignment newAlignment)
{
    if (alignment() != newAlignment)
    {
        //TODO 这里撤销的时候没传m_buttonDownCellIndex的信息，有可能撤销的时候m_buttonDownCellIndex已经改变了，存在问题。
        Qt::Alignment old = alignment();
        content_property property(font(), newAlignment, fontColor());
        setCellContentProperty(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second, property);
        notify("alignment", QVariant(old), QVariant(newAlignment));
    }
    update();
}

Qt::Alignment table_base_item::alignment()
{
    return cellContentProperty(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second).alignment();
}

QList<column_property> table_base_item::columnProperty()
{
    QMutexLocker locker(&m_columnPropertyMutex);
    return  m_columnProperty;
}

void table_base_item::setColumnProperty(QList<column_property> columnProperty)
{
    QVariant newV, oldV;
    {
        if (m_columnProperty == columnProperty)
        {
            return;
        }

        QMutexLocker locker(&m_columnPropertyMutex);
        newV.setValue(columnProperty);
        oldV.setValue(m_columnProperty);
        std::swap(columnProperty, m_columnProperty);
    }
    notify("columnProperty", oldV, newV);
    update();
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
}

QList<row_property> table_base_item::rowProperty()
{
    QMutexLocker locker(&m_rowPropertyMutex);
    return  m_rowProperty;
}

void table_base_item::setRowProperty(QList<row_property> rowProperty)
{
    QVariant newV, oldV;
    {
        if (m_rowProperty == rowProperty)
        {
            return;
        }
        QMutexLocker locker(&m_rowPropertyMutex);
        newV.setValue(rowProperty);
        oldV.setValue(m_rowProperty);
        std::swap(rowProperty, m_rowProperty);
    }
    notify("rowProperty", oldV, newV);
    update();
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
}

unsigned int table_base_item::propertyTimeCount()
{
    return contentPropertyTimeCount;
}

void table_base_item::setPropertyTimeCount(unsigned int TimeCount)
{
    contentPropertyTimeCount = TimeCount;
}

content_property table_base_item::tableContentProperty()
{
    return m_tableContentProperty;
}

void table_base_item::setTableContentProperty(content_property tableContentProperty)
{
    m_tableContentProperty = tableContentProperty;
    update();
}

QMap<QPair<int, int>, content_property> table_base_item::TableCellContentProperty()
{
    QMutexLocker locker(&m_cellContentPropertyMutex);
    return m_cellContentProperty;
}

void table_base_item::setTableCellContentProperty(QMap<QPair<int, int>, content_property> cellContentProperty)
{
    QMutexLocker locker(&m_cellContentPropertyMutex);
    std::swap(cellContentProperty, m_cellContentProperty);
    update(); ;
}

int table_base_item::dataColumnCount()
{
    QMutexLocker locker(&m_columnPropertyMutex);
    return m_columnProperty.count() - 1;
}

void table_base_item::setDataColumnCount(int dataColumnCount)
{
    int old;
    {
        QMutexLocker locker(&m_columnPropertyMutex);
        if (dataColumnCount == m_columnProperty.count() - 1)
        {
            return;
        }
        else
        {
            old  = m_columnProperty.count() - 1;
            m_columnProperty.clear();
            for (int i = 0; i <= dataColumnCount; i++)
            {
                m_columnProperty.push_back(column_property(i * m_columnWidth, m_columnWidth));
            }
        }
    }
    notify("dataColumnCount", old, dataColumnCount);
    if (itemAlign() == DesignedItemAlign)
    {
        setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    }
    update();
}

int table_base_item::dataRowCount()
{
    QMutexLocker locker(&m_rowPropertyMutex);
    return m_rowProperty.count() - 1;
}

void table_base_item::setDataRowCount(int dataRowCount)
{
    int old;
    {
        QMutexLocker locker(&m_rowPropertyMutex);
        if (dataRowCount == m_rowProperty.count() - 1)
        {
            return;
        }
        else
        {
            old = m_columnProperty.count() - 1;
            m_rowProperty.clear();
            for (int i = 0; i <= dataRowCount; i++)
            {
                m_rowProperty.push_back(row_property(i * m_rowHeight, m_rowHeight));
            }
        }
    }
    notify("dataRowCount", old, dataRowCount);
    if (itemAlign() == DesignedItemAlign)
    {
        setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    }
    update();
}

int table_base_item::columnWidth()
{
    return m_columnWidth;
}

void table_base_item::setColumnWidth(int columnWidth)
{
    bool flag = false;
    {
        QMutexLocker locker(&m_columnPropertyMutex);
        for (int i = 0; i < m_columnProperty.count(); ++i)
        {
            if (m_columnProperty[i].width() != columnWidth)
            {
                flag = true;
                break;
            }
        }
    }

    if (!flag)
    {
        return;
    }

    {
        QMutexLocker locker(&m_columnPropertyMutex);
        for (int i = 0; i < m_columnProperty.count(); i++)
        {
            m_columnProperty[i].setColumnPosx(i * columnWidth);
            m_columnProperty[i].setWidth(columnWidth);
        }
    }
    int old = m_columnWidth;
    m_columnWidth = columnWidth;
    notify("columnWidth", old, columnWidth);
    if (itemAlign() == DesignedItemAlign)
    {
        setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    }
    update();
}

int table_base_item::rowHeight()
{
    return m_rowHeight;
}

void table_base_item::setRowHeight(int rowHeight)
{
    bool flag = false;
    {
        QMutexLocker locker(&m_rowPropertyMutex);
        for (int i = 0; i < m_rowProperty.count(); ++i)
        {
            if (m_rowProperty[i].height() != rowHeight)
            {
                flag = true;
                break;
            }
        }
    }

    if (!flag)
    {
        return;
    }

    {
        QMutexLocker locker(&m_rowPropertyMutex);
        for (int i = 0; i < m_rowProperty.count(); ++i)
        {
            m_rowProperty[i].setRowPosY(i * rowHeight);
            m_rowProperty[i].setHeight(rowHeight);
        }
    }
    int old = m_rowHeight;
    m_rowHeight = rowHeight;
    notify("rowHeight", old, rowHeight);
    if (itemAlign() == DesignedItemAlign)
    {
        setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    }
    update();
}

QPair<int, int> table_base_item::pointInCellIndex(const QPoint &pos)
{
    for (int i = 0; i <= dataRowCount(); ++i)
    {
        for (int j = 0; j <= dataColumnCount(); ++j)
        {
            if (cellRect(i, j).contains(pos))
            {
                return QPair<int, int>(i, j);
            }
        }
    }

    return QPair<int, int>(-1, -1);
}

void table_base_item::resizeColumnWidth(QGraphicsSceneMouseEvent *event, int col, bool opposite /*= false*/)
{
    int delta = event->pos().x() - event->lastPos().x();
    QMutexLocker columnPropertyLocker(&m_columnPropertyMutex);
    if (col >= m_columnProperty.count() || col < 0)
    {
        return;
    }
    for (int i = col; i < m_columnProperty.count(); ++i)
    {
        if (i == col)
        {
            if (opposite)
            {
                delta = -1 * delta;
            }
            int width = delta + m_columnProperty[col].width();
            if (width <= MIN_COLUMN_WIDTH)
            {
                return;
            }
            m_columnProperty[i].setWidth(width);
        }
        else
        {
            m_columnProperty[i].setColumnPosx(m_columnProperty[i].columnPosx() + delta);
        }
    }
    update();
}

void table_base_item::resizeRowHight(int delta, int row, bool opposite)
{
    QMutexLocker rowPropertyyLocker(&m_rowPropertyMutex);
    for (int i = row; i < m_rowProperty.count(); ++i)
    {
        if (i == row)
        {
            if (opposite)
            {
                delta = -1 * delta;
            }
            int height = delta + m_rowProperty[row].height();
            if (height <= MIN_ROW_HEIGHT)
            {
                return;
            }
            m_rowProperty[i].setHeight(height);
        }
        else
        {
            m_rowProperty[i].setRowPosY(m_rowProperty[i].rowPosY() + delta);
        }
    }
}

void table_base_item::resizeRowHight(QGraphicsSceneMouseEvent *event, int row, bool opposite /*= false*/)
{
    int delta = event->pos().y() - event->lastPos().y();
    resizeRowHight(delta, row, opposite);
    update();
}

void table_base_item::deleteColumn(int col)
{
    {
        QMutexLocker columnPropertyyLocker(&m_columnPropertyMutex);
        // 表头列不删出
        if (col <= 0 || col >= m_columnProperty.count())
        {
            return;
        }

        for (int i = col + 1; i < m_columnProperty.count(); ++i)
        {
            m_columnProperty[i].setColumnPosx(m_columnProperty[i].columnPosx() - m_columnProperty[col].width());
        }
        m_columnProperty.removeAt(col);
    }

    {
        QMutexLocker cellContentPropertLocker(&m_cellContentPropertyMutex);
        QMap<QPair<int, int>, content_property> temContentProperty;
        // 删除带列号的 单元格属性
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {
            if (item.key().second == col)
            {
                item = m_cellContentProperty.erase(item);
            }
            else if (item.key().second > col)
            {
                QPair<int, int> newIndex = QPair<int, int>(item.key().first, item.key().second - 1);
                temContentProperty.insert(newIndex, item.value());
                item = m_cellContentProperty.erase(item);
            }
            else
            {
                item++;
            }
        }
        m_cellContentProperty.unite(temContentProperty);
    }

    {
        // 变量删除
        QMutexLocker variantLocker(&m_variantMutex);
        QMap<QPair<int, int>, variant_info> temVariant;
        for (auto it = m_variant.begin(); it != m_variant.end();)
        {
            if (it.key().second == col)
            {
                it = m_variant.erase(it);
            }
            else if (it.key().second > col)
            {
                QPair<int, int> newIndex = QPair<int, int>(it.key().first, it.key().second - 1);
                temVariant.insert(newIndex, it.value());
                it = m_variant.erase(it);
            }
            else
            {
                it++;
            }
        }
        m_variant.unite(temVariant);
    }
    propertyUpdate(PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY));
    update();
}

void table_base_item::deleteRowRange(int begin, int end)
{
    QMap<int, int> deleteMap;
    int deleteCount = end - begin + 1;
    {
        QMutexLocker rowPropertyyLocker(&m_rowPropertyMutex);
        // 表头行不删出
        if (!((begin > 0 && begin < m_rowProperty.count()) &&
              (end > 0 && end < m_rowProperty.count()) &&
              begin <= end))
        {
            return;
        }

        // 计算begin 到 end之间的height总和 sum(height)
        int sumHeight = 0;
        for (int i = begin; i <= end; ++i)
        {
            sumHeight += m_rowProperty[i].height();
            // 记录删除行，后面删变量和属性使用
            deleteMap.insert(i, i);
        }

        // 设置end以后得所有元素的Y值都减去sum(height)
        for (int i = end + 1; i < m_rowProperty.count(); ++i)
        {
            m_rowProperty[i].setRowPosY(m_rowProperty[i].rowPosY() - sumHeight);
        }

        // remove掉begin 到 end的所有行
        for (int i = end; i >= begin; --i)
        {
            m_rowProperty.removeAt(i);
        }
    }

    {

        // 删除带begin 到 end之间所有行号的单元格属性
        QMutexLocker cellContentPropertLocker(&m_cellContentPropertyMutex);
        QMap<QPair<int, int>, content_property> temContentProperty;
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {
            if (deleteMap.contains(item.key().first))
            {
                item = m_cellContentProperty.erase(item);
            }
            else if (item.key().first > end)
            {
                QPair<int, int> newIndex = QPair<int, int>(item.key().first - deleteCount, item.key().second);
                temContentProperty.insert(newIndex, item.value());
                item = m_cellContentProperty.erase(item);
            }
            else
            {
                item++;
            }
        }
        m_cellContentProperty.unite(temContentProperty);
    }

    {
        // 变量删除
        QMutexLocker variantLocker(&m_variantMutex);
        QMap<QPair<int, int>, variant_info> temVariant;
        for (auto it = m_variant.begin(); it != m_variant.end();)
        {
            if (deleteMap.contains(it.key().first))
            {
                it = m_variant.erase(it);
            }
            else if (it.key().first > end)
            {
                QPair<int, int> newIndex = QPair<int, int>(it.key().first - deleteCount, it.key().second);
                temVariant.insert(newIndex, it.value());
                it = m_variant.erase(it);
            }
            else
            {
                it++;
            }
        }
        m_variant.unite(temVariant);
    }
    propertyUpdate(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY));
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    update();
}

void table_base_item::deleteRow(int row)
{
    {
        QMutexLocker rowPropertyyLocker(&m_rowPropertyMutex);
        // 表头行不删出
        if (row <= 0 || row >= m_rowProperty.count())
        {
            return;
        }

        for (int i = row + 1; i < m_rowProperty.count(); ++i)
        {
            m_rowProperty[i].setRowPosY(m_rowProperty[i].rowPosY() - m_rowProperty[row].height());
        }
        m_rowProperty.removeAt(row);
    }

    {
        // 删除带行号的 单元格属性
        QMutexLocker cellContentPropertLocker(&m_cellContentPropertyMutex);
        QMap<QPair<int, int>, content_property> temContentProperty;
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {
            if (item.key().first == row)
            {
                item = m_cellContentProperty.erase(item);
            }
            else if (item.key().first > row)
            {
                QPair<int, int> newIndex = QPair<int, int>(item.key().first - 1, item.key().second);
                temContentProperty.insert(newIndex, item.value());
                item = m_cellContentProperty.erase(item);
            }
            else
            {
                item++;
            }
        }
        m_cellContentProperty.unite(temContentProperty);
    }

    {
        // 变量删除
        QMutexLocker variantLocker(&m_variantMutex);
        QMap<QPair<int, int>, variant_info> temVariant;
        for (auto it = m_variant.begin(); it != m_variant.end();)
        {
            if (it.key().first == row)
            {
                it = m_variant.erase(it);
            }
            else if (it.key().first > row)
            {
                QPair<int, int> newIndex = QPair<int, int>(it.key().first - 1, it.key().second);
                temVariant.insert(newIndex, it.value());
                it = m_variant.erase(it);
            }
            else
            {
                it++;
            }
        }
        m_variant.unite(temVariant);
    }
    propertyUpdate(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY));
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
    update();
}

int table_base_item::pixelToRow(int pixel)
{
    QPointF pixlePoint(2, pixel);
    for (int i = 0; i <= dataRowCount(); ++i)
    {
        for (int j = 0; j <= dataColumnCount(); ++j)
        {
            if (cellRect(i, j).contains(pixlePoint.toPoint()))
            {
                return i;
            }
        }
    }
    return -1;
}

void table_base_item::addColumn(int indexCol)
{
    {
        QMutexLocker columnPropertyyLocker(&m_columnPropertyMutex);
        if (indexCol < 0 || indexCol >= m_columnProperty.count())
        {
            return;
        }

        column_property newColumn(m_columnProperty[indexCol].columnPosx(), m_columnWidth);
        m_columnProperty.insert(indexCol, newColumn);
        for (int i = indexCol + 1; i < m_columnProperty.count(); ++i)
        {
            m_columnProperty[i].setColumnPosx(m_columnProperty[i].columnPosx() + m_columnWidth);
        }
    }

    {
        QMutexLocker cellContentPropertLocker(&m_cellContentPropertyMutex);
        QMap<QPair<int, int>, content_property> temContentProperty;
        // 删除带列号的 单元格属性
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {

            if (item.key().second >= indexCol)
            {
                QPair<int, int> newIndex = QPair<int, int>(item.key().first, item.key().second + 1);
                temContentProperty.insert(newIndex, item.value());
                item = m_cellContentProperty.erase(item);
            }
            else
            {
                item++;
            }
        }
        m_cellContentProperty.unite(temContentProperty);
    }

    {
        // 变量删除
        QMutexLocker variantLocker(&m_variantMutex);
        QMap<QPair<int, int>, variant_info> temVariant;
        for (auto it = m_variant.begin(); it != m_variant.end();)
        {
            if (it.key().second >= indexCol)
            {
                QPair<int, int> newIndex = QPair<int, int>(it.key().first, it.key().second + 1);
                temVariant.insert(newIndex, it.value());
                it = m_variant.erase(it);
            }
            else
            {
                it++;
            }
        }
        m_variant.unite(temVariant);
    }

    propertyUpdate(PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY));
    update();
}

void table_base_item::addRow(int indexRow)
{
    {
        QMutexLocker columnPropertyyLocker(&m_rowPropertyMutex);
        if (indexRow < 0 || indexRow >= m_rowProperty.count())
        {
            return;
        }

        row_property newRow(m_rowProperty[indexRow].rowPosY(), m_rowHeight);
        m_rowProperty.insert(indexRow, newRow);
        for (int i = indexRow + 1; i < m_rowProperty.count(); ++i)
        {
            m_rowProperty[i].setRowPosY(m_rowProperty[i].rowPosY() + m_rowHeight);
        }
    }

    {
        // 删除带列号的 单元格属性
        QMutexLocker cellContentPropertLocker(&m_cellContentPropertyMutex);
        QMap<QPair<int, int>, content_property> temContentProperty;
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {
            if (item.key().first >= indexRow)
            {
                QPair<int, int> newIndex = QPair<int, int>(item.key().first + 1, item.key().second);
                temContentProperty.insert(newIndex, item.value());
                item = m_cellContentProperty.erase(item);
            }
            else
            {
                item++;
            }
        }
        m_cellContentProperty.unite(temContentProperty);
    }

    {
        // 变量删除
        QMutexLocker variantLocker(&m_variantMutex);
        QMap<QPair<int, int>, variant_info> temVariant;
        for (auto it = m_variant.begin(); it != m_variant.end();)
        {
            if (it.key().first >= indexRow)
            {
                QPair<int, int> newIndex = QPair<int, int>(it.key().first + 1, it.key().second);
                temVariant.insert(newIndex, it.value());
                it = m_variant.erase(it);
            }
            else
            {
                it++;
            }
        }
        m_variant.unite(temVariant);
    }

    propertyUpdate(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY));
    update();
}

QSize table_base_item::tableShowSize()
{
    QSize TableSize;
    QRect DataRect = cellRect(dataRowCount(), dataColumnCount());
    // 计算表格宽度
    TableSize.setWidth(DataRect.x() + DataRect.width());

    // 计算表格高
    TableSize.setHeight(DataRect.y() + DataRect.height());

    return TableSize;
}

QRect table_base_item::cellRect(int row, int columnn)
{
    QMutexLocker columnPropertyLocker(&m_columnPropertyMutex);
    // 列越界。[0]为表头交叉区域。
    if (columnn  >= m_columnProperty.count() || columnn < 0)
    {
        return QRect();
    }

    // 行越界
    QMutexLocker rowPropertyLocker(&m_rowPropertyMutex);
    if (row >= m_rowProperty.count() || row < 0)
    {
        return QRect();
    }

    int header_widh = 0;
    int header_height = 0;
    // 如果不显示表头，则所有节点的pos需要减去表头的偏移量
    if (!m_showHeader)
    {
        header_widh = m_columnProperty[0].width();
        header_height = m_rowProperty[0].height();
    }

    return QRect(m_columnProperty[columnn].columnPosx() - header_widh,
                 m_rowProperty[row].rowPosY() - header_height,
                 m_columnProperty[columnn].width(),
                 m_rowProperty[row].height());
}

content_property table_base_item::cellContentProperty(int row, int columnn)
{
    QMutexLocker columnPropertyLocker(&m_columnPropertyMutex);
    // 列越界。[0]为表头交叉区域。
    if (columnn  >= m_columnProperty.count() || columnn < 0)
    {
        return content_property();
    }

    // 行越界
    QMutexLocker rowPropertyLocker(&m_rowPropertyMutex);
    if (row >= m_rowProperty.count() || row < 0)
    {
        return content_property();
    }

    // 从单元格属性中查找是否有属性设置
    QMutexLocker cellContentPropertyLocker(&m_cellContentPropertyMutex);
    //QMap<QPair<int,int>,content_property>::iterator
    auto item =  m_cellContentProperty.find(QPair<int, int>(row, columnn));
    if (item != m_cellContentProperty.end())
    {
        return item.value();
    }

    // 从行和列属性中查找是否有属性设置
    content_property columnProperty = m_columnProperty[columnn].contentProperty();
    content_property rowProperty = m_rowProperty[row].contentProperty();
    if (!columnProperty.isNull() && !rowProperty.isNull())
    {
        if (m_columnProperty[columnn].timeCount() > m_rowProperty[row].timeCount())
        {
            return columnProperty;
        }
        else
        {
            return rowProperty;
        }
    }

    if (columnProperty.isNull() && !rowProperty.isNull())
    {
        return rowProperty;
    }

    if (!columnProperty.isNull() && rowProperty.isNull())
    {
        return columnProperty;
    }

    // 从整表属性中获取属性设置
    return m_tableContentProperty;
}

QMap<QPair<int, int>, variant_info> table_base_item::tableVariant()
{
    QMutexLocker variantLocker(&m_variantMutex);
    return m_variant;
}

void table_base_item::setTableVariant(QMap<QPair<int, int>, variant_info> Variants)
{
    QMutexLocker variantLocker(&m_variantMutex);
    m_variant.swap(Variants);
}

variant_info table_base_item::getTableVariant(QPair<int, int> index)
{
    QMutexLocker variantLocker(&m_variantMutex);
    if (!m_variant.contains(index))
    {
        return variant_info();
    }
    else
    {
        return m_variant[index];
    }
}

void table_base_item::setITableVariant(QPair<int, int> index, variant_info Variant)
{
    QMutexLocker variantLocker(&m_variantMutex);
    if (!m_variant.contains(index))
    {
        m_variant.insert(index, Variant);
    }
    else
    {
        m_variant[index] = Variant;
    }
}

QPair<int, int> table_base_item::selectCellIndex()
{
    return m_buttonDownCellIndex;
}

void table_base_item::slotSetVariant(variant_info variable)
{
    QPair<int, int> cellIndex = selectCellIndex();
    setITableVariant(cellIndex, variable);
}

void table_base_item::setCellContentProperty(int row, int columnn, content_property property)
{
    QMutexLocker columnPropertyLocker(&m_columnPropertyMutex);
    // 列越界。[0]为表头交叉区域。
    if (columnn  >= m_columnProperty.count() || columnn < 0)
    {
        return;
    }

    // 行越界
    QMutexLocker rowPropertyLocker(&m_rowPropertyMutex);
    if (row >= m_rowProperty.count() || row < 0)
    {
        return ;
    }

    QMutexLocker cellContentPropertyLocker(&m_cellContentPropertyMutex);
    //    //QMap<QPair<int,int>,content_property>::iterator
    //    auto item =  m_cellContentProperty.find(QPair<int, int>(row, columnn));
    // 设置整表
    if (row == 0 && columnn == 0)
    {
        m_tableContentProperty = property;
        // 清空 行、列和单元格属性。
        m_cellContentProperty.clear();
        for (int i = 1; i < m_columnProperty.count(); ++i)
        {
            if (!m_columnProperty[i].contentProperty().isNull())
            {
                m_columnProperty[i].setContentProperty(content_property());
                m_columnProperty[i].setTimeCount(0);
            }
        }

        for (int i = 1; i < m_rowProperty.count(); ++i)
        {
            if (!m_rowProperty[i].contentProperty().isNull())
            {
                m_rowProperty[i].setContentProperty(content_property());
                m_rowProperty[i].setTimeCount(0);
            }
        }
        return;
    }

    // 设置行
    if (row != 0 && columnn == 0)
    {
        m_rowProperty[row].setContentProperty(property);
        m_rowProperty[row].setTimeCount(++contentPropertyTimeCount);
        // 清空单元格属性容器中带有该行号的属性
        /*QMap<QPair<int,int>,content_property>::iterator */
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {
            if (item.key().first == row)
            {
                item =  m_cellContentProperty.erase(item);
            }
            else
            {
                ++item;
            }
        }
    }

    // 设置列
    if (row == 0 && columnn != 0)
    {
        m_columnProperty[columnn].setContentProperty(property);
        m_columnProperty[columnn].setTimeCount(++contentPropertyTimeCount);
        // 清空单元格属性容器中带有该列号的属性
        for (auto item = m_cellContentProperty.begin(); item != m_cellContentProperty.end();)
        {
            if (item.key().second == columnn)
            {
                item =  m_cellContentProperty.erase(item);
            }
            else
            {
                ++item;
            }
        }
    }

    // 设置单元格
    if ((row != 0 && columnn != 0) &&
        (row != -1 && columnn != -1))
    {
        m_cellContentProperty[QPair<int, int>(row, columnn)] = property;
    }
}

void table_base_item::paintHeader(QPainter *painter)
{
    if (!m_showHeader)
    {
        return;
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    if (itemMode() == DesignMode)
    {
        painter->setBrush(m_headerBrush);
    }
    else
    {
        painter->setBrush(QBrush(TABLE_HEADER_CORLOR));
    }


    // 绘制灰色矩形区域
    QVector<QRectF> vec_rect;
    QRect HRect, VRect;
    // 水平矩形
    HRect.setTopLeft(cellRect(0, 0).topLeft());
    HRect.setWidth(cellRect(0, dataColumnCount()).x() + cellRect(0, dataColumnCount()).width());
    HRect.setHeight(cellRect(0, 0).height());
    if (rect().intersects(HRect))
    {
        vec_rect.push_back(rect().intersected(HRect));
    }

    // 垂直矩形
    VRect.setTopLeft(cellRect(0, 0).topLeft());
    VRect.setWidth(cellRect(0, 0).width());
    VRect.setHeight(cellRect(dataRowCount(), 0).y() + cellRect(dataRowCount(), 0).height());
    if (rect().intersects(VRect))
    {
        vec_rect.push_back(rect().intersected(VRect));
    }

    painter->drawRects(vec_rect);

    // 绘制标号
    QPen pen;
    pen.setWidthF(borderLineSize());
    pen.setColor(borderColor());
    painter->setPen(pen);
    painter->setFont(transformToSceneFont(QFont()));
    for (int i = 1; i <= dataColumnCount() ; ++i)
    {
        if (rect().intersects(cellRect(0, i)))
        {
            painter->drawText(rect().intersected(cellRect(0, i)), Qt::AlignCenter, QString().number(i));
        }
    }

    for (int i = 1; i <= dataRowCount() ; ++i)
    {
        if (rect().intersects(cellRect(i, 0)))
        {
            painter->drawText(rect().intersected(cellRect(i, 0)), Qt::AlignCenter, QString().number(i));
        }
    }

    painter->restore();
}

void table_base_item::paintGrid(QPainter *painter)
{
    if (!m_showGrid)
    {
        return;
    }

    painter->save();
    painter->setOpacity(1);
    QPen pen;
    pen.setWidthF(borderLineSize());
    pen.setColor(borderColor());
    painter->setPen(pen);
    QVector<QRectF> Rect_Vec;
    for (int i = 0; i <= dataColumnCount(); ++i)
    {
        for (int j = 0; j <= dataRowCount(); ++j)
        {
            QRect item = cellRect(j, i);

            // 包含也是相交
            if (rect().intersects(item))
            {
                Rect_Vec.push_back(rect().intersected(item));
            }
        }
    }
    painter->drawRects(Rect_Vec);
    painter->restore();
}

void table_base_item::paintSelectRect(QPainter *painter)
{
    if (isSelected())
    {
        painter->save();
        QPen pen(SELECT_CORLOR);
        pen.setWidth(SELECT_LINE_WIDTH);
        painter->setPen(pen);
        QRect headRect = cellRect(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second);

        QVector<QRectF> selectRectVec;
        // 列选
        if (m_buttonDownCellIndex.first == 0)
        {
            QRect tem = cellRect(dataRowCount(), m_buttonDownCellIndex.second);
            selectRectVec.push_back(rect().intersected(QRect(headRect.topLeft(),
                                                             QSize(headRect.width(), tem.y() + tem.height()))));
        }

        // 行选
        if (m_buttonDownCellIndex.second == 0)
        {
            QRect tem = cellRect(m_buttonDownCellIndex.first, dataColumnCount());
            selectRectVec.push_back(rect().intersected(QRect(headRect.topLeft(), QSize(tem.x() + tem.width(), headRect.height()))));
        }

        // 单元格选中
        if (m_buttonDownCellIndex.first != 0 && m_buttonDownCellIndex.second != 0)
        {
            QRect tem = cellRect(m_buttonDownCellIndex.first, m_buttonDownCellIndex.second);
            selectRectVec.push_back(rect().intersected(tem));
        }

        painter->drawRects(selectRectVec);
        painter->restore();
    }
}

void table_base_item::paintContent(QPainter *painter)
{
    painter->save();
    // 添加外边距（这里假设为10像素），保持rectContent大小不变
    // DEFAULT_ROW_OFFSET在另一个地方有关联
    const int margin = DEFAULT_ROW_OFFSET / 2;
    QRect outerRect;
    for (int i = 1; i <= dataColumnCount(); ++i)
    {
        for (int j = 1; j <= dataRowCount(); ++j)
        {
            cellContent data = contentData(j, i);
            QRect item = cellRect(j, i);
            content_property property = cellContentProperty(j, i);
            // 包含也是相交
            if (rect().intersects(item))
            {
                painter->setFont(transformToSceneFont(property.font()));
                painter->setPen(property.fontColor());

                QTextOption textOption;
                textOption.setWrapMode(QTextOption::WrapAnywhere);  // 最小单位为字母
                textOption.setAlignment(property.alignment());      // 设置对齐方式

                // 添加外边距（这里假设为10像素），保持rectContent大小不变
                outerRect = item.adjusted(margin, margin, -margin, -margin);
                // 使用outerRect作为绘制区域，但实际绘制文本的位置仍为rectContent
                painter->setClipRect(item);
                painter->drawText(outerRect, data.content(), textOption);
                painter->setClipping(false);
            }
        }
    }
    painter->restore();
}

void table_base_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget/* = Q_NULLPTR*/)
{
    setupPainter(painter);
    prepareRect(painter, nullptr, widget);

    fillPaint(painter, option, widget);
    paintHeader(painter);
    paintGrid(painter);
    paintContent(painter);
    paintSelectRect(painter);
    base_design_item::paint(painter, option, widget);
}

void table_base_item::fillPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void table_base_item::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    base_design_item::hoverMoveEvent(event);
}

void table_base_item::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    base_design_item::hoverLeaveEvent(event);
}

void table_base_item::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{

    base_design_item::hoverEnterEvent(event);
}

void table_base_item::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (getResizeDirectionFlags() == Fixed)
    {
        bool sizeNotChange = false;
        // 列宽改变
        if (m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second != 0)
        {
            resizeColumnWidth(event, m_buttonDownCellIndex.second);
        }// 行高改变
        else if (m_buttonDownCellIndex.first != 0 && m_buttonDownCellIndex.second == 0)
        {
            resizeRowHight(event, m_buttonDownCellIndex.first);
        }
        else if (m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second == 0)
        {
            resizeColumnWidth(event, m_buttonDownCellIndex.second);
            resizeRowHight(event, m_buttonDownCellIndex.first);
            //            base_design_item::mouseMoveEvent(event);
            //            sizeNotChange = true;
        }
        else
        {
            base_design_item::mouseMoveEvent(event);
            sizeNotChange = true;
        }

        if (!sizeNotChange)
        {
            setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
        }
        return;
    }

    if (getResizeDirectionFlags() & ResizeRight)
    {
        resizeColumnWidth(event, dataColumnCount());
    }

    if (getResizeDirectionFlags() & ResizeLeft)
    {
        resizeColumnWidth(event, dataColumnCount(), true);
    }

    if (getResizeDirectionFlags() & ResizeBottom)
    {
        resizeRowHight(event, dataRowCount());
    }

    if (getResizeDirectionFlags() & ResizeTop)
    {
        resizeRowHight(event, dataRowCount(), true);
    }

    base_design_item::mouseMoveEvent(event);
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
}

void table_base_item::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 更新数据到 属性节点
        emit propertyUpdate(PROPERTY_TO_STRING(FONT_PROPERTY));
        emit propertyUpdate(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY));
        emit propertyUpdate(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY));
        if (getResizeDirectionFlags() == Fixed)
        {
            m_buttonDownCellIndex = pointInCellIndex(event->pos().toPoint());
        }

        if (isPressHeaderRow() ||
            getResizeDirectionFlags() & ResizeRight ||
            getResizeDirectionFlags() & ResizeLeft)
        {
            m_temColumnProperty = m_columnProperty;
        }

        if (isPressHeaderCol() ||
            getResizeDirectionFlags() & ResizeBottom ||
            getResizeDirectionFlags() & ResizeTop)
        {
            m_temRowProperty = m_rowProperty;
        }
    }
    base_design_item::mousePressEvent(event);
    update();
}

void table_base_item::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isPressHeaderRow() ||
        getResizeDirectionFlags() & ResizeRight ||
        getResizeDirectionFlags() & ResizeLeft)
    {
        if (m_columnProperty != m_temColumnProperty)
        {
            QVariant newV, oldV;
            {
                QMutexLocker locker(&m_columnPropertyMutex);
                newV.setValue(m_columnProperty);
                oldV.setValue(m_temColumnProperty);
            }
            notify("columnProperty", oldV, newV);
        }
    }

    if (isPressHeaderCol() ||
        getResizeDirectionFlags() & ResizeBottom ||
        getResizeDirectionFlags() & ResizeTop)
    {
        if (m_rowProperty != m_temRowProperty)
        {
            QVariant newV, oldV;
            {
                QMutexLocker locker(&m_rowPropertyMutex);
                newV.setValue(m_rowProperty);
                oldV.setValue(m_temRowProperty);
            }
            notify("rowProperty", oldV, newV);
        }
    }

    base_design_item::mouseReleaseEvent(event);
}

void table_base_item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    base_design_item::mouseDoubleClickEvent(event);
}

void table_base_item::keyPressEvent(QKeyEvent *event)
{
    if (isPressHeaderRow())
    {
        m_temColumnProperty = m_columnProperty;
    }
    else if (isPressHeaderCol())
    {
        m_temRowProperty = m_rowProperty;
    }

    // I增加行
    if (event->key() == Qt::Key_I)
    {
        if (m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second != 0)
        {
            addColumn(m_buttonDownCellIndex.second);
        }
        else if (m_buttonDownCellIndex.second == 0 && m_buttonDownCellIndex.first != 0)
        {
            addRow(m_buttonDownCellIndex.first);
        }
    }

    // D删除
    if (event->key() == Qt::Key_D)
    {
        if (m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second != 0)
        {
            deleteColumn(m_buttonDownCellIndex.second);
        }
        else if (m_buttonDownCellIndex.second == 0 && m_buttonDownCellIndex.first != 0)
        {
            deleteRow(m_buttonDownCellIndex.first);
        }
        else if (m_buttonDownCellIndex.second == 0 && m_buttonDownCellIndex.first == 0)
        {
            setParentItem(nullptr);
            delete this;
            return;
        }
    }
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));

    QGraphicsItem::keyPressEvent(event);
}

void  table_base_item::keyReleaseEvent(QKeyEvent *event)
{
    if (isPressHeaderRow())
    {
        if (m_columnProperty != m_temColumnProperty)
        {
            QVariant newV, oldV;
            {
                QMutexLocker locker(&m_columnPropertyMutex);
                newV.setValue(m_columnProperty);
                oldV.setValue(m_temColumnProperty);
            }
            notify("columnProperty", oldV, newV);
        }
    }
    else if (isPressHeaderCol())
    {
        if (m_rowProperty != m_temRowProperty)
        {
            QVariant newV, oldV;
            {
                QMutexLocker locker(&m_rowPropertyMutex);
                newV.setValue(m_rowProperty);
                oldV.setValue(m_temRowProperty);
            }
            notify("rowProperty", oldV, newV);
        }
    }
    QGraphicsItem::keyReleaseEvent(event);
}

// 鼠标右击，弹出下拉菜单栏
void table_base_item::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    base_design_item::contextMenuEvent(event);
}

void table_base_item::preparePopUpMenu(QMenu &menu)
{
    if ((m_buttonDownCellIndex.first == 0 || m_buttonDownCellIndex.second == 0) &&
        !(m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second == 0)
       )
    {
        QAction *deleteAction = new  QAction(QIcon(":/tableDelete"), tr("delete"), &menu);
        deleteAction->setShortcut(QKeySequence(Qt::Key_D));
        menu.addAction(deleteAction);
    }

    if (m_buttonDownCellIndex.first == 0)
    {
        QAction *insertCloumn = new  QAction(QIcon(""), tr("insert cloumn"), &menu);
        insertCloumn->setShortcut(QKeySequence(Qt::Key_I));
        menu.addAction(insertCloumn);
    }

    if (m_buttonDownCellIndex.second == 0)
    {
        QAction *insertRow = new  QAction(QIcon(""), tr("insert row"), &menu);
        insertRow->setShortcut(QKeySequence(Qt::Key_I));
        menu.addAction(insertRow);
    }
}

void table_base_item::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("delete")) == 0)
    {
        if (m_buttonDownCellIndex.first == 0)
        {
            deleteColumn(m_buttonDownCellIndex.second);
        }
        else if (m_buttonDownCellIndex.second == 0)
        {
            deleteRow(m_buttonDownCellIndex.first);
        }
    }
    else if (action->text().compare(tr("insert cloumn")) == 0)
    {
        addColumn(m_buttonDownCellIndex.second);
    }
    else if (action->text().compare(tr("insert row")) == 0)
    {
        addRow(m_buttonDownCellIndex.first);
    }
    setGeometry(QRect(rect().topLeft().toPoint(), tableShowSize()));
}

bool table_base_item::isPressHeaderRow()
{
    return m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second != 0;
}

bool table_base_item::isPressHeaderCol()
{
    return m_buttonDownCellIndex.first != 0 && m_buttonDownCellIndex.second == 0;
}

bool table_base_item::isPressHeaderColRow()
{
    return m_buttonDownCellIndex.first == 0 && m_buttonDownCellIndex.second == 0;
}

}
