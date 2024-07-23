/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
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
#include "shape_item.hpp"
#include "design_elements_factory.hpp"
#include <QStyleOptionGraphicsItem>
#include <QPainter>

namespace
{
ReportDesign::base_design_item *createShapeItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::shape_item(owner, parent);
}
bool registred = ReportDesign::DesignElementsFactory::instance().registerCreator(
                     ReportDesign::SHAPE_ITEM_TYPE, ReportDesign::ItemAttribs(QObject::tr("Shape Item"), "Item"), createShapeItem
                 );
}

namespace ReportDesign
{

shape_item::shape_item(QObject *owner, QGraphicsItem *parent)
    : base_design_item(ReportDesign::SHAPE_ITEM_TYPE, owner, parent),
      m_shape(HorizontalLine),
      m_shapeColor(Qt::black),
      m_shapeBrushColor(Qt::black),
      m_shapeBrushType(Qt::NoBrush),
      m_lineWidth(1),
      m_penStyle(Qt::SolidLine),
      m_cornerRadius(0)
{
}

void shape_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    painter->save();

    QPen pen(m_shapeColor);
    pen.setWidthF(m_lineWidth);
    pen.setStyle(m_penStyle);
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);
    QBrush brush(m_shapeBrushColor, m_shapeBrushType);
    brush.setTransform(painter->worldTransform().inverted());
    painter->setBrush(brush);
    painter->setBackground(QBrush(Qt::NoBrush));
    painter->setOpacity(qreal(opacity()) / 100);

    QRectF rectangleRect = rect().adjusted((borderLineSize() / 2),
                                           (borderLineSize() / 2),
                                           -(borderLineSize() / 2),
                                           -(borderLineSize() / 2));

    switch (m_shape)
    {
    case HorizontalLine:
        painter->drawLine(0, rect().height() / 2, rect().right(), rect().height() / 2);
        break;
    case VerticalLine:
        painter->drawLine(rect().width() / 2, 0, rect().width() / 2, rect().height());
        break;
    }

    painter->restore();
    base_design_item::paint(painter, option, widget);
    drawMasterMarkColor(painter);
}

void shape_item::setBorderColor(QColor value)
{
    if ((value != m_shapeColor))
    {
        QColor oldValue = m_shapeColor;
        m_shapeColor = value;
        update();
        notify("shapeColor", oldValue, value);
    }
}

void shape_item::setShapeType(shape_item::ShapeType value)
{
    if (m_shape != value)
    {
        ShapeType oldValue = m_shape;
        m_shape = value;
        update();
        notify("shape", oldValue, value);
    }
}

void shape_item::setBorderLineSize(qreal value)
{
    if (m_lineWidth != value)
    {
        qreal oldValue = m_lineWidth;
        m_lineWidth = value;
        update();
        notify("lineWidth", oldValue, value);

    }
}

base_design_item *shape_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new shape_item(owner, parent);
}


}
