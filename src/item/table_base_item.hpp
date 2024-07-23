/***************************************************************************
*   This file is part of the Andromedae project                           *
*   Copyright (C) 2024 by Mike Medical Electronics Co., Ltd               *
*   Andromedae@maccura.com                                                *
*                                                                         *
**                   GNU General Public License Usage                    **
*                                                                         *
*   This library is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation, either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*   You should have received a copy of the GNU General Public License     *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
*                                                                         *
**                  GNU Lesser General Public License                    **
*                                                                         *
*   This library is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU Lesser General Public License as        *
*   published by the Free Software Foundation, either version 3 of the    *
*   License, or (at your option) any later version.                       *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library.                                      *
*   If not, see <http://www.gnu.org/licenses/>.                           *
*                                                                         *
*   This library is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
****************************************************************************/
#ifndef TABLE_BASE_ITEM_H
#define TABLE_BASE_ITEM_H

#include "base_design_item.hpp"
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QMutex>
#include <QMap>
#include <QPair>
#include <QSharedPointer>
#include <QVariant>
#include <QList>
#include <QMetaType>
#include "data_manager/base_homogeneous_variant.hpp"

// 表头灰色 Qt::gray
#define TABLE_HEADER_CORLOR  QColor(0xed, 0xee, 0xf0)
// 表格item默认间隔
#define DEFAULT_ROW_OFFSET    12

namespace ReportDesign
{

// 记录每一列的属性(不包含表头，只设置数据的属性)
class content_property
{
public:
    content_property(): m_isNull(true), m_font(QFont("Arial", 10)),
        m_alignment(Qt::AlignLeft | Qt::AlignTop) {}
    content_property(QFont font, Qt::Alignment alignment, QColor fontColor):
        m_isNull(false), m_font(font), m_alignment(alignment), m_fontColor(fontColor) {}

    bool operator==(const content_property &property) const
    {
        return  m_font == property.m_font
                && m_alignment == property.m_alignment
                && m_fontColor == property.m_fontColor
                && m_isNull == property.m_isNull;
    }

    QFont font()
    {
        return m_font;
    }
    void setFont(QFont font)
    {
        m_font = font;
        // m_isNull = false;
    }
    Qt::Alignment alignment()
    {
        return m_alignment;
    }
    void setAlignment(Qt::Alignment alignment)
    {
        m_alignment = alignment;
        // m_isNull = false;
    }

    QColor fontColor()
    {
        return m_fontColor;
    }
    void setFontColor(QColor vaule)
    {
        m_fontColor = vaule;
    }

    void setIsNull(bool isNull)
    {
        m_isNull = isNull;
    }
    bool isNull()
    {
        return m_isNull;
    }
public:
    bool m_isNull;
    // 内容格式
    QFont m_font;
    // 内容对齐
    Qt::Alignment m_alignment;
    // 字体颜色
    QColor m_fontColor;
};

class column_property
{
public:
    column_property(int columnPosx, int width, content_property contentProperty = content_property(), unsigned int timeCount = 0):
        m_columnPosx(columnPosx),
        m_width(width),
        m_contentProperty(contentProperty),
        m_contentPropertyTimeCount(timeCount) {}
    // 列的x坐标（列宽）
    int columnPosx()
    {
        return m_columnPosx;
    }
    void setColumnPosx(int columnPosx)
    {
        m_columnPosx = columnPosx;
    }
    int width()
    {
        return m_width;
    }
    void setWidth(int width)
    {
        m_width = width;
    }
    content_property contentProperty()
    {
        return m_contentProperty;
    }
    void setContentProperty(content_property contentProperty)
    {
        m_contentProperty = contentProperty;
    }
    unsigned int timeCount()
    {
        return m_contentPropertyTimeCount;
    }
    void setTimeCount(unsigned int timeCount)
    {
        m_contentPropertyTimeCount = timeCount;
    }

    bool operator==(const column_property &property) const
    {
        return  m_columnPosx == property.m_columnPosx
                && m_width == property.m_width
                && m_contentProperty == property.m_contentProperty
                && m_contentPropertyTimeCount == property.m_contentPropertyTimeCount;
    }
public:
    // 头部的坐标位置
    int m_columnPosx;
    // 列宽度
    int m_width;
    // 文字属性
    content_property m_contentProperty;
    // 文字属性时间计数，仅用来比较大小,没有实际意义
    unsigned int m_contentPropertyTimeCount;
};

class row_property
{
public:
    row_property(int rowPosY, int hight, content_property contentProperty = content_property(), unsigned int timeCount = 0):
        m_rowPosY(rowPosY),
        m_height(hight),
        m_contentProperty(contentProperty),
        m_contentPropertyTimeCount(timeCount) {}
    // 列的x坐标（列宽）
    int rowPosY()
    {
        return m_rowPosY;
    }
    int height()
    {
        return m_height;
    }
    void setRowPosY(int rowPosY)
    {
        m_rowPosY = rowPosY;
    }
    void setHeight(int height)
    {
        m_height = height;
    }
    content_property contentProperty()
    {
        return m_contentProperty;
    }
    void setContentProperty(content_property contentProperty)
    {
        m_contentProperty = contentProperty;
    }
    unsigned int timeCount()
    {
        return m_contentPropertyTimeCount;
    }
    void setTimeCount(unsigned int timeCount)
    {
        m_contentPropertyTimeCount = timeCount;
    }

    bool operator==(const row_property &property) const
    {
        return  m_rowPosY == property.m_rowPosY
                && m_height == property.m_height
                && m_contentProperty == property.m_contentProperty
                && m_contentPropertyTimeCount == property.m_contentPropertyTimeCount;
    }

public:
    // 头部的坐标位置
    int m_rowPosY;
    // 行高
    int m_height;
    // 文字属性
    content_property m_contentProperty;
    // 文字属性时间计数，仅用来比较大小
    unsigned int m_contentPropertyTimeCount;
};

class cellContent
{
public:
    cellContent(QString content): m_content(content) {}
    QString content()
    {
        return m_content;
    }
    void setContent(QString content)
    {
        m_content = content;
    }
private:
    // 展示的内容
    QString m_content;
};

class table_base_item: public base_design_item
{
    Q_OBJECT
    // 行数 （不包含）
    Q_PROPERTY(int TABLE_ROW_COUNT_PROPERTY READ dataRowCount WRITE setDataRowCount)
    // 行高（设置整个表，实际每一行的行高存放在行属性中，这里是统一设置，单行的设置用鼠标拖动表头设置）
    Q_PROPERTY(int TABLE_ROW_HEIGHT_PROPERTY READ rowHeight WRITE setRowHeight)
    // 列数（和行数类似)
    Q_PROPERTY(int TABLE_COLUMN_COUNT_PROPERTY READ dataColumnCount WRITE setDataColumnCount)
    // 列宽
    Q_PROPERTY(int TABLE_COLUMN_WIDTH_PROPERTY READ columnWidth WRITE setColumnWidth)
    // 显示网格
    Q_PROPERTY(bool TABLE_SHOW_GRID_PROPERTY READ showGrid WRITE setShowGrid)
    // 显示表头
    Q_PROPERTY(bool TABLE_SHOW_HEADER_PROPERTY READ showHeader WRITE setShowHeader)
    // 字体格式(表格、行列、单元软件编辑时使用), xml中保存无实际意义
    Q_PROPERTY(QFont FONT_PROPERTY READ font WRITE setFont)
    Q_PROPERTY(QColor FONT_COLOR_PROPERTY READ fontColor WRITE setFontColor)
    // 字体对齐(表格、行列、单元格软件编辑时使用), xml中保存无实际意义
    Q_PROPERTY(Qt::Alignment ALIGNMENT_PROPERTY READ alignment WRITE setAlignment)
    // 列属性(每一列的列宽，列数，序列化)
    Q_PROPERTY(QList<column_property> columnProperty READ columnProperty WRITE setColumnProperty)
    // 行属性(每一行的行高，行数，序列化)
    Q_PROPERTY(QList<row_property> rowProperty READ rowProperty WRITE setRowProperty)
    // 整表文本属性，序列化
    Q_PROPERTY(content_property tableContentProperty READ tableContentProperty WRITE setTableContentProperty)
    // 单元文本属性，序列化
    Q_PROPERTY(QMap<QPair<int, int>, content_property> TableCellContentProperty READ TableCellContentProperty WRITE setTableCellContentProperty)
    // 属性时间戳
    Q_PROPERTY(unsigned int propertyTimeCount READ propertyTimeCount WRITE setPropertyTimeCount)
    // 变量属性 （序列化）
    Q_PROPERTY(QMap<QPair<int, int>, variant_info> tableVariant READ tableVariant WRITE setTableVariant)
    // 上下左右边框
    Q_PROPERTY(BorderLines BORDER_PROPERTY READ borderLines WRITE setBorderLinesFlags)
    Q_PROPERTY(qreal BORDERLINESIZE_PROPERTY READ borderLineSize WRITE setBorderLineSize)
    Q_PROPERTY(QColor BORDERCOLOR_PROPERTY READ borderColor WRITE setBorderColor)
public:
    table_base_item(QString storageTypeName, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~table_base_item() {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    void init();
    // 填充类的绘制
    virtual void fillPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

    // 数据列数(不含表头)
    int dataColumnCount();
    void setDataColumnCount(int dataColumnCount);

    // 数据行数(不含表头)
    int dataRowCount();
    void setDataRowCount(int dataRowCount);
signals:
    void clickCellIndex(int row, int column);
protected:
    // 鼠标移动触发，按住鼠标左键移动不触发（处理的光标变化逻辑）
    virtual void  hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void  hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void  hoverEnterEvent(QGraphicsSceneHoverEvent *) override;

    // 按住鼠标左键并移动鼠标才会触发mouseMoveEvent(尺寸化和元件位置移动逻辑处理)
    virtual void  mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void  mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void  mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    // 键盘按下
    virtual void  keyPressEvent(QKeyEvent *event) override;
    virtual void  keyReleaseEvent(QKeyEvent *event) override;

    // 鼠标右击，弹出下拉菜单栏
    virtual void  contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    virtual void preparePopUpMenu(QMenu &menu) override;
    virtual void processPopUpAction(QAction *action) override;

    // 展示内容和风格由子类提供
    virtual cellContent contentData(int row, int column) = 0;

    virtual void paintHeader(QPainter *painter);


    // 0为起始,row = 0 || column == 0 都是表头索引, row和columnn越界时 ，返回空矩形
    QRect cellRect(int row, int columnn);
    // 0为起始,row = 0 || column == 0 都是表头索引, row和columnn越界时, 返回空content_property.isNull() == true;
    content_property cellContentProperty(int row, int columnn);

    // 软件编辑时使用，序列化时不用该函数（编辑时有优先级的判定，此逻辑不用于序列化）。
    void setCellContentProperty(int row, int columnn, content_property property);

    QPair<int, int> selectCellIndex();

public slots:
    void slotSetVariant(variant_info variable);

protected:
    bool isPressHeaderRow();
    bool isPressHeaderCol();
    bool isPressHeaderColRow();

    void paintGrid(QPainter *painter) ;
    void paintSelectRect(QPainter *painter);
    void paintContent(QPainter *painter);

    void setHeaderBrush(QBrush brush)
    {
        m_headerBrush = brush;
    }
    // 返回Table的展示大小
    QSize tableShowSize();

    // QPair<row,col> , 0为起始,row = 0 || column == 0 都是表头索引, 不在区域内时，返回QPair<int,int>(-1,-1)
    QPair<int, int> pointInCellIndex(const QPoint &pos);

    void resizeColumnWidth(QGraphicsSceneMouseEvent *event, int col, bool opposite = false);
    void resizeRowHight(QGraphicsSceneMouseEvent *event, int row, bool opposite = false);
    void resizeRowHight(int delta, int row, bool opposite = false);

    int pixelToRow(int pixel);
    void deleteColumn(int col);
    void deleteRow(int row);
    void deleteRowRange(int begin, int end);

    // 在索引为indexCol的列前面增加一列
    void addColumn(int indexCol);
    // 在索引为indexRow的行前面增加一行
    void addRow(int indexRow);

    // 整表的列宽，不包含单列定制化后的宽度
    int columnWidth();
    // 设置整表的列宽
    void setColumnWidth(int columnWidth);

    // 整表的行高度
    int rowHeight();
    // 设置整表的行高
    void setRowHeight(int rowHeight);

    bool showGrid();
    void setShowGrid(bool showGrid);

    bool showHeader();
    void setShowHeader(bool showHeader);

    QFont font();
    void setFont(QFont font);

    QColor fontColor();
    void setFontColor(QColor color);

    Qt::Alignment alignment();
    void setAlignment(Qt::Alignment alignment);

    QList<column_property> columnProperty();
    void setColumnProperty(QList<column_property> columnProperty);

    QList<row_property> rowProperty();
    void setRowProperty(QList<row_property> rowProperty);

    unsigned int propertyTimeCount();
    void setPropertyTimeCount(unsigned int TimeCount);

    content_property tableContentProperty();
    void setTableContentProperty(content_property tableContentProperty);

    QMap<QPair<int, int>, content_property> TableCellContentProperty();
    void setTableCellContentProperty(QMap<QPair<int, int>, content_property> cellContentProperty);

    // 序列化使用
    QMap<QPair<int, int>, variant_info> tableVariant();
    void setTableVariant(QMap<QPair<int, int>, variant_info> Variants);

    variant_info getTableVariant(QPair<int, int> index);
    void setITableVariant(QPair<int, int> index, variant_info Variant);

private:
    // 列属性（包含文字属性）
    QMutex m_columnPropertyMutex;
    // [0]为表头交叉区域，存列的位置,因为允许用户编辑列宽，所以需要记录每一列的属性数据
    QList<column_property> m_columnProperty;
    QList<column_property> m_temColumnProperty;
    // 行属性
    QMutex m_rowPropertyMutex;
    // 同上
    QList<row_property> m_rowProperty;
    QList<row_property> m_temRowProperty;

    // 整表文字属性
    content_property m_tableContentProperty;
    //
    QMutex m_cellContentPropertyMutex;
    // 每个单元格属性（存放对单个单元格进行文字属性设置的集合）
    QMap<QPair<int, int>, content_property> m_cellContentProperty;


    //
    QMutex m_variantMutex;
    // 变量map
    QMap<QPair<int, int>, variant_info> m_variant;

    // 列宽
    int m_columnWidth;
    // 行高（表头和数据行高）
    int m_rowHeight;

    // 显示表头
    bool m_showHeader;
    // 显示网格
    bool m_showGrid;

    // 鼠标按下时的表格index
    QPair<int, int> m_buttonDownCellIndex;

    // 文字属性时间计数（时间戳）Base,用来比较行属性和列属性的设置先后顺序
    unsigned int contentPropertyTimeCount;

    // 编辑模式下的表头刷子
    QBrush m_headerBrush;
};
}
Q_DECLARE_METATYPE(QList<ReportDesign::column_property>)
Q_DECLARE_METATYPE(QList<ReportDesign::row_property>)
Q_DECLARE_METATYPE(ReportDesign::content_property)


#endif // TABLE_BASE_ITEM_H
