#include "preview_report_widget.hpp"
#include <QColor>
#include <QVBoxLayout>
#include <QPrinter>
#include <QFileDialog>
#include <QPainter>
#include <QPdfWriter>
#include <QScrollBar>
#include <QMainWindow>
#include <QApplication>
#include "print_processor.hpp"
#include "global.hpp"
#define ZOOMBUTTONSIZE  50
#define BUTTONSTYLESHEET "background-color: #ffffff; border-radius: 6px; font-family: HarmonyOS Sans SC Medium; font-size: 14px; font-weight: normal; line-height: 16px; color: #57bd7c;"

namespace ReportDesign
{
preview_page_view::preview_page_view(QWidget *parent /*= NULL*/) : QGraphicsView(parent),
    m_maxScale(MAXSCALE), m_minScale(MINSCALE), m_ctrolFlag(false)
{

}
preview_page_view::preview_page_view(QGraphicsScene *scene, QWidget *parent /*= NULL*/) : QGraphicsView(scene, parent),
    m_maxScale(MAXSCALE), m_minScale(MINSCALE), m_ctrolFlag(false)
{

}

void preview_page_view::setMaxScale(qreal maxScale)
{
    m_maxScale = maxScale;
}

void preview_page_view::setMinScale(qreal minScale)
{
    m_minScale = minScale;
}

void preview_page_view::wheelEvent(QWheelEvent *event)
{
    // 缩放
    if (event->modifiers() == Qt::ControlModifier)
    {
        // 获取当前的缩放级别
        qreal currentScale = transform().m11();

        // 获取滚轮事件的delta值
        int delta = event->angleDelta().y();

        // 根据滚轮事件的delta值来调整缩放级别
        if (delta > 0)
        {
            // 向上滚动，放大
            currentScale *= 1.1; // 调整缩放比例因子
        }
        else
        {
            // 向下滚动，缩小
            currentScale /= 1.1; // 调整缩放比例因子
        }

        currentScale = qBound(m_minScale, currentScale, m_maxScale);

        // 应用新的缩放级别
        setTransform(QTransform::fromScale(currentScale, currentScale));
        // 接受滚轮事件
        event->accept();
    }
    else// 滚轮上下翻动
    {
        int numDegrees = event->delta() ;
        int numSteps = numDegrees ;
        if (event->orientation() == Qt::Horizontal)
        {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - numSteps);
        }
        else
        {
            verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps);
        }

    }

}

void preview_page_view::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
}

void preview_page_view::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyReleaseEvent(event);
}

preview_report_widget::preview_report_widget(): base_mask_widget(QApplication::activeWindow(), false)
{
    QWidget *MainWindow = QApplication::activeWindow();
    m_close = new image_button(MainWindow, ":/close");
    m_zoomInButton = new CircularButton(MainWindow, ":/previewZoomIn");
    m_zoomOutButton = new CircularButton(MainWindow, ":/previewZoomOut");
    m_print = new QPushButton(MainWindow);
    m_printPDF = new QPushButton(MainWindow);
    m_back = new QPushButton(MainWindow);
    m_previewPage = nullptr;
    m_pageView = nullptr;

    m_pageView = new preview_page_view(this);
    VLayout()->setContentsMargins(0, 0, 0, 0);
    VLayout()->addWidget(m_pageView);

    init();
}

preview_report_widget::~preview_report_widget()
{
    m_close->deleteLater();
    m_zoomInButton->deleteLater();
    m_zoomOutButton->deleteLater();
    m_back->deleteLater();
    m_print->deleteLater();
    m_printPDF->deleteLater();
}

void preview_report_widget::init()
{
    raise();
    int widgetWidth = parentWidget()->width() / 2;
    int widgetHeight = parentWidget()->height() * 8 / 9;
    setGeometry(parentWidget()->width() / 2 - widgetWidth / 2, parentWidget()->height() / 2 - widgetHeight / 2, widgetWidth, widgetHeight);
    show();

    QPoint StartPoint = geometry().topLeft();
    m_close->move(StartPoint.x() + width(), StartPoint.y() - m_close->height());
    connect(m_close, &QPushButton::clicked, [this]
    {
        close();
    });
    m_close->show();

    int y_offset = 50;
    int x_offset = 30;

    m_zoomInButton->resize(ZOOMBUTTONSIZE, ZOOMBUTTONSIZE);
    m_zoomInButton->move(StartPoint.x() - m_zoomInButton->width() - x_offset, StartPoint.y() + height() - m_zoomInButton->height() - y_offset);
    connect(m_zoomInButton, &QPushButton::clicked, this, &preview_report_widget::slotZoomIn);
    m_zoomInButton->show();

    m_zoomOutButton->resize(ZOOMBUTTONSIZE, ZOOMBUTTONSIZE);
    QPoint pos = m_zoomInButton->pos();
    pos.setY(pos.y() - 30 - m_zoomOutButton->height());
    m_zoomOutButton->move(pos);
    connect(m_zoomOutButton, &QPushButton::clicked, this, &preview_report_widget::slotZoomOut);
    m_zoomOutButton->show();

    m_back->setStyleSheet(BUTTONSTYLESHEET);
    m_back->resize(100, 40);
    m_back->move(StartPoint.x() + width() + 20, StartPoint.y() + height() - m_back->height());
    m_back->setText(tr("back"));
    connect(m_back, &QPushButton::clicked, [this] {close();});
    m_back->show();

    pos = m_back->pos();
    pos.setY(pos.y() - m_back->height() - 10);
    m_print->setStyleSheet(BUTTONSTYLESHEET);
    m_print->move(pos);
    m_print->resize(100, 40);
    m_print->setText(tr("print"));
    connect(m_print, &QPushButton::clicked, [this] {slotPrint();});
    m_print->show();

    pos = m_print->pos();
    pos.setY(pos.y() - m_print->height() - 10);
    m_printPDF->setStyleSheet(BUTTONSTYLESHEET);
    m_printPDF->move(pos);
    m_printPDF->resize(120, 40);
    m_printPDF->setText(tr("save as PDF"));
    connect(m_printPDF, &QPushButton::clicked, [this] {slotPrintPDF();});
    m_printPDF->show();
}

void preview_report_widget::setPages(ReportPages pages)
{
    if (m_previewPage)
    {
        delete m_previewPage;
    }

    if (m_pageView)
    {
        delete m_pageView;
    }

    m_reportPages = pages;
    if (!m_reportPages.isEmpty())
    {
        m_previewPage = new page_scene("", this);
        QBrush brush(PAGE_GRID_COLOR);
        m_previewPage->setBackgroundBrush(brush);
        m_previewPage->setObjectName("preview");
        m_previewPage->pageItem()->setObjectName("ReportPreview");
        m_previewPage->setItemMode(PreviewMode);
        m_previewPage->setPageItems(m_reportPages);
        m_pageView = new preview_page_view(this);
        m_pageView->setScene(m_previewPage);

        QPointF centeOnPoint = pages.front().data()->pos();
        centeOnPoint.setX(centeOnPoint.x() +  pages.front().data()->width() / 2);
        m_pageView->centerOn(m_pageView->mapFromScene(centeOnPoint));
        slotFitPageWidth();
        VLayout()->setContentsMargins(0, 0, 0, 0);
        VLayout()->addWidget(m_pageView);
    }
}

void preview_report_widget::slotFitPage()
{
    qreal scaleSize = qMin(width() / m_previewPage->width(), height() / m_previewPage->height());
    setScalePercent(scaleSize);
}

void preview_report_widget::slotFitPageWidth()
{
    setScalePercent(width() / m_previewPage->width());
}

void preview_report_widget::slotZoomIn()
{
    if (!m_previewPage || !m_pageView)
    {
        return;
    }

    qreal cruPercent = m_pageView->transform().m11();
    cruPercent *= 1.1;
    cruPercent = qBound(MINSCALE, cruPercent, MAXSCALE);
    if (cruPercent == MAXSCALE)
    {
        m_zoomInButton->SetImage(":/previewZoomInEnd");
    }

    setScalePercent(cruPercent);
    if (m_zoomOutButton->ImageUrl() == ":/previewZoomOutEnd")
    {
        m_zoomOutButton->SetImage(":/previewZoomOut");
    }
}

void preview_report_widget::slotZoomOut()
{
    if (!m_previewPage || !m_pageView)
    {
        return;
    }
    qreal cruPercent = m_pageView->transform().m11();
    cruPercent /= 1.1;
    cruPercent = qBound(MINSCALE, cruPercent, MAXSCALE);
    if (cruPercent == MINSCALE)
    {
        m_zoomOutButton->SetImage(":/previewZoomOutEnd");
    }
    setScalePercent(cruPercent);

    if (m_zoomInButton->ImageUrl() == ":/previewZoomInEnd")
    {
        m_zoomInButton->SetImage(":/previewZoomIn");
    }
}

void preview_report_widget::slotPrint()
{
    // 创建打印机对象
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(QPrinterInfo::defaultPrinter().printerName());
    PrintProcessor printProcessor(&printer/*, m_edite_reportPages.back()*/);
    for (auto it : m_reportPages)
    {
        if (!printProcessor.printPage(it))
        {
            // 打印失败
            break;
        }
    }

    // 重新加载
    setPages(m_reportPages);
    return;
}

void preview_report_widget::slotPrintPDF()
{
    if (m_reportPages.isEmpty())
    {
        return;
    }
    printPdf(m_reportPages);

    // 重新加载
    setPages(m_reportPages);
}

bool preview_report_widget::isPrintPdf(QPrinterInfo &printerInfo)
{
    if (printerInfo.printerName().contains("pdf", Qt::CaseInsensitive))
    {
        return true;
    }
    return false;
}

void preview_report_widget::printPdf(ReportPages pages)
{
    // 这里遍历当地打印机找到pdf打印机。 不采用Qt直接转PDF，因为渲染水印图片时会有问题。
    QList<QPrinterInfo>  InfoList = QPrinterInfo::availablePrinters();
    for (QPrinterInfo &it : InfoList)
    {
        if (isPrintPdf(it))
        {
            // 创建打印机对象
            QPrinter printer(QPrinter::HighResolution);
            printer.setPrinterName(it.printerName());
            PrintProcessor printProcessor(&printer);
            for (auto it : pages)
            {
                if (!printProcessor.printPage(it))
                {
                    return;
                }
            }
            return;
        }
    }

    // 如果本地没有PDF打印机，再使用Qt的直接转成PDF,但图片水印会有问题。
    saveToQtPDF(pages);

}

void preview_report_widget::saveToQtPDF(ReportPages pages)
{
    // 让用户选择PDF文件的存储路径
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save PDF", "", "PDF Files (*.pdf)");
    if (!fileName.isEmpty())
    {
        // 创建打印机对象
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        PrintProcessor printProcessor(&printer);
        for (auto it : pages)
        {
            if (!printProcessor.printPage(it))
            {
                return;
            }
        }
        return;
    }
}

void preview_report_widget::setScalePercent(qreal percent)
{
    if (m_pageView)
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        m_pageView->resetMatrix();
#else
        m_pageView->resetTransform();
#endif
        m_pageView->scale(percent, percent);
    }
}

}

