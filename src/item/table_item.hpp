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
#ifndef TABLE_ITEM_H
#define TABLE_ITEM_H
#include "table_base_item.hpp"
namespace ReportDesign
{

class table_item : public table_base_item
{
    Q_OBJECT
    // 自动填充
    Q_PROPERTY(bool TABLE_AUTO_FILL_PROPERTY READ autoFill WRITE setAutoFill)
    // 无数据时隐藏表格
    //Q_PROPERTY(bool hideTable  READ hideTable WRITE setHideTable)
    // 首行下划线
    Q_PROPERTY(bool TABLE_UNDER_LINE_PROPERTY  READ underLine WRITE setUnderLine)
    // 表格变量ID（一个模块中可能有多个表格变量，用于区分是哪个表格变量）
    Q_PROPERTY(variant_info tableVariantID  READ tableVariantID WRITE setTableVariantID DESIGNABLE false)
    // 表格控件ID
    Q_PROPERTY(int TABLE_TABLEITEM_ID_PROPERTY  READ tableItemID WRITE setTableItemID DESIGNABLE false)
    Q_PROPERTY(int TABLE_CONNECTTABLEITEMID_PROPERTY  READ connectTableItemID WRITE setconnectTableItemID DESIGNABLE false)
    // 关联ID对应的color,关联拷贝的时候使用
    Q_PROPERTY(QColor designHeaderColor  READ designHeaderColor WRITE setDesignHeaderColor DESIGNABLE false)
public:
    table_item(QObject *owner = 0, QGraphicsItem *parent = 0);
    table_item(QString storageTypeName, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~table_item() {}
    virtual cellContent contentData(int row, int column) override;
    virtual base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0) override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QWidget *defaultEditor() override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    virtual void reloadVariantName(const VariantNameMap &variantName);
    virtual void renderPrintData(const PrintDataMap &PrintData) override;
    virtual bool printDataIsEmpty();
    // 根据渲染数据做控件的尺寸适配,表格包括行数和每行的行高
    virtual QMap<qreal, qreal>  renderDataAdaptive() override;

    virtual void clearPrintData() override;
    virtual bool drawDesignBorders() const
    {
        return true;
    }
    virtual bool variantIsEmpty() override;
    virtual void clearVariant() override;

    // 自动填充子类可以调用的逻辑
    virtual void setAutoFillImpl(int dataRow, int dataCol, bool autoFill) {}
    // 从给定的row，向下截断表格，并把截断的内容作为新的表格控件返回
    virtual table_item *separateTable(int row, int pixel, int &deltaY);
    //
    virtual int maxRowHeight(int row, QRect pageItemShowRect);

    static int genTableItemID();
    bool isConnectTableItem();

    int connectTableItemID();
    void setconnectTableItemID(int ID);

    int tableItemID()
    {
        return m_tableItemID;
    }
    void setTableItemID(int ID)
    {
        m_tableItemID = ID;
    }
    // 用于显示渲染的数据行数
    int availableRowCount();

    // 用于显示渲染的数据列数
    int availableColCount();

    variant_info tableVariantID()
    {
        return m_tableVariant;
    }
    void setTableVariantID(variant_info Variant)
    {
        m_tableVariant = Variant;
    }

    QColor designHeaderColor()
    {
        return m_designHeaderColor;
    }
    void setDesignHeaderColor(QColor Color)
    {
        m_designHeaderColor = Color;
        setHeaderBrush(QBrush(m_designHeaderColor));
    }

    int pixelToRow(int pixel);
    int pixelToBottomDistance(int pixel);
    void setPrintDataMap(QMap<QString, QVector<QString>> printDataMap);
    QMap<QString, QVector<QString>> printDataMap()
    {
        QMutexLocker lock(&m_printDataMapMutex);
        return m_printDataMap;
    }

    void deleteBelowRow(int row);
    void deleteRangeRow(int begin, int end);
public slots:
    void slotSetTableVariant(variant_info itemVariant, variant_info tableVariant);
    void SetAutoFillAction(bool AutoFill);
protected:
    QRect adpContent(QFont font, QRect rect, QString content);
    QRect adpContent_c(QFont font, QRect rect, QString content);
    QRect adpContent(QFont font, QRect rect, QString content, Qt::Alignment Alignment);

    void drawUnderLine(QPainter *painter);
    bool autoFill();
    virtual void setAutoFill(bool autoFill);
    //    bool keepHeader();
    //    void setKeepHeader(bool keepHeader);
    bool hideTable();
    void setHideTable(bool hideTable);

    bool underLine();
    void setUnderLine(bool underLine);
protected:
    // 自动填充
    bool m_autoFill;
    //    // 跨页保持表头
    //    bool m_keepHeader;
    // 隐藏表格（无数据时，隐藏表格）
    bool m_hideTable;
    // 首行下滑线
    bool m_underLine;

    QMutex m_printDataMapMutex;
    // 打印数据
    QMap<QString, QVector<QString>> m_printDataMap;

    // 表格ID及表名等
    variant_info m_tableVariant;

    // 表格控件ID
    int m_tableItemID;
    // 被关联的表格控件ID
    int m_connectTableItemID;

    //
    QColor m_designHeaderColor;
};
}

#endif // TABLE_ITEM_H
