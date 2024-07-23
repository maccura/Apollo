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
#ifndef SHAPEITEM_H
#define SHAPEITEM_H
#include "base_design_item.hpp"
#include "global.hpp"
#include <QtGlobal>

namespace ReportDesign
{

class shape_item: public base_design_item
{
    Q_OBJECT
    Q_PROPERTY(ShapeType SHAPE_PROPERTY READ shapeType WRITE setShapeType)
    Q_PROPERTY(QColor BORDERCOLOR_PROPERTY READ borderColor WRITE setBorderColor)
    Q_PROPERTY(qreal BORDERLINESIZE_PROPERTY READ borderLineSize WRITE setBorderLineSize)
public:
    enum ShapeType {HorizontalLine, VerticalLine/*,Ellipse,Rectangle*/};
#if QT_VERSION >= 0x050500
    Q_ENUM(ShapeType)
#else
    Q_ENUMS(ShapeType)
#endif
    shape_item(QObject *owner, QGraphicsItem *parent);
    virtual ~shape_item() {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void    setBorderColor(QColor value);
    QColor  borderColor() const
    {
        return m_shapeColor;
    }
    void        setShapeType(ShapeType value);
    ShapeType   shapeType() const
    {
        return m_shape;
    }
    void    setBorderLineSize(qreal value);
    qreal   borderLineSize() const
    {
        return m_lineWidth;
    }
    virtual bool variantIsEmpty() override
    {
        return true;
    }
protected:
    base_design_item *createSameTypeItem(QObject *owner, QGraphicsItem *parent);
private:
    ShapeType m_shape;
    QColor m_shapeColor;
    QColor m_shapeBrushColor;
    Qt::BrushStyle m_shapeBrushType;
    qreal m_lineWidth;
    Qt::PenStyle m_penStyle;
    //    int m_opacity;
    int m_cornerRadius;
};

}
#endif // SHAPEITEM_H
