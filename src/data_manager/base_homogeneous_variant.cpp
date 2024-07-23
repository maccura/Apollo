#include "base_homogeneous_variant.hpp"
namespace ReportDesign
{
int res = qRegisterMetaType<variant_info>("variant_info");
base_homogeneous_variant::base_homogeneous_variant(QString moduleJson)
{
    m_jsonDoc = QJsonDocument::fromJson(moduleJson.toUtf8());
}

void base_homogeneous_variant::load()
{
    if (m_variants.isEmpty())
    {
        if (!m_jsonDoc.isObject())
        {
            return;
        }

        QJsonObject moduleJsonObject = m_jsonDoc.object();
        QStringList keys = moduleJsonObject.keys();
        if (keys.isEmpty())
        {
            return;
        }

        if (!moduleJsonObject.value(keys[0]).isArray())
        {
            return;
        }

        QJsonArray modeArr = moduleJsonObject.value(keys[0]).toArray();
        for (const QJsonValue &it : modeArr)
        {
            if (it.isArray())
            {
                QJsonArray itArr = it.toArray();
                QJsonValue variantType = itArr.at(VARIANT_TYPE_INDEX);
                if (inField(variantType.toString(), it))
                {
                    variant_info variant;
                    variant.m_ID = itArr.at(VARIANT_ID_INDEX).toString();
                    variant.m_name = itArr.at(VARIANT_NAME_INDEX).toString();
                    variant.m_type = itArr.at(VARIANT_TYPE_INDEX).toString();
                    m_variants.push_back(variant);
                }
            }
        }
    }
}

QVector<variant_info> base_homogeneous_variant::variant()
{
    return m_variants;
}

// 获取当前类型下，某个ID的变量信息
variant_info base_homogeneous_variant::variantInfo(QString ID)
{
    variant_info variant;
    for (auto it : m_variants)
    {
        if (it.m_ID == ID)
        {
            variant = it;
            break;
        }
    }
    return variant;
}

QVector<variant_info> base_homogeneous_variant::childVariants(QString ParantID)
{
    return QVector<variant_info>();
}


}
