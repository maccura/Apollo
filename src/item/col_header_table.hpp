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
#ifndef COL_HEADER_TABLE_H
#define COL_HEADER_TABLE_H
#include "table_item.hpp"
namespace ReportDesign
{
class col_header_table : public table_item
{
    Q_OBJECT
public:
    col_header_table(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~col_header_table() {}
    virtual cellContent contentData(int row, int column) override;
    virtual base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void setAutoFillImpl(int dataRow, int dataCol, bool autoFill) override;
    virtual table_item *separateTable(int row, int pixel, int &deltaY);
    virtual QMap<qreal, qreal>  renderDataAdaptive() override;
    virtual void setAutoFill(bool autoFill) override;
    virtual int maxRowHeight(int row, QRect pageItemShowRect);
private:
    int addNewTableItem(int num, QMap<qreal, qreal> &deltaYMap);
    int contentAdapt();
};
}
#endif // COL_HEADER_TABLE_H
