#include "group_variant.hpp"
#include "global.hpp"
#include "device_variant_manager.hpp"

namespace ReportDesign
{

bool group_variant_result = device_variant_manager::instance().registerVariant(ReportDesign::VariantType::Group, [](QString modulejson)
{
    return new group_variant(modulejson);
});

group_variant::group_variant(QString moduleJson): base_homogeneous_variant(moduleJson)
{
    load();
}

bool group_variant::inField(QString Field, const QJsonValue &jsonValue)
{
    if (Field == GROUP_VARIANT_TYPE)
    {
        if (!jsonValue.isArray())
        {
            return true;
        }

        // 组装数据
        QJsonObject newJson;
        QJsonArray  dataArr;
        QJsonArray  inDataArr = jsonValue.toArray();
        QJsonArray  goupDataArr = inDataArr.at(VARIANT_GROUP_DATA_INDEX).toArray();
        // 第一个元素为 group 的name(描述信息)
        dataArr.push_back(inDataArr.at(VARIANT_NAME_INDEX).toString());
        //dataArr[0] = inDataArr.at(VARIANT_NAME_INDEX).toString();
        for (QJsonValue item : goupDataArr)
        {
            dataArr.push_back(item);
        }

        QString ID = inDataArr.at(VARIANT_ID_INDEX).toString();
        newJson[ID] = dataArr;
        m_groupJson[ID] =  QJsonDocument(newJson).toJson();
        return true;
    }
    return false;
}

// 符合类型变量，子级里面包含多种类型的变量，如group类型的变量结构
base_homogeneous_variant *group_variant::typeChildVariants(QString ID, VariantType type)
{
    //  组数据组装为json格式
    //  groupItems:
    //      ["一组数据",
    //          [ "groupName", "姓名", "ST" ],
    //          [ "groupPicBase64Data", "图片1", "BS" ],
    //          [ "groupVecItems", "项目信息", "TB",
    //              [
    //                  [ "uItemCode", "项目通道号", "ST" ],
    //                  [ "strItemName", "项目名称", "ST" ],
    //                  [ "strPrintResult", "测试结果", "ST" ]
    //              ]
    //          ]
    //      ]
    if (!m_groupJson.contains(ID))
    {
        return nullptr;
    }

    return device_variant_manager::instance().creatorMap()[type](m_groupJson.value(ID));
}
}
