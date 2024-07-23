#include "design_widget.hpp"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QIcon>
#include <QMessageBox>
#include "data_manager/device_variant_manager.hpp"
#include "mask_dialog/preview_report_widget.hpp"
#include "mask_dialog/message_info_dialog.hpp"
#include "print_processor.hpp"

#include <qDebug>
namespace ReportDesign
{

design_widget::design_widget(QWidget *parent) :
    QWidget(parent), m_tabWidget(new QTabWidget(this)), m_defaultUnits(base_design_item::Millimeters),
    m_report(nullptr), m_watermark(nullptr)
{
    initTabWidget();
}

void design_widget::initTabWidget()
{
    m_tabWidget->setObjectName("PageTabWidget");
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setMovable(true);
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SIGNAL(activePageChanged()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    setLayout(mainLayout);
    m_tabWidget->setStyleSheet("background-color: #ebeced;");

    // 新建按钮
    m_newReport = new QPushButton(m_tabWidget);
    m_newReport->setObjectName("newReport");
    connect(m_newReport, &QPushButton::clicked, [this]
    {
        emit newReportButtonClicked();
    });
    m_newReport->setFixedSize(100, 40);
    m_newReport->setStyleSheet("background-color: #57bd7c; border-radius: 4px; color: #ffffff; font-size: 14px;");
    m_newReport->setText(tr("+ new report"));

    // 创建一个垂直布局管理器
    QGridLayout *layout = new QGridLayout(m_tabWidget);

    // 将按钮添加到布局管理器中
    layout->addWidget(m_newReport, 0, 0, Qt::AlignCenter);

    // 将布局管理器设置为中心部件的布局
    m_tabWidget->setLayout(layout);

}

void design_widget::connectPage(page_scene *page)
{
    connect(page, SIGNAL(itemInserted(page_scene *, QPointF, QString)),
            this, SIGNAL(itemInserted(page_scene *, QPointF, QString)));
    //    connect(page, SIGNAL(itemPropertyChanged(QString, QString, QVariant, QVariant)),
    //            this, SIGNAL(itemPropertyChanged(QString, QString, QVariant, QVariant)));
    connect(page, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    connect(page, SIGNAL(selectionChanged()), this, SLOT(slotSynchronization()));
    connect(page, SIGNAL(commandHistoryChanged()), this, SIGNAL(commandHistoryChanged()));

    connect(m_tabWidget, &QTabWidget::currentChanged, [this]
    {
        if (activePage())
        {
            emit activePage()->selectionChanged();
        }
    });

    //emit activePageChanged();
}

page_view *design_widget::createPageView(page_scene *page)
{
    page_view *view = new page_view(this);
    view->setFrameShape(QFrame::NoFrame);
    view->setScene(page);
    view->setPageItem(page->pageItem());
    view->scale(0.5, 0.5);
    view->centerOn(0, 0);
    return view;
}

page_scene *design_widget::createStartPage(QString module, QString moduleName)
{
    if (m_report != nullptr)
    {
        delete m_report;
    }

    m_report = new report_manager(device_variant_manager::instance().DeviceID(), module, moduleName);
    m_fileName.clear();
    page_scene *page = m_report->appendPage("page1");
    page->pageItem()->setUnitType(m_defaultUnits);
    createTabs();
    return page;
}

void design_widget::addPage()
{
    if (!m_report)
    {
        return;
    }

    page_scene *page = nullptr;
    if (m_tabWidget->count() != 0 && activePage() != nullptr)
    {
        page = m_report->appendPage(activePage()->pageItem(), "page" + QString::number(m_report->pageCount() + 1));
    }
    else
    {
        page = m_report->appendPage("page" + QString::number(m_report->pageCount() + 1));
        if (!m_report->watermarkIsEmpty())
        {
            watermark_item *mark = m_report->watermarkItem();
            if (mark)
            {
                page->setWatermark(mark);
            }
        }
    }

    connectPage(page);
    page_view *view = createPageView(page);
    int index = m_report->pageCount() - 1;
    m_tabWidget->insertTab(index, view, QIcon(), page->pageItem()->objectName());
    m_tabWidget->setTabWhatsThis(index, "page");
    m_tabWidget->setCurrentIndex(index);
    emit pageAdded(page);
}

void design_widget::createTabs()
{
    m_tabWidget->clear();
    for (int i = 0; i < m_report->pageCount(); ++i)
    {
        page_scene *page = m_report->pageAt(i);
        if (page)
        {
            page->clearSelection();
            page_view *view = createPageView(page);
            int pageIndex = m_tabWidget->addTab(view, QIcon(), page->pageItem()->objectName());
            m_tabWidget->setTabWhatsThis(pageIndex, "page");
            //隐藏页bar
            m_tabWidget->setTabBarAutoHide(true);
            connectPage(page);
        }
    }
}

bool design_widget::isCanUndo()
{
    if (activePage())
    {
        return activePage()->isCanUndo();
    }
    return false;
}

bool design_widget::isCanRedo()
{
    if (activePage())
    {
        return activePage()->isCanRedo();
    }
    return false;
}

void design_widget::startInsertMode(const QString &itemType)
{
    if (activePage())
    {
        activePage()->startInsertMode(itemType);
    }
}

void design_widget::scale(qreal sx, qreal sy)
{
    if (!activeView())
    {
        return;
    }

    if (activeView()->transform().m11() >=  MAXSCALE &&
        (sx > 1.0 || sy > 1.0))
    {
        return;
    }

    if (activeView()->transform().m11() <= MINSCALE &&
        (sx < 1.0 || sy < 1.0))
    {
        return;
    }

    if (activeView())
    {
        activeView()->scale(sx, sy);
    }
}

void design_widget::copy()
{
    if (activePage())
    {
        activePage()->copy();
    }
}

void design_widget::variantLock()
{
    if (activePage())
    {
        if (activePage()->isVariantLock())
        {
            activePage()->itemVariantUnLock();
        }
        else
        {
            if (!activePage()->itemVariantLock())
            {
                QMessageBox::warning(nullptr, tr("tip"), tr("Only a single table!"));
            }
        }
    }
}

void design_widget::paste()
{
    if (activePage())
    {
        activePage()->paste();
    }
}

void design_widget::cut()
{
    if (activePage())
    {
        activePage()->cut();
    }
}


void design_widget::undo()
{
    if (activePage())
    {
        activePage()->undo();
    }
}

void design_widget::redo()
{
    if (activePage())
    {
        activePage()->redo();
    }
}

void design_widget::PageSeting()
{
    if (activePage())
    {
        itemSelected(activePage()->pageItem());
    }
}

page_scene *design_widget::activePage()
{
    if (activeView())
    {
        return dynamic_cast<page_scene *>(activeView()->scene());
    }
    return nullptr;
}

QGraphicsView *design_widget::activeView()
{
    return dynamic_cast<QGraphicsView *>(m_tabWidget->currentWidget());
}

void design_widget::slotSelectionChanged()
{
    QGraphicsScene *page = dynamic_cast<QGraphicsScene *>(sender());
    if (page)
    {
        if (page->selectedItems().count() == 1)
        {
            base_design_item *item = dynamic_cast<base_design_item *>(page->selectedItems().at(0));
            if (item)
            {
                emit(itemSelected(item));
            }
        }
        else if (page->selectedItems().count() > 1)
        {
            emit(multiItemSelected());
        }
        else if (page->selectedItems().count() == 0)
        {
            emit noItemSelected();
        }
    }
}

void design_widget::slotSynchronization()
{
    // 进行信号连接
    QGraphicsScene *page = dynamic_cast<QGraphicsScene *>(sender());
    if (page && m_report)
    {
        for (QGraphicsItem *item : page->selectedItems())
        {
            base_design_item *baseItem = dynamic_cast<base_design_item *>(item);
            if (baseItem)
            {
                connect(baseItem, SIGNAL(deleteItem()), m_report, SLOT(slotSynchronizationItemDestroyed()), Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
                connect(baseItem, SIGNAL(synchronization()), m_report, SLOT(slotItemSynchronization()), Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
            }
        }
    }
    //
}

void design_widget::deleteSelectedItems()
{
    if (activePage())
    {
        activePage()->deleteSelected();
    }
}

void design_widget::bringToFront()
{
    if (activePage())
    {
        activePage()->bringToFront();
    }
}

void design_widget::sendToBack()
{
    if (activePage())
    {
        activePage()->sendToBack();
    }
}

void design_widget::alignToLeft()
{
    if (activePage())
    {
        activePage()->alignToLeft();
    }
}

void design_widget::alignToRight()
{
    if (activePage())
    {
        activePage()->alignToRigth();
    }
}

void design_widget::alignToVCenter()
{
    if (activePage())
    {
        activePage()->alignToVCenter();
    }
}

void design_widget::alignToTop()
{
    if (activePage())
    {
        activePage()->alignToTop();
    }
}

void design_widget::alignToBottom()
{
    if (activePage())
    {
        activePage()->alignToBottom();
    }
}

void design_widget::alignToHCenter()
{
    if (activePage())
    {
        activePage()->alignToHCenter();
    }
}

void design_widget::sameHeight()
{
    if (activePage())
    {
        activePage()->sameHeight();
    }
}

void design_widget::sameWidth()
{
    if (activePage())
    {
        activePage()->sameWidth();
    }
}

bool design_widget::isNeedToSave()
{
    if (!m_report)
    {
        return false;
    }
    return m_report->isNeedToSave();
}
void design_widget::newReportButtonHide()
{
    m_newReport->hide();
}

void design_widget::previewReport()
{
    preview_report_widget *preview = new preview_report_widget();
    if (m_report)
    {
        preview->setPages(m_report->renderPages());
        //
        preview->setEditePage(m_report->editePages());
    }
    connect(preview, SIGNAL(destroyed()), this, SLOT(slotPreviewReportWidgetDesdroy()));
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show(); // 显示窗口
}

void design_widget::exportReport()
{
    if (m_report)
    {
        preview_report_widget::printPdf(m_report->renderPages());
        m_report->reloadPages();
    }
}

void design_widget::SimulatedDataOn()
{
    if (!m_report)
    {
        return;
    }
    QString json = device_variant_manager::instance().genSimulatedData(m_report->module());
    if (json.isEmpty())
    {
        return;
    }
    m_report->setRenderPrintData(json);
}

void design_widget::SimulatedDataOff()
{
    if (!m_report)
    {
        return;
    }
    m_report->clearPrintData();
}

void design_widget::setWatermarkData(QString watermarkText, const QImage &image)
{
    if (!m_watermark)
    {
        m_watermark = new watermark_item();
    }
    m_watermark->setWatermarkText(watermarkText);
    m_watermark->setWatermarkImage(image);
}

void design_widget::watermarkOn()
{
    if (!m_report)
    {
        return;
    }
    if (m_report->watermarkIsEmpty() && m_watermark)
    {
        m_report->setWatermark(m_watermark);
    }
}

void design_widget::watermarkOff()
{
    if (!m_report)
    {
        return;
    }
    watermark_item *watermark = m_report->watermarkItem();
    if (watermark)
    {
        // 文件本身存在水印，但是设置区、窗口没有内容时
        if (!m_watermark)
        {
            m_watermark = watermark;
        }
        m_report->clearWatermark();
    }
}


bool design_widget::reportWatermarkIsEmpty()
{
    if (!m_report)
    {
        return true;
    }

    return m_report->watermarkIsEmpty();
}

report_manager *design_widget::report()
{
    return m_report;
}

void design_widget::clear()
{
    if (m_report)
    {
        delete m_report;
        m_report = nullptr;
    }

    m_tabWidget->clear();
}

void design_widget::newReport(QString module, QString moduleName)
{
    createStartPage(module, moduleName);
}

bool design_widget::save()
{
    if (!m_fileName.isEmpty())
    {
        return saveToFile(m_fileName);
    }
    else
    {
        m_fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Report file name"), m_defaultFileName, "Report files (*.lrxml);; All files (*)");
        return saveToFile(m_fileName);
    }
}

bool design_widget::saveToFile(const QString &fileName)
{
    if (!m_report)
    {
        return false;
    }
    return m_report->saveToFile(fileName);
}

int design_widget::loadFile(const QString &fileName)
{
    m_report = new report_manager("", "");
    if (m_report->slotLoadFromFile(fileName))
    {
        // 根据当前软件语言，加载不同的语言翻译文件
        m_report->reloadVariantName(report_manager::genVariantNameMap(m_report->module(), device_variant_manager::instance().DeviceJson()));

        // 加载的模板文件和当前选择的设备型号不匹配
        if (m_report->deviceID() != device_variant_manager::instance().DeviceID())
        {
            delete m_report;
            m_report = nullptr;
            return -2;
        }

        createTabs();
        m_fileName = fileName;
        m_newReport->hide();
        return 0;
    }
    return -1;
}

int design_widget::loadDefalutFile(const QString &fileName, const QString &module, const QString &deviceID)
{
    m_report = new report_manager("", "");
    if (m_report->slotLoadFromFile(fileName))
    {
        m_report->setModule(module);
        m_report->setDeviceID(deviceID);

        // 根据当前软件语言，加载不同的语言翻译文件
        m_report->reloadVariantName(report_manager::genVariantNameMap(m_report->module(), device_variant_manager::instance().DeviceJson()));

        createTabs();
        m_fileName = fileName;
        m_newReport->hide();
        return 0;
    }
    return -1;
}

void design_widget::slotPreviewReportWidgetDesdroy()
{
    if (!m_report)
    {
        return;
    }
    // item 添加到预览窗口的场景后会自动从设计窗口的场景中移除，所以预览结束后需要重新添加到设计窗口的场景中。
    m_report->reloadPages();
}

int design_widget::pageCount()
{
    if (!m_report)
    {
        return 0;
    }

    return m_report->pageCount();
}

void design_widget::deleteCurrentPage()
{
    if (m_report->pageCount() > 1)
    {
        QGraphicsView *view = dynamic_cast<QGraphicsView *>(m_tabWidget->currentWidget());
        if (view)
        {
            page_scene *page = dynamic_cast<page_scene *>(view->scene());
            if (page)
            {
                if (m_report->deletePage(page))
                {
                    int index = m_tabWidget->currentIndex();
                    m_tabWidget->removeTab(m_tabWidget->currentIndex());
                    if (index > 0)
                    {
                        m_tabWidget->setCurrentIndex(index - 1);
                    }
                    emit pageDeleted();
                }
            }
        }
    }
}

QString design_widget::fileName()
{
    if (!m_report)
    {
        return "";
    }

    return m_fileName;
}

QString design_widget::module()
{
    if (!m_report)
    {
        return "";
    }

    return m_report->module();
}

QString design_widget::moduleName()
{
    if (!m_report)
    {
        return "";
    }

    return m_report->moduleName();
}

void design_widget::setBackgroundColor(QColor color)
{
    QString hexString = QString("#%1%2%3").arg(color.red(), 2, 16, QChar('0')).
                        arg(color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0'));

    QString sheet = QString("background-color: %1;").arg(hexString);
    m_tabWidget->setStyleSheet(sheet);
}

}
