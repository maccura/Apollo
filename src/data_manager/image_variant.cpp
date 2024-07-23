#include "image_variant.hpp"
#include "device_variant_manager.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include "global.hpp"

namespace ReportDesign
{

bool imageRegisterResult = device_variant_manager::instance().registerVariant(ReportDesign::VariantType::Image,
                                                                              [](QString modulejson)
{
    return new image_variant(modulejson);
});
image_variant::image_variant(QString moduleJson): base_homogeneous_variant(moduleJson)
{
    load();
}

bool image_variant::inField(QString Field, const QJsonValue &jsonValue)
{
    if (Field == IMAGE_DATA_VARIANT_TYPE || Field == IMAGE_PATH_VARIANT_TYPE ||
        Field == IMAGE_GZBS_VARIANT_TYPE)
    {
        return true;
    }
    return false;
}

}
