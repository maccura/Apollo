#include "property_widget_factory.hpp"
namespace ReportDesign
{
bool  property_widget_factory::registerPropertyWidget(QString property, CreatePropWidget  creator)
{
    m_createPropWidgetMap[property] = creator;
    return true;
}

CreatePropWidget property_widget_factory::creator(QString property)
{
    if (m_createPropWidgetMap.contains(property))
    {
        return m_createPropWidgetMap.value(property);
    }

    return nullptr;
}

}
