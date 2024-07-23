#include "variable_image_item.hpp"
#include "design_elements_factory.hpp"
#include "mask_dialog/variable_edit_dialog.hpp"
#include "page_scene.hpp"

#define VARIABLE_IMAGE_SIZE  QSize(850,550)
namespace
{
ReportDesign::base_design_item *createImageItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::variable_image_item(owner, parent);
}
bool  registred = ReportDesign::DesignElementsFactory::instance().registerCreator(
                      ReportDesign::SVG_ITEM_TYPE, ReportDesign::ItemAttribs(QObject::tr("SVG Item"),
                                                                             ReportDesign::SVG_ITEM_TYPE), createImageItem);
}

namespace  ReportDesign
{
variable_image_item::variable_image_item(QObject *owner, QGraphicsItem *parent):
    image_base_item(ReportDesign::SVG_ITEM_TYPE, owner, parent), m_BGText(tr("SVG Item"))
{
    setSize(VARIABLE_IMAGE_SIZE);
    setBorderLinesFlags(AllLines);
}

QString variable_image_item::BGText()
{
    return m_BGText;
}

base_design_item *variable_image_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new variable_image_item(owner, parent);
}

QWidget *variable_image_item::defaultEditor()
{
    base_homogeneous_variant *GroupVariant = nullptr;
    base_homogeneous_variant *variant = nullptr;
    if (groupVariant() != variant_info())
    {
        GroupVariant =  device_variant_manager::instance().
                        HomogeneousVariant(page()->pageItem()->module(), VariantType::Group);
    }

    if (GroupVariant != nullptr)
    {
        variant = GroupVariant->typeChildVariants(groupVariant().m_ID, VariantType::Image);
    }
    else
    {
        variant = device_variant_manager::instance().
                  HomogeneousVariant(page()->pageItem()->module(), VariantType::Image);
    }

    variable_edit_dialog *dlg = new variable_edit_dialog(false, variant, m_variant);
    connect(dlg, SIGNAL(variable(variant_info)), this, SLOT(setVariant(variant_info)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    return dlg;
}

void variable_image_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (image().isNull())
    {
        painter->save();
        painter->fillRect(rect(), QBrush(Qt::gray));
        painter->restore();
    }
    image_base_item::paint(painter, option, widget);
    drawMasterMarkColor(painter);
}

variant_info variable_image_item::variant()
{
    return m_variant;
}

void variable_image_item::setVariant(variant_info variant)
{
    m_variant = variant;
    m_BGText = m_variant.m_name;
    //    if (!m_variant.m_ID.isEmpty())
    //    {
    //        m_BGText = m_variant.m_name;
    //    }
}

void variable_image_item::setVariantName(variant_info variant)
{
    if (!m_variant.m_nameChange)
    {
        m_variant.m_name =  variant.m_name;
        m_BGText = m_variant.m_name;
    }
}

void variable_image_item::reloadVariantName(const VariantNameMap &variantName)
{
    // repeat
    variant_info GroupVariant = groupVariant();
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (variantName.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupMap = variantName.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            if (groupMap.contains(m_variant.m_ID))
            {
                variant_info var = groupMap.value(m_variant.m_ID).value<variant_info>();
                setVariantName(var);
            }
        }
    }
    else
    {
        //
        if (variantName.contains(m_variant.m_ID))
        {
            variant_info var = variantName.value(m_variant.m_ID).value<variant_info>();
            setVariantName(var);
        }
    }
}

void variable_image_item::renderPrintData(const PrintDataMap &PrintData)
{
    variant_info GroupVariant = groupVariant();
    QString imageValue;
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (PrintData.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupMap = PrintData.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            if (groupMap.contains(m_variant.m_ID))
            {
                QVector<QString> valueArr = groupMap.value(m_variant.m_ID).value<QVector<QString>>();
                if (valueArr.count() > groupDataIndex())
                {
                    imageValue = valueArr[groupDataIndex()];
                }
            }
        }
    }
    else // 非repeat元件
    {
        if (PrintData.contains(m_variant.m_ID))
        {
            imageValue = PrintData.value(m_variant.m_ID).toString();
        }
    }

    //  图片数据处理
    if (imageValue.isEmpty())
    {
        return;
    }

    // 二進制
    if (m_variant.m_type == IMAGE_DATA_VARIANT_TYPE)
    {
        QByteArray binaryData = QByteArray::fromBase64(imageValue.toUtf8());
        QImage image;
        image.loadFromData(binaryData);
        setImage(image);
    }
    // 路徑
    else if (m_variant.m_type == IMAGE_PATH_VARIANT_TYPE)
    {
        setImage(QImage(imageValue));
    }
    // 压缩包
    else if (m_variant.m_type == IMAGE_GZBS_VARIANT_TYPE)
    {

    }
}

bool variable_image_item::printDataIsEmpty()
{
    return image().isNull();
}

void variable_image_item::clearPrintData()
{
    setImage(QImage());
}

void variable_image_item::clearVariant()
{
    setVariant(variant_info());
    m_BGText = "";
}

}
