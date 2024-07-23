#include "report_manager.hpp"
#include "serializators/lrxmlwriter.hpp"
#include "serializators/lrxmlreader.hpp"
#include "global.hpp"
#include "render/report_render.hpp"
#include <QDateTime>
void printTime()
{
    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();

    // 使用指定的格式化选项将当前日期和时间转换为字符串
    QString timestampString = currentDateTime.toString("hh:mm:ss.zzz");

    // 打印时分秒毫秒时间戳
    qDebug() << timestampString;
}

namespace ReportDesign
{
report_manager::LogRecall report_manager::m_logRecall;
// 注册日志回调，用于打印接口库打印日志
void report_manager::registerLogReall(LogRecall logRecall)
{
    m_logRecall = logRecall;
}

// 记录日志
void report_manager::logInfo(const QString &message)
{
    QString logStr;
    if (m_logRecall)
    {
        logStr += QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " [" + QString(__FILE__) + ":" + QString::number(__LINE__) + "] ";
        logStr += message;
        m_logRecall(logStr);
    }
}

report_manager::report_manager(QString deviceID, QString module, QString moduleName, QObject *parent):
    QObject(parent), m_deviceID(deviceID), m_reportVesion(SOFTWARE_VERSION), m_printDataVesion(PRINTDATA_VERSION),
    m_printDataManager(nullptr), m_module(module), m_moduleName(moduleName), m_translate(true), m_item_synchronization(nullptr)
{
    m_printDataManager = new print_data_manager;
    m_item_synchronization = new item_synchronization;
    connect(m_printDataManager, SIGNAL(idFindTableItem(int, base_design_item *&)),
            this, SLOT(slotIdFindTableItem(int, base_design_item *&)));

    connect(this, SIGNAL(addConnectTableItem(int, int)),
            m_printDataManager, SLOT(slotAddConnectTableItem(int, int)));

    connect(this, SIGNAL(deleteConnectTableItem(int, int)),
            m_printDataManager, SLOT(slotDeleteConnectTableItem(int, int)));
}

report_manager::~report_manager()
{
    if (m_printDataManager)
    {
        delete m_printDataManager;
    }

    if (m_item_synchronization)
    {
        delete m_item_synchronization;
    }
}

page_scene *report_manager::appendPage(const QString &pageName)
{
    page_scene *page = createPage(pageName);

    // 信号和槽链接
    connect(page, SIGNAL(addConnectTableItem(int, int)),
            this, SIGNAL(addConnectTableItem(int, int)));

    connect(page, SIGNAL(deleteConnectTableItem(int, int)),
            this, SIGNAL(deleteConnectTableItem(int, int)));

    m_pages.append(page);
    return page;
}

page_scene *report_manager::appendPage(page_item *basePage, const QString &pageName)
{
    page_scene *page = createPage(pageName, basePage);

    // 信号和槽链接
    connect(page, SIGNAL(addConnectTableItem(int, int)),
            this, SIGNAL(addConnectTableItem(int, int)));

    connect(page, SIGNAL(deleteConnectTableItem(int, int)),
            this, SIGNAL(deleteConnectTableItem(int, int)));

    m_pages.append(page);
    return page;
}

bool report_manager::deletePage(page_scene *page)
{
    QList<page_scene *>::iterator it = m_pages.begin();
    while (it != m_pages.end())
    {
        if (*it == page)
        {
            it = m_pages.erase(it);
            return true;
        }
        else
        {
            ++it;
        }
    }
    return false;
}

page_scene *report_manager::createPage(const QString &pageName, page_item *pageItem)
{
    page_scene *page = new page_scene(m_module, nullptr, page_item::Ptr(createMasterMaskPage(pageItem)));
    page->setParent(this);
    page->setObjectName(pageName);
    page->pageItem()->setObjectName("Report" + pageName);
    return page;
}

page_item  *report_manager::createMasterMaskPage(page_item *page)
{
    if (page == nullptr)
    {
        return nullptr;
    }

    // copy此page
    page_item *result = dynamic_cast<page_item *>(page->cloneItem(page->itemMode()));
    if (result)
    {
        // 删除不是母版控件的控件
        QList<base_design_item *> itemList =  result->childBaseItems();
        for (base_design_item *item : itemList)
        {
            // 创建新页，包含模板控件和水印
            if (!(item->isMasterMask() || dynamic_cast<watermark_item *>(item) != nullptr))
            {
                item->setParentItem(nullptr);
                item->setParent(nullptr);
                if (item->scene())
                {
                    item->scene()->removeItem(item);
                }
                item->deleteLater();
            }
        }
        return result;
    }
    return nullptr;
}

QObject *report_manager::createElement(const QString &, const QString &)
{
    return appendPage();
}

QObject *report_manager::elementAt(const QString &, int index)
{
    return pageAt(index);
}

int report_manager::elementsCount(const QString &)
{
    return m_pages.count();
}

void report_manager::collectionLoadFinished(const QString &)
{
    foreach (page_scene *page, m_pages)
    {
        page->setSceneRect(-SCENE_MARGIN, -SCENE_MARGIN,
                           page->pageItem()->width() + SCENE_MARGIN * 2,
                           page->pageItem()->boundingRect().height() + SCENE_MARGIN * 2);
    }
}

bool report_manager::isNeedToSave()
{
    foreach (page_scene *page, m_pages)
    {
        if (page->isHasChanges())
        {
            return true;
        }
    }
    return  false;
}

bool report_manager::saveToFile(const QString &fileName)
{
    QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
    writer->setPassPhrase("");
    writer->putItem(this);
    return writer->saveToFile(fileName);
}

ReportPages report_manager::renderPages()
{
    // 克隆page
    ReportPages newPages;
    if (m_pages.isEmpty())
    {
        logInfo("editePages is null");
        return newPages;
    }

    foreach (page_scene *page, m_pages)
    {
        page_item::Ptr pageItem = page->pageItemSharePtr();
        if (!pageItem.isNull())
        {
            page_item *newPage = dynamic_cast<page_item *>(pageItem->cloneItem(PreviewMode & PrintMode));
            //page_item *newPage = dynamic_cast<page_item *>(pageItem->cloneItem(pageItem->itemMode()));
            if (newPage)
            {
                newPages.push_back(page_item::Ptr(newPage));
            }
        }
    }

    // 拿到当前打印数据，如果没有则为空
    PrintDataMap data = m_pages.back()->pageItem()->printData();
    if (data.isEmpty())
    {
        logInfo("PrintData is null");
    }
    // 渲染
    report_render::renderPage(newPages, data);
    logInfo(QString("editePages size %1, renderPage size %2;").arg(m_pages.size()).arg(newPages.size()));
    // 返回克隆的page
    return newPages;
}

ReportPages report_manager::editePages()
{
    ReportPages reportPages;
    foreach (page_scene *page, m_pages)
    {
        reportPages.append(page->pageItemSharePtr());
    }
    return reportPages;
}

QString report_manager::module()
{
    return m_module;
}

void report_manager::setModule(QString module)
{
    foreach (page_scene *page, m_pages)
    {
        page->setModule(module);
    }
    m_module = module;
}

QString report_manager::moduleName()
{
    return m_moduleName;
}

void report_manager::setModuleName(QString moduleName)
{
    m_moduleName = moduleName;
}

void report_manager::reloadPages()
{
    foreach (page_scene *page, m_pages)
    {
        page->reloadPageItem();
    }
}

VariantNameMap report_manager::genVariantNameMap(QString module, QString json)
{
    return print_data_manager::variantNameMap(module, json);
}

void report_manager::reloadVariantName(const VariantNameMap &variantNameMap)
{
    if (!m_translate)
    {
        return;
    }

    VariantNameMap::const_iterator it =  variantNameMap.find(m_module);
    if (it != variantNameMap.end())
    {
        m_moduleName = it.value().value<variant_info>().m_name;
    }

    foreach (page_scene *page, m_pages)
    {
        page->reloadVariantName(variantNameMap);
    }
}

void report_manager::setRenderPrintData(QString json)
{
    clearPrintData();

    // 数据分割和数据转换
    PrintDataMap printData = m_printDataManager->printData(json);
    if (printData.isEmpty())
    {
        return;
    }

    foreach (page_scene *page, m_pages)
    {
        page->renderPrintData(printData);
    }
}

void report_manager::clearPrintData()
{
    foreach (page_scene *page, m_pages)
    {
        page->clearPrintData();
    }
}

void report_manager::removeAllCmd()
{
    foreach (page_scene *page, m_pages)
    {
        page->removeAllCmd();
    }
}

bool report_manager::slotLoadFromFile(const QString &fileName)
{
    ItemsReaderIntf::Ptr reader = FileXMLReader::create(fileName);
    reader->setPassPhrase("");
    if (reader->first())
    {
        if (reader->readItem(this))
        {
            removeAllCmd();
            return true;
        }
    }
    QString m_lastError = reader->lastError();
    return false;
}

void report_manager::setWatermark(watermark_item *watermark)
{
    foreach (page_scene *page, m_pages)
    {
        page->setWatermark(watermark);
    }
}

void report_manager::clearWatermark()
{
    foreach (page_scene *page, m_pages)
    {
        page->clearWatermark();
    }
}

bool report_manager::watermarkIsEmpty()
{
    foreach (page_scene *page, m_pages)
    {
        if (!page->watermarkIsEmpty())
        {
            return false;
        }
    }
    return true;
}

watermark_item *report_manager::watermarkItem()
{
    foreach (page_scene *page, m_pages)
    {
        return page->watermarkItem();
    }
    return nullptr;
}

void report_manager::slotIdFindTableItem(int tableItemID, base_design_item *&tableItem)
{
    foreach (page_scene *page, m_pages)
    {
        tableItem = page->findTableItem(tableItemID);
        if (tableItem)
        {
            return;
        }
    }
    tableItem = nullptr;
}

void report_manager::slotItemSynchronization()
{
    // 同步
    base_design_item *item = dynamic_cast<base_design_item *>(sender());
    if (item)
    {
        QList<base_design_item *> itemList = m_item_synchronization->itemSynchronization(item, editePages());
        for (base_design_item *newItem : itemList)
        {
            connect(newItem, SIGNAL(deleteItem()), this,
                    SLOT(slotSynchronizationItemDestroyed()), Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
            connect(newItem, SIGNAL(synchronization()), this,
                    SLOT(slotItemSynchronization()), Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
        }
    }
}

void report_manager::slotSynchronizationItemDestroyed()
{
    base_design_item *item = dynamic_cast<base_design_item *>(sender());
    if (item)
    {
        m_item_synchronization->synchronizationItemDestroyed(item, editePages());
    }
}

}
