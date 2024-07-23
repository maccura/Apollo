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
#ifndef ABSTACT_HOMOGENEOUS_VARIANT_H
#define ABSTACT_HOMOGENEOUS_VARIANT_H
#include <QString>
#include <QVector>
#include <QVariant>
#include <QMetaType>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#define VARIANT_ID_INDEX            0
#define VARIANT_NAME_INDEX          1
#define VARIANT_TYPE_INDEX          2
#define VARIANT_TABLE_INDEX         3
#define VARIANT_GROUP_DATA_INDEX    3
namespace ReportDesign
{

struct variant_info
{
    variant_info()
    {
        m_nameChange = false;
    }

    // 重载不等于运算符（成员函数版本）
    bool operator!=(const variant_info &other) const
    {
        return m_name != other.m_name ||
               m_ID != other.m_ID ||
               m_type != other.m_type ||
               m_nameChange != other.m_nameChange;
    }

    bool operator==(const variant_info &other) const
    {
        return m_name == other.m_name &&
               m_ID == other.m_ID &&
               m_type == other.m_type &&
               m_nameChange == other.m_nameChange;
    }

    QString m_name;
    QString m_ID;
    QString m_type;
    bool m_nameChange;
};

class base_homogeneous_variant
{
public:
    base_homogeneous_variant(QString moduleJson);
    virtual ~base_homogeneous_variant() {}
    // 获取当前种类的变量集合
    virtual QVector<variant_info> variant();

    // 获取当前类型下，某个ID的变量信息
    virtual variant_info variantInfo(QString ID);

    // 当前种类的变量集合下，再按父节点ID获取变量集合,用于有层次结构的变量结构，比如table
    virtual QVector<variant_info> childVariants(QString ParantID);

    // 当前种类的变量集合下，再按父节点ID获取变量集合,用于有层次结构的变量结构，比如table
    virtual variant_info childVariants(QString ParantID, QString ID)
    {
        Q_UNUSED(ParantID)
        Q_UNUSED(ID)
        return variant_info();
    }

    // 符合类型变量，子级里面包含多种类型的变量，如group类型的变量结构
    virtual base_homogeneous_variant *typeChildVariants(QString ID, VariantType type)
    {
        Q_UNUSED(ID)
        Q_UNUSED(type)
        return nullptr;
    }

protected:
    // 类型字段过滤器
    virtual bool inField(QString Field, const QJsonValue &jsonValue) = 0;
    void load();
protected:
    QJsonDocument m_jsonDoc;
    QVector<variant_info> m_variants;
};
}
Q_DECLARE_METATYPE(ReportDesign::variant_info)
#endif // ABSTACT_HOMOGENEOUS_VARIANT_H
