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
#ifndef WATER_MARK_ITEM_HPP
#define WATER_MARK_ITEM_HPP

#include <QPainter>
#include <QPixmap>
#include <QGraphicsItem>
#include "base_design_item.hpp"
#include "text_item.hpp"
namespace ReportDesign
{

class watermark_item: public TextItem
{
    Q_OBJECT
    Q_PROPERTY(QString watermarkText READ watermarkText WRITE setWatermarkText)
    Q_PROPERTY(QImage watermarkImage READ watermarkImage WRITE setWatermarkImage)
public:
    watermark_item(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~watermark_item() {}
    QString watermarkText()
    {
        return m_watermarkText;
    }
    void setWatermarkText(QString watermarkText)
    {
        m_watermarkText = watermarkText;
        setContent(m_watermarkText);
    }
    QImage watermarkImage()
    {
        return m_watermarkImage;
    }
    void setWatermarkImage(QImage watermarkImage)
    {
        m_watermarkImage = watermarkImage;
    }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    virtual base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual void setItemMode(base_design_item::ItemMode mode) override;
    virtual void renderPrintData(const PrintDataMap &PrintData) override;
    virtual void clearPrintData() override;
    virtual bool variantIsEmpty() override
    {
        return true;
    }
private:
    void paintText(QPainter *painter);
    void paintImage(QPainter *painter);
private:
    QString m_watermarkText;
    QImage  m_watermarkImage;
};
}
#endif // WATER_MARK_ITEM_HPP
