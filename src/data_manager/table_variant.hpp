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
#ifndef TABLE_VARIANT_HPP
#define TABLE_VARIANT_HPP
#include "base_homogeneous_variant.hpp"
#include <QMap>
#include <QJsonDocument>
namespace ReportDesign
{

class table_variant: public base_homogeneous_variant
{
public:
    table_variant(QString moduleJson);
    ~table_variant() {}
    virtual QVector<variant_info> childVariants(QString parentID) override;
    virtual variant_info childVariants(QString ParantID, QString ID) override;
    virtual bool inField(QString Field, const QJsonValue &jsonValue) override;
private:
    QMap<QString, QVector<variant_info>> tableVariant;
};
}
#endif // TABLE_VARIANT_HPP
