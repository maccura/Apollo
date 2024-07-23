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
#ifndef BASEDESIGNINTF_H
#define BASEDESIGNINTF_H
#include <QObject>
#include <QFrame>
#include <QGraphicsItem>
#include <QtGui>
#include <QMenu>
#include <QAtomicInt>
#include "global.hpp"
#include "lrcollection.hpp"
#include "data_manager/print_data_manager.hpp"
#include "data_manager/base_homogeneous_variant.hpp"
Q_DECLARE_METATYPE(QList<QObject *> *)

namespace ReportDesign
{

enum ItemModes { DesignMode = 1, PreviewMode = 2, PrintMode = 4, EditMode = 8, LayoutEditMode = 16 };

class page_scene;
class base_design_item;

class  Marker : public QGraphicsItem
{
public:
    Marker(QGraphicsItem *parent = 0, base_design_item *owner = 0): QGraphicsItem(parent), m_owner(owner) {}
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setRect(QRectF rect)
    {
        prepareGeometryChange();
        m_rect = rect;
    }
    void setColor(QColor color)
    {
        m_color = color;
    }
    QRectF rect() const
    {
        return m_rect;
    }
    virtual QColor color() const;
    base_design_item *owner() const
    {
        return m_owner;
    }
private:
    QRectF m_rect;
    QColor m_color;
    base_design_item *m_owner;
};

// item选中后矩形边框的红色边框标记
class  SelectionMarker : public Marker
{
public:
    SelectionMarker(QGraphicsItem *parent = 0, base_design_item *owner = 0);
    QColor color() const;
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

class ReportRender;

class  base_design_item :
    public QObject, public QGraphicsItem, public ICollectionContainer
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_FLAGS(BorderLines)

    // 几何信息，位置，宽高。
    Q_PROPERTY(QRect GEOMETRY_PROPERTY READ geometry WRITE setGeometryProperty NOTIFY geometryChanged)
    Q_PROPERTY(ACollectionProperty children READ fakeCollectionReader DESIGNABLE false)
    //Q_PROPERTY(qreal zOrder READ zValue WRITE setZValueProperty DESIGNABLE false)

    // 父对象名字：QObject和QGraphicsItem上的父对象的OObjectName. 且必须是page_scene的子对象。
    Q_PROPERTY(QString parentName READ parentReportItemName WRITE setParentReportItem DESIGNABLE false)

    Q_PROPERTY(int itemZValue READ itemZValue WRITE setItemZValue DESIGNABLE false)

    // 母版属性（页眉页脚控件）
    Q_PROPERTY(int MASTERMASK_PROPERTY READ masterMask WRITE setMasterMask)

    // 母版属性ID，同步编辑使用
    Q_PROPERTY(int MASTERMASK_ITEM_ID_PROPERTY READ masterMarkItemID WRITE setMasterMarkItemID)

    // 此属性只是为了创建属性栏中的对齐元件
    Q_PROPERTY(int ITEM_ALIGNMENT_PROPERTY READ itemAlignment WRITE setItemAlignment)

    Q_PROPERTY(bool GEOMETRY_LOCKED_PROPERTY READ isGeometryLocked WRITE setGeometryLocked)
    //Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

    // 组id
    Q_PROPERTY(int groupID READ groupID WRITE setGroupID)
    // 段前分页
    Q_PROPERTY(bool groupPageBreak READ groupPageBreak WRITE setGroupPageBreak)
    // 组变量
    Q_PROPERTY(variant_info groupVariant READ groupVariant WRITE setGroupVariant)
    //
    friend class ReportRender;
public:
    enum BGMode { TransparentMode, OpaqueMode};

    // resize区域标志，用于鼠标进入区域后进行标志
    enum ResizeFlags
    {
        Fixed = 0,
        ResizeLeft = 1,
        ResizeRight = 2,
        ResizeTop = 4,
        ResizeBottom = 8,
        AllDirections = 15
    };

    enum MoveFlags
    {
        None = 0,
        LeftRight = 1,
        TopBotom = 2,
        All = 3
    };

    enum BorderSide
    {
        NoLine = 0,
        TopLine = 1,
        BottomLine = 2,
        LeftLine = 4,
        RightLine = 8,
        AllLines = 15
    };

    enum ObjectState
    {
        ObjectLoading,
        ObjectLoaded,
        ObjectCreated
    };

    enum ItemAlign
    {
        LeftItemAlign,
        RightItemAlign,
        CenterItemAlign,
        ParentWidthItemAlign,
        DesignedItemAlign
    };

    enum UnitType
    {
        Millimeters,
        Inches
    };
#if QT_VERSION >= 0x050500
    Q_ENUM(BGMode)
    Q_ENUM(ResizeFlags)
    Q_ENUM(MoveFlags)
    Q_ENUM(BorderSide)
    Q_ENUM(ObjectState)
    Q_ENUM(ItemAlign)
    Q_ENUM(UnitType)

#else
    Q_ENUMS(BGMode)
    Q_ENUMS(ResizeFlags)
    Q_ENUMS(MoveFlags)
    Q_ENUMS(BorderSide)
    Q_ENUMS(ObjectState)
    Q_ENUMS(ItemAlign)
    Q_ENUMS(UnitType)

#endif
    Q_DECLARE_FLAGS(BorderLines, BorderSide)
    Q_DECLARE_FLAGS(ItemMode, ItemModes)
    friend class SelectionMarker;
public:
    base_design_item(const QString &storageTypeName, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~base_design_item();

    int static genMasterMarkItemID()
    {
        return m_baseMasterMarkItemID.fetchAndAddOrdered(1);
    }
    // 页眉页脚同步信号
    void itemSynchronization();
    // 页眉页脚删除信号
    void itemDelete();

    // 通过object_name找打page中的元件，并设置为自己的父对象（QObject和QGraphicsItem）。
    void  setParentReportItem(const QString &value);

    // 返回父对象的名字，（object的父对象和QGraphicsItem的父对象）
    QString parentReportItemName() const;

    int groupDataIndex()
    {
        return m_groupDataIndex;
    }
    void setGroupDataIndex(int index)
    {
        m_groupDataIndex = index;
    }

    int genGroupID();

    bool isGroupItem()
    {
        return m_groupID != -1;
    }

    int groupID()
    {
        return m_groupID;
    }

    void setGroupID(int groupID);


    bool groupPageBreak()
    {
        return m_groupPageBreak;
    }

    void setGroupPageBreak(bool groupPageBreak)
    {
        m_groupPageBreak = groupPageBreak;
    }

    variant_info groupVariant()
    {
        return m_groupVariant;
    }

    void setGroupVariant(variant_info variant)
    {
        m_groupVariant = variant;
    }

    QColor backgroundColor() const
    {
        return m_backgroundColor;
    }
    void setBackgroundColor(QColor value);

    QPen pen() const;
    void setPen(QPen &pen);
    QFont font() const;
    void setFont(QFont &font);
    QColor fontColor() const
    {
        return m_fontColor;
    }
    void setFontColor(QColor value)
    {
        m_fontColor = value;
    }

    int itemZValue()
    {
        return zValue();
    }
    void setItemZValue(int itemZValue)
    {
        setZValue(itemZValue);
    }

    bool isMasterMask()
    {
        return m_masterMask != 0;
    }

    int masterMarkItemID()
    {
        return m_masterMarkItemID;
    }

    void setMasterMarkItemID(int masterMarkItemID)
    {
        m_masterMarkItemID = masterMarkItemID;
    }

    int masterMask()
    {
        return m_masterMask;
    }

    void setMasterMask(int masterMask);

    int itemAlignment()
    {
        return 0;
    }
    void setItemAlignment(int Alignment)
    {
        Q_UNUSED(Alignment)
    }

    virtual void    clearVariant() {}
    virtual bool    variantIsEmpty() = 0;
    // 渲染打印数据，示例数据/实际打印数据
    virtual QMap<qreal, qreal>  renderAdaptive(const PrintDataMap &PrintData);
    // 根据渲染数据做控件的尺寸适配
    virtual QMap<qreal, qreal>  renderDataAdaptive();

    virtual void    renderPrintData(const PrintDataMap &PrintData) {}
    virtual bool    printDataIsEmpty()
    {
        return true;
    }
    virtual void    reloadVariantName(const VariantNameMap &variantName) {}
    virtual void    clearPrintData() {}

    virtual BGMode  backgroundMode() const;
    virtual void    setBackgroundMode(BGMode bgMode);
    virtual qreal   width() const;
    virtual qreal   widthMM() const;
    virtual void    setWidth(qreal width);
    virtual qreal   height() const;
    virtual qreal   heightMM() const;
    virtual void    setHeight(qreal height);
    virtual QPointF posMM() const;
    virtual int     opacity() const;
    virtual void    setOpacity(int opacity);
    virtual void    setSize(QSizeF size);
    virtual QSizeF  size() const;
    virtual QSizeF  sizeMM() const;

    void    paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void    prepareRect(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void    drawMasterMarkColor(QPainter *painter);
    void    setMasterMarkColor(QColor color)
    {
        m_masterMarkColor = color;
        update();
    }
    virtual QPainterPath shape() const;

    void setFixedPos(bool fixedPos);
    bool isFixedPos()
    {
        return  m_fixedPos;
    }
    int resizeHandleSize() const;

    qreal    unitFactor() const;
    void     setUnitType(UnitType unitType);
    UnitType unitType();
    virtual QRect  geometry() const;
    void    setGeometry(QRectF rect);

    QRectF  rect() const;
    void    setupPainter(QPainter *painter) const;
    virtual QRectF boundingRect() const;

    virtual void moveRight();
    virtual void moveLeft();
    virtual void moveDown();
    virtual void moveUp();

    virtual void sizeRight();
    virtual void sizeLeft();
    virtual void sizeUp();
    virtual void sizeDown();

    void setItemPos(const QPointF &newPos);
    void setItemPos(qreal x, qreal y);

    virtual void setItemMode(base_design_item::ItemMode mode);
    ItemMode itemMode() const
    {
        return m_itemMode;
    }

    virtual void setBorderLinesFlags(base_design_item::BorderLines flags);
    void setGeometryProperty(QRect rect);
    page_scene *page();

    BorderLines borderLines() const;

    QString storageTypeName() const;
    //ReportEnginePrivate *reportEditor();

    virtual void updateItemSize(RenderPass pass = FirstPass, int maxHeight = 0);
    virtual bool isNeedUpdateSize(RenderPass) const;
    virtual base_design_item *cloneItem(base_design_item::ItemMode mode, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual base_design_item *cloneItemWOChild(base_design_item::ItemMode mode, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0) = 0;
    virtual void initFromItem(base_design_item *source);

    virtual bool canBeSplitted(int height) const;
    virtual qreal minHeight() const
    {
        return 0;
    }
    virtual bool isSplittable() const
    {
        return false;
    }
    virtual bool isEmpty() const;
    virtual base_design_item *cloneUpperPart(int height, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual base_design_item *cloneBottomPart(int height, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual base_design_item *cloneEmpty(int height, QObject *owner = 0, QGraphicsItem *parent = 0);

    bool isLoaded()
    {
        return m_objectState == ObjectLoaded;
    }
    bool isLoading()
    {
        return m_objectState == ObjectLoading;
    }
    void objectLoadStarted();
    void objectLoadFinished();
    virtual void parentObjectLoadFinished();
    virtual void beforeDelete();

    QList<base_design_item *> childBaseItems() const;
    QList<base_design_item *> allChildBaseItems();
    base_design_item *childByName(const QString &name);

    virtual QWidget *defaultEditor();

    // 属性更改通知，会存入cmd队列,用于撤销功能。
    void notify(const QString &propertyName, const QVariant &oldValue, const QVariant &newValue);
    void notify(const QVector<QString> &propertyNames);

    /************************** resize矩形相关逻辑 **********************/
    int possibleResizeDirectionFlags() const;
    void setPossibleResizeDirectionFlags(int directionsFlags);

    int possibleMoveDirectionFlags() const;
    void setPossibleMoveFlags(int directionsFlags);

    int marginSize() const;
    void setMarginSize(int value);

    QString itemTypeName() const;
    void setItemTypeName(const QString &itemTypeName);

    qreal borderLineSize() const;
    //void setBorderStyle(BorderStyle b);
    void setBorderLineSize(qreal value);
    void showEditorDialog();
    ItemAlign itemAlign() const;
    virtual void setItemAlign(const ItemAlign &itemAlign);
    void updateItemAlign();
    QPointF modifyPosForAlignedItem(const QPointF &pos);
    void turnOnJoinMarker(bool value);
    virtual bool isBand()
    {
        return false;
    }
    QColor borderColor() const;
    void setBorderColor(const QColor &borderColor);
    void setItemVisible(const bool &value);
    virtual bool canContainChildren() const
    {
        return false;
    }
    virtual bool canAcceptPaste() const
    {
        return false;
    }
    ReportSettings *reportSettings() const;
    void setReportSettings(ReportSettings *reportSettings);
    void setZValueProperty(qreal value);
    QString patternName() const;
    void setPatternName(const QString &patternName);
    base_design_item *patternItem() const;
    void setPatternItem(base_design_item *patternItem);
    virtual QMap<QString, QString> getStringForTranslation();
    bool fillInSecondPass() const;
    void setFillInSecondPass(bool fillInSecondPass);
    bool isWatermark() const;
    virtual void setWatermark(bool watermark);
    void updateSelectionMarker();
    void turnOnSelectionMarker(bool value);
    bool fillTransparentInDesignMode() const;
    void setFillTransparentInDesignMode(bool fillTransparentInDesignMode);
    void emitPosChanged(QPointF oldPos, QPointF newPos);
    void emitObjectNamePropertyChanged(const QString &oldName, const QString &newName);
    bool isGeometryLocked() const;
    void setGeometryLocked(bool itemLocked);
    bool isChangingPos() const;
    void setIsChangingPos(bool isChangingPos);
    bool isShapeItem() const;
    bool hasShadow();
    void setShadow(bool sh);
    Q_INVOKABLE QString setItemWidth(qreal width);
    Q_INVOKABLE QString setItemHeight(qreal height);
    Q_INVOKABLE qreal getItemWidth();
    Q_INVOKABLE qreal getItemHeight();
    Q_INVOKABLE qreal getItemPosX();
    Q_INVOKABLE qreal getItemPosY();
    Q_INVOKABLE qreal getAbsolutePosX();
    Q_INVOKABLE qreal getAbsolutePosY();
    Q_INVOKABLE QString setItemPosX(qreal xValue);
    Q_INVOKABLE QString setItemPosY(qreal yValue);

protected:
    //ICollectionContainer
    virtual QObject *createElement(const QString &, const QString &elementType) override;
    virtual int elementsCount(const QString &) override;
    virtual QObject *elementAt(const QString &, int index) override;
    virtual void collectionLoadFinished(const QString &collectionName) override;
    //ICollectionContainer

    /************************** resize矩形相关逻辑 **************************/
    // 鼠标移动触发，按住鼠标左键移动不触发（处理的光标变化逻辑）
    void  hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void  hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void  hoverEnterEvent(QGraphicsSceneHoverEvent *) override;

    // 按住鼠标左键并移动鼠标才会触发mouseMoveEvent(尺寸化和元件位置移动逻辑处理)
    void  mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void  mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void  mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    // 鼠标右击，弹出下拉菜单栏
    void  contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    virtual void geometryChangedEvent(QRectF newRect, QRectF oldRect);
    virtual QPen borderPen(BorderSide side) const;
    virtual QColor selectionColor() const;
    virtual void initFlags();
    virtual void initMode(base_design_item::ItemMode mode);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void childAddedEvent(base_design_item *child);
    virtual void parentChangedEvent(base_design_item *);
    void restoreLinks();
    virtual void restoreLinksEvent() {}

    // 绘制鼠标放到元件上时的边框
    void drawHoverRect(QPainter *painter);

    // 根据item的边框属性，绘制边框，绘制边框则为黑色且更粗，否则绘制灰色边框。
    void drawTopLine(QPainter *painter, QRectF rect) const;
    void drawBootomLine(QPainter *painter, QRectF rect) const;
    void drawRightLine(QPainter *painter, QRectF rect) const;
    void drawLeftLine(QPainter *painter, QRectF rect) const;


    /* 绘制元件size矩形框 */
    void drawBorder(QPainter *painter, QRectF rect) const;
    void drawShadow(QPainter *painter, QRectF rect, qreal shadowSize) const;
    void drawDesignModeBorder(QPainter *painter, QRectF rect) const;
    void drawRenderModeBorder(QPainter *painter, QRectF rect) const;

    /* resize框的绿色效果绘制，暂时没接入 */
    void drawResizeZone(QPainter *);
    void DrawResizeRects(QPainter *painter);

    void invalidateRect(const QRectF &rect);
    void invalidateRects(QVector<QRectF *> rects);
    QFont transformToSceneFont(const QFont &value) const;

    RenderPass currentRenderPass()
    {
        return m_currentPass;
    }

    virtual bool drawDesignBorders() const
    {
        return true;
    }
    virtual QColor selectionMarkerColor()
    {
        return SELECTION_COLOR;
    }


    QVariant m_varValue;

    virtual void preparePopUpMenu(QMenu &menu)
    {
        Q_UNUSED(menu)
    }
    virtual void processPopUpAction(QAction *action);

    void addChildItems(QList<base_design_item *> *list);
    qreal calcAbsolutePosY(qreal currentOffset, base_design_item *item);
    qreal calcAbsolutePosX(qreal currentOffset, base_design_item *item);

    QWidget *findRootWidget(QWidget *widget);
    void showDialog(QWidget *widget);
    int getResizeDirectionFlags();

private:
    int resizeDirectionFlags(QPointF position);
    void moveSelectedItems(QPointF delta);
    Qt::CursorShape getPossibleCursor(int cursorFlags);
    void updatePossibleDirectionFlags();
    static int genZValue();

private:
    QPointF m_startPos;
    int     m_resizeHandleSize;
    int     m_selectionPenSize;
    int     m_possibleResizeDirectionFlags;
    int     m_possibleMoveDirectionFlags;
    int     m_savedPossibleResizeDirectionFlags;
    int     m_savedPossibleMoveDirectionFlags;
    int     m_savedFixedPos;
    int     m_resizeDirectionFlags;
    qreal   m_width;
    qreal   m_height;
    QPen    m_pen;
    QFont   m_font;
    QColor  m_fontColor;
    bool    m_fixedPos;
    qreal     m_borderLineSize;

    QRectF  m_rect;
    //mutable QRectF  m_boundingRect;

    QRectF  m_oldGeometry;
    BGMode  m_BGMode;
    int     m_opacity;
    BorderLines m_borderLinesFlags;
    //BorderStyle m_borderStyle;

    // item矩形框的边界区域，上下左右分布四个小的矩形，用于用户使用鼠标改变item大小。
    QRectF m_bottomRect;
    QRectF m_topRect;
    QRectF m_leftRect;
    QRectF m_rightRect;

    // 画resize边框效果使用（未使用）
    // QVector<QRectF*> m_resizeAreas;
    QString m_storageTypeName;
    ItemMode m_itemMode;

    ObjectState m_objectState;

    //BrushStyle  m_backgroundBrushStyle;
    QColor m_backgroundColor;

    QColor m_masterMarkColor;

    RenderPass m_currentPass;
    int     m_margin;
    QString m_itemTypeName;
    ItemAlign m_itemAlign;
    bool    m_changingItemAlign;
    QColor  m_borderColor;
    ReportSettings *m_reportSettings;
    QString m_patternName;
    base_design_item *m_patternItem;
    bool    m_fillInSecondPass;
    bool    m_watermark;
    bool    m_hovered;
    bool    m_joinMarkerOn;
    SelectionMarker *m_selectionMarker;
    Marker          *m_joinMarker;
    bool     m_fillTransparentInDesignMode;
    QRect    m_itemGeometry;
    UnitType m_unitType;
    bool     m_itemGeometryLocked;
    bool     m_isChangingPos;
    bool     m_isMoveable;
    bool    m_shadow;
    int     m_masterMask;       // 页眉页脚标识
    int     m_masterMarkItemID; // 页眉页脚同步编辑使用
    static QAtomicInt m_baseMasterMarkItemID;
    int     m_groupID;
    bool    m_groupPageBreak;
    variant_info m_groupVariant;

    // 取组数据时使用的下标索引，在渲染动态计算元件个数时，需要设置此值，在渲染数据时元件就按照此下标去组数据中获取对应的数据。
    // 因为协议中组数据是一个数组，key:[data1,data2,...]; 渲染时临时产生，无需持久化。
    int     m_groupDataIndex;

signals:
    void geometryChanged(QObject *object, QRectF newGeometry, QRectF oldGeometry);
    void posChanging(QObject *object, QPointF newPos, QPointF oldPos);
    void posChanged(QObject *object, QPointF newPos, QPointF oldPos);
    void itemSelected(base_design_item *item);
    void itemSelectedHasBeenChanged(base_design_item *item, bool value);
    void loadCollectionFinished(const QString &collectionName);
    void objectLoaded(QObject *object);
    void objectChanged(QObject *object);
    void propertyChanged(const QString &propertName, const QVariant &oldValue, const QVariant &newValue);
    void propertyUpdate(const QString &propertName);
    void propertyObjectNameChanged(const QString &oldValue, const QString &newValue);
    void propertyesChanged(QVector<QString> propertyNames);
    void itemAlignChanged(base_design_item *item, const ItemAlign &oldValue, const ItemAlign &newValue);
    void itemVisibleHasChanged(base_design_item *item);
    void beforeRender();
    void afterData();
    void afterRender();
    void synchronization();
    void deleteItem();
};

} //namespace ReportDesign

#endif // BASEDESIGNINTF_H
