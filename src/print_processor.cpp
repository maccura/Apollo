#include "print_processor.hpp"
namespace ReportDesign
{
PrintProcessor::PrintProcessor(QPrinter *printer)
    : m_printer(printer), m_painter(0), m_firstPage(true),
      m_renderPage(nullptr)
{

}

PrintProcessor::PrintProcessor(QPrinter *printer, page_item::Ptr page)
    : m_printer(printer), m_painter(0), m_firstPage(true),
      m_renderPage(nullptr)
{
    initPrinter(page.data());
    m_painter = new (std::nothrow) QPainter(m_printer);
}

PrintProcessor::~PrintProcessor()
{
    if (m_painter)
    {
        delete m_painter;
    }

    if (m_renderPage)
    {
        delete m_renderPage;
    }
}

bool PrintProcessor::printPage(page_item::Ptr page)
{
    if (!m_renderPage)
    {
        m_renderPage =  new page_scene(page->module());
        m_renderPage->setItemMode(PrintMode);
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    if (!m_firstPage && !m_painter->isActive())
    {
        return false;
    }
    page_scene *backupPage = dynamic_cast<page_scene *>(page->scene());

    QPointF backupPagePos = page->pos();
    page->setPos(0, 0);
    m_renderPage->setPageItem(page);
    m_renderPage->setSceneRect(m_renderPage->pageItem()->mapToScene(m_renderPage->pageItem()->rect()).boundingRect());
    initPrinter(m_renderPage->pageItem());

    if (!m_firstPage)
    {
        m_printer->newPage();
    }
    else
    {
        if (!m_painter)
        {
            m_painter = new QPainter(m_printer);
        }
        if (!m_painter->isActive())
        {
            return false;
        }
        m_firstPage = false;
    }

    qreal leftMargin, topMargin, rightMargin, bottomMargin;
    m_printer->getPageMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin, QPrinter::Millimeter);

    QRectF printerPageRect = m_printer->pageRect(QPrinter::Millimeter);
    printerPageRect = QRectF(0, 0, (printerPageRect.size().width() + rightMargin + leftMargin) * page->unitFactor(),
                             (printerPageRect.size().height() + bottomMargin + topMargin) * page->unitFactor());

    //
    if (page->printBehavior() == page_item::Split && m_printer->pageSize() != static_cast<QPrinter::PageSize>(page->pageSize()) &&
        printerPageRect.width() < page->geometry().width())
    {
        qreal pageWidth = page->geometry().width();
        qreal pageHeight =  page->geometry().height();
        QRectF currentPrintingRect = printerPageRect;
        qreal curHeight = 0;
        qreal curWidth = 0;
        bool first = true;
        while (pageHeight > 0)
        {
            while (curWidth < pageWidth)
            {
                if (!first)
                {
                    m_printer->newPage();
                }
                else
                {
                    first = false;
                }
                m_renderPage->render(m_painter, m_printer->pageRect(), currentPrintingRect);
                currentPrintingRect.adjust(printerPageRect.size().width(), 0, printerPageRect.size().width(), 0);
                curWidth += printerPageRect.size().width();
            }
            pageHeight -= printerPageRect.size().height();
            curHeight += printerPageRect.size().height();
            currentPrintingRect = printerPageRect;
            currentPrintingRect.adjust(0, curHeight, 0, curHeight);
            curWidth = 0;
        }

    }
    else
    {
        if (page->getSetPageSizeToPrinter())
        {
            QRectF source = page->geometry();
            QSizeF inchSize = source.size() / (100 * 2.54);
            QRectF target = QRectF(QPoint(0, 0), inchSize  * m_printer->resolution());
            m_renderPage->render(m_painter, target, source);
        }
        else
        {
            m_renderPage->render(m_painter);
        }
    }

    page->setPos(backupPagePos);
    m_renderPage->removePageItem(page);
    if (backupPage)
    {
        backupPage->reactivatePageItem(page);
    }

#else
    if (!m_firstPage && !m_painter->isActive())
    {
        return false;
    }
    page_scene *backupPage = dynamic_cast<page_scene *>(page->scene());

    QPointF backupPagePos = page->pos();
    page->setPos(0, 0);
    m_renderPage->setPageItem(page);
    m_renderPage->setSceneRect(m_renderPage->pageItem()->mapToScene(m_renderPage->pageItem()->rect()).boundingRect());
    initPrinter(m_renderPage->pageItem());

    if (!m_firstPage)
    {
        m_printer->newPage();
    }
    else
    {
        m_painter = new QPainter(m_printer);
        if (!m_painter->isActive())
        {
            return false;
        }
        m_firstPage = false;
    }

    qreal leftMargin = m_printer->pageLayout().margins().left();
    qreal topMargin = m_printer->pageLayout().margins().top();
    qreal rightMargin = m_printer->pageLayout().margins().right();
    qreal bottomMargin = m_printer->pageLayout().margins().bottom();

    QRectF printerPageRect = m_printer->pageRect(QPrinter::Millimeter);
    printerPageRect = QRectF(0, 0, (printerPageRect.size().width() + rightMargin + leftMargin) * page->unitFactor(),
                             (printerPageRect.size().height() + bottomMargin + topMargin) * page->unitFactor());
    if (page->printBehavior() == page_item::Split && m_printer->pageLayout().pageSize() != QPageSize((QPageSize::PageSizeId)page->pageSize()) &&
        printerPageRect.width() < page->geometry().width())
    {
        qreal pageWidth = page->geometry().width();
        qreal pageHeight =  page->geometry().height();
        QRectF currentPrintingRect = printerPageRect;
        qreal curHeight = 0;
        qreal curWidth = 0;
        bool first = true;
        while (pageHeight > 0)
        {
            while (curWidth < pageWidth)
            {
                if (!first)
                {
                    m_printer->newPage();
                }
                else
                {
                    first = false;
                }
                m_renderPage->render(m_painter, m_printer->pageRect(QPrinter::Millimeter), currentPrintingRect);
                currentPrintingRect.adjust(printerPageRect.size().width(), 0, printerPageRect.size().width(), 0);
                curWidth += printerPageRect.size().width();
            }
            pageHeight -= printerPageRect.size().height();
            curHeight += printerPageRect.size().height();
            currentPrintingRect = printerPageRect;
            currentPrintingRect.adjust(0, curHeight, 0, curHeight);
            curWidth = 0;
        }

    }
    else
    {
        if (page->getSetPageSizeToPrinter())
        {
            QRectF source = page->geometry();
            QSizeF inchSize = source.size() / (100 * 2.54);
            QRectF target = QRectF(QPoint(0, 0), inchSize  * m_printer->resolution());
            m_renderPage->render(m_painter, target, source);
        }
        else
        {
            m_renderPage->render(m_painter);
        }
    }
    page->setPos(backupPagePos);
    m_renderPage->removePageItem(page);
    if (backupPage)
    {
        backupPage->reactivatePageItem(page);
    }
#endif
    return true;
}

void PrintProcessor::initPrinter(page_item *page)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    if (page->oldPrintMode())
    {
        m_printer->setPageMargins(page->leftMargin(),
                                  page->topMargin(),
                                  page->rightMargin(),
                                  page->bottomMargin(),
                                  QPrinter::Millimeter);
        m_printer->setOrientation(static_cast<QPrinter::Orientation>(page->pageOrientation()));
        QSizeF pageSize = (page->pageOrientation() == page_item::Landscape) ?
                          QSizeF(page->sizeMM().height(), page->sizeMM().width()) :
                          page->sizeMM();
        m_printer->setPaperSize(pageSize, QPrinter::Millimeter);
    }
    else
    {
        m_printer->setFullPage(page->fullPage());
        if (page->dropPrinterMargins())
        {
            m_printer->setPageMargins(0, 0, 0, 0, QPrinter::Point);
        }
        m_printer->setOrientation(static_cast<QPrinter::Orientation>(page->pageOrientation()));
        if (page->pageSize() == page_item::Custom)
        {
            QSizeF pageSize = (page->pageOrientation() == page_item::Landscape) ?
                              QSizeF(page->sizeMM().height(), page->sizeMM().width()) :
                              page->sizeMM();
            if (page->getSetPageSizeToPrinter() || m_printer->outputFormat() == QPrinter::PdfFormat)
            {
                m_printer->setPaperSize(pageSize, QPrinter::Millimeter);
            }
        }
        else
        {
            if (page->getSetPageSizeToPrinter() || m_printer->outputFormat() == QPrinter::PdfFormat)
            {
                m_printer->setPaperSize(static_cast<QPrinter::PageSize>(page->pageSize()));
            }
        }
    }
#else
    if (page->oldPrintMode())
    {
        m_printer->setPageMargins(QMarginsF(page->leftMargin(), page->topMargin(), page->rightMargin(), page->bottomMargin()), QPageLayout::Millimeter);
        m_printer->setPageOrientation((QPageLayout::Orientation)page->pageOrientation());
        QSizeF pageSize = (page->pageOrientation() == page_item::Landscape) ?
                          QSizeF(page->sizeMM().height(), page->sizeMM().width()) :
                          page->sizeMM();
        m_printer->setPageSize(QPageSize(pageSize, QPageSize::Millimeter));
    }
    else
    {
        m_printer->setFullPage(page->fullPage());
        if (page->dropPrinterMargins())
        {
            m_printer->setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Point);
        }
        m_printer->setPageOrientation((QPageLayout::Orientation)page->pageOrientation());
        if (page->pageSize() == page_item::Custom)
        {
            QSizeF pageSize = (page->pageOrientation() == page_item::Landscape) ?
                              QSizeF(page->sizeMM().height(), page->sizeMM().width()) :
                              page->sizeMM();
            if (page->getSetPageSizeToPrinter() || m_printer->outputFormat() == QPrinter::PdfFormat)
            {
                m_printer->setPageSize(QPageSize(pageSize, QPageSize::Millimeter));
            }
        }
        else
        {
            if (page->getSetPageSizeToPrinter() || m_printer->outputFormat() == QPrinter::PdfFormat)
            {
                m_printer->setPageSize(QPageSize((QPageSize::PageSizeId)page->pageSize()));
            }
        }
    }
#endif
}
}
