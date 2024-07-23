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
#ifndef PAGE_NUMBER_ITEM_H
#define PAGE_NUMBER_ITEM_H
#include "text_item.hpp"
namespace ReportDesign
{
class page_number_item: public TextItem
{
    Q_OBJECT
public:
    // 上下左右边框
    Q_PROPERTY(BorderLines BORDER_PROPERTY READ borderLines WRITE setBorderLinesFlags)
    Q_PROPERTY(qreal BORDERLINESIZE_PROPERTY READ borderLineSize WRITE setBorderLineSize)
    Q_PROPERTY(QColor BORDERCOLOR_PROPERTY READ borderColor WRITE setBorderColor)
    page_number_item(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~page_number_item() {}
    void setPageNumber(int current, int total);
    virtual QWidget *defaultEditor() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    virtual void renderPrintData(const PrintDataMap &PrintData) override;
    virtual void clearPrintData() override;
    virtual  base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual bool variantIsEmpty() override
    {
        return true;
    }
private:
    void updatePageNumber();
};
}
#endif // PAGE_NUMBER_ITEM_H
