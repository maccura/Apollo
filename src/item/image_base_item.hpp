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
#ifndef IMAGEITEM_H
#define IMAGEITEM_H
#include "base_design_item.hpp"
#include <QtGlobal>

namespace ReportDesign
{

class image_base_item : public base_design_item
{
    Q_OBJECT
    Q_PROPERTY(bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio)
    // 上下左右边框
    Q_PROPERTY(BorderLines BORDER_PROPERTY READ borderLines WRITE setBorderLinesFlags)
    Q_PROPERTY(qreal BORDERLINESIZE_PROPERTY READ borderLineSize WRITE setBorderLineSize)
    Q_PROPERTY(QColor BORDERCOLOR_PROPERTY READ borderColor WRITE setBorderColor)
public:
    enum Format
    {
        Binary  = 0,
        Hex     = 1,
        Base64  = 2
    };
#if QT_VERSION >= 0x050500
    Q_ENUM(Format)
#else
    Q_ENUMS(Format)
#endif

    image_base_item(QString itemTypeName, QObject *owner, QGraphicsItem *parent);
    virtual ~image_base_item() {}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QString BGText();

protected:
    void setImage(QImage value);
    QImage image() const;

    bool scale() const;
    void setScale(bool scale);
    bool keepAspectRatio() const;
    void setKeepAspectRatio(bool keepAspectRatio);
    bool center() const;
    void setCenter(bool center);
    QByteArray imageAsByteArray();
    void setImageAsByteArray(QByteArray image);
    QString fileFilter();
private:
    QImage drawImage() const;
private:
    QImage  m_picture;
    bool    m_scale;
    bool    m_keepAspectRatio;
    bool    m_center;

public:
    bool isEmpty() const override;
};

}
#endif // IMAGEITEM_H
