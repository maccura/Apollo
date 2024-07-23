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
#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP
#include <QObject>
#include <QJsonDocument>
#include <QStringList>
#include <QVector>
#include <functional>
#include "base/singleton.hpp"
#include "base_homogeneous_variant.hpp"
#include "base/simple_abstract_factory.hpp"
#include "ini_manager.hpp"
namespace ReportDesign
{
class device_variant_manager: public singleton<device_variant_manager>
{
public:
    using Creator = std::function<base_homogeneous_variant*(QString)>;
    // 返回仪器列表（通过读取指定目录下的所有json文件中device 字段）
    QStringList deviceList(QString filePath, ini_manager::Languages language);

    // 加载对应设备类型的变量文件
    bool loadDeviceJson(QString device);

    QString DeviceJson();

    // 当前加载配置文件设备ID
    QString DeviceID()
    {
        return m_deviceID;
    }

    // 返回当前设备文件中的模块列表
    QList<variant_info> moduleList();

    // 返回模块中某一类型的变量
    base_homogeneous_variant *HomogeneousVariant(QString module, VariantType variantType);

    // 注册变量类型，用于实现变量类型的扩展
    bool  registerVariant(VariantType variantType, Creator  creator);

    // 根据对应模块名生产出模拟数据。
    QString genSimulatedData(QString module);

    QMap<VariantType, Creator> &creatorMap()
    {
        return m_homogeneous_variant_map;
    }
private:
    QString  imageSimulatedBa64File(QString fileName);
    bool formatVerification(QJsonDocument jsonDoc);
    QString device(QString fileName);
    QString getDeviceIDFromfile(QString filePath);
    bool isCurrentLanguage(QString filePath);
private:
    // json文件对象
    QJsonDocument m_jsonDoc;
    QMap<VariantType, Creator> m_homogeneous_variant_map;
    QMap<QString, QString> m_deviceType_deviceFile_map;
    QString m_deviceID;
    ini_manager::Languages m_language;
};

}

#endif
