#include "text_variant.hpp"
#include "device_variant_manager.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include "global.hpp"
namespace ReportDesign
{

bool resrult = device_variant_manager::instance().registerVariant(ReportDesign::VariantType::Text, [](QString modulejson)
{
    return new text_variant(modulejson);
});

text_variant::text_variant(QString moduleJson): base_homogeneous_variant(moduleJson)
{
    load();
}

bool text_variant::inField(QString Field, const QJsonValue &jsonValue)
{
    if (Field == TEXT_VARIANT_TYPE || Field == NM_VARIANT_TYPE)
    {
        return true;
    }
    return false;
}

}
