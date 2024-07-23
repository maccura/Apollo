#include "watermark_item.hpp"
#include "design_elements_factory.hpp"
#define WATERMARK_OPACITY       0.2
#define WATERMARK_TEXT_SIZE     30
#define WATERMARK_IMAGE_WIDE    400
#define WATERMARK_IMAGE_HEIGHT  400
namespace
{
ReportDesign::base_design_item *createWatermarkItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::watermark_item(owner, parent);
}
bool registred = ReportDesign::DesignElementsFactory::instance().
                 registerCreator(ReportDesign::WATERMARK_ITEM_TYPE, ReportDesign::ItemAttribs(QObject::tr("Watermark Item"),
                                 ReportDesign::WATERMARK_ITEM_TYPE), createWatermarkItem);
}

namespace ReportDesign
{
watermark_item::watermark_item(QObject *owner, QGraphicsItem *parent)
    : TextItem(ReportDesign::WATERMARK_ITEM_TYPE, owner, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFontColor(Qt::red);
    QFont frot("Arial", WATERMARK_TEXT_SIZE);
    setFont(frot);
    m_alignment = Qt::AlignLeft | Qt::AlignTop;
    if (parent != nullptr)
    {
        setGeometry(parent->boundingRect());
    }
}

void watermark_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    paintText(painter);
    paintImage(painter);
}

base_design_item *watermark_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    watermark_item *item = new watermark_item(owner, parent);
    item->setWatermarkText(m_watermarkText);
    item->setWatermarkImage(m_watermarkImage);
    return item;
}

void watermark_item::paintImage(QPainter *painter)
{
    painter->save();

    // 设置透明度
    painter->setOpacity(WATERMARK_OPACITY);

    // 加载图片
    QPixmap watermark = QPixmap::fromImage(m_watermarkImage);

    // 计算每个水印的间距
    int spacing = watermark.width() / 2;

    // 计算每行每列能容纳的水印数量
    int rowCount = height() / (watermark.height() + spacing);
    int columnCount = width() / (watermark.width() + spacing);

    // 计算水印的起始位置
    int startX = (width() - (columnCount * watermark.width() + (columnCount - 1) * spacing)) / 2;
    int startY = (height() - (rowCount * watermark.height() + (rowCount - 1) * spacing)) / 2;

    // 绘制水印
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < columnCount; j++)
        {
            int x = startX + j * (watermark.width() + spacing);
            int y = startY + i * (watermark.height() + spacing);
            painter->drawPixmap(x, y, watermark);
        }
    }
    painter->restore();
}

void watermark_item::paintText(QPainter *painter)
{
    painter->save();
    setupPainter(painter);
    painter->setOpacity(WATERMARK_OPACITY);

    // 创建一个QTextDocument对象，并设置文本内容
    TextPtr doc = textDocument();

    // 获取文本的宽度和高度
    qreal textWidth = doc->idealWidth();
    qreal textHeight = doc->size().height();

    // 计算每个水印的间距
    qreal spacing = textWidth / 3;

    // 计算每行每列能容纳的水印数量
    int rowCount = height() / (textHeight + spacing);
    int columnCount = width() / (textWidth + spacing);

    // 计算水印的起始位置
    qreal startX = (width() - (columnCount * textWidth + (columnCount - 1) * spacing)) / 2;
    qreal startY = (height() - (rowCount * textHeight + (rowCount - 1) * spacing)) / 2;

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, fontColor());
    // 绘制水印文本
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < columnCount; j++)
        {
            qreal x = startX + j * (textWidth + spacing);
            qreal y = startY + i * (textHeight + spacing);

            painter->translate(x, y);
            doc->documentLayout()->draw(painter, ctx);
            painter->translate(-x, -y);
        }
    }

    painter->restore();
}

void watermark_item::renderPrintData(const PrintDataMap &PrintData)
{

}

void watermark_item::clearPrintData()
{

}

void watermark_item::setItemMode(base_design_item::ItemMode mode)
{
    if (mode == DesignMode)
    {
        base_design_item::setItemMode(PreviewMode);
    }
    else
    {
        base_design_item::setItemMode(mode);
    }
}

}
