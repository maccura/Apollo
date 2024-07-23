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
#ifndef TABLE_TEXT_ITEM_H
#define TABLE_TEXT_ITEM_H
#include "table_base_item.hpp"
#include "mask_dialog/variable_edit_dialog.hpp"
namespace ReportDesign
{

class group_text_item: public table_base_item
{
    Q_OBJECT
public:
    group_text_item(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~group_text_item() {}
    virtual cellContent contentData(int row, int column) override;
    virtual base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0) override;
    virtual void  mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QWidget *defaultEditor() override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    virtual void fillPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    virtual void reloadVariantName(const VariantNameMap &variantName);
    virtual bool  printDataIsEmpty();
    virtual void renderPrintData(const PrintDataMap &PrintData) override;
    virtual void clearPrintData() override;
    virtual bool variantIsEmpty() override;
    virtual void clearVariant() override;
private:
    void paintRect(QPainter *painter);
private:
    QMutex m_printDataMapMutex;
    QMap<QString, QString> m_printDataMap;
};
}

#endif // TABLE_TEXT_ITEM_H
