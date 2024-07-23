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
#include "image_base_item.hpp"
#include "page_scene.hpp"
#include "global.hpp"
namespace ReportDesign
{

image_base_item::image_base_item(QString itemTypeName, QObject *owner, QGraphicsItem *parent)
    : base_design_item(itemTypeName, owner, parent),
      m_scale(true), m_keepAspectRatio(true), m_center(true) {}

QString image_base_item::BGText()
{
    return QString();
}

QImage image_base_item::drawImage() const
{
    return image();
}

QByteArray image_base_item::imageAsByteArray()
{
    QByteArray result;
    QBuffer buffer(&result);
    buffer.open(QIODevice::WriteOnly);
    m_picture.save(&buffer, "PNG");

    return result;
}

void image_base_item::setImageAsByteArray(QByteArray image)
{
    QImage value;
    value.loadFromData(image);
    setImage(value);
}

QString image_base_item::fileFilter()
{
    return tr("Images (*.gif *.icns *.ico *.jpeg *.tga *.tiff *.wbmp *.webp *.png *.jpg *.bmp);;All(*.*)");
}

bool image_base_item::center() const
{
    return m_center;
}

void image_base_item::setCenter(bool center)
{
    if (m_center != center)
    {
        m_center = center;
        update();
        notify("center", !center, center);
    }
}

bool image_base_item::keepAspectRatio() const
{
    return m_keepAspectRatio;
}

void image_base_item::setKeepAspectRatio(bool keepAspectRatio)
{
    if (m_keepAspectRatio != keepAspectRatio)
    {
        m_keepAspectRatio = keepAspectRatio;
        update();
        notify("keepAspectRatio", !keepAspectRatio, keepAspectRatio);
    }
}

bool image_base_item::scale() const
{
    return m_scale;
}

void image_base_item::setScale(bool scale)
{
    if (m_scale != scale)
    {
        m_scale = scale;
        update();
        notify("scale", !scale, scale);
    }
}

void image_base_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    if (isSelected())
    {
        painter->setOpacity(ReportDesign::SELECTION_OPACITY);
    }
    else
    {
        painter->setOpacity(qreal(opacity()) / 100);
    }

    QPointF point = rect().topLeft();
    QImage img;

    if (m_scale && !drawImage().isNull())
    {
        img = drawImage().scaled(rect().width(), rect().height(), keepAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        img = drawImage();
    }

    qreal shiftHeight = rect().height() - img.height();
    qreal shiftWidth = rect().width() - img.width();

    if (m_center)
    {
        if (shiftHeight < 0 || shiftWidth < 0)
        {
            qreal cutX = 0;
            qreal cutY = 0;
            qreal cutWidth = img.width();
            qreal cutHeigth = img.height();

            if (shiftWidth > 0)
            {
                point.setX(point.x() + shiftWidth / 2);
            }
            else
            {
                cutX = fabs(shiftWidth / 2);
                cutWidth += shiftWidth;
            }

            if (shiftHeight > 0)
            {
                point.setY(point.y() + shiftHeight / 2);
            }
            else
            {
                cutY = fabs(shiftHeight / 2);
                cutHeigth += shiftHeight;
            }

            img = img.copy(cutX, cutY, cutWidth, cutHeigth);
        }
        else
        {
            point.setX(point.x() + shiftWidth / 2);
            point.setY(point.y() + shiftHeight / 2);
        }
    }

    if (img.isNull() && itemMode() == DesignMode)
    {
        painter->setFont(transformToSceneFont(QFont("Arial", 10)));
        painter->setPen(Qt::black);
        painter->drawText(rect().adjusted(4, 4, -4, -4), Qt::AlignCenter, BGText());
    }
    else
    {
        painter->drawImage(point, img);
    }

    painter->restore();
    base_design_item::paint(painter, option, widget);
}

void image_base_item::setImage(QImage value)
{
    if (m_picture != value)
    {
        QImage oldValue = m_picture;
        m_picture = value;
        update();
        notify("image", oldValue, value);
    }
}

QImage image_base_item::image() const
{
    return m_picture;
}

bool image_base_item::isEmpty() const
{
    return drawImage().isNull();
}

}
