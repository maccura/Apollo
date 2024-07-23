#include "resource_path_image_item.hpp"
#include "design_elements_factory.hpp"
#include <QFileDialog>
#define RESOURCE_PATH_IMAGE_SIZE  QSize(300,200)

namespace
{
ReportDesign::base_design_item *createImageItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::resource_path_image_item(owner, parent);
}
bool  registred = ReportDesign::DesignElementsFactory::instance().registerCreator(
                      ReportDesign::IMAGE_ITEM_TYPE, ReportDesign::ItemAttribs(QObject::tr("Image Item"),
                                                                               ReportDesign::IMAGE_ITEM_TYPE), createImageItem);
}

namespace  ReportDesign
{
resource_path_image_item::resource_path_image_item(QObject *owner, QGraphicsItem *parent):
    image_base_item(ReportDesign::IMAGE_ITEM_TYPE, owner, parent)
{
    setSize((RESOURCE_PATH_IMAGE_SIZE));
    setImage(QImage(":/imageBG"));
}

QString resource_path_image_item::BGText()
{
    return tr("");
}

base_design_item *resource_path_image_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new resource_path_image_item(owner, parent);
}

void resource_path_image_item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Select image file"), "", fileFilter());
    if (!fileName.isEmpty())
    {
        setImage(QImage(fileName));
    }
    image_base_item::mouseDoubleClickEvent(event);
}

void resource_path_image_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    image_base_item::paint(painter, nullptr, nullptr);
    drawMasterMarkColor(painter);
}

bool resource_path_image_item::printDataIsEmpty()
{
    return image().isNull();
}



}
