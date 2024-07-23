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
#ifndef VARIABLE_IMAGE_ITEM_H
#define VARIABLE_IMAGE_ITEM_H

#include "image_base_item.hpp"
#include "data_manager/base_homogeneous_variant.hpp"
namespace  ReportDesign
{
class variable_image_item : public image_base_item
{
    Q_OBJECT
    // 变量
    Q_PROPERTY(variant_info variant READ variant WRITE setVariant)
public:
    variable_image_item(QObject *owner, QGraphicsItem *parent);
    virtual ~variable_image_item() {}
    virtual QString BGText() override;
    virtual base_design_item *createSameTypeItem(QObject *owner, QGraphicsItem *parent) override;
    virtual QWidget *defaultEditor() override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    variant_info variant();

    virtual void reloadVariantName(const VariantNameMap &variantName) override;
    virtual void renderPrintData(const PrintDataMap &PrintData) override;
    virtual bool printDataIsEmpty();
    virtual void clearPrintData() override;
    virtual bool drawDesignBorders() const
    {
        return true;
    }
    virtual bool variantIsEmpty() override
    {
        return m_variant.m_ID.isEmpty();
    }
    virtual void clearVariant() override;
public slots:
    void setVariant(variant_info variant);
private:
    void setVariantName(variant_info variant);

private:
    variant_info m_variant;
    QString m_BGText;
};
}

#endif // VARIABLE_IMAGE_ITEM_H
