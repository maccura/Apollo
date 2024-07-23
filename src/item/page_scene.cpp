#include "page_scene.hpp"
#include <QPrinter>
#include <QDebug>
#include <QGraphicsItem>
#include <QString>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QList>
#include <QGraphicsView>
#include <QScopedPointer>
#include "base_design_item.hpp"
#include "item/table_item.hpp"
#include "design_elements_factory.hpp"
#include "serializators/lrxmlreader.hpp"
#include "serializators/lrxmlwriter.hpp"
#include "global.hpp"
#include "data_manager/device_variant_manager.hpp"
#include "page_number_item.hpp"
#include "mask_dialog/group_property_dialog.hpp"
namespace ReportDesign
{

page_scene::page_scene(QString module, QObject *parent, page_item::Ptr pageItem):
    QGraphicsScene(parent),
    m_pageItem(0),
    m_insertMode(false),
    m_itemInsertRect(0),
    m_itemMode(DesignMode),
    m_cutterBorder(0),
    //    m_infoPosRect(0),
    m_currentCommand(-1),
    m_changeSizeMode(false),
    m_changePosMode(false),
    m_viewChang(false),
    m_changePosOrSizeMode(false),
    m_executingCommand(false),
    m_hasChanges(false),
    m_isLoading(false),
    m_executingGroupCommand(false),
    m_settings(0),
    m_selectionRect(0),
    m_verticalGridStep(DEFAULT_GRID_STEP),
    m_horizontalGridStep(DEFAULT_GRID_STEP),
    m_updating(false),
    m_currentObjectIndex(1),
    m_multiSelectStarted(false),
    m_movedItem(0),
    m_joinItem(0),
    m_magneticMovement(false),
    m_reportSettings(0),
    m_currentPage(0),
    m_module(module)

{
    updatePageRect(pageItem);
    connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    connect(this, SIGNAL(pasteItemInsert(base_design_item *)), this, SLOT(slotPasteItemInsert(base_design_item *)));
    setBackgroundBrush(QBrush(Qt::white));

}

page_scene::~page_scene()
{
    if (!m_pageItem.isNull())
    {
        removeItem(m_pageItem.data());
        m_pageItem.clear();
    }
    foreach (page_item::Ptr pageItem, m_reportPages)
    {
        removeItem(pageItem.data());
    }
    m_commandsList.clear();
}

void page_scene::slotNewPage(QString module)
{

}

void page_scene::slotAddPage()
{

}

void page_scene::updatePageRect(page_item::Ptr pageItemP)
{
    if (m_pageItem.isNull())
    {
        if (pageItemP != nullptr)
        {
            m_pageItem = pageItemP;
        }
        else
        {
            m_pageItem =  page_item::create(this);
        }

        addItem(m_pageItem.data());
        m_pageItem->setTopMargin(PAGE_MARGIN);
        m_pageItem->setBottomMargin(PAGE_MARGIN);
        m_pageItem->setLeftMargin(PAGE_MARGIN);
        m_pageItem->setRightMargin(PAGE_MARGIN);
        m_pageItem->setObjectName("ReportPage1");
        // test
        m_pageItem->setModule(m_module);

        connect(m_pageItem.data(), SIGNAL(itemSelected(base_design_item *)), this, SIGNAL(itemSelected(base_design_item *)));
        connect(m_pageItem.data(), SIGNAL(geometryChanged(QObject *, QRectF, QRectF)), this, SLOT(slotPageGeometryChanged(QObject *, QRectF, QRectF)));
        connect(m_pageItem.data(), SIGNAL(objectLoaded(QObject *)), this, SLOT(slotPageItemLoaded(QObject *)));
    }

    setSceneRect(-SCENE_MARGIN, -SCENE_MARGIN,
                 pageItem()->geometry().width() + SCENE_MARGIN * 2,
                 pageItem()->boundingRect().height() + SCENE_MARGIN * 2);

    emit sceneRectChanged(sceneRect());
}

void page_scene::keyPressEvent(QKeyEvent *event)
{
    // ctrl + 滚轮放大缩小视图
    if (event->modifiers() & Qt::ControlModifier &&
        (!(event->modifiers()& Qt::ShiftModifier)))
    {
        m_viewChang = true;
    }


    // ctrl + A全选
    if ((event->modifiers() == Qt::ControlModifier) &&
        (event->key() == Qt::Key_A))
    {
        for (auto item : items())
        {
            item->setSelected(true);
        }
    }

    if (event->modifiers() == Qt::NoModifier ||
        (event->key() != Qt::Key_Left &&
         event->key() != Qt::Key_Right &&
         event->key() != Qt::Key_Up &&
         event->key() != Qt::Key_Down &&
         !m_changePosMode && !m_changeSizeMode)
       )
    {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    if ((event->modifiers()& Qt::ControlModifier) && (!m_changeSizeMode))
    {
        if (!m_changePosMode)
        {
            saveSelectedItemsPos();
            m_changePosMode = true;
        }
    }

    if ((event->modifiers()& Qt::ShiftModifier) && (!m_changePosMode))
    {
        if (!m_changeSizeMode)
        {
            saveSelectedItemsGeometry();
            m_changeSizeMode = true;
        }
    }

    if ((event->modifiers()& Qt::ControlModifier) && m_changePosMode && (!(event->modifiers()& Qt::ShiftModifier)))
    {
        foreach (QGraphicsItem *item, selectedItems())
        {
            if (dynamic_cast<base_design_item *>(item))
            {
                switch (event->key())
                {
                case Qt::Key_Right:
                    dynamic_cast<base_design_item *>(item)->moveRight();
                    break;
                case Qt::Key_Left:
                    dynamic_cast<base_design_item *>(item)->moveLeft();
                    break;
                case Qt::Key_Up:
                    dynamic_cast<base_design_item *>(item)->moveUp();
                    break;
                case Qt::Key_Down:
                    dynamic_cast<base_design_item *>(item)->moveDown();
                    break;
                }
            }
        }
    }

    if ((event->modifiers()& Qt::ShiftModifier) && m_changeSizeMode && (!(event->modifiers()& Qt::ControlModifier)))
    {
        foreach (QGraphicsItem *item, selectedItems())
        {
            if (dynamic_cast<base_design_item *>(item))
            {
                switch (event->key())
                {
                case Qt::Key_Right:
                    dynamic_cast<base_design_item *>(item)->sizeRight();
                    break;
                case Qt::Key_Left:
                    dynamic_cast<base_design_item *>(item)->sizeLeft();
                    break;
                case Qt::Key_Up:
                    dynamic_cast<base_design_item *>(item)->sizeUp();
                    break;
                case Qt::Key_Down:
                    dynamic_cast<base_design_item *>(item)->sizeDown();
                    break;
                }
            }
        }
    }
}

void page_scene::keyReleaseEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Control) && m_changePosMode)
    {
        checkSizeOrPosChanges();
    }

    if ((event->key() == Qt::Key_Shift) && m_changeSizeMode)
    {
        checkSizeOrPosChanges();
    }

    if ((event->key() == Qt::Key_Control) && m_viewChang)
    {
        m_viewChang = false;
    }

    QGraphicsScene::keyReleaseEvent(event);
}

void page_scene::startInsertMode(const QString &ItemType)
{
    if (m_insertMode)
    {
        emit itemInsertCanceled(m_insertItemType);
    }

    emit insertModeStarted();
    m_insertMode = true;
    m_insertItemType = ItemType;
    m_itemInsertRect = this->addRect(0, 0, 200, 50);
    m_itemInsertRect->setVisible(false);
    page_item *page = pageItem() ? pageItem() : getCurrentPage();
    if (page)
    {
        m_itemInsertRect->setParentItem(page);
    }
}

void page_scene::startEditMode()
{
    if (m_insertMode)
    {
        emit itemInsertCanceled(m_insertItemType);
    }

    finalizeInsertMode();
    m_insertMode = false;
}

page_item *page_scene::pageItem()
{
    return m_currentPage ? m_currentPage : m_pageItem.data();
}

page_item::Ptr page_scene::pageItemSharePtr()
{
    return m_pageItem;
}

void page_scene::reloadPageItem()
{
    setPageItem(m_pageItem);
    m_pageItem->setPos(0, 0);
    m_pageItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void page_scene::setPageItem(page_item::Ptr pageItem)
{
    if (pageItem.isNull())
    {
        return;
    }
    if (!m_pageItem.isNull())
    {
        removeItem(m_pageItem.data());
        m_pageItem->setParent(0);
    }
    m_pageItem = pageItem;
    m_pageItem->setItemMode(itemMode());
    setSceneRect(pageItem->rect().adjusted(-10 * mmFACTOR,
                                           -10 * mmFACTOR,
                                           10 * mmFACTOR,
                                           10 * mmFACTOR));
    addItem(m_pageItem.data());
    registerItem(m_pageItem.data());
}

void page_scene::setPageItems(QList<page_item::Ptr> pages)
{
    m_currentPage = 0;
    if (!m_pageItem.isNull())
    {
        if (m_pageItem->scene() == this)
        {
            removeItem(m_pageItem.data());
        }
        m_pageItem.clear();
    }
    int curHeight = 0;
    int curWidth = 0;
    m_reportPages = pages;
    foreach (page_item::Ptr pageItem, pages)
    {
        pageItem->setItemMode(itemMode());
        addItem(pageItem.data());
        registerItem(pageItem.data());
        pageItem->setPos(0, curHeight);
        curHeight += pageItem->height() + 20;
        if (curWidth < pageItem->width())
        {
            curWidth = pageItem->width();
        }
    }
    setSceneRect(QRectF(0, 0, curWidth, curHeight).adjusted(-10 * mmFACTOR,
                                                            -10 * mmFACTOR,
                                                            10 * mmFACTOR,
                                                            10 * mmFACTOR));
    if (m_reportPages.count() > 0)
    {
        m_currentPage = m_reportPages.at(0).data();
    }

}

void page_scene::removePageItem(page_item::Ptr pageItem)
{
    if (m_pageItem == pageItem)
    {
        removeItem(m_pageItem.data());
        m_pageItem.clear();
    }
}

void page_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton &&
        event->modifiers() == Qt::ShiftModifier)
    {
        m_multiSelectStarted = true;
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void page_scene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    int delta = event->delta(); // 获取鼠标滚轮的滚动距离

    // 根据滚动距离执行相应的操作
    if (delta > 0 && m_viewChang)
    {
        // 鼠标向上滚动
        QList <QGraphicsView *> viewsList = views();
        for (auto item : viewsList)
        {
            item->scale(ZOOM_IN_UNIT, ZOOM_IN_UNIT);
        }

    }
    else if (delta <= 0 && m_viewChang)
    {
        // 鼠标向下滚动
        QList <QGraphicsView *> viewsList = views();
        for (auto item : viewsList)
        {
            item->scale(ZOOM_OUT_UNIT, ZOOM_OUT_UNIT);
        }
    }

    // 将事件传递给父类处理
    QGraphicsScene::wheelEvent(event);
}

void page_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (!m_changePosOrSizeMode)
        {
            saveSelectedItemsPos();
            saveSelectedItemsGeometry();
            m_changePosOrSizeMode = true;
        }

    }

    if (event->buttons() & Qt::LeftButton && m_multiSelectStarted)
    {
        if (!m_selectionRect)
        {
            m_selectionRect = new QGraphicsRectItem();
            QBrush brush(QColor(140, 190, 30, 50));
            m_selectionRect->setBrush(brush);
            m_selectionRect->setPen(Qt::DashLine);
            addItem(m_selectionRect);
        }

        QRectF selectionRect;
        selectionRect.setX(qMin(event->buttonDownScenePos(Qt::LeftButton).x(), event->scenePos().x()));
        selectionRect.setY(qMin(event->buttonDownScenePos(Qt::LeftButton).y(), event->scenePos().y()));
        selectionRect.setRight(qMax(event->buttonDownScenePos(Qt::LeftButton).x(), event->scenePos().x()));
        selectionRect.setBottom(qMax(event->buttonDownScenePos(Qt::LeftButton).y(), event->scenePos().y()));
        m_selectionRect->setRect(selectionRect);
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void page_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton))
    {
        if (m_joinItem && selectedItems().count() == 1)
        {
            base_design_item *selectedItem = dynamic_cast<base_design_item *>(selectedItems().at(0));
            if (m_magneticMovement)
            {
                if (m_joinType == Width)
                {

                    QPointF tmpPos;
                    if (selectedItem->pos().y() > m_joinItem->pos().y())
                    {
                        tmpPos = QPointF(m_joinItem->x(), m_joinItem->pos().y() + m_joinItem->height());
                    }
                    else
                    {
                        tmpPos = QPointF(m_joinItem->x(), m_joinItem->pos().y() - selectedItem->height());
                    }

                    selectedItem->setPos(tmpPos);
                    selectedItem->setWidth(m_joinItem->width());

                }
                else
                {

                    QPointF tmpPos;
                    if (selectedItem->pos().x() > m_joinItem->pos().x())
                    {
                        tmpPos = QPointF(m_joinItem->x() + m_joinItem->width(), m_joinItem->pos().y());
                    }
                    else
                    {
                        tmpPos = QPointF(m_joinItem->x() - selectedItem->width(), m_joinItem->pos().y());
                    }

                    selectedItem->setPos(tmpPos);
                    selectedItem->setHeight(m_joinItem->height());
                }
            }
            m_joinItem->turnOnJoinMarker(false);
            m_joinItem = 0;
        }
        checkSizeOrPosChanges();
    }
    if (m_selectionRect)
    {
        setSelectionRect(m_selectionRect->rect());
        delete m_selectionRect;
        m_selectionRect = 0;
        m_multiSelectStarted = false;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void page_scene::setSelectionRect(QRectF selectionRect)
{

}


QPointF page_scene::placePosOnGrid(QPointF point)
{
    return QPointF(div(point.x(), horizontalGridStep()).quot * horizontalGridStep(),
                   div(point.y(), verticalGridStep()).quot * verticalGridStep());
}

QSizeF page_scene::placeSizeOnGrid(QSizeF size)
{
    return QSizeF(div(size.width(), horizontalGridStep()).quot * horizontalGridStep(),
                  div(size.height(), verticalGridStep()).quot * verticalGridStep());
}

base_design_item *page_scene::addReportItem(const QString &itemType, QPointF pos, QSizeF size)
{
    page_item *page = pageItem() ? pageItem() : m_currentPage;
    if (page)
    {
        base_design_item *reportItem = addReportItem(itemType, page, page);
        if (!reportItem)
        {
            return reportItem;
        }

        reportItem->setPos(placePosOnGrid(page->mapFromScene(pos)));
        if (reportItem->size() == QSizeF(0, 0))
        {
            reportItem->setSize(placeSizeOnGrid(size));
        }
        reportItem->setUnitType(pageItem()->unitType());
        return reportItem;
    }
    return 0;
}

base_design_item *page_scene::addReportItem(const QString &itemType, QObject *owner, base_design_item  *parent)
{
    // 从工厂拿到构造器，创建item
    auto CreatorFunction = DesignElementsFactory::instance().objectCreator(itemType);
    if (CreatorFunction == nullptr)
    {
        return nullptr;
    }

    base_design_item *item = CreatorFunction((owner) ? owner : pageItem(), (parent) ? parent : pageItem());
    item->setObjectName(genObjectName(*item));
    item->setItemTypeName(itemType);
    item->setUnitType(pageItem()->unitType());

    // 非page元件通过设置父item的方法，添加到场景当中
    // 非page元件的父item都为page,paged的父item为Scene(框架内最顶级的父item).
    // 非page元件设置父item（page）
    item->setParentItem(parent);

    // QObject父对象设置（元对象体系）
    item->setParent(owner);

    // 关联到属性编辑信号和槽
    registerItem(item);
    return item;
}

base_design_item *page_scene::createReportItem(const QString &itemType, QObject *owner, base_design_item *parent)
{
    return DesignElementsFactory::instance().objectCreator(itemType)((owner) ? owner : pageItem(), (parent) ? parent : pageItem());
}

CommandIf::Ptr page_scene::removeReportItemCommand(base_design_item *item)
{
    if (item)
    {
        item->itemDelete();
    }
    return  DeleteItemCommand::create(this, item);
}

void page_scene::removeReportItem(base_design_item *item, bool createComand)
{
    if (!createComand)
    {
        removeItem(item);
        emit itemRemoved(this, item);
        delete item;
    }
    else
    {
        CommandIf::Ptr command = DeleteItemCommand::create(this, item) ;
        saveCommand(command);
    }
}

bool page_scene::saveCommand(CommandIf::Ptr command, bool runCommand)
{
    if (m_executingCommand || m_isLoading)
    {
        return false;
    }

    if (runCommand)
    {
        m_executingCommand = true;
        if (!command->doIt())
        {
            m_executingCommand = false;
            return false;
        }
        m_executingCommand = false;
    }

    if (m_currentCommand < (m_commandsList.count() - 1))
    {
        m_commandsList.remove(m_currentCommand + 1,
                              m_commandsList.size() - (m_currentCommand + 1));
    }


    m_commandsList.push_back(command);
    m_currentCommand = m_commandsList.count() - 1;
    m_hasChanges = true;
    emit commandHistoryChanged();
    return true;
}

bool page_scene::isCanRedo()
{
    return m_currentCommand < m_commandsList.count() - 1;
}

bool page_scene::isCanUndo()
{
    return m_currentCommand >= 0;
}

bool page_scene::isHasChanges()
{
    return (m_commandsList.count() > 0) && m_hasChanges;
}

bool page_scene::isItemInsertMode()
{
    return m_insertMode;
}

QString page_scene::genObjectName(const QObject &object)
{
    //int index = 1;
    QString className(object.metaObject()->className());
    className = className.right(className.length() - (className.lastIndexOf("::") + 2));

    QString tmpName = QString("%1%2").arg(className).arg(m_currentObjectIndex);

    QList<QGraphicsItem *> itemsList = items();
    while (isExistsObjectName(tmpName, itemsList))
    {
        ++m_currentObjectIndex;
        tmpName = QString("%1%2").arg(className).arg(m_currentObjectIndex);
    }

    return tmpName;
}

bool page_scene::isExistsObjectName(const QString &objectName, QList<QGraphicsItem *> &itemsList) const
{
    QObject *item = 0;
    //QList<QGraphicsItem*> itemList = items();
    for (int i = 0; i < itemsList.count(); i++)
    {
        item = dynamic_cast<QObject *>(itemsList[i]);
        if (item)
            if (item->objectName() == objectName)
            {
                return true;
            }
    }

    return false;
}

bool page_scene::isLoading()
{
    return m_isLoading;
}

void page_scene::objectLoadStarted()
{
    m_isLoading = true;
}

void page_scene::objectLoadFinished()
{
    m_isLoading = false;
}


void page_scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (!event->mimeData()->text().isEmpty())
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else if (event->mimeData()->hasFormat("inserItem"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void page_scene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("inserItem"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void page_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // 创建元件
    if (event->mimeData()->hasFormat("inserItem"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        if (m_insertMode)
        {
            finalizeInsertMode();

            // 保存item相关属性到command对象
            CommandIf::Ptr command = InsertItemCommand::create(this, m_insertItemType, event->scenePos(), ITEM_SIZE);

            // 创建item
            saveCommand(command);

            emit itemInserted(this, event->scenePos(), m_insertItemType);
        }
    }
}

void page_scene::dragLeaveEvent(QGraphicsSceneDragDropEvent *)
{

}

void page_scene::slotPageGeometryChanged(QObject *, QRectF /*newGeometry*/, QRectF)
{
    if (!m_isLoading)
    {
        pageItem()->relocateBands();
        updatePageRect();
        emit geometryChanged(sceneRect());
    }
}

void page_scene::slotItemPropertyChanged(QString propertyName, const QVariant &oldValue, const QVariant &newValue)
{
    if (!m_isLoading && m_animationList.isEmpty())
    {
        // 撤销，反撤销
        saveChangeProppertyCommand(sender()->objectName(), propertyName, oldValue, newValue);

        // 通知相关显示控件，更新属性的显示
        emit itemPropertyChanged(sender()->objectName(), propertyName, oldValue, newValue);
    }
}

void page_scene::slotItemPropertyObjectNameChanged(const QString &oldName, const QString &newName)
{
    if (oldName.compare(newName) != 0 && !m_executingCommand)
    {
        CommandIf::Ptr command = PropertyObjectNameChangedCommand::create(this, oldName, newName);
        saveCommand(command, false);
        emit itemPropertyObjectNameChanged(oldName, newName);
    }
}

void page_scene::slotPageItemLoaded(QObject *)
{
    setItemMode(m_itemMode);
}

void page_scene::slotSelectionChanged()
{
    if (selectedItems().count() == 1)
    {
        m_firstSelectedItem = dynamic_cast<base_design_item *>(selectedItems().at(0));
    }
}

void page_scene::slotAnimationStoped(QObject *animation)
{
    m_animationList.removeOne(animation);
}

void page_scene::finalizeInsertMode()
{
    if (m_insertMode)
    {
        m_insertMode = false;
        if (m_itemInsertRect)
        {
            removeItem(m_itemInsertRect);
            delete m_itemInsertRect;
            m_itemInsertRect = 0;
        }
    }
}

void page_scene::saveSelectedItemsPos()
{
    m_positionStamp.clear();
    foreach (QGraphicsItem *item, selectedItems())
    {
        base_design_item *reportItem = dynamic_cast<base_design_item *>(item);

        if (reportItem)
        {
            ReportItemPos rp;
            rp.objectName = reportItem->objectName();
            rp.pos = reportItem->pos();
            m_positionStamp.push_back(rp);
        }
    }
}

void page_scene::saveSelectedItemsGeometry()
{
    m_geometryStamp.clear();
    foreach (QGraphicsItem *item, selectedItems())
    {
        base_design_item *reportItem = dynamic_cast<base_design_item *>(item);

        if (reportItem)
        {
            ReportItemSize rs;
            rs.objectName = reportItem->objectName();
            rs.size = reportItem->size();
            m_geometryStamp.append(rs);
        }
    }
}

void page_scene::checkSizeOrPosChanges()
{
    CommandIf::Ptr posCommand;
    if ((selectedItems().count() > 0) && (m_positionStamp.count() > 0))
    {
        bool bandFound = false;
        if (!bandFound && (m_positionStamp[0].pos != selectedItems().at(0)->pos()))
        {
            posCommand = createChangePosCommand();
        }
        m_positionStamp.clear();
    }

    CommandIf::Ptr sizeCommand;
    if ((selectedItems().count() > 0) && (m_geometryStamp.count() > 0))
    {
        base_design_item *reportItem = dynamic_cast<base_design_item *>(selectedItems()[0]);
        if (reportItem && (m_geometryStamp[0].size != reportItem->size()))
        {
            if (!dynamic_cast<table_base_item *>(reportItem))
            {
                sizeCommand = createChangeSizeCommand();
            }
        }
        m_geometryStamp.clear();
    }

    if (sizeCommand && posCommand)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        cm->addCommand(sizeCommand, false);
        cm->addCommand(posCommand, false);
        saveCommand(cm);
    }
    else
    {
        if (sizeCommand)
        {
            saveCommand(sizeCommand);
        }
        if (posCommand)
        {
            saveCommand(posCommand);
        }
    }

    m_changeSizeMode = false;
    m_changePosMode = false;
    m_changePosOrSizeMode = false;
}

CommandIf::Ptr page_scene::createChangePosCommand()
{
    QVector<ReportItemPos> newPoses;
    foreach (ReportItemPos itemPos, m_positionStamp)
    {
        base_design_item *reportItem = reportItemByName(itemPos.objectName);
        if (reportItem)
        {
            ReportItemPos newPos;
            newPos.objectName = reportItem->objectName();
            newPos.pos = reportItem->pos();
            newPoses.append(newPos);
            reportItem->itemSynchronization();
        }
    }
    return PosChangedCommand::create(this, m_positionStamp, newPoses);
}

CommandIf::Ptr page_scene::createChangeSizeCommand()
{
    QVector<ReportItemSize> newSizes;

    foreach (ReportItemSize itemPos, m_geometryStamp)
    {
        base_design_item *reportItem = reportItemByName(itemPos.objectName);

        if (reportItem)
        {
            ReportItemSize newSize;
            newSize.objectName = reportItem->objectName();
            newSize.size = reportItem->size();
            newSizes.append(newSize);
            reportItem->itemSynchronization();
        }
    }
    return SizeChangedCommand::create(this, m_geometryStamp, newSizes);
}

void page_scene::reactivatePageItem(page_item::Ptr pageItem)
{
    pageItem->setItemMode(itemMode());
    if (pageItem.data()->scene() != this)
    {
        addItem(pageItem.data());
    }
}

void page_scene::animateItem(base_design_item *item)
{
    if (item && (item->metaObject()->indexOfProperty("backgroundColor") > -1))
    {

        foreach (QObject *obj, m_animationList)
        {
            QPropertyAnimation *animation = dynamic_cast<QPropertyAnimation *>(obj);
            if (animation->targetObject() == item)
            {
                return;
            }
        }

        QPropertyAnimation *ani1 = new QPropertyAnimation(item, "backgroundColor");
        m_animationList.append(ani1);

        QColor startColor = QColor(Qt::red);
        QColor endColor = item->backgroundColor();

        ani1->setDuration(500);
        ani1->setEasingCurve(QEasingCurve::Linear);
        ani1->setStartValue(startColor);
        ani1->setEndValue(endColor);
        ani1->start(QAbstractAnimation::DeleteWhenStopped);


        connect(ani1, SIGNAL(destroyed(QObject *)), this, SLOT(slotAnimationStoped(QObject *)));
    }
}

void page_scene::registerItem(base_design_item *item)
{
    item->setItemMode(itemMode());
    connect(item, SIGNAL(propertyChanged(QString, QVariant, QVariant)), this, SLOT(slotItemPropertyChanged(QString, QVariant, QVariant)));
    connect(item, SIGNAL(propertyObjectNameChanged(QString, QString)), this, SLOT(slotItemPropertyObjectNameChanged(QString, QString)));
    table_item *tableItem = dynamic_cast<table_item *>(item);
    if (tableItem)
    {
        connect(tableItem, SIGNAL(destroyed(QObject *)), this, SLOT(slotTableItemDestroyed(QObject *)));

        // 关联控件
        if (tableItem && tableItem->isConnectTableItem())
        {
            emit addConnectTableItem(tableItem->tableItemID(), tableItem->connectTableItemID());
        }
    }
    emit itemAdded(this, item);
}

void page_scene::emitRegisterdItem(base_design_item *item)
{
    emit itemAdded(this, item);
}

void page_scene::emitItemRemoved(base_design_item *item)
{
    emit itemRemoved(this, item);
}

void page_scene::slotTableItemDestroyed(QObject *item)
{
    //base_design_item* tem = reinterpret_cast<base_design_item*>(item);
    table_item *tableItem = reinterpret_cast<table_item *>(item);
    if (tableItem && tableItem->isConnectTableItem())
    {
        emit deleteConnectTableItem(tableItem->tableItemID(), tableItem->connectTableItemID());
    }
}

void page_scene::slotPasteItemInsert(base_design_item *item)
{
    // 表格控件
    table_item *tableItem = dynamic_cast<table_item *>(item);
    if (tableItem)
    {
        tableItem->setTableItemID(table_item::genTableItemID());
        // 新增的关联tabkeItem 需要让发出信号，让manager知道
        if (tableItem->isConnectTableItem())
        {
            addConnectTableItem(tableItem->tableItemID(), tableItem->connectTableItemID());
        }

        // 触发屬性窗口刷新数据
        tableItem->setSelected(false);
        tableItem->setSelected(true);
    }
}

void page_scene::slotUpdateItemSize()
{

}

void page_scene::saveChangeProppertyCommand(const QString &objectName, const QString &propertyName, const QVariant &oldPropertyValue, const QVariant &newPropertyValue)
{
    if (!m_executingCommand)
    {
        CommandIf::Ptr command;
        if (propertyName.compare("ItemAlign", Qt::CaseInsensitive) == 0)
        {
            command = PropertyItemAlignChangedCommand::create(this, objectName,
                                                              base_design_item::ItemAlign(oldPropertyValue.toInt()),
                                                              base_design_item::ItemAlign(newPropertyValue.toInt())
                                                             );
        }
        else
        {
            command = PropertyChangedCommand::create(this, objectName, propertyName, oldPropertyValue, newPropertyValue);
        }
        saveCommand(command, false);
    }
}

void page_scene::changeSelectedGroupProperty(const QString &name, const QVariant &value)
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        m_executingCommand = true;
        foreach (QGraphicsItem *item, selectedItems())
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem)
            {
                QVariant oldValue = bdItem->property(name.toLatin1());
                if (oldValue.isValid())
                {
                    bdItem->setProperty(name.toLatin1(), value);
                    CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), name, oldValue, value);
                    cm->addCommand(command, false);
                }
            }
        }
        m_executingCommand = false;
        saveCommand(cm, false);
    }
}

page_item *page_scene::getCurrentPage() const
{
    return m_currentPage;
}

void page_scene::setCurrentPage(page_item *currentPage)
{
    if (m_currentPage != currentPage)
    {
        if (m_currentPage)
        {
            m_currentPage->setItemMode(PreviewMode);
        }
        m_currentPage = currentPage;
        if (m_itemMode == DesignMode)
        {
            m_currentPage->setItemMode(DesignMode);
        }
    }
}

ReportSettings *page_scene::getReportSettings() const
{
    return m_reportSettings;
}

void page_scene::setReportSettings(ReportSettings *reportSettings)
{
    m_reportSettings = reportSettings;
    m_pageItem->setReportSettings(m_reportSettings);
}

void page_scene::setPropertyToSelectedItems(const char *name, const QVariant &value)
{
    foreach (QGraphicsItem *gi, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(gi);
        if (item && item->metaObject()->indexOfProperty(name) != -1)
        {
            item->setProperty(name, value);
        }
    }
}

bool page_scene::magneticMovement() const
{
    return m_magneticMovement;
}

void page_scene::setMagneticMovement(bool magneticMovement)
{
    m_magneticMovement = magneticMovement;
}

int page_scene::horizontalGridStep() const
{
    return m_horizontalGridStep;
}

void page_scene::setHorizontalGridStep(int horizontalGridStep)
{
    m_horizontalGridStep = horizontalGridStep;
}

void page_scene::endUpdate()
{
    m_updating = false;
    emit pageUpdateFinished(this);
}


void page_scene::activateItemToJoin(QRectF itemRect, QList<ItemProjections> &items)
{
    QRectF r1(itemRect.x(), itemRect.y() - 50, itemRect.width(), itemRect.height() + 100);
    QRectF r2(itemRect.x() - 50, itemRect.y(), itemRect.width() + 100, itemRect.height());
    qreal maxSquare = 0;

    if (m_joinItem)
    {
        m_joinItem->turnOnJoinMarker(false);
        m_joinItem = 0;
    }

    foreach (ItemProjections p, items)
    {
        qreal tmpSquare = qMax(p.square(r1) / itemRect.width(), p.square(r2) / itemRect.height());
        if (tmpSquare > maxSquare)
        {
            maxSquare = tmpSquare;
            m_joinItem = p.item();
            if (p.square(r1) / itemRect.width() > p.square(r2) / itemRect.height())
            {
                m_joinType = Width;
            }
            else
            {
                m_joinType = Height;
            }
        }
    }

    if (m_joinItem)
    {
        m_joinItem->turnOnJoinMarker(true);
    }
}

void page_scene::selectAllChildren(base_design_item *item)
{
    if (item)
        foreach (base_design_item *child, item->childBaseItems())
        {
            child->setSelected(true);
        }
}

void page_scene::rectMoved(QRectF itemRect, base_design_item *container)
{

}

void page_scene::itemMoved(base_design_item *item)
{
    if (m_movedItem != item)
    {
        m_movedItem = item;
        base_design_item *curItem = dynamic_cast<base_design_item *>(item->parentItem());
        while (curItem)
        {
            m_movedItemContainer = dynamic_cast<page_item *>(curItem);
            if (m_movedItemContainer)
            {
                break;
            }
            else
            {
                curItem = dynamic_cast<base_design_item *>(curItem->parentItem());
            }

        }

        if (m_movedItemContainer)
        {
            m_projections.clear();
            foreach (base_design_item *bi, m_movedItemContainer->childBaseItems())
            {
                if (bi != item)
                {
                    m_projections.append(ItemProjections(bi));
                }
            }
        }
    }

    activateItemToJoin(item->geometry(), m_projections);

}

int page_scene::verticalGridStep() const
{
    return m_verticalGridStep;
}

void page_scene::setVerticalGridStep(int verticalGridStep)
{
    m_verticalGridStep = verticalGridStep;
}

Qt::AlignmentFlag transformFlags(bool horizontalAlign, Qt::AlignmentFlag value, Qt::AlignmentFlag flag)
{
    int tmpValue = value;
    if (horizontalAlign)
    {
        tmpValue &= ~(Qt::AlignHCenter  | Qt::AlignLeft | Qt::AlignRight | Qt::AlignJustify);
        tmpValue |= flag;
    }
    else
    {
        tmpValue &= ~(Qt::AlignVCenter | Qt::AlignTop | Qt::AlignBottom);
        tmpValue |= flag;
    }
    return Qt::AlignmentFlag(tmpValue);
}

void page_scene::changeSelectedGrpoupTextAlignPropperty(const bool &horizontalAlign, Qt::AlignmentFlag flag)
{
    if (selectedItems().count() > 0)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        m_executingCommand = true;
        foreach (QGraphicsItem *item, selectedItems())
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem)
            {
                QVariant oldValue = bdItem->property("alignment");
                if (oldValue.isValid())
                {
                    QVariant value = transformFlags(horizontalAlign, Qt::AlignmentFlag(oldValue.toInt()), flag);
                    bdItem->setProperty("alignment", value);
                    CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "alignment", oldValue, value);
                    cm->addCommand(command, false);
                }
            }
        }
        m_executingCommand = false;
        saveCommand(cm, false);
    }
}

void page_scene::undo()
{
    if (m_currentCommand >= 0)
    {
        m_executingCommand = true;
        m_commandsList.at(m_currentCommand)->undoIt();
        m_currentCommand--;
        m_hasChanges = true;
        m_executingCommand = false;
    }
}

void page_scene::redo()
{
    if (m_currentCommand < m_commandsList.count() - 1)
    {
        m_executingCommand = true;
        m_currentCommand++;
        m_commandsList.at(m_currentCommand)->doIt();
        m_hasChanges = true;
        m_executingCommand = false;
    }
}

void page_scene::copy()
{
    QList<QGraphicsItem *> selectedItemsList = selectedItems();
    if (!selectedItemsList.isEmpty())
    {
        QClipboard *clipboard = QApplication::clipboard();
        QScopedPointer<ItemsWriterIntf> writer(new XMLWriter);
        bool shouldWrite = false;
        // 按照层级顺序返回
        std::sort(selectedItemsList.begin(), selectedItemsList.end(), [](QGraphicsItem * item1, QGraphicsItem * item2)
        {
            return item1->zValue() < item2->zValue();
        });

        foreach (QGraphicsItem *item, selectedItemsList)
        {
            if (!dynamic_cast<page_item *>(item))
            {
                base_design_item *reportItem = dynamic_cast<base_design_item *>(item);
                if (reportItem)
                {
                    // 序列化
                    writer->putItem(reportItem);
                    shouldWrite = true;
                }
            }
        }

        if (shouldWrite)
        {
            // 存入到系统剪切板
            clipboard->setText(writer->saveToString());
        }
    }
}

bool page_scene::isVariantLock()
{
    QList<QGraphicsItem *> selectedItemsList = selectedItems();
    if (selectedItemsList.count() != 1)
    {
        return false;
    }

    // 目前只支持表格控件
    table_item *tableItem = dynamic_cast<table_item *>(selectedItemsList.back());
    if (tableItem)
    {
        return tableItem->isConnectTableItem();
    }
    return false;
}

bool page_scene::itemVariantLock()
{
    QList<QGraphicsItem *> selectedItemsList = selectedItems();
    if (selectedItemsList.count() != 1)
    {
        return false;
    }

    // 目前只支持表格控件
    table_item *tableItem = dynamic_cast<table_item *>(selectedItemsList.back());
    if (tableItem)
    {
        // 被关联的item,connectTableItemID 等于自己的ID
        if (!tableItem->isConnectTableItem())
        {
            tableItem->setconnectTableItemID(tableItem->tableItemID());
            addConnectTableItem(tableItem->tableItemID(), tableItem->connectTableItemID());
        }
    }
    else
    {
        return false;
    }
    update();
    return true;
}

bool page_scene::itemVariantUnLock()
{
    QList<QGraphicsItem *> selectedItemsList = selectedItems();
    if (selectedItemsList.count() != 1)
    {
        return false;
    }

    // 目前只支持表格控件
    table_item *tableItem = dynamic_cast<table_item *>(selectedItemsList.back());
    if (tableItem)
    {
        if (tableItem->connectTableItemID() == tableItem->tableItemID())
        {
            tableItem->setconnectTableItemID(-1);
            tableItem->setDesignHeaderColor(TABLE_HEADER_CORLOR);
            deleteConnectTableItem(tableItem->tableItemID(), tableItem->tableItemID());
        }
    }
    else
    {
        return false;
    }
    update();
    return true;
}

void page_scene::paste()
{
    QClipboard *clipboard = QApplication::clipboard();
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(clipboard->text());
    if (reader->first() && reader->itemType() == "Object")
    {
        base_design_item *destItem = 0;
        if (!selectedItems().isEmpty())
        {
            destItem = findDestObject(dynamic_cast<base_design_item *>(selectedItems().at(0)));
        }
        else
        {
            destItem = this->pageItem();
        }

        if (destItem)
        {
            CommandIf::Ptr command = PasteCommand::create(this, clipboard->text(), destItem);
            saveCommand(command);
        }
    }
}

base_design_item *page_scene::findDestObject(base_design_item *item)
{
    if (item && item->canAcceptPaste())
    {
        return item;
    }
    base_design_item *curItem = item;
    while (curItem && !curItem->canAcceptPaste())
    {
        curItem = dynamic_cast<base_design_item *>(curItem->parentItem());
    }
    return curItem;
}

void page_scene::deleteSelected()
{
    if (selectedItems().count() == 1)
    {
        if (!dynamic_cast<page_item *>(selectedItems().at(0)))
        {
            saveCommand(removeReportItemCommand(dynamic_cast<base_design_item *>(selectedItems().at(0))));
        }
    }
    else
    {
        QList<QGraphicsItem *> itemsToDelete = selectedItems();
        CommandGroup::Ptr commandGroup = CommandGroup::create();

        if (!itemsToDelete.isEmpty())
        {
            foreach (QGraphicsItem *item, itemsToDelete)
            {
                item->setSelected(false);
                if (!dynamic_cast<page_item *>(item))
                {
                    commandGroup->addCommand(removeReportItemCommand(dynamic_cast<base_design_item *>(item)), false);
                }
            }
        }
        saveCommand(commandGroup);
    }
}

void page_scene::cut()
{
    CommandIf::Ptr command = CutCommand::create(this);
    saveCommand(command);
}

void page_scene::setToSaved()
{
    m_hasChanges = false;
}

void page_scene::bringToFront()
{
    foreach (QGraphicsItem *item, selectedItems())
    {
        qreal zOrder = 0;
        foreach (QGraphicsItem *colItem, collidingItems(item))
        {
            if (zOrder <= colItem->zValue())
            {
                zOrder = colItem->zValue() + 0.1;
            }
        }
        base_design_item *bdItem = dynamic_cast<base_design_item *>(item);

        if (bdItem)
        {
            saveChangeProppertyCommand(bdItem->objectName(), "zOrder", bdItem->zValue(), zOrder);
        }
        else
        {
            item->setZValue(zOrder);
        }
    }
}

void page_scene::sendToBack()
{
    foreach (QGraphicsItem *item, selectedItems())
    {
        qreal zOrder = 0;
        foreach (QGraphicsItem *colItem, collidingItems(item))
        {
            if (zOrder >= colItem->zValue())
            {
                zOrder = colItem->zValue() - 0.1;
            }
        }
        base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
        if (bdItem)
        {
            saveChangeProppertyCommand(bdItem->objectName(), "zOrder", bdItem->zValue(), zOrder);
        }
        else
        {
            item->setZValue(zOrder);
        }
    }
}

bool page_scene::selectionContainsBand()
{
    return false;
}

QGraphicsItem *page_scene::minXGraphicsItem(QList<QGraphicsItem *> &itemList)
{
    QGraphicsItem *minItem = nullptr;
    if (itemList.isEmpty())
    {
        return minItem;
    }

    minItem = itemList.back();
    foreach (QGraphicsItem *item, itemList)
    {
        if (item->x() < minItem->x())
        {
            minItem = item;
        }
    }
    return minItem;
}

QGraphicsItem *page_scene::minYGraphicsItem(QList<QGraphicsItem *> &itemList)
{
    QGraphicsItem *minItem = nullptr;
    if (itemList.isEmpty())
    {
        return minItem;
    }

    minItem = itemList.back();
    foreach (QGraphicsItem *item, itemList)
    {
        if (item->y() < minItem->y())
        {
            minItem = item;
        }
    }
    return minItem;
}

QGraphicsItem *page_scene::maxXGraphicsItem(QList<QGraphicsItem *> &itemList)
{
    QGraphicsItem *maxItem = nullptr;
    if (itemList.isEmpty())
    {
        return maxItem;
    }

    maxItem = itemList.back();
    foreach (QGraphicsItem *item, itemList)
    {
        if (item->x() + item->boundingRect().width() > maxItem->x() + maxItem->boundingRect().width())
        {
            maxItem = item;
        }
    }
    return maxItem;
}

QGraphicsItem *page_scene::maxYGraphicsItem(QList<QGraphicsItem *> &itemList)
{
    QGraphicsItem *maxItem = nullptr;
    if (itemList.isEmpty())
    {
        return maxItem;
    }

    maxItem = itemList.back();
    foreach (QGraphicsItem *item, itemList)
    {
        if (item->y() + item->boundingRect().height()  > maxItem->y() + maxItem->boundingRect().height())
        {
            maxItem = item;
        }
    }
    return maxItem;
}

void page_scene::alignToLeft()
{
    QList<QGraphicsItem *> itemList = selectedItems();
    if ((itemList.count() > 0) && m_firstSelectedItem)
    {
        QGraphicsItem *minItem = minXGraphicsItem(itemList);
        if (!minItem)
        {
            return;
        }
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach (QGraphicsItem *item, itemList)
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked())
            {
                QRectF oldGeometry = bdItem->geometry();
                QRectF newGeometry = oldGeometry;
                if (itemList.count() == 1)
                {
                    newGeometry.moveTopLeft(QPointF(pageItem()->leftMargin() *  mmFACTOR, item->pos().y()));
                }
                else
                {
                    newGeometry.moveTopLeft(QPointF(moveInBand ? 0 : minItem->pos().x(), item->pos().y()));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, newGeometry);
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, true);
    }
}

void page_scene::alignToRigth()
{
    QList<QGraphicsItem *> itemList = selectedItems();
    if ((itemList.count() > 0) && m_firstSelectedItem)
    {
        QGraphicsItem  *maxItem = maxXGraphicsItem(itemList);
        base_design_item *maxDesignItem = dynamic_cast<base_design_item *>(maxItem);
        if (!maxItem && !maxDesignItem)
        {
            return;
        }
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach (QGraphicsItem *item, itemList)
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand())
            {
                QRectF oldGeometry = bdItem->geometry();
                QRectF newGeometry = oldGeometry;
                qreal x = maxDesignItem->geometry().right() - bdItem->width();
                if (itemList.count() == 1)
                {
                    newGeometry.moveTopLeft(QPointF(pageItem()->width() - pageItem()->rightMargin() *  mmFACTOR - bdItem->width(), item->pos().y()));
                }
                else
                {
                    newGeometry.moveTopLeft(QPointF(x + 1, bdItem->pos().y()));
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, newGeometry);
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, true);
    }
}

void page_scene::alignToVCenter()
{
    QList<QGraphicsItem *> itemList = selectedItems();
    if ((itemList.count() > 0) && m_firstSelectedItem)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach (QGraphicsItem *item, itemList)
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand())
            {
                QRectF oldGeometry = bdItem->geometry();
                QRectF newGeometry = oldGeometry;
                qreal x = (m_firstSelectedItem->geometry().right() - m_firstSelectedItem->width() / 2) - bdItem->width() / 2;
                newGeometry.moveTopLeft(QPointF(pageItem()->width() / 2 - bdItem->width() / 2, item->pos().y()));
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, newGeometry);
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, true);
    }
}

void page_scene::alignToTop()
{
    QList<QGraphicsItem *> itemList = selectedItems();
    if ((itemList.count() > 0) && m_firstSelectedItem)
    {
        QGraphicsItem  *minItem = minYGraphicsItem(itemList);
        if (!minItem)
        {
            return;
        }

        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach (QGraphicsItem *item, itemList)
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand())
            {
                QRectF oldGeometry = bdItem->geometry();
                QRectF newGeometry = oldGeometry;
                if (itemList.count() == 1)
                {
                    newGeometry.moveTopLeft(QPointF(bdItem->pos().x(), pageItem()->topMargin() * mmFACTOR));
                }
                else
                {
                    if (moveInBand)
                    {
                        newGeometry.moveTopLeft(QPointF(0, minItem->pos().y()));
                    }
                    else
                    {
                        newGeometry.moveTopLeft(QPointF(bdItem->pos().x(), minItem->pos().y()));
                    }
                }
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, newGeometry);
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, true);
    }
}

void page_scene::alignToBottom()
{
    QList<QGraphicsItem *> itemList = selectedItems();
    if ((itemList.count() > 0) && m_firstSelectedItem)
    {
        QGraphicsItem  *maxItem = maxYGraphicsItem(itemList);
        base_design_item *maxDesignItem = dynamic_cast<base_design_item *>(maxItem);
        if (!maxItem && !maxDesignItem)
        {
            return;
        }
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach (QGraphicsItem *item, itemList)
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand())
            {
                QRectF oldGeometry = bdItem->geometry();
                QRectF newGeometry = oldGeometry;
                qreal y = maxDesignItem->geometry().bottom() - bdItem->height();
                if (itemList.count() == 1)
                {
                    newGeometry.moveTopLeft(QPointF(bdItem->pos().x(), pageItem()->height() - pageItem()->bottomMargin() * mmFACTOR - bdItem->height()));
                }
                else
                {
                    newGeometry.moveTopLeft(QPointF(bdItem->pos().x(), y + 1));
                }

                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, newGeometry);
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, true);
    }
}

void page_scene::alignToHCenter()
{
    QList<QGraphicsItem *> itemList = selectedItems();
    if ((itemList.count() > 0) && m_firstSelectedItem)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        bool moveInBand = selectionContainsBand();
        foreach (QGraphicsItem *item, itemList)
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked() && !bdItem->isBand())
            {
                QRectF oldGeometry = bdItem->geometry();
                QRectF newGeometry = oldGeometry;
                qreal y = (m_firstSelectedItem->geometry().bottom() - m_firstSelectedItem->height() / 2) - bdItem->height() / 2;
                newGeometry.moveTopLeft(QPointF(bdItem->pos().x(), pageItem()->height() / 2 - bdItem->height() / 2));
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, newGeometry);
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, true);
    }
}

void page_scene::sameWidth()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        foreach (QGraphicsItem *item, selectedItems())
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked())
            {
                QRectF oldGeometry = bdItem->geometry();
                bdItem->setWidth(m_firstSelectedItem->width());
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void page_scene::sameHeight()
{
    if ((selectedItems().count() > 0) && m_firstSelectedItem)
    {
        CommandGroup::Ptr cm = CommandGroup::create();
        foreach (QGraphicsItem *item, selectedItems())
        {
            base_design_item *bdItem = dynamic_cast<base_design_item *>(item);
            if (bdItem && !bdItem->isGeometryLocked())
            {
                QRectF oldGeometry = bdItem->geometry();
                bdItem->setHeight(m_firstSelectedItem->height());
                CommandIf::Ptr command = PropertyChangedCommand::create(this, bdItem->objectName(), "geometry", oldGeometry, bdItem->geometry());
                cm->addCommand(command, false);
            }
        }
        saveCommand(cm, false);
    }
}

void page_scene::setFont(const QFont &font)
{
    changeSelectedGroupProperty("font", font);
}

void page_scene::setTextAlign(const Qt::Alignment &alignment)
{
    changeSelectedGroupProperty("alignment", QVariant(alignment));
}

void page_scene::setBorders(const base_design_item::BorderLines &border)
{
    changeSelectedGroupProperty("borders", (int)border);
}

void page_scene::lockSelectedItems()
{
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item)
        {
            item->setProperty("geometryLocked", true);
        }
    }
}

void page_scene::unlockSelectedItems()
{
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item)
        {
            item->setProperty("geometryLocked", false);
        }
    }
}

void page_scene::setPageHeader()
{
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item)
        {
            item->setMasterMask(PAGE_TOP_ID);
        }
    }
}

void page_scene::unheader()
{
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item)
        {
            item->setMasterMask(0);
        }
    }
}

void page_scene::setPageFooter()
{
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item)
        {
            item->setMasterMask(PAGE_BOTTOM_ID);
        }
    }
}

void page_scene::unfooter()
{
    unheader();
}

QMap<int, QList<base_design_item *>> page_scene::selectGroupItem()
{
    QMap<int, QList<base_design_item *>> groupItemList;
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item != nullptr && item->isGroupItem())
        {
            groupItemList[item->groupID()].push_back(item);
        }
    }
    return groupItemList;
}

bool page_scene::groupSelectItems(int groupID)
{
    // 打组的元件必须是非组元件，避免产生组嵌套的歧义。
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item != nullptr && item->isGroupItem())
        {
            QMessageBox::information(nullptr, "info", "Only non-group item can be grouped");
            return false;
        }
    }

    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item)
        {
            item->setGroupID(groupID);
        }
    }
    return true;
}

void page_scene::ungroupSelectItems()
{
    // 取消组，不能单个取消，只能全部组一起取消，然后在从新打组
    QMap<int, QList<base_design_item *>> groupItemList = selectGroupItem();
    foreach (QGraphicsItem *graphicItem, items())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item && item->isGroupItem())
        {
            if (groupItemList.contains(item->groupID()))
            {
                item->setGroupID(-1);
                // 清空组变量
                item->setGroupVariant(variant_info());
                // 清除所有变量
                item->clearVariant();
            }
        }
    }
}

void page_scene::setGroupVariant()
{
    // 判断选中的item,是否存在多个groupID,如果是则给出提示，组属性只能单个编辑
    QMap<int, QList<base_design_item *>> groupItemList = selectGroupItem();
    if (groupItemList.count() > 1)
    {
        QMessageBox::information(nullptr, "info", "You can set properties only for a single group");
        return;
    }

    group_property_dialog *groupVariant =  new group_property_dialog(this);
    connect(groupVariant, &group_property_dialog::GroupProperty, this, &page_scene::slotGroupProperty);
    groupVariant->setAttribute(Qt::WA_DeleteOnClose);
    groupVariant->exec();
}

void page_scene::slotGroupProperty(bool pageGroupBreak, variant_info groupVariant)
{
    QMap<int, QList<base_design_item *>> groupItemList = selectGroupItem();
    foreach (QGraphicsItem *graphicItem, items())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem);
        if (item && groupItemList.contains(item->groupID()))
        {
            // 设置段前分页
            item->setGroupPageBreak(pageGroupBreak);
            // 设置组变量
            item->setGroupVariant(groupVariant);
        }
    }
}

void page_scene::selectOneLevelItems()
{
    foreach (QGraphicsItem *graphicItem, selectedItems())
    {
        base_design_item *item = dynamic_cast<base_design_item *>(graphicItem->parentItem());
        if (item)
        {
            selectAllChildren(item);
        }
        else
        {
            selectAllChildren(dynamic_cast<base_design_item *>(graphicItem));
        }
    }
}

void page_scene::removeAllItems()
{
    pageItem()->clear();
    m_commandsList.clear();
}

void page_scene::setItemMode(base_design_item::ItemMode mode)
{
    if (m_itemMode != mode)
    {
        m_itemMode = mode;
        if (m_currentPage)
        {
            m_currentPage->setItemMode(mode);
        }
        else
        {
            foreach (QGraphicsItem *item, items())
            {
                base_design_item *reportItem = dynamic_cast<base_design_item *>(item);
                if (reportItem)
                {
                    reportItem->setItemMode(itemMode());
                }
            }
        }
        foreach (page_item::Ptr page, m_reportPages)
        {
            page->setItemMode(mode);
        }
    }
}

base_design_item *page_scene::reportItemByName(const QString &name)
{
    foreach (QGraphicsItem *item, items())
    {
        base_design_item *bd = dynamic_cast<base_design_item *>(item);
        if (bd && (bd->objectName().compare(name, Qt::CaseInsensitive) == 0))
        {
            return bd;
        }
    }

    return nullptr;
}

QList<base_design_item *> page_scene::reportItemsByName(const QString &name)
{
    QList<base_design_item *> result;
    foreach (QGraphicsItem *item, items())
    {
        base_design_item *bd = dynamic_cast<base_design_item *>(item);
        if (bd && (bd->objectName().compare(name, Qt::CaseInsensitive) == 0))
        {
            result.append(bd);
        }
    }
    return result;
}

base_design_item *page_scene::findTableItem(int ID)
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            base_design_item *tableItem = page->findTableItem(ID);
            if (tableItem)
            {
                return tableItem;
            }
        }
    }
    return nullptr;
}

void page_scene::reloadVariantName(VariantNameMap variantNameMap)
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            page->reloadVariantName(variantNameMap);
        }
    }
    update();
}

void page_scene::renderPrintData(PrintDataMap printData)
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            page->renderPrintData(printData);
        }
    }
    update();
}

void page_scene::clearPrintData()
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            page->clearPrintData();
        }
    }
    update();
}

bool page_scene::watermarkIsEmpty()
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            if (!page->watermarkIsEmpty())
            {
                return false;
            }
        }
    }
    return true;
}

watermark_item *page_scene::watermarkItem()
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            watermark_item *mark = page->reportWatermark();
            if (mark)
            {
                return mark;
            }
        }
    }
    return nullptr;
}

void page_scene::setWatermark(watermark_item *watermark)
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            page->setWatermark(watermark);
        }
    }
}

void page_scene::setModule(QString module)
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            page->setModule(module);
        }
    }
}

void page_scene::clearWatermark()
{
    foreach (QGraphicsItem *item, items())
    {
        page_item *page = dynamic_cast<page_item *>(item);
        if (page)
        {
            page->clearWatermark();
        }
    }
}

void CommandIf::addCommand(Ptr command, bool execute)
{
    Q_UNUSED(command)
    Q_UNUSED(execute)
}

CommandIf::Ptr InsertItemCommand::create(page_scene *page, const QString &itemType, QPointF pos, QSizeF size)
{
    InsertItemCommand *command = new InsertItemCommand();
    command->setPage(page);
    command->setType(itemType);
    command->setPos(pos);
    command->setSize(size);
    return CommandIf::Ptr(command);
}

bool InsertItemCommand::doIt()
{
    base_design_item *item = page()->addReportItem(m_itemType, m_pos, m_size);
    if (item)
    {
        m_itemName = item->objectName();
    }
    return item != 0;
}

void InsertItemCommand::undoIt()
{
    base_design_item *item = page()->reportItemByName(m_itemName);
    if (item)
    {
        page()->removeReportItem(item, false);
    }
}

CommandIf::Ptr DeleteItemCommand::create(page_scene *page, base_design_item *item)
{
    DeleteItemCommand *command = new DeleteItemCommand();
    command->setPage(page);
    command->setItem(item);
    return CommandIf::Ptr(command);
}

bool DeleteItemCommand::doIt()
{
    base_design_item *item = page()->reportItemByName(m_itemName);
    if (item)
    {
        item->beforeDelete();
        page()->removeItem(item);
        page()->emitItemRemoved(item);
        delete item;
        return true;
    }
    return false;
}

void DeleteItemCommand::undoIt()
{
    base_design_item *item = page()->createReportItem(m_itemType);
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemXML);
    if (reader->first())
    {
        reader->readItem(item);
    }
    page()->registerItem(item);
}

void DeleteItemCommand::setItem(base_design_item *value)
{
    m_itemName = value->objectName();
    m_itemType = value->storageTypeName();
    QScopedPointer<ItemsWriterIntf> writer(new XMLWriter());
    writer->putItem(value);
    m_itemXML = writer->saveToString();
}

CommandIf::Ptr PasteCommand::create(page_scene *page, const QString &itemsXML, base_design_item *parent)
{
    PasteCommand *command = new PasteCommand();
    command->setPage(page);
    command->setItemsXML(itemsXML);
    command->setParent(parent);
    return CommandIf::Ptr(command);
}

bool PasteCommand::doIt()
{
    m_itemNames.clear();

    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemsXML);

    // 复制前的元件置为非选中状态
    for (auto Item : page()->selectedItems())
    {
        Item->setSelected(false);
    }

    // 更新剪切板内容，这里更新是为了在一次复制，多次粘贴的情况下，元件创建的位置不重叠。
    QScopedPointer<ItemsWriterIntf> writer(new XMLWriter);
    QClipboard *clipboard = QApplication::clipboard();

    if (reader->first() && reader->itemType() == "Object")
    {
        page()->beginUpdate();
        do
        {
            base_design_item *item = insertItem(reader);
            if (item)
            {
                writer->putItem(item);
                item->setSelected(true);
                emit item->page()->pasteItemInsert(item);
            }
        }
        while (reader->next());

        page()->endUpdate();
        clipboard->setText(writer->saveToString());
    }
    else
    {
        return false;
    }

    return m_itemNames.count() > 0;
}

void PasteCommand::undoIt()
{
    foreach (QString name, m_itemNames)
    {
        base_design_item *item = page()->reportItemByName(name);
        page()->emitItemRemoved(item);
        page()->removeItem(item);
        delete item;
    }
}

void PasteCommand::setItemsXML(const QString &itemsXML)
{
    m_itemsXML = itemsXML;
}

base_design_item *PasteCommand::insertItem(ItemsReaderIntf::Ptr reader)
{
    base_design_item *parentItem = page()->reportItemByName(m_parentItemName);
    if (parentItem)
    {
        base_design_item *item = page()->addReportItem(reader->itemClassName(), parentItem, parentItem);
        if (item)
        {
            QString objectName = item->objectName();
            reader->readItem(item);
            // 复制元件时 + 位置属性加上一个偏移量，使用体验式更好
            item->setItemPos(item->pos().x() + COPY_POS_OFFSET, item->pos().y() + COPY_POS_OFFSET);
            item->setParent(parentItem);
            item->setParentItem(parentItem);
            if (page()->reportItemsByName(item->objectName()).size() > 1)
            {
                item->setObjectName(objectName);
            }

            foreach (base_design_item *child, item->childBaseItems())
            {
                changeName(page(), child);
            }

            m_itemNames.push_back(item->objectName());
            return item;
        }

    }
    return nullptr;
}

void PasteCommand::changeName(page_scene *page, base_design_item *item)
{
    item->setObjectName(page->genObjectName(*item));
    foreach (base_design_item *child, item->childBaseItems())
    {
        changeName(page, child);
    };
}

CommandIf::Ptr CutCommand::create(page_scene *page)
{
    CutCommand *command = new CutCommand();
    command->setPage(page);
    QScopedPointer<ItemsWriterIntf> writer(new XMLWriter());
    foreach (QGraphicsItem *item, page->selectedItems())
    {
        if (!dynamic_cast<page_item *>(item))
        {
            base_design_item *reportItem = dynamic_cast<base_design_item *>(item);

            if (reportItem)
            {
                command->m_itemNames.push_back(reportItem->objectName());
                writer->putItem(reportItem);
            }
        }
    }
    command->setXML(writer->saveToString());

    if (command->m_itemNames.count() > 0)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(writer->saveToString());
    }

    return CommandIf::Ptr(command);
}

bool CutCommand::doIt()
{
    foreach (QString name, m_itemNames)
    {
        base_design_item *item = page()->reportItemByName(name);
        page()->removeItem(item);
        page()->emitItemRemoved(item);
        delete item;
    }
    return m_itemNames.count() > 0;
}

void CutCommand::undoIt()
{
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(m_itemsXML);

    if (reader->first())
    {
        base_design_item *item = page()->addReportItem(reader->itemClassName());

        if (item)
        {
            reader->readItem(item);
        }

        while (reader->next())
        {
            item = page()->addReportItem(reader->itemClassName());
            if (item)
            {
                reader->readItem(item);
            }
        }
    }
}

CommandIf::Ptr PosChangedCommand::create(page_scene *page, QVector<ReportItemPos> &oldPos, QVector<ReportItemPos> &newPos)
{
    PosChangedCommand *command = new PosChangedCommand();
    command->setPage(page);
    command->m_newPos = newPos;
    command->m_oldPos = oldPos;
    return CommandIf::Ptr(command);
}

bool PosChangedCommand::doIt()
{
    for (int i = 0; i < m_newPos.count(); i++)
    {
        base_design_item *reportItem = page()->reportItemByName(m_newPos[i].objectName);

        if (reportItem && (reportItem->pos() != m_newPos[i].pos))
        {
            QPointF oldValue = reportItem->pos();
            reportItem->setPos(m_newPos[i].pos);
            reportItem->emitPosChanged(oldValue, reportItem->pos());
        }
    }

    return true;
}

void PosChangedCommand::undoIt()
{
    for (int i = 0; i < m_oldPos.count(); i++)
    {
        base_design_item *reportItem = page()->reportItemByName(m_oldPos[i].objectName);

        if (reportItem && (reportItem->pos() != m_oldPos[i].pos))
        {
            QPointF oldValue = reportItem->pos();
            reportItem->setPos(m_oldPos[i].pos);
            reportItem->emitPosChanged(oldValue, reportItem->pos());
        }
    }
}

CommandIf::Ptr SizeChangedCommand::create(page_scene *page, QVector<ReportItemSize> &oldSize, QVector<ReportItemSize> &newSize)
{
    SizeChangedCommand *command = new SizeChangedCommand();
    command->setPage(page);
    command->m_newSize = newSize;
    command->m_oldSize = oldSize;
    return CommandIf::Ptr(command);
}

bool SizeChangedCommand::doIt()
{
    for (int i = 0; i < m_newSize.count(); i++)
    {
        base_design_item *reportItem = page()->reportItemByName(m_newSize[i].objectName);

        if (reportItem && (reportItem->size() != m_newSize[i].size))
        {
            reportItem->setSize(m_newSize[i].size);
        }
    }

    return true;
}

void SizeChangedCommand::undoIt()
{
    for (int i = 0; i < m_oldSize.count(); i++)
    {
        base_design_item *reportItem = page()->reportItemByName(m_oldSize[i].objectName);

        if (reportItem && (reportItem->size() != m_oldSize[i].size))
        {
            reportItem->setSize(m_oldSize[i].size);
        }
    }
}

CommandIf::Ptr PropertyChangedCommand::create(page_scene *page, const QString &objectName, const QString &propertyName,
                                              const QVariant &oldValue, const QVariant &newValue)
{
    PropertyChangedCommand *command = new PropertyChangedCommand();
    command->setPage(page);
    command->m_objectName = objectName;
    command->m_propertyName = propertyName;
    command->m_oldValue = oldValue;
    command->m_newValue = newValue;
    return CommandIf::Ptr(command);
}

bool PropertyChangedCommand::doIt()
{
    base_design_item *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_newValue))
    {
        reportItem->setProperty(m_propertyName.toLatin1(), m_newValue);
    }

    return true;
}

void PropertyChangedCommand::undoIt()
{
    base_design_item *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_oldValue))
    {
        reportItem->setProperty(m_propertyName.toLatin1(), m_oldValue);
    }
}

CommandIf::Ptr CommandGroup::create()
{
    return CommandIf::Ptr(new CommandGroup);
}

bool CommandGroup::doIt()
{
    foreach (CommandIf::Ptr command, m_commands)
    {
        if (!command->doIt())
        {
            return false;
        }
    }
    return true;
}

void CommandGroup::undoIt()
{
    foreach (CommandIf::Ptr command, m_commands)
    {
        command->undoIt();
    }
}

void CommandGroup::addCommand(CommandIf::Ptr command, bool execute)
{
    if (execute)
    {
        if (command->doIt())
        {
            m_commands.append(command);
        }
    }
    else
    {
        m_commands.append(command);
    }
}

CommandIf::Ptr PropertyObjectNameChangedCommand::create(page_scene *page, const QString &oldValue, const QString &newValue)
{
    PropertyObjectNameChangedCommand *command = new PropertyObjectNameChangedCommand();
    command->setPage(page);
    command->m_oldName = oldValue;
    command->m_newName = newValue;
    return CommandIf::Ptr(command);
}

bool PropertyObjectNameChangedCommand::doIt()
{
    base_design_item *reportItem = page()->reportItemByName(m_oldName);

    if (reportItem)
    {
        reportItem->setObjectName(m_newName);
        reportItem->emitObjectNamePropertyChanged(m_oldName, m_newName);
        return true;
    }
    return false;
}

void PropertyObjectNameChangedCommand::undoIt()
{
    base_design_item *reportItem = page()->reportItemByName(m_newName);

    if (reportItem)
    {
        reportItem->setObjectName(m_oldName);
        reportItem->emitObjectNamePropertyChanged(m_newName, m_oldName);
    }

}

CommandIf::Ptr PropertyItemAlignChangedCommand::create(page_scene *page, const QString &objectName,
                                                       base_design_item::ItemAlign oldValue, base_design_item::ItemAlign newValue)
{
    PropertyItemAlignChangedCommand *command = new PropertyItemAlignChangedCommand();
    command->setPage(page);
    command->m_objectName = objectName;
    command->m_propertyName = "itemAlign";
    command->m_oldValue = oldValue;
    command->m_newValue = newValue;

    base_design_item *reportItem = page->reportItemByName(objectName);
    if (oldValue == base_design_item::DesignedItemAlign)
    {
        command->m_savedPos = reportItem->pos();
    }

    return CommandIf::Ptr(command);
}

bool PropertyItemAlignChangedCommand::doIt()
{
    base_design_item *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_newValue))
    {
        reportItem->setProperty(m_propertyName.toLatin1(), m_newValue);
    }

    return true;
}

void PropertyItemAlignChangedCommand::undoIt()
{
    base_design_item *reportItem = page()->reportItemByName(m_objectName);

    if (reportItem && (reportItem->property(m_propertyName.toLatin1()) != m_oldValue))
    {
        reportItem->setProperty(m_propertyName.toLatin1(), m_oldValue);
    }
    if (reportItem && (m_oldValue == base_design_item::DesignedItemAlign))
    {
        reportItem->setPos(m_savedPos);
    }
}

bool Projection::intersect(Projection projection)
{
    if (
        (projection.start() >= start() && projection.start() <= end()) ||
        (projection.end() >= start() && projection.end() <= end()) ||
        (projection.start() <= start() && projection.end() >= end())
    )
    {
        return true;
    }
    return false;
}

qreal Projection::start() const
{
    return m_start;
}

qreal Projection::end() const
{
    return m_end;
}

bool ItemProjections::intersect(QRectF rect)
{
    Projection xProjection(rect.x(), rect.x() + rect.width());
    Projection yProjection(rect.y(), rect.y() + rect.height());
    if (m_xProjection.intersect(xProjection) && m_yProjection.intersect(yProjection))
    {
        return true;
    }
    return false;
}

bool ItemProjections::intersect(base_design_item *item)
{
    return intersect(QRectF(item->pos().x(), item->pos().y(), item->width(), item->height()));
}

qreal lineLength(qreal start, qreal end, Projection p)
{
    qreal result = 0;
    if (start >= p.start() && end <= p.end())
    {
        result = end - start;
    }
    if (start >= p.start() && start <= p.end())
    {
        result = p.end() - start;
    }
    else if (end >= p.start() && end <= p.end())
    {
        result = end - p.start();
    }
    else if (start <= p.start() && end >= p.end())
    {
        result = p.end() - p.start();
    }
    return result;
}

qreal ItemProjections::square(QRectF rect)
{
    qreal a = lineLength(rect.left(), rect.right(), m_xProjection);
    qreal b = lineLength(rect.top(), rect.bottom(), m_yProjection);
    return a * b;
}

qreal ItemProjections::square(base_design_item *item)
{
    return square(QRectF(item->pos().x(), item->pos().y(), item->width(), item->height()));
}
}

