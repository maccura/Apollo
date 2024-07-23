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
#ifndef PRINT_DATA_MANAGER_HPP
#define PRINT_DATA_MANAGER_HPP

#include <QString>
#include <QVariant>
#include <QMap>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QMutex>
#include <QMetaType>

namespace ReportDesign
{
using PrintDataMap = QMap<QString, QVariant>;
using VariantNameMap = QMap<QString, QVariant>;
class base_design_item;

struct ConnectTableInfo
{
    QList<int> tableIDList;
};

class print_data_manager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMap<int, ConnectTableInfo> connectTableMap READ connectTableMap WRITE setConnectTableMap)
public:
    print_data_manager(QObject *parent = nullptr);
    PrintDataMap printData(QString json);
    static VariantNameMap variantNameMap(QString module, QString json);
signals:
    // 需要直接连接，拿到table对象
    void idFindTableItem(int tableItemID, base_design_item *&);
public slots:
    void slotAddConnectTableItem(int addTableItemID, int ConnectTableItemID);
    void slotDeleteConnectTableItem(int deleteTableItemID, int ConnectTableItemID);
private:
    QMap<int, ConnectTableInfo> connectTableMap()
    {
        return m_connectTableMap;
    }
    void setConnectTableMap(QMap<int, ConnectTableInfo> TableMap)
    {
        m_connectTableMap = TableMap;
    }
    PrintDataMap gen_print_data(QString json);
    void gen_group_data(PrintDataMap &map, QString key, QJsonValue &groupJson);
    QMutex m_mapMutex;
    QMap<int, ConnectTableInfo> m_connectTableMap;
};
}
Q_DECLARE_METATYPE(ReportDesign::ConnectTableInfo)

#endif // PRINT_DATA_MANAGER_HPP
