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
#ifndef RESOURCE_PATH_IMAGE_ITEM_H
#define RESOURCE_PATH_IMAGE_ITEM_H
#include "image_base_item.hpp"
namespace  ReportDesign
{
class resource_path_image_item: public image_base_item
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)
public:
    resource_path_image_item(QObject *owner, QGraphicsItem *parent);
    virtual ~resource_path_image_item() {}
    virtual QString BGText() override;
    virtual base_design_item *createSameTypeItem(QObject *owner, QGraphicsItem *parent) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    virtual bool    printDataIsEmpty();
    virtual bool variantIsEmpty() override
    {
        return true;
    }
protected:
    virtual void  mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};
}


#endif // RESOURCE_PATH_IMAGE_ITEM_H
