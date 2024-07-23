#include "design_main_window.hpp"
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QImage>
#include <QSizeGrip>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QtWidgets>
#include "item/design_elements_factory.hpp"
#include "mask_dialog/module_type_dialog.hpp"
#include "mask_dialog/device_type_dialog.hpp"
#include "data_manager/device_variant_manager.hpp"
#include "mask_dialog/message_info_dialog.hpp"
#include "mask_dialog/base_push_button.hpp"
#include "mask_dialog/language_type_dialog.hpp"
#include "mask_dialog/about_dialog.hpp"

namespace ReportDesign
{
design_main_window::design_main_window(QWidget *parent)
    : QMainWindow(parent), m_watermarkSetingDlg(nullptr), dragging(false),
      m_cruItem(nullptr), m_reportInfo(nullptr), m_settings(nullptr), m_ownedSettings(false),
      topLeftGrip(nullptr), topRightGrip(nullptr), bottomLeftGrip(nullptr),
      bottomRightGrip(nullptr), rightEdgeGrip(nullptr), bottomEdgeGrip(nullptr),
      m_backColor(QColor(0xeb, 0xec, 0xed))
{
    initMainWindow();
    initReportEditor();
    // 创建设计元件dock
    createReportItemDock();

    createMainToolDock();

    createObjectInspectorDock();

    restoreSetting();
    QTimer::singleShot(50, this, [this]
    {
        DeviceTypeSelect();
    });

    setItemObject(this);
}

design_main_window::~design_main_window()
{
    if (m_ownedSettings && m_settings)
    {
        delete m_settings;
    }
}

void design_main_window::DeviceTypeSelect()
{
    device_type_dialog *dlg = new device_type_dialog(this);
    connect(dlg, SIGNAL(DeviceType(QString)), this, SLOT(slotDeviceType(QString)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();

    if (m_deviceType.isEmpty())
    {
        exit(0);
    }
}

void design_main_window::slotDeviceType(QString DeviceType)
{
    if (DeviceType.isEmpty())
    {
        exit(0);
    }
    m_deviceType = DeviceType;
}

void design_main_window::slotModuleType(variant_info ModuleType, QString preFileName, QString moduleFileName)
{
    if (moduleFileName.isEmpty())
    {
        // 新模板文件创建流程
        m_design_widget->newReportButtonHide();
        m_design_widget->newReport(ModuleType.m_ID, ModuleType.m_name);
        m_design_widget->SetDefaultFileName(preFileName);
    }
    else// 加载提供的默认模板，并设为新建的文件
    {
        loadDefauleReport(moduleFileName, preFileName, ModuleType.m_ID);
    }

    setReportInfoToTitle(ModuleType.m_name, preFileName);
    //
    m_simulatedData->setEnabled(true);
    m_watermark->setEnabled(true);
}

void design_main_window::initMainWindow()
{
    topLeftGrip = new QSizeGrip(this);
    topLeftGrip->setGeometry(rect().adjusted(0, 0, -cornerOffset.width() * 2, -cornerOffset.height() * 2));

    topRightGrip = new QSizeGrip(this);
    topRightGrip->setGeometry(rect().adjusted(rect().width() - 15 * 2, 0, 0, -15));

    bottomLeftGrip = new QSizeGrip(this);
    bottomLeftGrip->setGeometry(rect().adjusted(0, rect().height() - 15, -15, 0));

    bottomRightGrip = new QSizeGrip(this);
    bottomRightGrip->setGeometry(QRect(rect().width() - 15, rect().height() - 15, 15, 15));

    rightEdgeGrip = new QSizeGrip(this);
    rightEdgeGrip->setGeometry(rect().adjusted(rect().width() - edgeOffset.width(), 0, 0, rect().height()));

    bottomEdgeGrip = new QSizeGrip(this);
    bottomEdgeGrip->setGeometry(rect().adjusted(0, rect().height() - edgeOffset.height(), rect().width(), 0));

    cornerOffset.setWidth(15);
    cornerOffset.setHeight(15);
    edgeOffset.setWidth(20);
    edgeOffset.setHeight(0);

    // nomal显示尺寸
    // 获取屏幕大小
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int normalWidth = screenGeometry.width() * 4 / 5;
    int normalHeight = screenGeometry.height() * 4 / 5;
    setGeometry(screenGeometry.width() / 2 - normalWidth / 2, screenGeometry.height() / 2 - normalHeight / 2, normalWidth, normalHeight);
    setContentsMargins(5, 5, 5, 5); // 设置窗口内容的边距为0

    // 最大显示
    showMaximized();
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAcceptDrops(true);
}

void design_main_window::initReportEditor()
{
    m_design_widget = new design_widget(this);
    setCentralWidget(m_design_widget);

    // item创建完成后 action setCheckable(false);
    connect(m_design_widget, SIGNAL(itemInserted(page_scene *, QPointF, QString)),
            this, SLOT(slotItemInserted(page_scene *, QPointF, QString)));

    // 属性窗口和元件属性关联
    connect(m_design_widget, SIGNAL(itemSelected(base_design_item *)),
            this, SLOT(slotItemSelected(base_design_item *)));

    connect(m_design_widget, SIGNAL(multiItemSelected()), this, SLOT(slotMultiItemSelected()));

    connect(m_design_widget, SIGNAL(noItemSelected()), this, SLOT(slotNoItemSelected()));

    connect(m_design_widget, SIGNAL(commandHistoryChanged()), this, SLOT(slotCommandHistoryChanged()));

    connect(m_design_widget, SIGNAL(activePageChanged()), this, SLOT(slotActivePageChanged()));

    connect(m_design_widget, SIGNAL(pageAdded(page_scene *)), this, SLOT(slotPageAdded(page_scene *)));

    connect(m_design_widget, SIGNAL(pageDeleted()), this, SLOT(slotPageDeleted()));

    // 按下新建按钮
    connect(m_design_widget, SIGNAL(newReportButtonClicked()), this, SLOT(slotNewPage()));
}

void design_main_window::createMainMenuActions()
{
    m_newPageAction = new QAction(tr("new report"), this);
    connect(m_newPageAction, SIGNAL(triggered()), this, SLOT(slotNewPage()));

    m_Language = new QAction(tr("language"), this);
    connect(m_Language, SIGNAL(triggered()), this, SLOT(slotLanguage()));

    m_saveReportAction = new QAction(tr("Save Report"), this);
    m_saveReportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(m_saveReportAction, SIGNAL(triggered()), this, SLOT(slotSaveReport()));

    m_saveReportAsAction = new QAction(tr("Save Report As"), this);
    m_saveReportAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    connect(m_saveReportAsAction, SIGNAL(triggered()), this, SLOT(slotSaveReportAs()));

    m_loadReportAction = new QAction(tr("Load Report"), this);
    m_loadReportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    connect(m_loadReportAction, SIGNAL(triggered()), this, SLOT(slotLoadReport()));

    m_exitAction = new QAction(tr("exit"), this);
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(slotCloseWindow()));

    m_about = new QAction(tr("about"), this);
    connect(m_about, SIGNAL(triggered()), this, SLOT(slotShowAbout()));
}

QList<QLabel *>  design_main_window::createReportToolActions()
{
    QList<QLabel *> itemList;
    m_newTextItemAction = new Item_button(tr("TextItem"), this);
    m_newTextItemAction->setWhatsThis(TEXT_ITEM_TYPE);
    m_newTextItemAction->setImage(":/TextItem");
    connect(m_newTextItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newTextItemAction);

    m_newGroupTextItemAction = new Item_button(tr("GroupTextItem"), this);
    m_newGroupTextItemAction->setWhatsThis(GROUP_TEXT_ITEM_TYPES);
    m_newGroupTextItemAction->setImage(":/GroupTextItem");
    connect(m_newGroupTextItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newGroupTextItemAction);

    m_newShapeItemAction = new Item_button(tr("ShapeItem"), this);
    m_newShapeItemAction->setWhatsThis(SHAPE_ITEM_TYPE);
    m_newShapeItemAction->setImage(":/ShapeItem");
    connect(m_newShapeItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newShapeItemAction);

    m_newTableItemAction = new Item_button(tr("TableItem"), this);
    m_newTableItemAction->setWhatsThis(TABLE_ITEM_TYPE);
    m_newTableItemAction->setImage(":/row_header_table");
    connect(m_newTableItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newTableItemAction);

    m_newSVGItemAction = new Item_button(tr("SVGItem"), this);
    m_newSVGItemAction->setWhatsThis(SVG_ITEM_TYPE);
    m_newSVGItemAction->setImage(":/SVGItem");
    connect(m_newSVGItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newSVGItemAction);

    m_newImageItemAction = new Item_button(tr("ImageItem"), this);
    m_newImageItemAction->setWhatsThis(IMAGE_ITEM_TYPE);
    m_newImageItemAction->setImage(":/ImageItem");
    connect(m_newImageItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newImageItemAction);

    m_newPageNumberItemAction = new Item_button(tr("PageNumberItem"), this);
    m_newPageNumberItemAction->setWhatsThis(PAGENUMBER_ITEM_TYPE);
    m_newPageNumberItemAction->setImage(":/PageNumber");
    connect(m_newPageNumberItemAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_newPageNumberItemAction);

    m_colHeaderTableAction = new Item_button(tr("ColHeaderTable"), this);
    m_colHeaderTableAction->setWhatsThis(COL_HEADER_TABLE_ITEM_TYPE);
    m_colHeaderTableAction->setImage(":/col_header_table");
    connect(m_colHeaderTableAction, SIGNAL(pressed()), this, SLOT(slotAddNewItem()));
    itemList.push_back(m_colHeaderTableAction);

    return itemList;
}

void design_main_window::createReportItemDock()
{
    QList<QLabel *> itemList = createReportToolActions();
    QDockWidget *objectDoc = new QDockWidget(this);
    objectDoc->setTitleBarWidget(new QWidget(this));
    QWidget *w = new QWidget(objectDoc);

    QVBoxLayout *vlayout = new QVBoxLayout(w);
    vlayout->addSpacing(10);
    QHBoxLayout *hlayout = new QHBoxLayout();
    //
    hlayout->addSpacing(10);
    m_ReportItemDockTitle = new base_Label(tr("Design Elements"), this);
    m_ReportItemDockTitle->setFixedWidth(150);
    hlayout->addWidget(m_ReportItemDockTitle);//组件元素
    hlayout->addStretch();
    hlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addLayout(hlayout);
    vlayout->addSpacing(10);
    hlayout = new QHBoxLayout();
    vlayout->addLayout(hlayout);
    hlayout->setContentsMargins(0, 0, 0, 0);
    int num = 0;
    for (QLabel *it : itemList)
    {
        it->setContentsMargins(0, 0, 0, 0);
        if (num != 0 && num % 2 == 0)
        {
            hlayout->addSpacing(10);
            hlayout = new QHBoxLayout();
            vlayout->addLayout(hlayout);
            hlayout->setContentsMargins(0, 0, 0, 0);
        }
        hlayout->addSpacing(5);
        hlayout->addWidget(it);
        num++;
    }

    if (num % 2)
    {
        hlayout->addStretch();
    }
    else
    {
        hlayout->addSpacing(10);
    }

    vlayout->addStretch();
    vlayout->setContentsMargins(0, 0, 0, 0);
    objectDoc->setWindowFlags(Qt::FramelessWindowHint);
    objectDoc->setFeatures(QDockWidget::NoDockWidgetFeatures);
    objectDoc->setWidget(w);
    objectDoc->setObjectName("ReportItemBar");
    m_pageEditors.append(objectDoc);
    addDockWidget(Qt::LeftDockWidgetArea, objectDoc);
}

void design_main_window::createMainToolButton()
{
    createMainMenuActions();
    m_infoMenu = new QMenu(this);
    m_infoButton = new QToolButton(this);
    m_infoButton->setIcon(QIcon(":/menu"));
    m_infoButton->setMinimumSize(50, 44);
    m_infoButton->setToolTip(tr("menu"));
    m_infoButton->setPopupMode(QToolButton::InstantPopup);
    m_infoButton->setMenu(m_infoMenu);
    m_infoMenu->addAction(m_newPageAction);
    m_recentFilesMenu = m_infoMenu->addMenu(tr("Recent Files"));
    m_recentFilesMenu->setObjectName("recentFilesMenu");
    m_infoMenu->addAction(m_Language);
    m_infoMenu->addAction(m_saveReportAction);
    m_infoMenu->addAction(m_saveReportAsAction);
    m_infoMenu->addAction(m_loadReportAction);
    m_infoMenu->addAction(m_about);
    m_infoMenu->addAction(m_exitAction);

    m_recentFilesSignalMap = new QSignalMapper(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    connect(m_recentFilesSignalMap, SIGNAL(mappedString(QString)), this, SLOT(slotLoadRecentFile(QString)));
#else
    connect(m_recentFilesSignalMap, SIGNAL(mapped(QString)), this, SLOT(slotLoadRecentFile(QString)));
#endif
    m_recentFilesMenu->setDisabled(m_recentFiles.isEmpty());

    m_reportInfo = new base_Label(tr("module-file"), this);
    m_reportInfo->setFixedWidth(300);
    m_reportInfo->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_zoomIn = new image_button(this, ":/zoomIn", ":/zoomIn_c");
    m_zoomIn->setToolTip(tr("Zoom In"));
    connect(m_zoomIn, SIGNAL(clicked(bool)), this, SLOT(slotZoomIn()));

    m_zoomOut = new image_button(this, ":/zoomOut", ":/zoomOut_c");
    m_zoomOut->setToolTip(tr("Zoom Out"));
    connect(m_zoomOut, SIGNAL(clicked(bool)), this, SLOT(slotZoomOut()));

    m_copy = new image_button(this, ":/copy", ":/copy_c");
    m_copy->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    m_copy->setToolTip(tr("Copy"));
    connect(m_copy, SIGNAL(clicked(bool)), this, SLOT(slotCopy()));

    m_connectCopy = new image_button(this);
    m_connectCopy->hide();
    m_connectCopy->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_connectCopy->setToolTip(tr("connectCopy"));
    connect(m_connectCopy, SIGNAL(clicked(bool)), this, SLOT(slotVariantLock()));

    m_paste = new image_button(this, ":/paste", ":/paste_c");
    m_paste->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    m_paste->setToolTip(tr("Paste"));
    connect(m_paste, SIGNAL(clicked(bool)), this, SLOT(slotPaste()));

    m_cut = new image_button(this, ":/cut", ":/cut_c");
    m_cut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_X));
    m_cut->setToolTip(tr("Cut"));
    connect(m_cut, SIGNAL(clicked(bool)), this, SLOT(slotCut()));

    m_undo = new image_button(this, ":/undo", ":/undo_c");
    m_undo->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));
    m_undo->setToolTip(tr("Undo"));
    connect(m_undo, SIGNAL(clicked(bool)), this, SLOT(slotUndo()));

    m_delete = new image_button(this, ":/delete", ":/delete_c");
    m_delete->setShortcut(QKeySequence("Del"));
    m_delete->setToolTip(tr("delete"));
    connect(m_delete, SIGNAL(clicked(bool)), this, SLOT(slotDelete()));

    m_previewReport = new base_push_button(this);
    m_previewReport->setObjectName("previewReportButton");
    m_previewReport->setMinimumSize(56, 28);
    m_previewReport->setMaximumWidth(100);
    m_previewReport->setText(tr("Render Report"));
    m_previewReport->setToolTip(tr("Render Report"));
    connect(m_previewReport, SIGNAL(clicked(bool)), this, SLOT(slotPreviewReport()));

    m_exportReport = new gbase_push_button(this);
    m_exportReport->setObjectName("exportReportButton");
    m_exportReport->setMinimumSize(56, 28);
    m_exportReport->setMaximumWidth(100);
    m_exportReport->setText(tr("export Report"));
    m_exportReport->setToolTip(tr("export Report"));
    //m_exportReport->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    connect(m_exportReport, SIGNAL(clicked(bool)), this, SLOT(slotExportReport()));

    m_addPage = new image_button(this, ":/addPage", ":/addPage_c");
    m_addPage->setToolTip(tr("Add Page"));
    connect(m_addPage, SIGNAL(clicked(bool)), this, SLOT(slotAddPage()));

    m_deletePage = new image_button(this, ":/deletePage", ":/deletePage_c");
    m_deletePage->setToolTip(tr("Delete Page"));
    connect(m_deletePage, SIGNAL(clicked(bool)), this, SLOT(slotDeletePage()));

    m_widgetMin = new image_button(this, ":/minWindow", ":/minWindow_c");
    connect(m_widgetMin, &QPushButton::clicked, [this]
    {
        showMinimized();
    });

    m_widgetMax = new image_button(this, ":/showMax", ":/showMax_c");
    connect(m_widgetMax, &QPushButton::clicked, [this]
    {
        if (m_widgetMax->imageUrl() == ":/showNomal")
        {
            showMaximized();
            m_widgetMax->SetImage(":/showMax", ":/showMax_c");
        }
        else
        {
            showNormal();
            m_widgetMax->SetImage(":/showNomal", ":/showNomal_c");
        }
    });

    m_widgetClose = new image_button(this, ":/closeWindow", ":/closeWindow_c");
    connect(m_widgetClose, &QPushButton::clicked, [this]
    {
        slotCloseWindow();
    });
}

void design_main_window::createMainToolDock()
{
    createMainToolButton();
    m_mainTooldock = new QDockWidget(this);
    QDockWidget *objectDoc = m_mainTooldock;
    objectDoc->setTitleBarWidget(new QWidget(this));
    QWidget *w = new QWidget(objectDoc);
    const int offset = 30;
    QHBoxLayout *hlayout = new QHBoxLayout(w);

    hlayout->addWidget(m_infoButton);
    hlayout->addSpacing(10);
    hlayout->addWidget(m_reportInfo);

    int spacing = width() / 2 - m_infoButton->width() - 10 - m_reportInfo->width();

    int offset2 = spacing - (m_zoomIn->width() * 4 + 4 * offset);

    hlayout->addStretch();
    hlayout->addSpacing(offset2);
    hlayout->addWidget(m_zoomIn);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_zoomOut);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_copy);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_paste);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_cut);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_undo);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_delete);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_addPage);
    hlayout->addSpacing(offset);
    hlayout->addWidget(m_deletePage);
    hlayout->addSpacing(150);
    hlayout->addStretch();
    hlayout->addWidget(m_previewReport);
    hlayout->addSpacing(8);
    hlayout->addWidget(m_exportReport);
    hlayout->addSpacing(50);
    hlayout->addStretch();
    hlayout->addWidget(m_widgetMin);
    hlayout->addSpacing(1);
    hlayout->addWidget(m_widgetMax);
    hlayout->addSpacing(1);
    hlayout->addWidget(m_widgetClose);
    hlayout->addSpacing(10);


    objectDoc->setWindowFlags(Qt::FramelessWindowHint);
    objectDoc->setFeatures(QDockWidget::NoDockWidgetFeatures);
    objectDoc->setWidget(w);
    objectDoc->setObjectName("ReportItemBar");
    m_pageEditors.append(objectDoc);
    addDockWidget(Qt::TopDockWidgetArea, objectDoc);
}

void design_main_window::createObjectInspectorDock()
{
    m_page_prop_widget = new object_prop_widget(this);
    m_item_prop_widget = new object_prop_widget(this);

    // 示例开关数据
    m_simulatedData =  new switchButton(this);
    m_simulatedData->setEnabled(false);
    connect(m_simulatedData, &switchButton::turnOn, this, &design_main_window::slotSimulatedDataOn);
    connect(m_simulatedData, &switchButton::turnOff, this, &design_main_window::slotSimulatedDataOff);

    // 水印开关按钮
    m_watermark =  new switchButton(this);
    m_watermark->setEnabled(false);
    connect(m_watermark, &switchButton::turnOn, this, &design_main_window::slotWatermarkOn);
    connect(m_watermark, &switchButton::turnOff, this, &design_main_window::slotWatermarkOff);

    // 水印设置按钮
    m_watermarkSeting =  new QPushButton(this);
    m_watermarkSeting->setObjectName("watermarkSeting");
    m_watermarkSeting->setText(tr("watermark seting"));
    connect(m_watermarkSeting, &switchButton::clicked, this, &design_main_window::slotWatermarkWidgetOpen);

    QDockWidget *objectDoc = new QDockWidget(this);
    objectDoc->setTitleBarWidget(new QWidget(this));
    QWidget *w = new QWidget(objectDoc);
    w->setMinimumWidth(350);

    //    QWidget *containWidget = new QWidget(w);
    //    QVBoxLayout *l = new QVBoxLayout(containWidget);
    QVBoxLayout *l = new QVBoxLayout(w);
    //QVBoxLayout *lw = new QVBoxLayout(w);
    //lw->addLayout(l);

    // page属性窗口
    l->addWidget(m_page_prop_widget);
    // item属性窗口
    l->addWidget(m_item_prop_widget);
    l->addStretch();

    //    QScrollArea *scrollArea = new QScrollArea(this);
    //    scrollArea->setWidgetResizable(true);
    //    scrollArea->setWidget(containWidget);
    //    lw->addWidget(scrollArea);
    //    m_item_prop_widget->setMinimumHeight(0);

    // 水印
    QHBoxLayout  *hlayout = new QHBoxLayout();
    hlayout->addSpacing(10);
    hlayout->addWidget(m_watermark, 0);
    hlayout->addSpacing(15);
    QLabel *watermarkLabel = new QLabel(tr("watermark"), w); // 水印
    //QLabel *watermarkLabel = new QLabel(tr("watermark"), containWidget); // 水印
    hlayout->addWidget(watermarkLabel);
    hlayout->addStretch();
    hlayout->addWidget(m_watermarkSeting, 0);
    hlayout->addSpacing(13);
    l->addLayout(hlayout);

    // 示例数据
    l->addSpacing(20);
    hlayout = new QHBoxLayout();
    hlayout->addSpacing(10);
    hlayout->addWidget(m_simulatedData);
    hlayout->addSpacing(15);
    QLabel *simulatedDataLabel = new QLabel(tr("simulated data"), w); // 示例数据
    //QLabel *simulatedDataLabel = new QLabel(tr("simulated data"), containWidget); // 示例数据
    hlayout->addWidget(simulatedDataLabel);
    hlayout->addStretch();
    l->addLayout(hlayout);

    l->addSpacing(40);
    l->setContentsMargins(0, 0, 0, 0);
    //lw->setContentsMargins(0, 0, 0, 0);

    objectDoc->setWindowFlags(Qt::FramelessWindowHint);
    objectDoc->setFeatures(QDockWidget::NoDockWidgetFeatures);

    objectDoc->setWidget(w);
    objectDoc->setObjectName("objectInspector");
    objectDoc->setContentsMargins(0, 0, 0, 0);
    m_pageEditors.append(objectDoc);
    objectDoc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addDockWidget(Qt::RightDockWidgetArea, objectDoc);
}

void design_main_window::slotAddNewItem()
{
    Item_button *action = dynamic_cast<Item_button *>(sender());
    m_reportItemIsLocked = QApplication::keyboardModifiers() == Qt::SHIFT;
    m_cruItem = action;
    m_design_widget->startInsertMode(action->whatsThis());
}

void design_main_window::slotNewPage()
{
    if (checkNeedToSave())
    {
        module_type_dialog *dlg = new module_type_dialog(this);
        connect(dlg, SIGNAL(moduleType(variant_info, QString, QString)), this, SLOT(slotModuleType(variant_info, QString, QString)));
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    }
    m_watermark->setTurnOff();
}

void design_main_window::slotLoadReport()
{
    if (!checkNeedToSave())
    {
        return;    // don't need to save
    }

    QString fileName = QFileDialog::getOpenFileName(
                           this, tr("Report file name"),
                           "",
                           "Report files(*.lrxml);; All files(*)"
                       );
    if (!fileName.isEmpty())
    {
        QApplication::processEvents();
        setCursor(Qt::WaitCursor);
        m_design_widget->clear();
        int ret = m_design_widget->loadFile(fileName);
        if (ret == 0)
        {
            setItemObject(nullptr);
            updateRedoUndo();
            setReportInfoToTitle(m_design_widget->moduleName(), fileName);

            // 更新最近打开文件
            if (!m_recentFiles.contains(fileName))
            {
                if (m_recentFiles.count() == 10)
                {
                    QMap<QString, QDateTime>::const_iterator it = m_recentFiles.constBegin();
                    QDateTime minDate = QDateTime::currentDateTime();
                    while (it != m_recentFiles.constEnd())
                    {
                        if (minDate > it.value())
                        {
                            minDate = it.value();
                        }
                        ++it;
                    }
                    m_recentFiles.remove(m_recentFiles.key(minDate));
                }
                m_recentFiles.insert(fileName, QDateTime::currentDateTime());
            }
            else
            {
                m_recentFiles[fileName] = QDateTime::currentDateTime();
            }
            createRecentFilesMenu();
            m_simulatedData->setEnabled(true);
            m_watermark->setEnabled(true);

            if (!m_design_widget->reportWatermarkIsEmpty())
            {
                m_watermark->setTurnOn();
            }
        }
        else if (ret == -1)
        {
            // 加载失败，文件加载错误?
            QMessageBox::warning(this, tr("error"), tr("File loading failure!"));
        }
        else if (ret == -2)
        {
            // 仪器类型不一致
            QMessageBox::warning(this, tr("error"), tr("Instrument types are inconsistent!"));
        }
        addRecentFile(fileName);
        unsetCursor();
    }
}

void design_main_window::loadDefauleReport(QString fileName, QString preFileName, QString moduleType)
{
    if (!fileName.isEmpty())
    {
        QApplication::processEvents();
        setCursor(Qt::WaitCursor);
        m_design_widget->clear();
        int ret = m_design_widget->loadDefalutFile(fileName, moduleType, device_variant_manager::instance().DeviceID());
        if (ret == 0)
        {
            m_design_widget->setFileNameEmpty();
            setItemObject(nullptr);
            updateRedoUndo();
            m_design_widget->SetDefaultFileName(preFileName);
            setReportInfoToTitle(m_design_widget->moduleName(), preFileName);
        }
        else if (ret == -1)
        {
            // 加载失败，文件加载错误?
            QMessageBox::warning(this, tr("error"), tr("File loading failure!"));
        }
        else if (ret == -2)
        {
            // 仪器类型不一致
            QMessageBox::warning(this, tr("error"), tr("Instrument types are inconsistent!"));
        }
        unsetCursor();
    }
}

void design_main_window::slotSaveReport()
{
    if (!m_design_widget->report())
    {
        return;
    }

    if (m_design_widget->save())
    {
        setReportInfoToTitle(m_design_widget->moduleName(), m_design_widget->fileName());
        if (!m_design_widget->fileName().isEmpty())
        {
            addRecentFile(m_design_widget->fileName());
        }
    }
}

void design_main_window::slotSaveReportAs()
{
    if (!m_design_widget->report())
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Report file name"), "", "Report files(*.lrxml);; All files(*)");
    if (!fileName.isEmpty())
    {
        if (m_design_widget->saveToFile(fileName))
        {
            addRecentFile(fileName);
        }
    }
}

void design_main_window::slotAddPage()
{
    m_design_widget->addPage();
}

void design_main_window::slotLanguage()
{
    language_type_dialog *dlg = new language_type_dialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void design_main_window::slotCloseWindow()
{
    if (checkNeedToSave())
    {
        writeState();
        close();
    }
}

void design_main_window::slotItemInserted(page_scene *, QPointF, const QString &ItemType)
{
    if (!m_reportItemIsLocked)
    {
        m_cruItem = nullptr;
    }
    else
    {
        m_design_widget->startInsertMode(ItemType);
    }
}

void design_main_window::slotItemSelected(base_design_item *item)
{
    setItemObject(item);
}

void design_main_window::slotNoItemSelected()
{
    setItemObject(nullptr);
}

void design_main_window::slotMultiItemSelected()
{
    if (m_design_widget->activePage() == nullptr)
    {
        return;
    }

    QList<QObject *> selectionList;
    foreach (QGraphicsItem *gi, m_design_widget->activePage()->selectedItems())
    {
        QObject *oi = dynamic_cast<QObject *>(gi);
        if (oi)
        {
            selectionList.append(oi);
        }
    }

    setItemObjects(selectionList);
}

void design_main_window::slotDelete()
{
    m_design_widget->deleteSelectedItems();
}

void design_main_window::slotDeletePage()
{
    if (m_design_widget->pageCount() > 1)
    {
        m_design_widget->deleteCurrentPage();
    }
}

void design_main_window::slotZoomIn()
{
    m_design_widget->scale(ZOOM_IN_UNIT, ZOOM_IN_UNIT);
}

void design_main_window::slotZoomOut()
{
    m_design_widget->scale(ZOOM_OUT_UNIT, ZOOM_OUT_UNIT);
}

void design_main_window::slotPreviewReport()
{
    m_design_widget->previewReport();
}

void design_main_window::slotExportReport()
{
    m_design_widget->exportReport();
}

void design_main_window::slotPaste()
{
    m_design_widget->paste();
}

void design_main_window::slotCut()
{
    m_design_widget->cut();
}

void design_main_window::slotCopy()
{
    m_design_widget->copy();
}

void design_main_window::slotVariantLock()
{
    m_design_widget->variantLock();
}

void design_main_window::slotUndo()
{
    m_design_widget->undo();
    updateRedoUndo();
}

void design_main_window::slotRedo()
{
    m_design_widget->redo();
    updateRedoUndo();
}

void design_main_window::updateRedoUndo()
{

}

void design_main_window::slotCommandHistoryChanged()
{
    updateRedoUndo();
}

void design_main_window::slotSimulatedDataOn()
{
    m_design_widget->SimulatedDataOn();
}

void design_main_window::slotSimulatedDataOff()
{
    m_design_widget->SimulatedDataOff();
}

void design_main_window::slotWatermarkOn()
{
    m_design_widget->watermarkOn();
}

void design_main_window::slotWatermarkOff()
{
    m_design_widget->watermarkOff();
}

void design_main_window::slotWatermarkWidgetOpen()
{
    if (!m_watermarkSetingDlg)
    {
        m_watermarkSetingDlg = new watermark_setting_dialog(this);
        connect(m_watermarkSetingDlg, SIGNAL(watermarkSetting(QString, QString)), this, SLOT(slotWatermarkSeting(QString, QString)));
    }
    m_watermarkSetingDlg->exec();
}

void design_main_window::slotWatermarkSeting(QString text, QString imagePath)
{
    //QImage watermarkImage(imagePath);
    m_design_widget->watermarkOff();
    m_design_widget->setWatermarkData(text, QImage(imagePath));
    if (m_watermark->isTurnOn())
    {
        m_design_widget->watermarkOn();
    }
}

void design_main_window::slotShowAbout()
{
    about_dialog *p = new about_dialog();
    p->exec();
}

bool design_main_window::checkNeedToSave()
{
    if (m_design_widget->isNeedToSave())
    {
        message_info_dialog::DlgResult ret = message_info_dialog::message(this, tr("tips"), tr("Do you want to save the current changes?"));
        switch (ret)
        {
        case message_info_dialog::cancle:
            return false;
        case message_info_dialog::yes:
            if (m_design_widget->save())
            {
                return true;
            }
            else
            {
                return false;
            }
        case message_info_dialog::no:
            return true;
        default:
            return false;
        }
    }
    return true;
}

void design_main_window::slotPageDeleted()
{

}

void design_main_window::slotPageAdded(page_scene *)
{

}

void design_main_window::slotActivePageChanged()
{
    if (m_design_widget->activePage())
    {
        setPageItemObject(m_design_widget->activePage()->pageItem());
    }
}

void design_main_window::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("inserItem"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void design_main_window::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("inserItem"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void design_main_window::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("inserItem"))
    {
        QByteArray itemData = event->mimeData()->data("inserItem");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString what;
        QPoint offset;
        dataStream >> what >> offset;

        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
    m_cruItem = nullptr;
}

void design_main_window::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        // 鼠标点击到TopDock,可以拖动
        if (m_mainTooldock->geometry().contains(event->pos()))
        {
            // 窗口拖动逻辑
            if (m_widgetMax->imageUrl() == ":/showMax")
            {
                m_widgetMax->clicked();
            }

            QPoint movePos = event->globalPos() - mousePressPos;
            this->move(windowPos + movePos);
        }
    }

    QMainWindow::mouseMoveEvent(event);
}

//! [1]
void design_main_window::mousePressEvent(QMouseEvent *event)
{
    // 元件拖动逻辑
    if (m_cruItem)
    {
        Item_button *child = m_cruItem;
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << child->whatsThis() << QPoint(event->pos() - child->pos());
        //! [1]

        //! [2]
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("inserItem", itemData);
        //! [2]

        //! [3]
        //!
        QPixmap pixmap(QPixmap::fromImage(child->dropImage()));
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);

        QPoint pos(event->pos().x() - child->pos().x(),
                   event->pos().y() - child->pos().y() - child->height() + child->textSize().height());
        drag->setHotSpot(pos);
        //! [3]
        child->setClickGray(true);

        if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
        {
            child->setClickGray(false);
        }
        else
        {
            child->setClickGray(false);
        }
        return;
    }

    // 点击顶部可以移动整个窗口
    if (event->button() == Qt::LeftButton)
    {
        // 鼠标点击到TopDock,可以拖动
        if (m_mainTooldock->geometry().contains(event->pos()))
        {
            mousePressPos = event->globalPos();
            windowPos = this->frameGeometry().topLeft();
        }

    }

    QMainWindow::mousePressEvent(event);
}

void design_main_window::mouseReleaseEvent(QMouseEvent *event)
{
    QMainWindow::mouseReleaseEvent(event);
}

void design_main_window::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 鼠标点击到TopDock,可以拖动
    if (m_mainTooldock->geometry().contains(event->pos()))
    {
        m_widgetMax->clicked();
    }
}

void design_main_window::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    // 获取窗口的边界矩形
    QRect rects = rect();
    rects.adjust(0, 0, -1, -1);

    QPainter painter(this);
    // 背景色
    painter.fillRect(rect(), m_backColor);

    // 使用QPainter在边界矩形上绘制边框
    painter.setPen(QPen(Qt::white, 10));
    painter.drawRect(rects);
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawRect(rects);
}

void design_main_window::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    topLeftGrip->setGeometry(rect().adjusted(0, 0, -cornerOffset.width(), -cornerOffset.height()));
    topRightGrip->setGeometry(rect().adjusted(rect().width() - cornerOffset.width(), 0, 0, -cornerOffset.height()));
    bottomLeftGrip->setGeometry(rect().adjusted(0, rect().height() - cornerOffset.height(), -cornerOffset.width(), 0));
    bottomRightGrip->setGeometry(rect().adjusted(rect().width() - cornerOffset.width(), rect().height() - cornerOffset.height(), 0, 0));
}

void design_main_window::setReportInfoToTitle(QString moduleName, QString fileName)
{
    QFileInfo fileInfo(fileName);
    fileName = fileInfo.fileName();
    if (m_reportInfo)
    {
        if (fileName.isEmpty())
        {
            fileName = tr("new");
        }
        int labelWidth = m_reportInfo->width(); // 获取QLabel控件的宽度
        QString text = moduleName + " - " + fileName; // 获取QLabel的文本内容
        QFontMetrics fontMetrics(m_reportInfo->font()); // 使用QLabel的字体创建QFontMetrics对象
        int textWidth = fontMetrics.width(text); // 计算文本的宽度
        if (textWidth > labelWidth)
        {
            QString ellipsizedText = fontMetrics.elidedText(moduleName + " - " + fileName, Qt::ElideRight, labelWidth); // 使用省略号替代超出部分
            m_reportInfo->setText(ellipsizedText); // 更新QLabel的文本内容
        }
        else
        {
            m_reportInfo->setText(text); // 更新QLabel的文本内容
        }
        m_reportInfo->setToolTip(text);
    }
}

QSettings *design_main_window::settings()
{
    if (m_settings)
    {
        return m_settings;
    }
    else
    {
        m_settings = new QSettings("MACCURA", QApplication::applicationName());
        m_ownedSettings = true;
        return m_settings;
    }
}

void design_main_window::setSettings(QSettings *value)
{
    if (m_ownedSettings)
    {
        delete m_settings;
    }
    m_settings = value;
    m_ownedSettings = false;
    restoreSetting();
}

void design_main_window::restoreSetting()
{
    settings()->beginGroup("RecentFiles");
    int filesCount = settings()->value("filesCount").toInt();
    QStringList keys = settings()->childKeys();
    for (int i = 0; i < filesCount; i++)
    {
        if (keys.contains("fileName" + QString::number(i)))
        {
            m_recentFiles[settings()->value("fileName" + QString::number(i)).toString()] =
                settings()->value("fileData" + QString::number(i)).toDateTime();

        }
        else
        {
            break;
        }

    }
    settings()->endGroup();
    createRecentFilesMenu();
}


void design_main_window::addRecentFile(const QString &fileName)
{
    if (!m_recentFiles.contains(fileName))
    {
        if (m_recentFiles.count() == 10)
        {
            QMap<QString, QDateTime>::const_iterator it = m_recentFiles.constBegin();
            QDateTime minDate = QDateTime::currentDateTime();
            while (it != m_recentFiles.constEnd())
            {
                if (minDate > it.value())
                {
                    minDate = it.value();
                }
                ++it;
            }
            m_recentFiles.remove(m_recentFiles.key(minDate));
        }
        m_recentFiles.insert(fileName, QDateTime::currentDateTime());
    }
    else
    {
        m_recentFiles[fileName] = QDateTime::currentDateTime();
    }
    createRecentFilesMenu();
}

void design_main_window::createRecentFilesMenu()
{
    if (m_recentFilesMenu)
    {
        m_recentFilesMenu->clear();
        removeNotExistedRecentFiles();
        foreach (QString fileName, m_recentFiles.keys())
        {
            QAction *tmpAction = new QAction(QIcon(":/newReport"), fileName, this);
            connect(tmpAction, SIGNAL(triggered()), m_recentFilesSignalMap, SLOT(map()));
            m_recentFilesSignalMap->setMapping(tmpAction, fileName);
            m_recentFilesMenu->addAction(tmpAction);
        }
        m_recentFilesMenu->setDisabled(m_recentFiles.isEmpty());
    }
}

void design_main_window::removeNotExistedRecentFiles()
{
    QMap<QString, QDateTime>::iterator it = m_recentFiles.begin();
    while (it != m_recentFiles.end())
    {
        if (!QFile::exists(it.key()))
        {
            it = m_recentFiles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void design_main_window::removeNotExistedRecentFilesFromMenu(const QString &fileName)
{
    if (m_recentFilesMenu)
    {
        foreach (QAction *action, m_recentFilesMenu->actions())
        {
            if (action->text().compare(fileName) == 0)
            {
                m_recentFilesMenu->removeAction(action);
                break;
            }
        }
    }
}

void design_main_window::slotLoadRecentFile(const QString fileName)
{
    if (checkNeedToSave())
    {
        QApplication::processEvents();
        if (QFile::exists(fileName))
        {
            setCursor(Qt::WaitCursor);
            m_design_widget->clear();
            int ret = m_design_widget->loadFile(fileName);
            if (ret == 0)
            {
                setReportInfoToTitle(m_design_widget->moduleName(), fileName);
                setItemObject(nullptr);
                unsetCursor();
                m_recentFiles[fileName] = QDateTime::currentDateTime();
                m_simulatedData->setEnabled(true);
                m_watermark->setEnabled(true);

                if (!m_design_widget->reportWatermarkIsEmpty())
                {
                    m_watermark->setTurnOn();
                }
            }
            else if (ret == -1)
            {
                // 加载失败，文件加载错误?
                QMessageBox::warning(this, tr("error"), tr("File loading failure!"));
            }
            else if (ret == -2)
            {
                // 仪器类型不一致
                QMessageBox::warning(this, tr("error"), tr("Instrument types are inconsistent!"));
            }
            setCursor(Qt::ArrowCursor);
        }
        else
        {
            m_recentFiles.remove(fileName);
            removeNotExistedRecentFilesFromMenu(fileName);
            QMessageBox::information(this, tr("Warning"), tr("File \"%1\" not found!").arg(fileName));
        }
    }
}

QColor design_main_window::artboardBackground()
{
    return m_backColor;
}

void design_main_window::setArtboardBackground(QColor color)
{
    m_backColor = color;
    if (m_design_widget)
    {
        m_design_widget->setBackgroundColor(color);
    }
    update();
}

void design_main_window::writeState()
{
    settings()->beginGroup("RecentFiles");
    settings()->setValue("filesCount", m_recentFiles.count());
    QMap<QString, QDateTime>::const_iterator it = m_recentFiles.constBegin();
    int count = 0;
    while (it != m_recentFiles.constEnd())
    {
        settings()->setValue("fileName" + QString::number(count), it.key());
        settings()->setValue("fileDate" + QString::number(count), it.value());
        ++count;
        ++it;
    }
    settings()->endGroup();
}

void design_main_window::setPageItemObject(QObject *object)
{
    if (m_page_prop_widget)
    {
        m_page_prop_widget->setObject(object);
    }
    setItemObject(nullptr);
}

void design_main_window::setItemObject(QObject *object)
{
    if (m_item_prop_widget)
    {
        if (m_item_prop_widget->object() != object)
        {
            m_item_prop_widget->setObject(object);
        }
    }
}

void design_main_window::setItemObjects(QList<QObject *> object)
{
    if (m_item_prop_widget)
    {
        if (m_item_prop_widget->objects() != object)
        {
            m_item_prop_widget->setMultiObjects(&object);
        }
    }
}

}
