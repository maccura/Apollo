#include "table_variant.hpp"
#include "device_variant_manager.hpp"
#include "global.hpp"
namespace ReportDesign
{
bool tableRegisterResult = device_variant_manager::instance().registerVariant(ReportDesign::VariantType::Table,
                                                                              [](QString modulejson)
{
    return new table_variant(modulejson);
});
table_variant::table_variant(QString moduleJson): base_homogeneous_variant(moduleJson)
{
    load();
}

QVector<variant_info> table_variant::childVariants(QString parentID)
{
    return tableVariant.value(parentID);
}

variant_info table_variant::childVariants(QString ParantID, QString ID)
{
    QVector<variant_info> tableVariantVec = tableVariant.value(ParantID);
    for (auto it : tableVariantVec)
    {
        if (it.m_ID == ID)
        {
            return it;
        }
    }

    return variant_info();
}

bool table_variant::inField(QString Field, const QJsonValue &jsonValue)
{
    if (Field == TABLE_VARIANT_TYPE)
    {
        QString  tableID = jsonValue.toArray().at(VARIANT_ID_INDEX).toString();
        QVector<variant_info> vec;
        QJsonArray tableVariantArr = jsonValue.toArray().at(VARIANT_TABLE_INDEX).toArray();
        for (const QJsonValue &it2 : tableVariantArr)
        {
            variant_info variant;
            variant.m_ID = it2.toArray().at(VARIANT_ID_INDEX).toString();
            variant.m_name = it2.toArray().at(VARIANT_NAME_INDEX).toString();
            variant.m_type = it2.toArray().at(VARIANT_TYPE_INDEX).toString();
            vec.push_back(variant);
        }
        tableVariant.insert(tableID, vec);
        return true;
    }
    return false;
}

}
