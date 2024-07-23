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
#ifndef DESIGNELEMENTSFACTORY_H
#define DESIGNELEMENTSFACTORY_H

#include "base/attribs_abstract_factory.hpp"
#include "base/singleton.hpp"
#include "base_design_item.hpp"

namespace ReportDesign
{

typedef base_design_item *(*CreateBand)(QObject *owner, base_design_item  *parent);

struct ItemAttribs
{
    QString m_alias;
    QString m_tag;
    ItemAttribs() {}
    ItemAttribs(const QString &alias, const QString &tag): m_alias(alias), m_tag(tag) {}
    bool operator==(const ItemAttribs &right) const
    {
        return (m_alias == right.m_alias) && (m_tag == right.m_tag);
    }
};

class DesignElementsFactory : public attribs_abstract_factory<base_design_item, QString, CreateBand, ItemAttribs>
{
private:
    // friend class singleton<DesignElementsFactory>;
private:
    DesignElementsFactory() {}
    ~DesignElementsFactory() {}
    DesignElementsFactory(const DesignElementsFactory &) {}
    DesignElementsFactory &operator = (const DesignElementsFactory &)
    {
        return *this;
    }
};

}
#endif // DESIGNELEMENTSFACTORY_H
