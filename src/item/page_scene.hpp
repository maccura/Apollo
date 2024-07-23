/***************************************************************************
*   This file is part of the Andromedae project                           *
*   Copyright (C) 2024 by Mike Medical Electronics Co., Ltd               *
*   Andromedae@maccura.com                                                *
*                                                                         *
**                   GNU General Public License Usage                    **
*                                                                         *
*   This library is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation, either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*   You should have received a copy of the GNU General Public License     *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
*                                                                         *
**                  GNU Lesser General Public License                    **
*                                                                         *
*   This library is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU Lesser General Public License as        *
*   published by the Free Software Foundation, either version 3 of the    *
*   License, or (at your option) any later version.                       *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library.                                      *
*   If not, see <http://www.gnu.org/licenses/>.                           *
*                                                                         *
*   This library is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
****************************************************************************/
#ifndef PAGEDEDIGNITF_H
#define PAGEDEDIGNITF_H

#include <QGraphicsScene>
#include "page_item.hpp"
#include "serializators/lrstorageintf.hpp"

namespace ReportDesign
{

class base_design_item;
class ReportEnginePrivate;
class PropertyChangedCommand;
class HorizontalLayout;
class VerticalLayout;

class CommandIf
{
public:
    virtual ~CommandIf() {}
    typedef QSharedPointer<CommandIf> Ptr;
    virtual bool doIt() = 0;
    virtual void undoIt() = 0;
    virtual void addCommand(CommandIf::Ptr command, bool execute);
};


struct ReportItemPos
{
    QString objectName;
    QPointF pos;
};

struct ReportItemSize
{
    QString objectName;
    QSizeF  size;
};

class Projection
{
public:
    Projection(qreal start, qreal end)
        : m_start(start), m_end(end) {}
    bool  intersect(Projection projection);
    qreal start() const;
    qreal end() const;
private:
    qreal m_start;
    qreal m_end;
};

class ItemProjections
{
public:
    ItemProjections(base_design_item *item)
        : m_xProjection(item->pos().x(), item->pos().x() + item->width()),
          m_yProjection(item->pos().y(), item->pos().y() + item->height()),
          m_item(item)
    {}
    bool intersect(QRectF rect);
    bool intersect(base_design_item *item);
    qreal square(QRectF rect);
    qreal square(base_design_item *item);
    base_design_item *item()
    {
        return m_item;
    }
private:
    Projection m_xProjection;
    Projection m_yProjection;
    base_design_item *m_item;
};

class page_scene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(QObject *pageItem READ pageItem)
public:
    friend class PropertyChangedCommand;
    friend class InsertHLayoutCommand;
    friend class InsertVLayoutCommand;
    explicit page_scene(QString module, QObject *parent = 0, page_item::Ptr pageItem = nullptr);
    ~page_scene();

    void updatePageRect(page_item::Ptr pageItem = nullptr);
    void startInsertMode(const QString &ItemType);
    void startEditMode();
    void reloadPageItem();

    base_design_item *findTableItem(int ID);

    void reloadVariantName(VariantNameMap variantNameMap);
    void renderPrintData(PrintDataMap jsonData);
    void clearPrintData();

    void setWatermark(watermark_item *watermark);
    void clearWatermark();

    void setModule(QString module);

    bool watermarkIsEmpty();
    watermark_item *watermarkItem();

    page_item *pageItem();
    page_item::Ptr pageItemSharePtr();
    void setPageItem(page_item::Ptr pageItem);
    void setPageItems(QList<page_item::Ptr> pages);
    void removePageItem(page_item::Ptr pageItem);
    QList<page_item::Ptr> pageItems()
    {
        return m_reportPages;
    }

    bool isItemInsertMode();
    void registerItem(base_design_item *item);

    void removeAllItems();

    void removeAllCmd()
    {
        m_commandsList.clear();
        m_currentCommand = -1;
    }

    void setItemMode(base_design_item::ItemMode state);
    base_design_item::ItemMode itemMode()
    {
        return m_itemMode;
    }
    base_design_item *reportItemByName(const QString &name);
    QList<base_design_item *> reportItemsByName(const QString &name);

    base_design_item *addReportItem(const QString &itemType, QPointF pos, QSizeF size);
    base_design_item *addReportItem(const QString &itemType, QObject *owner = 0, base_design_item *parent = 0);
    base_design_item *createReportItem(const QString &itemType, QObject *owner = 0, base_design_item *parent = 0);
    void removeReportItem(base_design_item *item, bool createComand = true);
    CommandIf::Ptr removeReportItemCommand(base_design_item *item);

    // 执行命令，并存入命令队列（实现撤销功能）
    bool saveCommand(CommandIf::Ptr command, bool runCommand = true);

    bool isCanRedo();
    bool isCanUndo();
    bool isHasChanges();

    void reactivatePageItem(page_item::Ptr pageItem);

    void setSettings(QSettings *settings)
    {
        m_settings = settings;
    }
    QSettings *settings()
    {
        return m_settings;
    }

    QString genObjectName(const QObject &object);

    void animateItem(base_design_item *item);
    void setSelectionRect(QRectF selectionRect);
    void emitRegisterdItem(base_design_item *item);
    void emitItemRemoved(base_design_item *item);


    bool isSaved()
    {
        return !m_hasChanges;
    }
    void changeSelectedGrpoupTextAlignPropperty(const bool &horizontalAlign, Qt::AlignmentFlag flag);

    int verticalGridStep() const;
    void setVerticalGridStep(int verticalGridStep);

    int horizontalGridStep() const;
    void setHorizontalGridStep(int horizontalGridStep);

    void beginUpdate()
    {
        m_updating = true;
    }
    bool isUpdating()
    {
        return m_updating;
    }
    void endUpdate();

    void rectMoved(QRectF itemRect, base_design_item *container = 0);
    void itemMoved(base_design_item *item);
    bool magneticMovement() const;
    void setMagneticMovement(bool magneticMovement);

    ReportSettings *getReportSettings() const;
    void setReportSettings(ReportSettings *reportSettings);

    void setPropertyToSelectedItems(const char *name, const QVariant &value);

    page_item *getCurrentPage() const;
    void setCurrentPage(page_item *currentPage);

    QString module()
    {
        return m_module;
    }

    bool isVariantLock();
    bool itemVariantLock();
    bool itemVariantUnLock();
protected:

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);


    base_design_item *findDestObject(base_design_item *item);

    bool isExistsObjectName(const QString &objectName, QList<QGraphicsItem *> &itemsList) const;

    bool isLoading();
    void objectLoadStarted();
    void objectLoadFinished();
    QPointF placePosOnGrid(QPointF point);
    QSizeF placeSizeOnGrid(QSizeF size);
signals:
    void pasteItemInsert(base_design_item *item);
    void geometryChanged(QRectF newGeometry);
    void insertModeStarted();
    void itemInserted(page_scene *report, QPointF pos, const QString &ItemType);
    void itemInsertCanceled(const QString &ItemType);
    void itemSelected(base_design_item *item);
    void multiItemsSelected(QList<QObject *> *objectsList);
    void miltiItemsSelectionFinished();
    void commandHistoryChanged();
    void itemPropertyChanged(const QString &objectName, const QString &propertyName,
                             const QVariant &oldValue, const QVariant &newValue);
    void itemPropertyObjectNameChanged(const QString &oldName, const QString &newName);
    void itemAdded(page_scene *page,  base_design_item *item);
    void itemRemoved(page_scene *page,  base_design_item *item);
    void pageUpdateFinished(page_scene *page);

    void addConnectTableItem(int addTableItemID, int ConnectTableItemID);
    void deleteConnectTableItem(int deleteTableItemID, int ConnectTableItemID);
public slots:
    //void bandGeometryChanged(QObject* object, QRectF newGeometry, QRectF oldGeometry);
    //void bandPosChanged(QObject* object, QPointF newPos, QPointF oldPos);
    void slotTableItemDestroyed(QObject *item);
    void slotPasteItemInsert(base_design_item *item);
    void slotUpdateItemSize();
    void undo();
    void redo();
    void copy();
    void paste();
    void deleteSelected();
    void cut();
    void setToSaved();
    void bringToFront();
    void sendToBack();
    void alignToLeft();
    void alignToRigth();
    void alignToVCenter();
    void alignToTop();
    void alignToBottom();
    void alignToHCenter();
    void sameWidth();
    void sameHeight();

    void setFont(const QFont &font);
    void setTextAlign(const Qt::Alignment &alignment);
    void setBorders(const base_design_item::BorderLines &border);

    void lockSelectedItems();
    void unlockSelectedItems();
    void selectOneLevelItems();

    void setPageHeader();
    void unheader();
    void setPageFooter();
    void unfooter();
    bool groupSelectItems(int groupID);
    void ungroupSelectItems();
    void setGroupVariant();
    void slotGroupProperty(bool pageGroupBreak, variant_info groupVariant);
    QMap<int, QList<base_design_item *>> selectGroupItem();

private slots:
    void slotNewPage(QString module);
    void slotAddPage();
    void slotPageGeometryChanged(QObject *, QRectF, QRectF);
    void slotItemPropertyChanged(QString propertyName,
                                 const QVariant &oldValue,
                                 const QVariant &newValue);
    void slotItemPropertyObjectNameChanged(const QString &oldName, const QString &newName);

    void slotPageItemLoaded(QObject *);
    void slotSelectionChanged();
    void slotAnimationStoped(QObject *animation);
private:
    //        template <typename T>
    //        base_design_item* internalAddBand(T bandType);
    void finalizeInsertMode();
    void saveSelectedItemsPos();
    void saveSelectedItemsGeometry();
    void checkSizeOrPosChanges();
    CommandIf::Ptr createChangePosCommand();
    CommandIf::Ptr createChangeSizeCommand();
    void saveChangeProppertyCommand(const QString &objectName,
                                    const QString &propertyName,
                                    const QVariant &oldPropertyValue,
                                    const QVariant &newPropertyValue);
    void changeSelectedGroupProperty(const QString &name, const QVariant &value);
    void activateItemToJoin(QRectF itemRect, QList<ItemProjections> &items);
    void selectAllChildren(base_design_item *item);
    bool selectionContainsBand();
    QGraphicsItem *minXGraphicsItem(QList<QGraphicsItem *> &itemList);
    QGraphicsItem *minYGraphicsItem(QList<QGraphicsItem *> &itemList);
    QGraphicsItem *maxXGraphicsItem(QList<QGraphicsItem *> &itemList);
    QGraphicsItem *maxYGraphicsItem(QList<QGraphicsItem *> &itemList);
private:
    enum JoinType {Width, Height};
    // 设计时,一个场景只有一个page,用的这个变量
    page_item::Ptr m_pageItem;

    // 预览的时候使用，一个场景多个Page
    QList<page_item::Ptr> m_reportPages;
    //ReportEnginePrivate* m_reportEditor;
    bool m_insertMode;
    QGraphicsItem *m_itemInsertRect;
    QString m_insertItemType;
    base_design_item::ItemMode m_itemMode;
    QGraphicsRectItem *m_cutterBorder;
    QGraphicsRectItem *m_pageRect;
    //        QGraphicsTextItem* m_infoPosRect;
    QVector<CommandIf::Ptr> m_commandsList;
    QVector<ReportItemPos> m_positionStamp;
    QVector<ReportItemSize> m_geometryStamp;
    base_design_item *m_firstSelectedItem;
    int m_currentCommand;
    bool m_changeSizeMode;
    bool m_changePosMode;
    bool m_changePosOrSizeMode;
    bool m_executingCommand;
    bool m_hasChanges;
    bool m_isLoading;
    bool m_executingGroupCommand;
    bool m_viewChang;
    QSettings *m_settings;
    QList<QObject *> m_animationList;
    QPointF m_startSelectionPoint;
    QGraphicsRectItem *m_selectionRect;
    int m_verticalGridStep;
    int m_horizontalGridStep;
    bool m_updating;
    int m_currentObjectIndex;
    bool m_multiSelectStarted;
    QList<ItemProjections> m_projections;
    base_design_item  *m_movedItem;
    base_design_item  *m_movedItemContainer;
    base_design_item  *m_joinItem;
    JoinType         m_joinType;
    bool             m_magneticMovement;
    ReportSettings  *m_reportSettings;
    page_item *m_currentPage;

    QString m_module;

};

class AbstractPageCommand : public CommandIf
{
public:
    void setPage(page_scene *value)
    {
        m_page = value;
    }
    page_scene *page()
    {
        return m_page;
    }
private:
    page_scene *m_page;
};

class InsertItemCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, const QString &itemType, QPointF pos, QSizeF size);
    bool doIt();
    void undoIt();
    void setPos(QPointF &value)
    {
        m_pos = value;
    }
    QPointF pos()
    {
        return m_pos;
    }
    void setSize(QSizeF &value)
    {
        m_size = value;
    }
    QSizeF size()
    {
        return m_size;
    }
    void setType(const QString &value)
    {
        m_itemType = value;
    }
private:
    InsertItemCommand() {}
private:
    QPointF m_pos;
    QSizeF m_size;
    QString m_itemType;
    QString m_itemName;
};

class DeleteItemCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, base_design_item *item);
    bool doIt();
    void undoIt();
    void setType(const QString &value)
    {
        m_itemType = value;
    }
    void setXml(const QString &value)
    {
        m_itemXML = value;
    }
    void setItem(base_design_item *value);
private:
    QString m_itemXML;
    QString m_itemType;
    QString m_itemName;
    QString m_layoutName;
};

class PasteCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, const QString &itemsXML, base_design_item *parent);
    bool doIt();
    void undoIt();
protected:
    void setItemsXML(const QString &itemsXML);
    void setParent(base_design_item *parent)
    {
        m_parentItemName = parent->objectName();
    }
    base_design_item *insertItem(ItemsReaderIntf::Ptr reader);
    void changeName(page_scene *page, base_design_item *item);
private:
    QString m_itemsXML;
    QString m_parentItemName;
    QVector<QString> m_itemNames;
};

class CutCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page);
    bool doIt();
    void undoIt();
    void setXML(const QString &value)
    {
        m_itemsXML = value;
    }
private:
    QString m_itemsXML;
    QVector<QString> m_itemNames;
};

class PosChangedCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, QVector<ReportItemPos> &oldPos, QVector<ReportItemPos> &newPos);
    bool doIt();
    void undoIt();
private:
    QVector<ReportItemPos> m_oldPos;
    QVector<ReportItemPos> m_newPos;
};

class SizeChangedCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, QVector<ReportItemSize> &oldSize, QVector<ReportItemSize> &newSize);
    bool doIt();
    void undoIt();
private:
    QVector<ReportItemSize> m_oldSize;
    QVector<ReportItemSize> m_newSize;
};

class PropertyChangedCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, const QString &objectName, const QString &propertyName, const QVariant &oldValue, const QVariant &newValue);
    bool doIt();
    void undoIt();
private:
    QString     m_objectName;
    QString     m_propertyName;
    QVariant    m_oldValue;
    QVariant    m_newValue;
};

class PropertyObjectNameChangedCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, const QString &oldValue, const QString &newValue);
    bool doIt();
    void undoIt();
private:
    QString m_oldName;
    QString m_newName;
};

class PropertyItemAlignChangedCommand : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create(page_scene *page, const QString &objectName, base_design_item::ItemAlign oldValue, base_design_item::ItemAlign newValue);
    bool doIt();
    void undoIt();
private:
    QString m_objectName;
    QString m_propertyName;
    base_design_item::ItemAlign m_oldValue;
    base_design_item::ItemAlign m_newValue;
    QPointF m_savedPos;
};

class CommandGroup : public AbstractPageCommand
{
public:
    static CommandIf::Ptr create();
    bool doIt();
    void undoIt();
    void addCommand(CommandIf::Ptr command, bool execute);
private:
    QList<CommandIf::Ptr> m_commands;
};
}
#endif //PAGEDEDIGNITF_H
