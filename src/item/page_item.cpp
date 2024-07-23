#include "page_item.hpp"
#include "page_scene.hpp"
#include "table_item.hpp"
#include "page_number_item.hpp"
#include <QGraphicsScene>
#include <QtPrintSupport/QPrinter>
#include <QMenu>
#include "render/report_render.hpp"
namespace ReportDesign
{
page_item::page_item(QObject *owner, QGraphicsItem *parent) : base_design_item("PageItem", owner, parent),
    m_topMargin(0), m_bottomMargin(0), m_leftMargin(0), m_rightMargin(0),
    m_pageOrientaion(Portrait), m_pageSize(A4), m_sizeChainging(false),
    m_fullPage(false), m_oldPrintMode(false), m_resetPageNumber(false),
    m_isExtendedInDesignMode(false), m_extendedHeight(1000), m_isTOC(false),
    m_setPageSizeToPrinter(false), m_endlessHeight(false), m_printable(true),
    m_printBehavior(Split), m_dropPrinterMargins(false),
    m_notPrintIfEmpty(false), m_mixWithPriorPage(false)
{
    setFixedPos(true);
    setPossibleResizeDirectionFlags(Fixed);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    initPageSize(m_pageSize);
    // 设置为不可选中，才能在page页中使用鼠标选择框
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(false);

}

page_item::page_item(const PageSize pageSize, const QRectF &rect, QObject *owner, QGraphicsItem *parent) :
    base_design_item("PageItem", owner, parent),
    m_topMargin(0), m_bottomMargin(0), m_leftMargin(0), m_rightMargin(0),
    m_pageOrientaion(Portrait), m_pageSize(pageSize), m_sizeChainging(false),
    m_fullPage(false), m_oldPrintMode(false), m_resetPageNumber(false),
    m_isExtendedInDesignMode(false), m_extendedHeight(1000), m_isTOC(false),
    m_setPageSizeToPrinter(false), m_endlessHeight(false), m_printable(true),
    m_printBehavior(Split), m_dropPrinterMargins(false),
    m_notPrintIfEmpty(false), m_mixWithPriorPage(false)
{
    setFixedPos(true);
    setPossibleResizeDirectionFlags(Fixed);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    initPageSize(rect.size());

    // 设置为不可选中，才能在page页中使用鼠标选择框
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(false);
}

page_item::~page_item()
{

}

void page_item::paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if (itemMode() & DesignMode)
    {
        QRectF rect = pageRect();
        if (isExtendedInDesignMode())
        {
            rect.adjust(0, 0, 0, m_extendedHeight);
        }
        ppainter->save();
        ppainter->setOpacity(0.8);
        ppainter->fillRect(boundingRect(), pageBorderColor());
        ppainter->setOpacity(1);
        ppainter->fillRect(rect, Qt::white);
        paintGrid(ppainter, rect);
        ppainter->setPen(gridColor());
        ppainter->drawRect(boundingRect());
        drawShadow(ppainter, boundingRect(), 15);
        ppainter->restore();
    }

    if (itemMode() & PreviewMode)
    {
        ppainter->save();
        ppainter->fillRect(rect(), Qt::white);
        QPen pen;
        pen.setColor(Qt::gray);
        pen.setWidth(1);
        pen.setStyle(Qt::SolidLine);
        ppainter->setPen(pen);
        QRectF tmpRect = rect();
        tmpRect.adjust(-4, -4, 4, 4);
        ppainter->drawRect(tmpRect);
        ppainter->restore();
        base_design_item::paint(ppainter, option, widget);
    }

    setWatermarkTop();

}

base_design_item *page_item::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new page_item(owner, parent);
}

void page_item::geometryChangedEvent(QRectF newRect, QRectF)
{
    Q_UNUSED(newRect)
    if (itemMode() == DesignMode || !endlessHeight())
    {
        updateMarginRect();
    }
    PageSize oldSize = m_pageSize;
    if (!m_sizeChainging && !isLoading())
    {
        m_pageSize = Custom;
    }
    notify("pageSize", oldSize, m_pageSize);
}

QColor page_item::selectionColor() const
{
    return Qt::gray;
}

QColor page_item::pageBorderColor() const
{
    //return QColor(100,150,50);
    return PAGE_BORDER_COLOR;
}

QColor page_item::gridColor() const
{
    //return QColor(170,200,150);
    return PAGE_GRID_COLOR;
}

QRectF page_item::boundingRect() const
{
    if (!isExtendedInDesignMode())
    {
        return base_design_item::boundingRect();
    }
    else
    {
        QRectF result = base_design_item::boundingRect();
        return result.adjusted(0, 0, 0, m_extendedHeight);
    }
}

void page_item::setItemMode(base_design_item::ItemMode mode)
{
    base_design_item::setItemMode(mode);
    relocateBands();
}

void page_item::clear()
{
    foreach (QGraphicsItem *item, childItems())
    {
        delete item;
    }
    childItems().clear();
    //m_bands.clear();
}

void page_item::increaseBandIndex(int startIndex)
{

}

void page_item::initColumnsPos(QVector<qreal> &posByColumns, qreal pos, int dataColumnCount)
{
    posByColumns.clear();
    for (int i = 0; i < dataColumnCount; ++i)
    {
        posByColumns.append(pos);
    }
}

bool page_item::notPrintIfEmpty() const
{
    return m_notPrintIfEmpty;
}

void page_item::setNotPrintIfEmpty(bool notPrintIfEmpty)
{
    m_notPrintIfEmpty = notPrintIfEmpty;
}

bool page_item::dropPrinterMargins() const
{
    return m_dropPrinterMargins;
}

void page_item::setDropPrinterMargins(bool dropPrinterMargins)
{
    m_dropPrinterMargins = dropPrinterMargins;
}

bool page_item::isEmpty() const
{
    return childBaseItems().isEmpty();
}

void page_item::setPrintBehavior(const PrintBehavior &printBehavior)
{
    m_printBehavior = printBehavior;
}

page_item::PrintBehavior page_item::printBehavior() const
{
    return m_printBehavior;
}

QString page_item::printerName() const
{
    return m_printerName;
}

void page_item::setPrinterName(const QString &printerName)
{
    m_printerName = printerName;
}

bool page_item::isPrintable() const
{
    return m_printable;
}

void page_item::setPrintable(bool printable)
{
    m_printable = printable;
}

bool page_item::endlessHeight() const
{
    return m_endlessHeight;
}

void page_item::setEndlessHeight(bool endlessPage)
{
    m_endlessHeight = endlessPage;
}

bool page_item::getSetPageSizeToPrinter() const
{
    return m_setPageSizeToPrinter;
}

void page_item::setSetPageSizeToPrinter(bool setPageSizeToPrinter)
{
    if (m_setPageSizeToPrinter != setPageSizeToPrinter)
    {
        m_setPageSizeToPrinter = setPageSizeToPrinter;
        notify("setPageSizeToPrinter", !setPageSizeToPrinter, setPageSizeToPrinter);
    }
}

bool page_item::isTOC() const
{
    return m_isTOC;
}

void page_item::setIsTOC(bool isTOC)
{
    if (m_isTOC != isTOC)
    {
        m_isTOC = isTOC;
        notify("pageIsTOC", !isTOC, isTOC);
    }
}

int page_item::extendedHeight() const
{
    return m_extendedHeight;
}

void page_item::setExtendedHeight(int extendedHeight)
{
    m_extendedHeight = extendedHeight;
    page_scene *page = dynamic_cast<page_scene *>(scene());
    if (page)
    {
        page->updatePageRect();
    }
    update();
}

bool page_item::isExtendedInDesignMode() const
{
    return m_isExtendedInDesignMode;
}

void page_item::setExtendedInDesignMode(bool pageIsExtended)
{
    m_isExtendedInDesignMode = pageIsExtended;
    page_scene *page = dynamic_cast<page_scene *>(scene());
    if (page)
    {
        page->updatePageRect();
    }
    update();
}

void page_item::placeTearOffBand()
{

}

bool page_item::resetPageNumber() const
{
    return m_resetPageNumber;
}

void page_item::setResetPageNumber(bool resetPageNumber)
{
    if (m_resetPageNumber != resetPageNumber)
    {
        m_resetPageNumber = resetPageNumber;
        notify("resetPageNumber", !m_resetPageNumber, m_resetPageNumber);
    }
}

void page_item::updateSubItemsSize(RenderPass pass/*, DataSourceManager *dataManager*/)
{

}

bool page_item::oldPrintMode() const
{
    return m_oldPrintMode;
}

void page_item::setOldPrintMode(bool oldPrintMode)
{
    m_oldPrintMode = oldPrintMode;
}

bool page_item::fullPage() const
{
    return m_fullPage;
}

void page_item::setFullPage(bool fullPage)
{
    if (m_fullPage != fullPage)
    {
        m_fullPage = fullPage;
        if (!isLoading())
        {
            updateMarginRect();
            notify("fullPage", !m_fullPage, m_fullPage);
        }
    }
}

void page_item::relocateBands()
{

}

QString page_item::fileName()
{
    return m_fileName;
}

void page_item::setFileName(QString fileName)
{
    m_fileName = fileName;
}

QString page_item::module()
{
    return m_module;
}

void page_item::setModule(QString module)
{
    m_module = module;
}

void page_item::setTopMargin(int value)
{
    if (m_topMargin != value)
    {
        m_sizeChainging = true;
        int oldValue = m_topMargin;
        m_topMargin = value;
        if (!isLoading())
        {
            updateMarginRect();
            notify("topMargin", oldValue, value);;
        }
        m_sizeChainging =  false;
    }
}

void page_item::setBottomMargin(int value)
{
    if (m_bottomMargin != value)
    {
        m_sizeChainging = true;
        int oldValue = m_bottomMargin;
        m_bottomMargin = value;
        if (!isLoading())
        {
            updateMarginRect();
            notify("bottomMargin", oldValue, value);
        }
        m_sizeChainging = false;
    }
}

void page_item::setLeftMargin(int value)
{
    if (m_leftMargin != value)
    {
        m_sizeChainging = true;
        int oldValue = m_leftMargin;
        m_leftMargin = value;
        if (!isLoading())
        {
            updateMarginRect();
            notify("leftMargin", oldValue, value);
        }
        m_sizeChainging = false;
    }
}

void page_item::setRightMargin(int value)
{
    if (m_rightMargin != value)
    {
        m_sizeChainging = true;
        int oldValue = m_rightMargin;
        m_rightMargin = value;
        if (!isLoading())
        {
            updateMarginRect();
            notify("rightMargin", oldValue, value);
        }
        m_sizeChainging = false;
    }
}

void page_item::setPageOrientation(page_item::Orientation value)
{
    if (!m_sizeChainging && m_pageOrientaion != value)
    {
        m_sizeChainging = true;
        page_item::Orientation oldValue = m_pageOrientaion;
        m_pageOrientaion = value;
        if (!isLoading())
        {
            qreal tmpWidth = width();
            setWidth(height());
            setHeight(tmpWidth);
            updateMarginRect();
            notify("pageOrientation", oldValue, value);
        }
        m_sizeChainging =  false;
    }
}

QSizeF page_item::getRectByPageSize(const PageSize &size)
{
    if (size != Custom)
    {
        // 此处只是求一个size,但是QPrinter的创建很耗时间，所以用static对象，减少对象创建，从而大大降低渲染耗时。
        static QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        printer.setOrientation((QPrinter::Orientation)pageOrientation());
        printer.setPaperSize((QPrinter::PageSize)size);
        return QSizeF(printer.paperSize(QPrinter::Millimeter).width() * 10,
                      printer.paperSize(QPrinter::Millimeter).height() * 10);
#else
        QPageSize pageSize = QPageSize((QPageSize::PageSizeId)size);
        qreal width = pageSize.size(QPageSize::Millimeter).width() * 10;
        qreal height = pageSize.size(QPageSize::Millimeter).height() * 10;
        return QSizeF(pageOrientation() == Portrait ? width : height,
                      pageOrientation() == Portrait ? height : width);
#endif
    }

    else
    {
        return QSizeF(width(), height());
    }
}

void page_item::initPageSize(const QSizeF &size)
{
    m_sizeChainging = true;
    setWidth(size.width());
    setHeight(size.height());
    m_sizeChainging = false;
}

void page_item::preparePopUpMenu(QMenu &menu)
{
    foreach (QAction *action, menu.actions())
    {
        if (action->text().compare(tr("Paste")) != 0)
        {
            action->setVisible(false);
        }
    }
    menu.addSeparator();
    menu.addAction(tr("Edit"));


    menu.addSeparator();

    QAction *action = menu.addAction(tr("Page is TOC"));
    action->setCheckable(true);
    action->setChecked(isTOC());

    action = menu.addAction(tr("Reset page number"));
    action->setCheckable(true);
    action->setChecked(resetPageNumber());

    action = menu.addAction(tr("Full page"));
    action->setCheckable(true);
    action->setChecked(fullPage());

    action = menu.addAction(tr("Set page size to printer"));
    action->setCheckable(true);
    action->setChecked(getSetPageSizeToPrinter());

    action = menu.addAction(tr("Mix with prior page"));
    action->setCheckable(true);
    action->setChecked(mixWithPriorPage());

}

void page_item::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("Page is TOC")) == 0)
    {
        page()->setPropertyToSelectedItems("pageIsTOC", action->isChecked());
    }
    if (action->text().compare(tr("Reset page number")) == 0)
    {
        page()->setPropertyToSelectedItems("resetPageNumber", action->isChecked());
    }
    if (action->text().compare(tr("Full page")) == 0)
    {
        page()->setPropertyToSelectedItems("fullPage", action->isChecked());
    }
    if (action->text().compare(tr("Set page size to printer")) == 0)
    {
        page()->setPropertyToSelectedItems("setPageSizeToPrinter", action->isChecked());
    }

    if (action->text().compare(tr("Mix with prior page")) == 0)
    {
        page()->setPropertyToSelectedItems("mixWithPriorPage", action->isChecked());
    }
    if (action->text() == tr("Edit"))
    {
        //        PageEditor pageEdit(NULL,this);
        //        pageEdit.exec();
    }

}
void page_item::initPageSize(const page_item::PageSize &size)
{
    m_sizeChainging = true;
    if (m_pageSize != Custom)
    {
        QSizeF pageSize = getRectByPageSize(size);
        setWidth(pageSize.width());
        setHeight(pageSize.height());
    }
    m_sizeChainging = false;
}

void page_item::setPageSize(const PageSize &size)
{
    if (m_pageSize != size)
    {
        m_pageSize = size;
        initPageSize(m_pageSize);
    }
}

void page_item::bandDeleted(QObject *band)
{

}

void page_item::moveBandFromTo(int from, int to)
{

}

void page_item::bandPositionChanged(QObject *object, QPointF newPos, QPointF oldPos)
{

}

void page_item::bandGeometryChanged(QObject *object, QRectF newGeometry, QRectF oldGeometry)
{
    bandPositionChanged(object, newGeometry.topLeft(), oldGeometry.topLeft());
}

void page_item::setUnitTypeProperty(base_design_item::UnitType value)
{
    if (unitType() != value)
    {
        UnitType oldValue = unitType();
        setUnitType(value);
        if (!isLoading())
        {
            update();
            notify("units", oldValue, value);
        }
    }
}

void page_item::collectionLoadFinished(const QString &collectionName)
{

}

void page_item::updateMarginRect()
{
    m_pageRect = rect();
    m_pageRect.adjust(leftMargin() * mmFACTOR,
                      topMargin() * mmFACTOR,
                      -rightMargin() * mmFACTOR,
                      -bottomMargin() * mmFACTOR
                     );

    foreach (base_design_item *item, childBaseItems())
    {
        if (item->itemAlign() != DesignedItemAlign)
        {
            item->updateItemAlign();
        }
    }
    update();
}

void page_item::paintGrid(QPainter *ppainter, QRectF rect)
{
    ppainter->save();
    ppainter->setPen(QPen(gridColor()));
    ppainter->setOpacity(0.5);
    for (int i = 0; i <= (rect.height() - 5 * unitFactor()) / (10 * unitFactor()); ++i)
    {
        if (i * 10 * unitFactor() + 5 * unitFactor() >= topMargin() * mmFACTOR)
            ppainter->drawLine(rect.x(), (i * 10 * unitFactor()) + ((rect.y() + 5 * unitFactor()) - (topMargin() * mmFACTOR)),
                               rect.right(), i * 10 * unitFactor() + ((rect.y() + 5 * unitFactor()) - (topMargin() * mmFACTOR)));
    };
    for (int i = 0; i <= ((rect.width() - 5 * unitFactor()) / (10 * unitFactor())); ++i)
    {
        if (i * 10 * unitFactor() + 5 * unitFactor() >= leftMargin() * mmFACTOR)
            ppainter->drawLine(i * 10 * unitFactor() + ((rect.x() + 5 * unitFactor()) - (leftMargin() * mmFACTOR)), rect.y(),
                               i * 10 * unitFactor() + ((rect.x() + 5 * unitFactor()) - (leftMargin() * mmFACTOR)), rect.bottom());
    };
    ppainter->setPen(QPen(gridColor()));
    ppainter->setOpacity(1);
    for (int i = 0; i <= (rect.width() / (10 * unitFactor())); ++i)
    {
        if (i * 10 * unitFactor() >= leftMargin() * mmFACTOR)
            ppainter->drawLine(i * 10 * unitFactor() + (rect.x() - (leftMargin() * mmFACTOR)), rect.y(),
                               i * 10 * unitFactor() + (rect.x() - (leftMargin() * mmFACTOR)), rect.bottom());
    };
    for (int i = 0; i <= rect.height() / (10 * unitFactor()); ++i)
    {
        if (i * 10 * unitFactor() >= topMargin() * mmFACTOR)
            ppainter->drawLine(rect.x(), i * 10 * unitFactor() + (rect.y() - (topMargin() * mmFACTOR)),
                               rect.right(), i * 10 * unitFactor() + (rect.y() - (topMargin() * mmFACTOR)));
    };
    ppainter->drawRect(rect);
    ppainter->restore();
}

void page_item::setGridStep(int value)
{
    if (page())
    {
        page()->setHorizontalGridStep(value);
        page()->setVerticalGridStep(value);
    }
}

int page_item::gridStep()
{
    if (page())
    {
        return page()->horizontalGridStep();
    }
    else
    {
        return DEFAULT_GRID_STEP;
    }
}

void page_item::objectLoadFinished()
{
    base_design_item::objectLoadFinished();
    updateMarginRect();
}

page_item::Ptr page_item::create(QObject *owner)
{
    return page_item::Ptr(new page_item(owner));
}

bool page_item::mixWithPriorPage() const
{
    return m_mixWithPriorPage;
}

void page_item::setMixWithPriorPage(bool value)
{
    if (m_mixWithPriorPage != value)
    {
        m_mixWithPriorPage = value;
        if (!isLoading())
        {
            update();
            notify("mixWithPriorPage", !value, value);
        }
    }
}

base_design_item *page_item::findTableItem(int ID)
{
    for (QGraphicsItem *it : childItems())
    {
        table_item *tabelItem = dynamic_cast<table_item *>(it);
        if (tabelItem)
        {
            if (tabelItem->tableItemID() == ID)
            {
                return tabelItem;
            }
        }
    }
    return nullptr;
}

void  page_item::reloadVariantName(const VariantNameMap &variantName)
{
    for (QGraphicsItem *it : childItems())
    {
        base_design_item *child = dynamic_cast<base_design_item *>(it);
        if (child)
        {
            child->reloadVariantName(variantName);
        }
    }
}

void  page_item::renderPrintData(const PrintDataMap &PrintData)
{
    m_printData = PrintData;
    for (QGraphicsItem *it : childItems())
    {
        base_design_item *child = dynamic_cast<base_design_item *>(it);
        if (child)
        {
            child->renderPrintData(PrintData);
        }
    }
}

void  page_item::clearPrintData()
{
    m_printData.clear();
    for (QGraphicsItem *it : childItems())
    {
        base_design_item *child = dynamic_cast<base_design_item *>(it);
        if (child)
        {
            child->clearPrintData();
        }
    }
}

watermark_item  *page_item::reportWatermark()
{
    watermark_item *watermark = nullptr;
    for (QGraphicsItem *it : childItems())
    {
        watermark = dynamic_cast<watermark_item *>(it);
        if (watermark)
        {
            break;
        }
    }
    return watermark;
}

bool page_item::watermarkIsEmpty()
{
    watermark_item *watermark = reportWatermark();
    if (!watermark)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void page_item::setWatermark(watermark_item *watermark)
{
    if (watermark)
    {
        watermark->createSameTypeItem(this, this);
    }
}

void page_item::clearWatermark()
{
    watermark_item *watermark = reportWatermark();
    if (watermark)
    {
        watermark->setParentItem(nullptr);
        watermark->setParent(nullptr);
        scene()->removeItem(watermark);
        watermark->deleteLater();
    }
}

void page_item::setWatermarkTop()
{
    int maxZ = 0;
    for (QGraphicsItem *it : childItems())
    {
        // 拿到非水印Item的Max Z
        if (dynamic_cast<watermark_item *>(it) == nullptr)
        {
            maxZ = it->zValue() > maxZ ? it->zValue() : maxZ;
        }
    }
    watermark_item *watermark  = reportWatermark();
    if (watermark && maxZ >= watermark->zValue())
    {
        watermark->setZValue(maxZ + 1);
    }
}

void page_item::setPageNumber(int current, int total)
{
    for (base_design_item *item : childBaseItems())
    {
        page_number_item *pageNumber = dynamic_cast<page_number_item *>(item);
        if (pageNumber != nullptr)
        {
            pageNumber->setPageNumber(current, total);
        }
    }
}

void page_item::initFlags()
{
    setFixedPos(true);
    setPossibleResizeDirectionFlags(Fixed);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    // 设置为不可选中，才能在page页中使用鼠标选择框
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setTopMargin(PAGE_MARGIN);
    setBottomMargin(PAGE_MARGIN);
    setLeftMargin(PAGE_MARGIN);
    setRightMargin(PAGE_MARGIN);
    setAcceptHoverEvents(false);
}

QMap<int, QList<base_design_item *>> page_item::groupItemsByRepeatVariant(QString varID)
{
    QMap<int, QList<base_design_item *>> itemsList;
    for (base_design_item *item : childBaseItems())
    {
        if (item->groupVariant().m_ID == varID)
        {
            itemsList[item->groupID()].push_back(item);
        }
    }

    return itemsList;
}

// 分组和排序
QMap<int, QList<base_design_item *>> page_item::repeatGroupItems()
{
    QMap<int, QList<base_design_item *>> itemsList;
    for (base_design_item *item : childBaseItems())
    {
        if (!item->groupVariant().m_ID.isEmpty())
        {
            itemsList[item->groupID()].push_back(item);
        }
    }

    // 排序
    for (int key : itemsList.keys())
    {
        std::sort(itemsList[key].begin(), itemsList[key].end(), compareItems);
    }
    return itemsList;
}

QRect page_item::itemShowRect()
{
    // TODO:单页显示区域矩形高度计算：
    //（1）存在页眉页脚
    //    单页显示区域矩形高度 = 页眉矩形的bottom - 页脚矩形的top
    //（2）没有页眉页脚
    //    单页显示区域矩形高度 = 页rect高度 - 2*页边距
    //(3) 只存在页眉
    //    单页显示区域矩形高度 = 页rect高度 - 页眉矩形的bottom - 页边距
    //(4) 只存在页脚
    //    单页显示区域矩形高度 = 页rect高度 - 页脚矩形的top - 页边距
    QRect MaskBottom = report_render::masterMaskBottom(this);
    QRect MaskTop = report_render::masterMaskTop(this);
    int Margin = topMargin() * mmFACTOR;
    //QRect ItemShowRect = rect().adjusted(Margin, Margin, -Margin, -Margin).toRect();
    QRect ItemShowRect = rect().toRect();
    if (MaskBottom.isNull() && MaskTop.isNull())
    {
        return ItemShowRect.adjusted(Margin, Margin, -Margin, -Margin);
    }
    else if (MaskBottom.isNull() && !MaskTop.isNull())
    {
        ItemShowRect.setY(MaskTop.bottom());
        ItemShowRect.setHeight(ItemShowRect.height() - MaskTop.bottom() - Margin);
    }
    else if (!MaskBottom.isNull() && MaskTop.isNull())
    {
        ItemShowRect.setY(Margin);
        ItemShowRect.setHeight(MaskBottom.top() - Margin);
    }
    else
    {
        ItemShowRect.setY(MaskTop.bottom());
        ItemShowRect.setHeight(MaskBottom.top() - MaskTop.bottom());
    }
    return  ItemShowRect;
}

}
