#include "print_data_manager.hpp"
#include <QMetaType>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include <QGraphicsScene>
#include <QJsonDocument>
#include "item/table_item.hpp"
#include "item/col_header_table.hpp"
#include "global.hpp"
namespace ReportDesign
{

int retQMapTableItemMap = qRegisterMetaType<QMap<int, ConnectTableInfo>>("QMap<int, ConnectTableInfo>");

print_data_manager::print_data_manager(QObject *parent): QObject(parent)
{

}

PrintDataMap print_data_manager::printData(QString json)
{
    PrintDataMap dataMap = gen_print_data(json);
    // 分割数据
    for (QMap<int, ConnectTableInfo>::iterator it = m_connectTableMap.begin(); it != m_connectTableMap.end(); it++)
    {
        // 拿到被关联的table中的Item
        base_design_item *Item;
        emit idFindTableItem(it.key(), Item);
        table_item *connectTableItem = dynamic_cast<table_item *>(Item);
        if (connectTableItem)
        {
            // 找表
            //QString str = connectTableItem->tableVariantID().m_ID;
            PrintDataMap::const_iterator varint = dataMap.find(connectTableItem->tableVariantID().m_ID);
            if (varint != dataMap.end())
            {
                // 整表数据
                QMap<QString, QVector<QString>> printDataMap = (*varint).value<QMap<QString, QVector<QString>>>();

                // 遍历关联tableItem,拿到新的索引和分割的行数
                ConnectTableInfo tableItemList = it.value();
                int offset = 0;
                for (auto tableID : tableItemList.tableIDList)
                {
                    base_design_item *Item;
                    emit idFindTableItem(tableID, Item);
                    table_item *tableItem = dynamic_cast<table_item *>(Item);
                    col_header_table *colHeaderTableItem = dynamic_cast<col_header_table *>(tableItem);
                    if (tableItem)
                    {
                        int offsetNum;
                        QString newkey = tableItem->tableVariantID().m_ID + TABELITEMUUSTR + QString::number(tableItem->tableItemID());
                        if (colHeaderTableItem)
                        {
                            offsetNum = tableItem->availableColCount();
                        }
                        else
                        {
                            offsetNum = tableItem->availableRowCount();
                        }
                        QMap<QString, QVector<QString>> newPrintDataMap;
                        // 把整表数据分割为多段，变量不变
                        for (QMap<QString, QVector<QString>>::iterator it2 = printDataMap.begin(); it2 != printDataMap.end(); it2++)
                        {
                            QVector<QString> colData;
                            if (it2.value().count() >= offset)
                            {
                                colData = it2.value().mid(offset, offsetNum);
                                newPrintDataMap.insert(it2.key(), colData);
                            }
                        }
                        offset += offsetNum;
                        QVariant PrintVaule;
                        PrintVaule.setValue(newPrintDataMap);
                        dataMap.insert(newkey, PrintVaule);

                    }
                }
            }
        }
    }
    return dataMap;
}

VariantNameMap print_data_manager::variantNameMap(QString module, QString json)
{
    VariantNameMap variantNameMap;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8());
    if (jsonDocument.isEmpty())
    {
        return VariantNameMap();
    }

    QJsonObject object = jsonDocument.object();
    if (!object.value("print_module_ds_list").isArray())
    {
        return VariantNameMap();
    }

    QJsonArray module_ds_list = object.value("print_module_ds_list").toArray();
    for (const QJsonValue &it : module_ds_list)
    {
        // 每个模块Json对象
        if (it.isObject())
        {
            QStringList keyList = it.toObject().keys();
            if (!keyList.isEmpty() &&
                keyList[0] ==  module)
            {
                QJsonValue moduleJson = it.toObject().value(keyList[0]);
                if (moduleJson.isArray())
                {
                    QJsonArray moduleDataArr = moduleJson.toArray();
                    for (int i = 0; i < moduleDataArr.count(); ++i)
                    {
                        QVariant variant;
                        variant_info variantInfo;
                        if (i == 0)
                        {
                            // 模块描述信息
                            if (moduleDataArr[i].isString())
                            {
                                variantInfo.m_ID = keyList[0];
                                variantInfo.m_name = moduleDataArr[i].toString();
                                variant.setValue(variantInfo);
                                variantNameMap.insert(variantInfo.m_ID, variant);
                            }
                        }
                        else
                        {
                            if (moduleDataArr[i].isArray())
                            {
                                QJsonArray variantArr =  moduleDataArr[i].toArray();
                                if (variantArr.count() >= 3)
                                {
                                    if (variantArr[2].isString())
                                    {
                                        QString VarType = variantArr[2].toString();
                                        if (VarType != TABLE_VARIANT_TYPE && VarType != GROUP_VARIANT_TYPE)// 非表和组
                                        {
                                            variantInfo.m_ID = variantArr[0].toString();
                                            variantInfo.m_name = variantArr[1].toString();
                                            variantInfo.m_type = variantArr[2].toString();
                                            variant.setValue(variantInfo);
                                            variantNameMap.insert(variantInfo.m_ID, variant);
                                        }
                                        else if (VarType == TABLE_VARIANT_TYPE) // 表格
                                        {
                                            QMap<QString, variant_info> variantVector;
                                            if (variantArr.count() >= 4 && variantArr[3].isArray())
                                            {
                                                QJsonArray tableVariantArr = variantArr[3].toArray();
                                                for (const QJsonValue &it : tableVariantArr)
                                                {
                                                    // 表头变量item
                                                    if (it.isArray() && it.toArray().count() > 2)
                                                    {
                                                        variant_info headerVariant;
                                                        QJsonArray tableHeaderVariant = it.toArray();
                                                        headerVariant.m_ID = tableHeaderVariant[0].toString();
                                                        headerVariant.m_name = tableHeaderVariant[1].toString();
                                                        headerVariant.m_type = tableHeaderVariant[2].toString();
                                                        variantVector.insert(headerVariant.m_ID, headerVariant);
                                                    }
                                                }
                                            }

                                            variant.setValue(variantVector);
                                            variantNameMap.insert(variantArr[0].toString(), variant);
                                        }
                                        else if (VarType == GROUP_VARIANT_TYPE) // 组
                                        {
                                            VariantNameMap groupNameMap;
                                            if (variantArr.count() >= 4 && variantArr[3].isArray())
                                            {
                                                QJsonArray GroupVariantArr = variantArr[3].toArray();
                                                for (const QJsonValue &it : GroupVariantArr)
                                                {
                                                    if (it.isArray())
                                                    {
                                                        QJsonArray GroupItemVariantArr = it.toArray();
                                                        QString GroupItemType = GroupItemVariantArr[2].toString();
                                                        if (GroupItemType != TABLE_VARIANT_TYPE && GroupItemType != GROUP_VARIANT_TYPE) // 非表格
                                                        {
                                                            variant_info var;
                                                            var.m_ID = GroupItemVariantArr[0].toString();
                                                            var.m_name = GroupItemVariantArr[1].toString();
                                                            var.m_type = GroupItemVariantArr[2].toString();

                                                            QVariant QVar;
                                                            QVar.setValue(var);
                                                            groupNameMap.insert(var.m_ID, QVar);
                                                        }
                                                        else // 表格
                                                        {
                                                            QMap<QString, variant_info> variantVector;
                                                            if (GroupItemVariantArr.count() >= 4 && GroupItemVariantArr[3].isArray())
                                                            {
                                                                QJsonArray tableVariantArr = GroupItemVariantArr[3].toArray();
                                                                for (const QJsonValue &it : tableVariantArr)
                                                                {
                                                                    // 表头变量item
                                                                    if (it.isArray() && it.toArray().count() > 2)
                                                                    {
                                                                        variant_info headerVariant;
                                                                        QJsonArray tableHeaderVariant = it.toArray();
                                                                        headerVariant.m_ID = tableHeaderVariant[0].toString();
                                                                        headerVariant.m_name = tableHeaderVariant[1].toString();
                                                                        headerVariant.m_type = tableHeaderVariant[2].toString();
                                                                        variantVector.insert(headerVariant.m_ID, headerVariant);
                                                                    }
                                                                }
                                                            }

                                                            QVariant QVar;
                                                            QVar.setValue(variantVector);
                                                            groupNameMap.insert(GroupItemVariantArr[0].toString(), QVar);
                                                        }
                                                    }
                                                }
                                            }
                                            variant.setValue(groupNameMap);
                                            variantNameMap.insert(variantArr[0].toString(), variant);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return variantNameMap;
}

void print_data_manager::slotAddConnectTableItem(int addTableItemID, int ConnectTableItemID)
{
    QMutexLocker locker(&m_mapMutex);
    // 被关联的表格控件首次建立节点
    if (addTableItemID == ConnectTableItemID)
    {
        ConnectTableInfo Tlist;
        Tlist.tableIDList.push_back(addTableItemID);
        m_connectTableMap.insert(ConnectTableItemID, Tlist);
    }
    else
    {
        if (m_connectTableMap.contains(ConnectTableItemID))
        {
            m_connectTableMap[ConnectTableItemID].tableIDList.push_back(addTableItemID);
        }
    }
}

void print_data_manager::slotDeleteConnectTableItem(int deleteTableItemID, int ConnectTableItemID)
{
    QMutexLocker locker(&m_mapMutex);
    ConnectTableInfo TableItemList = m_connectTableMap.value(ConnectTableItemID);
    if (!TableItemList.tableIDList.isEmpty())
    {
        // 被关联的表格控件被删除，（1）删除对应关联映射中的节点 （2）与所它关联的所有控件也直接删除
        if (deleteTableItemID == ConnectTableItemID)
        {
            for (QList<int>::iterator it = TableItemList.tableIDList.begin(); it != TableItemList.tableIDList.end(); it++)
            {
                base_design_item  *tableItem = nullptr;
                emit idFindTableItem(*it, tableItem);
                if (tableItem &&                // 删除逻辑，被关联Item已经被删除，tableItem为空结果为false
                    *it !=  ConnectTableItemID) // 解除锁定逻辑，被关联Item不能被删除
                {
                    tableItem->setParentItem(nullptr);
                    tableItem->setParent(nullptr);
                    tableItem->scene()->removeItem(tableItem);
                    tableItem->deleteLater();
                }
            }

            // 删除被关联的节点。
            QMap<int, ConnectTableInfo>::iterator it = m_connectTableMap.find(ConnectTableItemID);
            if (it != m_connectTableMap.end())
            {
                m_connectTableMap.erase(it);
            }
        }
        else
        {
            // 非 被关联的表格控件被删除
            for (QList<int>::iterator it = TableItemList.tableIDList.begin(); it != TableItemList.tableIDList.end(); it++)
            {
                if (*it == deleteTableItemID)
                {
                    TableItemList.tableIDList.erase(it);
                    m_connectTableMap[ConnectTableItemID] = TableItemList;
                    return;
                }
            }
        }
    }
}

PrintDataMap print_data_manager::gen_print_data(QString json)
{
    PrintDataMap printData;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject jsonObject;
    if (!jsonDocument.isObject())
    {
        return PrintDataMap();
    }

    jsonObject = jsonDocument.object();
    // 遍历对象中的所有键
    for (const QString &key : jsonObject.keys())
    {
        QJsonValue value = jsonObject.value(key);
        QJsonValue::Type valueType = value.type();
        QVariant printVaule;
        if (valueType == QJsonValue::String)
        {
            printVaule.setValue(value.toString());
            printData.insert(key, printVaule);
        }
        // 看看整数是否也是进这个if
        else if (valueType == QJsonValue::Double)
        {
            printVaule.setValue(QString("%1").arg(value.toDouble()));
            printData.insert(key, printVaule);
        }
        // 表格
        else if (valueType == QJsonValue::Object)
        {
            QMap<QString, QVector<QString>> tableMap;
            QVector<QString> vecKeys;
            QVector<QVector<QString>> vecVaules;
            QJsonObject objectValue = value.toObject();
            // 处理对象类型的值
            if (objectValue.contains("header"))
            {
                QJsonValue tableHeader = objectValue.value("header");
                if (tableHeader.isArray())
                {
                    QJsonArray tableHeaderArr = tableHeader.toArray();
                    for (const QJsonValue &headerKey : tableHeaderArr)
                    {
                        if (headerKey.isString())
                        {
                            vecKeys.push_back(headerKey.toString());
                        }

                    }
                }
            }

            if (objectValue.contains("items"))
            {
                QJsonValue items = objectValue.value("items");
                if (items.isArray())
                {
                    QJsonArray itemsArr = items.toArray();
                    for (const QJsonValue &tableValueItems : itemsArr)
                    {
                        if (tableValueItems.isArray())
                        {
                            QJsonArray ColItems = tableValueItems.toArray();
                            // 表头变量数量和数据的列数应该是相等的
                            if (vecKeys.count() != ColItems.count())
                            {
                                break;
                            }

                            if (vecVaules.count() != vecKeys.count())
                            {
                                vecVaules.resize(vecKeys.count());
                            }

                            bool ret = true;
                            for (int i = 0; i < ColItems.count(); ++i)
                            {
                                QString ColValue;
                                QJsonValue ColValueItems = ColItems.at(i);
                                if (ColValueItems.isDouble())
                                {
                                    ColValue = QString("%1").arg(ColValueItems.toInt());
                                }
                                else if (ColValueItems.isString())
                                {
                                    ColValue =  ColValueItems.toString();
                                }
                                else if (ColValueItems.isNull())
                                {
                                    ColValue = "";
                                }
                                else //未知类型，数据格式错误
                                {
                                    ret = false;
                                    break;
                                }
                                vecVaules[i].push_back(ColValue);
                            }

                            // 表格数据出现异常
                            if (!ret)
                            {
                                vecVaules.clear();
                                break;
                            }
                        }
                    }
                }
            }

            // 表格数据出现异常
            if (vecVaules.count() != vecKeys.count() ||
                vecVaules.count() == 0)
            {
                continue;
            }

            for (int i = 0; i < vecKeys.size(); ++i)
            {
                tableMap.insert(vecKeys.at(i), vecVaules.at(i));
            }
            QVariant tablePrintVaule;
            tablePrintVaule.setValue(tableMap);
            printData.insert(key, tablePrintVaule);
        }//group 数据
        else if (valueType == QJsonValue::Array)
        {
            gen_group_data(printData, key, value);
        }
    }
    return printData;
}

void print_data_manager::gen_group_data(PrintDataMap &map, QString GroupKey, QJsonValue &groupJson)
{
    PrintDataMap groupMap;
    if (!groupJson.isArray())
    {
        return;
    }

    QJsonArray groupArr = groupJson.toArray();
    for (const QJsonValue &Item : groupArr)
    {
        if (Item.isObject())
        {
            QJsonObject groupObj = Item.toObject();
            // 遍历对象中的所有键
            for (const QString &key : groupObj.keys())
            {
                QJsonValue value = groupObj.value(key);
                QJsonValue::Type valueType = value.type();
                QVariant printVaule;
                if (valueType == QJsonValue::Array)
                {
                    QJsonArray valueArr = value.toArray();
                    if (valueArr.count() == 0)
                    {
                        continue;
                    }

                    // 拿数组中的首元素判断其类型。
                    QJsonValue::Type groupItemType = valueArr.first().type();
                    if (groupItemType == QJsonValue::String)
                    {
                        QVector<QString> groupItemValueArr;
                        // 解析每个item中的value（arr）
                        for (const QJsonValue &groupItemValue : valueArr)
                        {
                            groupItemValueArr.push_back(groupItemValue.toString());
                        }
                        printVaule.setValue(groupItemValueArr);
                        groupMap.insert(key, printVaule);
                    }
                    // 看看整数是否也是进这个if
                    else if (groupItemType == QJsonValue::Double)
                    {
                        QVector<QString> groupItemValueArr;
                        // 解析每个item中的value（arr）
                        for (const QJsonValue &groupItemValue : valueArr)
                        {
                            groupItemValueArr.push_back(QString("%1").arg(groupItemValue.toDouble()));
                        }
                        printVaule.setValue(groupItemValueArr);
                        groupMap.insert(key, printVaule);
                    }
                }
                else if (valueType == QJsonValue::Object)
                {
                    QVector<QMap<QString, QVector<QString>>> vectorTableMap;
                    QMap<QString, QVector<QString>> tableMap;
                    QVector<QString> vecKeys;
                    QJsonObject objectValue = value.toObject();
                    // 处理对象类型的值
                    if (objectValue.contains("header"))
                    {
                        QJsonValue tableHeader = objectValue.value("header");
                        if (tableHeader.isArray())
                        {
                            QJsonArray tableHeaderArr = tableHeader.toArray();
                            for (const QJsonValue &headerKey : tableHeaderArr)
                            {
                                if (headerKey.isString())
                                {
                                    vecKeys.push_back(headerKey.toString());
                                }

                            }
                        }
                    }

                    if (objectValue.contains("items"))
                    {
                        QJsonValue items = objectValue.value("items");
                        if (items.isArray())
                        {
                            // 每一块
                            QJsonArray block_Arr = items.toArray();
                            for (const QJsonValue &blockValueItems : block_Arr)
                            {
                                QVector<QVector<QString>> vecVaules;
                                // 每一个表
                                QJsonArray itemsArr = blockValueItems.toArray();
                                for (const QJsonValue &tableValueItems : itemsArr)
                                {
                                    if (tableValueItems.isArray())
                                    {
                                        QJsonArray ColItems = tableValueItems.toArray();
                                        // 表头变量数量和数据的列数应该是相等的
                                        if (vecKeys.count() != ColItems.count())
                                        {
                                            break;
                                        }

                                        if (vecVaules.count() != vecKeys.count())
                                        {
                                            vecVaules.resize(vecKeys.count());
                                        }

                                        bool ret = true;
                                        for (int i = 0; i < ColItems.count(); ++i)
                                        {
                                            QString ColValue;
                                            QJsonValue ColValueItems = ColItems.at(i);
                                            if (ColValueItems.isDouble())
                                            {
                                                ColValue = QString("%1").arg(ColValueItems.toInt());
                                            }
                                            else if (ColValueItems.isString())
                                            {
                                                ColValue =  ColValueItems.toString();
                                            }
                                            else if (ColValueItems.isNull())
                                            {
                                                ColValue = "";
                                            }
                                            else //未知类型，数据格式错误
                                            {
                                                ret = false;
                                                break;
                                            }
                                            vecVaules[i].push_back(ColValue);
                                        }

                                        // 表格数据出现异常
                                        if (!ret)
                                        {
                                            vecVaules.clear();
                                            break;
                                        }
                                    }
                                }
                                // 表格数据出现异常
                                if (vecVaules.count() != vecKeys.count() ||
                                    vecVaules.count() == 0)
                                {
                                    continue;
                                }

                                for (int i = 0; i < vecKeys.size(); ++i)
                                {
                                    tableMap.insert(vecKeys.at(i), vecVaules.at(i));
                                }
                                vectorTableMap.push_back(tableMap);
                            }
                        }
                    }

                    QVariant tablePrintVaule;
                    tablePrintVaule.setValue(vectorTableMap);
                    groupMap.insert(key, tablePrintVaule);
                }
                //group 数据
            }
        }
    }

    map[GroupKey] = groupMap;
}

}
