#include "page_render_factory.hpp"
#include <memory>
namespace ReportDesign
{
PageRenderFactor::PageRenderFactor()
{

}

PageRenderFactor &PageRenderFactor::instance()
{
    static PageRenderFactor instanceObj;
    return instanceObj;
}

bool PageRenderFactor::registerRender(QString Type, RenderCreator creator)
{
    if (creator ==  nullptr)
    {
        return false;
    }
    m_renderCreatorMap[Type] = creator;
    return true;
}

abstract_render::renderPtr PageRenderFactor::creator(QString Type)
{
    if (!m_renderCreatorMap.contains(Type))
    {
        return nullptr;
    }
    return abstract_render::renderPtr(m_renderCreatorMap[Type]());
}

}
