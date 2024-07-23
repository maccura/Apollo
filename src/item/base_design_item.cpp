#include "base_design_item.hpp"
#include <QFrame>
#include "global.hpp"
#include "qgraphicsitem.h"
#include "design_elements_factory.hpp"
#include "serializators/lrxmlreader.hpp"
#include <memory>
#include <QMetaObject>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QMenu>
#include <QClipboard>
#include "page_scene.hpp"
#include "data_manager/base_homogeneous_variant.hpp"
#include <QGraphicsView>
#include <chrono>
#define HOVER_RECT_LINE_WIDTH 2
#define SELECTION_LINE_WIDTH  2

namespace ReportDesign
{

QColor generateGroupColor(int groupID, bool deleteflag = false)
{
    static QMap<int, int> groupColor;
    static QMap<int, QColor> usedGroupColor;
    // 定义20种浅色
    static QList<QColor> lightColors =
    {
        QColor("#FFC0CB"), // 粉红色
        QColor("#FFD700"), // 金色
        QColor("#FFFFE0"), // 浅黄色
        QColor("#ADFF2F"), // 绿黄色
        QColor("#00FFFF"), // 青色
        QColor("#87CEFA"), // 浅蓝色
        QColor("#ADD8E6"), // 浅灰蓝色
        QColor("#E0FFFF"), // 淡青色
        QColor("#F0FFF0"), // 蜜色
        QColor("#FFE4E1"), // 淡红色
        QColor("#FAFAD2"), // 浅黄绿色
        QColor("#FFF5EE"), // 浅黄色
        QColor("#F0E68C"), // 卡其色
        QColor("#FFA07A"), // 浅橙色
        QColor("#EEE8AA"), // 浅黄色
        QColor("#B0C4DE"), // 浅钢蓝色
        QColor("#FFEBCD"), // 浅橙色
        QColor("#F5DEB3")  // 小麦色
    };

    if (deleteflag)
    {
        if (groupColor.contains(groupID))
        {
            usedGroupColor.remove(groupColor[groupID]);
            groupColor.remove(groupID);
        }
        return QColor();
    }

    if (!groupColor.contains(groupID))
    {
        // 判断是否还有没使用的颜色
        for (int i = 0; i < lightColors.count(); i++)
        {
            // 还存在没使用的颜色
            if (!usedGroupColor.contains(i))
            {
                groupColor.insert(groupID, i);
                usedGroupColor.insert(i, lightColors[i]);
                return lightColors[i];
            }
        }
        // 无可使用的颜色
        return QColor();
    }
    else
    {
        return lightColors[groupColor[groupID]];
    }
}

QAtomicInt base_design_item::m_baseMasterMarkItemID = 1;
base_design_item::base_design_item(const QString &itemTypeName, QObject *owner, QGraphicsItem *parent) :
    QObject(owner), QGraphicsItem(parent),
    m_resizeHandleSize(RESIZE_HANDLE_SIZE * 2),
    m_selectionPenSize(SELECTION_PEN_SIZE),
    m_possibleResizeDirectionFlags(ResizeTop | ResizeBottom | ResizeLeft | ResizeRight),
    m_possibleMoveDirectionFlags(All),
    m_resizeDirectionFlags(0),
    m_width(0),
    m_height(0),
    m_fontColor(Qt::black),
    m_fixedPos(false),
    m_borderLineSize(1),
    m_BGMode(OpaqueMode),
    m_opacity(100),
    m_borderLinesFlags(BorderLines()),
    m_storageTypeName(itemTypeName),
    m_itemMode(DesignMode),
    m_objectState(ObjectCreated),
    m_backgroundColor(Qt::white),
    m_masterMarkColor(Qt::white),
    m_margin(4),
    m_itemAlign(DesignedItemAlign),
    m_changingItemAlign(false),
    m_borderColor(Qt::black),
    m_reportSettings(0),
    m_patternName(""),
    m_patternItem(0),
    m_fillInSecondPass(false),
    m_watermark(false),
    m_hovered(false),
    m_joinMarkerOn(false),
    m_selectionMarker(0),
    m_fillTransparentInDesignMode(true),
    m_unitType(Millimeters),
    m_itemGeometryLocked(false),
    m_isChangingPos(false),
    m_isMoveable(false),
    m_shadow(false),
    m_masterMask(0),
    m_masterMarkItemID(0),
    m_groupID(-1),
    m_groupPageBreak(false),
    m_groupDataIndex(0)
{
    setGeometry(QRectF(0, 0, m_width, m_height));
    if (base_design_item *item = dynamic_cast<base_design_item *>(parent))
    {
        m_font = item->font();
    }
    else
    {
        m_font = QFont("Arial", 10);
    }

    // 设置ZValue 让元件拥有层级关系。
    setZValue(genZValue());

    initFlags();

    m_itemAlign = DesignedItemAlign;

    // 属性改变需要做同步
    connect(this, &base_design_item::propertyChanged,
            [this]
    {
        itemSynchronization();
    });
}

int base_design_item::genZValue()
{
    static QAtomicInt m_ZValue = 0;
    return m_ZValue++;
}

QRectF base_design_item::boundingRect() const
{
    qreal halfpw = pen().widthF() / 2;
    halfpw += 2;
    return rect().adjusted(-halfpw, -halfpw, halfpw, halfpw);
}

base_design_item::~base_design_item(void)
{

}

void base_design_item::setParentReportItem(const QString &value)
{
    if (page() && !value.isEmpty())
    {
        base_design_item *p = page()->reportItemByName(value);
        if (p)
        {
            setParentItem(p);
            setParent(p);
        }
    }
}

QString base_design_item::parentReportItemName() const
{
    base_design_item *parent = dynamic_cast<base_design_item *>(parentItem());
    if (parent)
    {
        return parent->objectName();
    }
    else
    {
        return "";
    }
}

void base_design_item::setBackgroundColor(QColor value)
{
    if (value != m_backgroundColor)
    {
        QColor oldValue = m_backgroundColor;
        m_backgroundColor = value;
        if (!isLoading())
        {
            update();
        }
        notify("backgroundColor", oldValue, value);
    }
}

void base_design_item::setMasterMask(int masterMask)
{
    m_masterMask = masterMask;
    if (m_masterMask == PAGE_TOP_ID)
    {
        setMasterMarkItemID(genMasterMarkItemID());
        setMasterMarkColor(PAGE_TOP_COLOR);
    }
    else if (m_masterMask == PAGE_BOTTOM_ID)
    {
        setMasterMarkItemID(genMasterMarkItemID());
        setMasterMarkColor(PAGE_BOTTOM_COLOR);
    }
    else if (m_masterMask == 0)
    {
        // MasterMarkItemID不置0，在同步逻辑中需要使用
        setMasterMarkColor(Qt::white);
    }
    synchronization();
}

QPen base_design_item::pen() const
{
    return m_pen;
}

void base_design_item::setPen(QPen &pen)
{
    m_pen = pen;
    update();
}

QFont base_design_item::font() const
{
    return m_font;
}

void base_design_item::setFont(QFont &font)
{
    if (m_font != font)
    {
        m_font = font;
        if (!isLoading())
        {
            update();
        }
    }
}

qreal base_design_item::width() const
{
    return rect().width();
}

void base_design_item::setWidth(qreal width)
{
    setGeometry(QRectF(rect().x(), rect().y(), width, rect().height()));
    if (!m_changingItemAlign)
    {
        updateItemAlign();
    }
}

QString base_design_item::setItemWidth(qreal width)
{
    setWidth(width * unitFactor());
    return QString();
}

qreal base_design_item::height() const
{
    return rect().height();
}

QRect base_design_item::geometry() const
{
    return QRect(pos().x(), pos().y(), width(), height());
}

void base_design_item::setHeight(qreal height)
{
    setGeometry(QRectF(rect().x(), rect().y(), rect().width(), height));
}

QString base_design_item::setItemHeight(qreal height)
{
    setHeight(height * unitFactor());
    return QString();
}

qreal base_design_item::getItemWidth()
{
    return width() / unitFactor();
}

qreal base_design_item::getItemHeight()
{
    return height() / unitFactor();
}

qreal base_design_item::getItemPosX()
{
    return x() / unitFactor();
}

qreal base_design_item::getItemPosY()
{
    return y() / unitFactor();
}

qreal base_design_item::getAbsolutePosX()
{
    return calcAbsolutePosX(0, this);
}

qreal base_design_item::getAbsolutePosY()
{
    return calcAbsolutePosY(0, this);
}

QString base_design_item::setItemPosX(qreal xValue)
{
    setItemPos(xValue * unitFactor(), y());
    return QString();
}

QString base_design_item::setItemPosY(qreal yValue)
{
    setItemPos(x(), yValue * unitFactor());
    return QString();
}

QFont base_design_item::transformToSceneFont(const QFont &value) const
{
    QFont f = value;
    f.setPixelSize(f.pointSize()* fontFACTOR);
    return f;
}

void base_design_item::setupPainter(QPainter *painter) const
{
    if (!painter)
    {
        return;
    }
    painter->setFont(m_font);
    painter->setPen(m_fontColor);
}

QMap<qreal, qreal>  base_design_item::renderAdaptive(const PrintDataMap &PrintData)
{
    // 存入渲染数据结构体
    renderPrintData(PrintData);

    // 控件根据做数据长度做适配
    return renderDataAdaptive();
}

QMap<qreal, qreal>  base_design_item::renderDataAdaptive()
{
    QMap<qreal, qreal> deltaYMap;
    qreal orgHeight = height();
    // 如果控件本身高度超过可显示区域,则修改控件高度为显示区域高度 - 10px,否则会导致无线递归程序崩溃
    QRect pageItemShowRect;
    page_item *ptr = dynamic_cast<page_item *>(parentItem());
    if (ptr)
    {
        pageItemShowRect =  ptr->itemShowRect();
    }

    // 当前控件高度超过了显示区域
    if (orgHeight >= pageItemShowRect.height())
    {
        setHeight(pageItemShowRect.height() - 5);
    }

    // 是个负数，因为高度减小了。
    deltaYMap[pos().y()] = height() - orgHeight;
    return deltaYMap;
}

base_design_item::BGMode base_design_item::backgroundMode() const
{
    return m_BGMode;
}
void base_design_item::setBackgroundMode(BGMode bgMode)
{
    m_BGMode = bgMode;
    update(boundingRect());
}

int base_design_item::opacity() const
{
    return m_opacity;
}

void base_design_item::setOpacity(int opacity)
{
    if (m_opacity != opacity)
    {
        if (opacity < 0)
        {
            m_opacity = 0;
        }
        else if (opacity > 100)
        {
            m_opacity = 100;
        }
        else
        {
            m_opacity =  opacity;
        }
        update();
    }
}

void base_design_item::setSize(QSizeF size)
{
    setWidth(size.width());
    setHeight(size.height());
}

QSizeF base_design_item::size() const
{
    return QSizeF(width(), height());
}

QSizeF base_design_item::sizeMM() const
{
    return QSizeF(width() / mmFACTOR, height() / mmFACTOR);
}

qreal base_design_item::widthMM() const
{
    return width() / mmFACTOR;
}

qreal base_design_item::heightMM() const
{
    return height() / mmFACTOR;
}

qreal base_design_item::unitFactor() const
{
    if (m_unitType  == Millimeters)
    {
        return mmFACTOR;
    }
    else
    {
        return mmFACTOR * 2.54;
    }
}

void base_design_item::setUnitType(base_design_item::UnitType value)
{
    foreach (base_design_item *child, childBaseItems())
    {
        child->setUnitType(value);
    }
    m_unitType = value;
}

base_design_item::UnitType base_design_item::unitType()
{
    return m_unitType;
}

QPointF base_design_item::posMM() const
{
    return QPointF(pos().x() /   mmFACTOR, pos().y() /   mmFACTOR);
}

QRectF base_design_item::rect() const
{
    return m_rect;
}

void base_design_item::setFixedPos(bool fixedPos)
{
    m_fixedPos = fixedPos;
}

void base_design_item::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isChangingPos = true;
        m_resizeDirectionFlags = resizeDirectionFlags(event->pos());
        m_startPos = pos();
        m_oldGeometry = geometry();
        QGraphicsItem::mousePressEvent(event);

        // 通知属性窗口
        emit itemSelected(this);
    }
    else if (event->button() == Qt::RightButton)
    {
        if (scene()->selectedItems().count() > 0)
        {
            foreach (QGraphicsItem *item, scene()->selectedItems())
            {
                item->setSelected(true);
            }
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void base_design_item::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF newGeometry = geometry();
    m_isChangingPos = false;
    if (newGeometry != m_oldGeometry)
    {
        geometryChangedEvent(newGeometry, m_oldGeometry);
        emit posChanged(this, newGeometry.topLeft(), m_oldGeometry.topLeft());
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void base_design_item::paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    ppainter->save();

    // 设置painter属性
    setupPainter(ppainter);

    // 绘制size矩形
    drawBorder(ppainter, rect());
    if (m_shadow)
    {
        drawShadow(ppainter, rect(), 6);
    }
    drawHoverRect(ppainter);
    ppainter->restore();
}

QColor calcColor(QColor color)
{
    int R = color.red();
    int G = color.green();
    int B = color.blue();
    if (0.222 * R + 0.707 * G + 0.071 * B <= 127)
    {
        return Qt::white;
    }
    else
    {
        return Qt::black;
    }
}

void base_design_item::prepareRect(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->save();

    QRectF r = rect().adjusted(0, 0, borderLineSize(), borderLineSize());
    QBrush brush(m_backgroundColor/*,static_cast<Qt::BrushStyle>(m_backgroundBrushStyle)*/);
    brush.setTransform(painter->worldTransform().inverted());

    if (isSelected() && (opacity() == 100) && (m_BGMode != TransparentMode))
    {
        painter->fillRect(r, brush);
    }
    else
    {
        if (m_BGMode == OpaqueMode)
        {
            qreal o = (itemMode() & DesignMode) ? 0.5 : qreal(m_opacity) / 100;
            painter->setOpacity(o);
            painter->fillRect(r, brush);
        }
        else if ((itemMode() & DesignMode) && fillTransparentInDesignMode())
        {
            painter->setOpacity(0.1);
            painter->fillRect(r, QBrush(QPixmap(":/report/images/empty")));
        }
    }
    painter->restore();
}

void base_design_item::drawMasterMarkColor(QPainter *painter)
{
    //    if (isMasterMask() &&
    //            itemMode() & DesignMode)
    if (itemMode() & DesignMode)
    {
        painter->save();
        QRectF r = rect().adjusted(0, 0, borderLineSize(), borderLineSize());
        QBrush brush(m_masterMarkColor);
        brush.setTransform(painter->worldTransform().inverted());
        qreal o = 0.25;
        painter->setOpacity(o);
        painter->fillRect(r, brush);

        painter->restore();
    }
}

void base_design_item::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (page())
    {
        // 元件已被选中
        if (!page()->isItemInsertMode() && isSelected())
        {
            // 判断光标位置是否在矩形框的上下左右的四个矩形区域内resizeAreas
            if (m_resizeDirectionFlags != resizeDirectionFlags(event->pos()))
            {
                m_resizeDirectionFlags = resizeDirectionFlags(event->pos());

                switch (m_resizeDirectionFlags)
                {
                case ResizeRight:
                case ResizeLeft:
                    setCursor(Qt::SizeHorCursor);
                    break;
                case ResizeBottom:
                case ResizeTop:
                    setCursor(Qt::SizeVerCursor);
                    break;
                case ResizeRight | ResizeBottom:
                case ResizeLeft  | ResizeTop:
                    setCursor(Qt::SizeFDiagCursor);
                    break;
                case ResizeLeft  | ResizeBottom:
                case ResizeRight | ResizeTop:
                    setCursor(Qt::SizeBDiagCursor);
                    break;
                default:
                    setCursor(Qt::ArrowCursor);
                    break;
                }
                update();
            }
        }

    }
}


void base_design_item::invalidateRects(QVector<QRectF *> rects)
{
    foreach (QRectF *rect, rects)
    {
        scene()->update(mapToScene(*rect).boundingRect());
    }
}

void base_design_item::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    setCursor(QCursor(Qt::ArrowCursor));
    m_resizeDirectionFlags = 0;
    scene()->update(sceneBoundingRect());
    //m_resizeAreas.clear();
    m_hovered = false;
    update();
}

void base_design_item::hoverEnterEvent(QGraphicsSceneHoverEvent * /*event*/)
{
    m_hovered = true;
    update();
}

void base_design_item::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isSelected())
    {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    int hStep = dynamic_cast<page_scene *>(scene())->horizontalGridStep();
    int vStep = dynamic_cast<page_scene *>(scene())->verticalGridStep();


    if (m_resizeDirectionFlags & ResizeLeft)
    {
        if ((event->scenePos().x()) <= (mapToScene(0, 0).x() + (width() -   MINIMUM_ITEM_WIDTH)) &&
            (width() + (event->lastScenePos().x() - event->scenePos().x()) >   MINIMUM_ITEM_WIDTH)
           )
        {
            qreal posRightCorner = mapToScene(0, 0).x() + width();
            qreal posLeftCorner = div(mapToParent(event->pos()).x(), hStep).quot * hStep;
            if (posLeftCorner < 0)
            {
                posLeftCorner = 0;
            }
            setItemPos(posLeftCorner, y());
            setWidth(div(posRightCorner - mapToScene(0, 0).x(), hStep).quot * hStep);
        }
    }

    if (m_resizeDirectionFlags & ResizeTop)
    {
        if ((event->scenePos().y()) <= (mapToScene(0, 0).y() + (height() -   MINIMUM_ITEM_HEIGHT)) &&
            (height() + (event->lastScenePos().y() - event->scenePos().y()) >   MINIMUM_ITEM_HEIGHT)
           )
        {
            qreal posBottomCorner = mapToScene(0, 0).y() + height();
            qreal posTopCorner = div(mapToParent(event->pos()).y(), vStep).quot * vStep;
            if (posTopCorner < 0)
            {
                posTopCorner = 0;
            }
            setItemPos(x(), posTopCorner);
            setHeight(div(posBottomCorner - mapToScene(0, 0).y(), vStep).quot * vStep);
        }
    }

    if (m_resizeDirectionFlags & ResizeRight)
    {
        if ((event->scenePos().x() >= (mapToScene(0, 0).x() +   MINIMUM_ITEM_WIDTH)) ||
            (event->scenePos().x() >= (mapToScene(0, 0).x() + width())))
        {
            setWidth(div(event->scenePos().x() - mapToScene(0, 0).x(), hStep).quot * hStep);
        }
    }

    if (m_resizeDirectionFlags & ResizeBottom)
    {
        if ((event->scenePos().y() > (mapToScene(0, 0).y() + height())) ||
            (event->scenePos().y() > (mapToScene(0, 0).y() +   MINIMUM_ITEM_HEIGHT))
           )
        {
            setHeight(div(event->scenePos().y() - mapToScene(0, 0).y(), vStep).quot * vStep);
        }
    }

    // 拖动控件位置
    if ((m_resizeDirectionFlags == Fixed) && !m_fixedPos && event->buttons() & Qt::LeftButton)
    {
        QPointF tmpPos = pos();
        QPointF delta;
        switch (possibleMoveDirectionFlags())
        {
        case LeftRight:
        {
            delta = QPoint(div(event->buttonDownScenePos(Qt::LeftButton).x() - event->scenePos().x(), hStep).quot * hStep, 0);
            break;
        }
        case TopBotom:
        {
            delta = QPoint(0, div(event->buttonDownScenePos(Qt::LeftButton).y() - event->scenePos().y(), vStep).quot * vStep);
            break;
        }
        case All:
        {
            // delta为鼠标按下时的坐标 - 鼠标移动坐标之间的距离
            delta = QPoint(div(event->buttonDownScenePos(Qt::LeftButton).x() - event->scenePos().x(), hStep).quot * hStep,
                           div(event->buttonDownScenePos(Qt::LeftButton).y() - event->scenePos().y(), vStep).quot * vStep);
            break;
        }
        };

        // setpos使用的是item在父坐标系中的位置->m_startPos,但delta是一样的
        setItemPos(QPointF(div(m_startPos.x(), hStep).quot * hStep, div(m_startPos.y(), vStep).quot * vStep) - delta);

        if (page())
        {
            if (!isBand() && page()->selectedItems().count() > 1)
            {
                moveSelectedItems(tmpPos - pos());
            }
            if (page()->selectedItems().count() == 1 && (page()->magneticMovement()))
            {
                page()->itemMoved(this);
            }

        }
    }
}

int base_design_item::possibleResizeDirectionFlags() const
{
    return m_possibleResizeDirectionFlags;
}

int base_design_item::resizeHandleSize() const
{
    return m_resizeHandleSize;
}

int base_design_item::getResizeDirectionFlags()
{
    return m_resizeDirectionFlags;
}

int base_design_item::resizeDirectionFlags(QPointF position)
{
    int flags = Fixed;
    if (isGeometryLocked())
    {
        return flags;
    }

    if (/*possibleResizeDirectionFlags()&ResizeTop &&*/ m_topRect.contains(position))
    {
        flags |= ResizeTop;
    }

    if (/*possibleResizeDirectionFlags()&ResizeLeft &&*/ m_leftRect.contains(position))
    {
        flags |= ResizeLeft;
    }

    if (/*possibleResizeDirectionFlags()&ResizeBottom &&*/ m_bottomRect.contains(position))
    {
        flags |= ResizeBottom;
    }

    if (/*possibleResizeDirectionFlags()&ResizeRight &&*/ m_rightRect.contains(position))
    {
        flags |= ResizeRight;
    }

    return flags;
}

Qt::CursorShape base_design_item::getPossibleCursor(int cursorFlags)
{

    if ((cursorFlags == Fixed) || (scene()->selectedItems().count() > 1))
    {
        return Qt::ArrowCursor;
    }

    if (((cursorFlags & ResizeRight) && (cursorFlags & ResizeTop)) ||
        ((cursorFlags & ResizeLeft) && (cursorFlags & ResizeBottom)))
    {
        return Qt::SizeBDiagCursor;
    }
    if (((cursorFlags & ResizeLeft) && (cursorFlags & ResizeTop)) ||
        ((cursorFlags & ResizeRight) && (cursorFlags & ResizeBottom)))
    {
        return Qt::SizeFDiagCursor;
    }
    if ((cursorFlags & ResizeLeft) || (cursorFlags & ResizeRight))
    {
        return Qt::SizeHorCursor;
    }
    if ((cursorFlags & ResizeBottom) || (cursorFlags & ResizeTop))
    {
        return Qt::SizeVerCursor;
    }

    return Qt::ArrowCursor;
}

void base_design_item::setZValueProperty(qreal value)
{
    if (zValue() != value)
    {
        qreal oldValue = zValue();
        setZValue(value);
        notify("zOrder", oldValue, value);
    }
}

base_design_item::ItemAlign base_design_item::itemAlign() const
{
    return m_itemAlign;
}

QPointF base_design_item::modifyPosForAlignedItem(const QPointF &pos)
{
    QPointF result = pos;
    base_design_item *parent = dynamic_cast<base_design_item *>(parentItem());
    page_item *parentPage = dynamic_cast<page_item *>(parentItem());
    if (parent)
    {
        qreal leftBorder = parentPage ? parentPage->leftMargin() *   mmFACTOR : 0;
        qreal rightBorder = parentPage ? parentPage->rightMargin() *   mmFACTOR : 0;
        qreal avaibleSpace = parent->width() - (leftBorder + rightBorder);

        switch (m_itemAlign)
        {
        case LeftItemAlign:
            result.setX(leftBorder);
            break;
        case RightItemAlign:
            result.setX(parent->width() - (rightBorder + width()));
            break;
        case CenterItemAlign:
            result.setX((avaibleSpace - width()) / 2 + leftBorder);
            break;
        case ParentWidthItemAlign:
            result.setX(leftBorder);
        case DesignedItemAlign:
            break;
        }
    }
    return result;
}

void base_design_item::turnOnJoinMarker(bool value)
{
    m_joinMarkerOn = value;
    if (value)
    {
        m_joinMarker = new Marker(this, this);
        m_joinMarker->setColor(JOIN_COLOR);
        m_joinMarker->setRect(rect());
        m_joinMarker->setVisible(true);
    }
    else
    {
        delete m_joinMarker;
        m_joinMarker = 0;
    }
}

void base_design_item::updateItemAlign()
{
    base_design_item *parent = dynamic_cast<base_design_item *>(parentItem());
    page_item *parentPage = dynamic_cast<page_item *>(parentItem());
    m_changingItemAlign = true;
    if (parent)
    {
        qreal leftBorder = parentPage ? parentPage->leftMargin() *   mmFACTOR : 0;
        qreal rightBorder = parentPage ? parentPage->rightMargin() *   mmFACTOR : 0;
        qreal aviableSpace = parent->width() - (leftBorder + rightBorder);
        setPos(modifyPosForAlignedItem(pos()));
        if (m_itemAlign == ParentWidthItemAlign)
        {
            setWidth(aviableSpace);
        }
    }
    m_changingItemAlign = false;
}

void base_design_item::updatePossibleDirectionFlags()
{
    setPossibleResizeDirectionFlags(AllDirections);
    switch (m_itemAlign)
    {
    case LeftItemAlign:
        setPossibleResizeDirectionFlags(AllDirections ^ ResizeLeft);
        break;
    case RightItemAlign:
        setPossibleResizeDirectionFlags(AllDirections ^ ResizeRight);
        break;
    case ParentWidthItemAlign:
        setPossibleResizeDirectionFlags(ResizeBottom | ResizeTop);
    case CenterItemAlign:
    case DesignedItemAlign:
        break;
    }
}

bool base_design_item::isChangingPos() const
{
    return m_isChangingPos;
}

void base_design_item::setIsChangingPos(bool isChangingPos)
{
    m_isChangingPos = isChangingPos;
}

bool base_design_item::isShapeItem() const
{
    return QString(metaObject()->className()) == "ShapeItem";
}

bool base_design_item::hasShadow()
{
    return m_shadow;
}

void base_design_item::setShadow(bool sh)
{
    if (m_shadow != sh)
    {
        bool oldValue = m_shadow;
        m_shadow = sh;
        notify("shadow", oldValue, m_shadow);
        update();
    }
}

bool base_design_item::isGeometryLocked() const
{
    return m_itemGeometryLocked;
}

void base_design_item::setGeometryLocked(bool itemLocked)
{
    if (m_itemGeometryLocked != itemLocked)
    {
        m_itemGeometryLocked = itemLocked;
        if (itemLocked)
        {
            m_savedPossibleMoveDirectionFlags = m_possibleMoveDirectionFlags;
            m_savedPossibleResizeDirectionFlags = m_possibleResizeDirectionFlags;
            m_possibleMoveDirectionFlags = None;
            m_possibleResizeDirectionFlags = Fixed;
            m_savedFixedPos = m_fixedPos;
            m_fixedPos = true;
        }
        else
        {
            m_possibleMoveDirectionFlags = m_savedPossibleMoveDirectionFlags;
            m_possibleResizeDirectionFlags = m_savedPossibleResizeDirectionFlags;
            m_fixedPos = m_savedFixedPos;
        }
        if (!isLoading())
        {
            update();
            if (m_selectionMarker)
            {
                m_selectionMarker->update();
            }
            notify("geometryLocked", !itemLocked, itemLocked);
        }
    }
}

bool base_design_item::fillTransparentInDesignMode() const
{
    return m_fillTransparentInDesignMode;
}

void base_design_item::setFillTransparentInDesignMode(bool fillTransparentInDesignMode)
{
    m_fillTransparentInDesignMode = fillTransparentInDesignMode;
}

void base_design_item::emitPosChanged(QPointF oldPos, QPointF newPos)
{
    emit posChanged(this, oldPos, newPos);
}

bool base_design_item::fillInSecondPass() const
{
    return m_fillInSecondPass;
}

void base_design_item::setFillInSecondPass(bool fillInSecondPass)
{

    if (m_fillInSecondPass != fillInSecondPass)
    {
        m_fillInSecondPass = fillInSecondPass;
        notify("fillInSecondPass", !fillInSecondPass, fillInSecondPass);
    }

}

bool base_design_item::isWatermark() const
{
    return m_watermark;
}

void base_design_item::setWatermark(bool watermark)
{
    if (m_watermark != watermark)
    {
        m_watermark = watermark;
        notify("watermark", !watermark, watermark);
    }
}

void base_design_item::updateSelectionMarker()
{
    if (m_selectionMarker && (itemMode() & DesignMode || itemMode() & EditMode))
    {
        if ((!m_selectionMarker->scene()) && scene())
        {
            scene()->addItem(m_selectionMarker);
        }
        if (parentItem())
        {
            m_selectionMarker->setRect(rect());
            m_selectionMarker->setPos(0, 0);
        }
    }
}

void base_design_item::turnOnSelectionMarker(bool value)
{
    if (value && !m_selectionMarker)
    {
        m_selectionMarker = new SelectionMarker(this, this);
        m_selectionMarker->setColor(selectionMarkerColor());
        updateSelectionMarker();
        m_selectionMarker->setVisible(true);
    }
    else
    {
        delete m_selectionMarker;
        m_selectionMarker = 0;
    }
}

QString base_design_item::patternName() const
{
    return (m_patternName.isEmpty()) ? objectName() : m_patternName;
}

void base_design_item::setPatternName(const QString &patternName)
{
    m_patternName = patternName;
}

base_design_item *base_design_item::patternItem() const
{
    return m_patternItem;
}

void base_design_item::setPatternItem(base_design_item *patternItem)
{
    m_patternItem = patternItem;
}

ReportSettings *base_design_item::reportSettings() const
{
    return m_reportSettings;
}

void base_design_item::setReportSettings(ReportSettings *reportSettings)
{
    m_reportSettings = reportSettings;
    foreach (base_design_item *child, childBaseItems())
    {
        child->setReportSettings(reportSettings);
    }
}

QColor base_design_item::borderColor() const
{
    return m_borderColor;
}

void base_design_item::setBorderColor(const QColor &borderColor)
{
    if (m_borderColor != borderColor)
    {
        QColor oldValue = m_borderColor;
        m_borderColor = borderColor;
        notify("borderColor", oldValue, borderColor);
        update();
    }
}

void base_design_item::setItemVisible(const bool &value)
{
    if (isVisible() != value)
    {
        setVisible(value);
        emit itemVisibleHasChanged(this);
    }
}

void base_design_item::setItemAlign(const ItemAlign &itemAlign)
{
    if (m_itemAlign != itemAlign)
    {
        ItemAlign oldValue = m_itemAlign;
        m_itemAlign = itemAlign;
        notify("itemAlign", oldValue, itemAlign);
        updatePossibleDirectionFlags();
        updateItemAlign();
        emit itemAlignChanged(this, oldValue, itemAlign);
    }
}

QString base_design_item::itemTypeName() const
{
    return m_itemTypeName;
}

void base_design_item::setItemTypeName(const QString &itemTypeName)
{
    m_itemTypeName = itemTypeName;
}

void base_design_item::emitObjectNamePropertyChanged(const QString &oldName, const QString &newName)
{
    emit propertyObjectNameChanged(oldName, newName);
}

qreal base_design_item::borderLineSize() const
{
    return m_borderLineSize;
}

void base_design_item::setBorderLineSize(qreal value)
{
    qreal oldValue = m_borderLineSize;
    m_borderLineSize = value;
    update();
    notify("borderLineSize", oldValue, value);
}


void base_design_item::moveRight()
{
    if (!m_fixedPos && page())
    {
        setItemPos(pos().x() + page()->horizontalGridStep(), pos().y());
    }
}

void base_design_item::moveLeft()
{
    if (!m_fixedPos && page())
    {
        setItemPos(pos().x() - page()->horizontalGridStep(), pos().y());
    }
}

void base_design_item::moveDown()
{
    if (!m_fixedPos && page())
    {
        setItemPos(pos().x(), pos().y() + page()->verticalGridStep());
    }
}

void base_design_item::moveUp()
{
    if (!m_fixedPos && page())
    {
        setItemPos(pos().x(), pos().y() - page()->verticalGridStep());
    }
}

void base_design_item::sizeRight()
{
    if ((m_possibleResizeDirectionFlags & ResizeLeft) ||
        (m_possibleResizeDirectionFlags & ResizeRight))
    {
        if (page())
        {
            setWidth(width() + page()->horizontalGridStep());
        }
    }
}

void base_design_item::sizeLeft()
{
    if ((m_possibleResizeDirectionFlags & ResizeLeft) ||
        (m_possibleResizeDirectionFlags & ResizeRight))
    {
        if (page())
        {
            setWidth(width() - page()->horizontalGridStep());
        }
    }
}

void base_design_item::sizeUp()
{
    if ((m_possibleResizeDirectionFlags & ResizeTop) ||
        (m_possibleResizeDirectionFlags & ResizeBottom))
    {
        if (page())
        {
            setHeight(height() - page()->verticalGridStep());
        }
    }
}

void base_design_item::sizeDown()
{
    if ((m_possibleResizeDirectionFlags & ResizeTop) ||
        (m_possibleResizeDirectionFlags & ResizeBottom))
    {
        if (page())
        {
            setHeight(height() + page()->verticalGridStep());
        }
    }
}

void base_design_item::setBorderLinesFlags(BorderLines flags)
{
    if (m_borderLinesFlags != flags)
    {
        BorderLines oldValue = m_borderLinesFlags;
        m_borderLinesFlags = flags;
        if (!isLoading())
        {
            update(rect());
            notify("borders", QVariant(oldValue), QVariant(flags));
        }
    }
}

base_design_item::BorderLines base_design_item::borderLines() const
{
    return m_borderLinesFlags;
}

QString base_design_item::storageTypeName() const
{
    return m_storageTypeName;
}

void base_design_item::drawHoverRect(QPainter *painter)
{
    if (m_hovered && !isSelected())
    {
        painter->save();
        QPen pen;
        pen.setWidth(HOVER_RECT_LINE_WIDTH);
        pen.setColor(HOVER_RECT_COLOR);
        painter->setPen(pen);
        painter->drawRect(rect());
        painter->restore();
    }
}

void base_design_item::drawTopLine(QPainter *painter, QRectF rect) const
{
    if (isShapeItem())
    {
        return;
    }

    painter->setPen(borderPen(TopLine));
    painter->drawLine(rect.x(), rect.y(), rect.width(), rect.y());
}

void base_design_item::drawBootomLine(QPainter *painter, QRectF rect) const
{
    if (isShapeItem())
    {
        return;
    }

    painter->setPen(borderPen(BottomLine));
    painter->drawLine(rect.x(), rect.height(), rect.width(), rect.height());
}

void base_design_item::drawRightLine(QPainter *painter, QRectF rect) const
{
    if (isShapeItem())
    {
        return;
    }

    painter->setPen(borderPen(RightLine));
    painter->drawLine(rect.width(), rect.y(), rect.width(), rect.height());
}

void base_design_item::drawLeftLine(QPainter *painter, QRectF rect) const
{
    if (isShapeItem())
    {
        return;
    }

    painter->setPen(borderPen(LeftLine));
    painter->drawLine(rect.x(), rect.y(), rect.x(), rect.height());
}

void base_design_item::drawDesignModeBorder(QPainter *painter, QRectF rect) const
{
    if (isShapeItem())
    {
        return;
    }
    drawTopLine(painter, rect);
    drawBootomLine(painter, rect);
    drawLeftLine(painter, rect);
    drawRightLine(painter, rect);
}

void base_design_item::drawRenderModeBorder(QPainter *painter, QRectF rect) const
{
    if (isShapeItem())
    {
        return;
    }
    if (m_borderLinesFlags & RightLine)
    {
        drawRightLine(painter, rect);
    }
    if (m_borderLinesFlags & LeftLine)
    {
        drawLeftLine(painter, rect);
    }
    if (m_borderLinesFlags & TopLine)
    {
        drawTopLine(painter, rect);
    }
    if (m_borderLinesFlags & BottomLine)
    {
        drawBootomLine(painter, rect);
    }
}

void base_design_item::drawBorder(QPainter *painter, QRectF rect) const
{
    painter->save();
    // drawDesignBorders控件边框辅助线
    if (itemMode() & DesignMode && drawDesignBorders())
    {
        drawDesignModeBorder(painter, rect);
    }
    else
    {
        drawRenderModeBorder(painter, rect);
    }
    painter->restore();
}

void base_design_item::drawShadow(QPainter *painter, QRectF rect, qreal shadowSize) const
{
    qreal shWidth = shadowSize;
    QRectF rshadow(rect.topRight(),
                   rect.bottomRight() + QPointF(shWidth, 0));
    QLinearGradient rgrad(rshadow.topLeft(), rshadow.topRight());
    rgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    rgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(rshadow, QBrush(rgrad));
    QRectF bshadow(rect.bottomLeft(),
                   rect.bottomRight() + QPointF(0, shWidth));
    QLinearGradient bgrad(bshadow.topLeft(), bshadow.bottomLeft());
    bgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    bgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(bshadow, QBrush(bgrad));
    QRectF cshadow(rect.bottomRight(),
                   rect.bottomRight() + QPointF(shWidth, shWidth));
    QRadialGradient cgrad(cshadow.topLeft(), shWidth, cshadow.topLeft());
    cgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    cgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(cshadow, QBrush(cgrad));

    QRectF lshadow(rect.topLeft() - QPointF(shWidth, 0),
                   rect.bottomLeft());
    QLinearGradient lgrad(lshadow.topRight(), lshadow.topLeft());
    lgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    lgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(lshadow, QBrush(lgrad));
    QRectF tshadow(rect.topLeft() - QPointF(0, shWidth),
                   rect.topRight());
    QLinearGradient tgrad(tshadow.bottomLeft(), tshadow.topLeft());
    tgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    tgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(tshadow, QBrush(tgrad));

    QRectF ltshadow(rect.topLeft() - QPointF(shWidth, shWidth),
                    rect.topLeft());
    QRadialGradient ltgrad(ltshadow.bottomRight(), shWidth, ltshadow.bottomRight());
    ltgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    ltgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(ltshadow, QBrush(ltgrad));

    QRectF rtshadow(rect.topRight() - QPointF(0, shWidth),
                    rect.topRight() + QPointF(shWidth, 0));
    QRadialGradient rtgrad(rtshadow.bottomLeft(), shWidth, rtshadow.bottomLeft());
    rtgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    rtgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(rtshadow, QBrush(rtgrad));

    QRectF lbshadow(rect.bottomLeft() - QPointF(shWidth, 0),
                    rect.bottomLeft() + QPointF(0, shWidth));
    QRadialGradient lbgrad(lbshadow.topRight(), shWidth, lbshadow.topRight());
    lbgrad.setColorAt(0.0, QColor(200, 200, 200, 255));
    lbgrad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter->fillRect(lbshadow, QBrush(lbgrad));
}

void base_design_item::setGeometry(QRectF rect)
{
    if (m_rect == rect)
    {
        return;
    }
    m_oldGeometry = m_rect;
    if (!isLoading())
    {
        prepareGeometryChange();
    }
    m_rect = rect;
    m_topRect = QRectF(0 - resizeHandleSize(), 0 - resizeHandleSize(), width() + resizeHandleSize() * 2, resizeHandleSize() * 2);
    m_bottomRect = QRectF(0 - resizeHandleSize(), height() - resizeHandleSize(),  width() + resizeHandleSize() * 2, resizeHandleSize() * 2);
    m_leftRect = QRectF(0 - resizeHandleSize(), 0 - resizeHandleSize(), resizeHandleSize() * 2, height() + resizeHandleSize() * 2);
    m_rightRect = QRectF(width() - resizeHandleSize(), 0 - resizeHandleSize(), resizeHandleSize() * 2, height() + resizeHandleSize() * 2);

    updateSelectionMarker();
    if (!isLoading())
    {
        geometryChangedEvent(geometry(), m_oldGeometry);
        emit geometryChanged(this, geometry(), m_oldGeometry);
    }
}

void base_design_item::geometryChangedEvent(QRectF newRect, QRectF oldRect)
{
    Q_UNUSED(oldRect);
    Q_UNUSED(newRect);
}

void base_design_item::beforeDelete()
{

}

void base_design_item::setGeometryProperty(QRect rect)
{
    if (rect != m_itemGeometry)
    {
        QRectF oldValue = geometry();
        if ((rect.x() != geometry().x())
            || (rect.y() != geometry().y()))
        {
            setPos(rect.x(), rect.y());
        }
        if (rect.width() != geometry().width())
        {
            setWidth(rect.width());
        }
        if (rect.height() != geometry().height())
        {
            setHeight(rect.height());
        }
        if (!isLoading())
        {
            notify("geometry", oldValue, rect);
        }
    }
}

page_scene *base_design_item::page()
{
    return dynamic_cast<page_scene *>(scene());
}

void base_design_item::setPossibleResizeDirectionFlags(int directionsFlags)
{
    m_possibleResizeDirectionFlags = directionsFlags;
}

QPen base_design_item::borderPen(BorderSide side) const
{
    QPen pen;
    if (m_borderLinesFlags & side)
    {
        pen.setColor(m_borderColor);
        pen.setStyle(Qt::SolidLine);
        pen.setWidthF(m_borderLineSize + 1); //To draw with point precision (By default: 2px = 1 pt)

    }
    else
    {
        pen.setColor(Qt::darkGray);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(1);
    }
    return pen;
}

QColor base_design_item::selectionColor() const
{
    return  SELECTION_COLOR;
}

void base_design_item::initFlags()
{
    if ((itemMode()&DesignMode) || (itemMode()&EditMode))
    {
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);

        // 接受鼠标悬停事件
        setAcceptHoverEvents(true);
    }
    else
    {
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        setAcceptHoverEvents(false);
    }
}

void base_design_item::initMode(ItemMode mode)
{
    Q_UNUSED(mode);
}

QVariant base_design_item::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        updateSelectionMarker();
        emit geometryChanged(this, geometry(), geometry());
    }

    if (change == QGraphicsItem::ItemSelectedChange)
    {
        turnOnSelectionMarker(value.toBool());
        emit itemSelectedHasBeenChanged(this, value.toBool());
    }

    if (change == QGraphicsItem::ItemParentHasChanged)
    {
        parentChangedEvent(dynamic_cast<base_design_item *>(value.value<QGraphicsItem *>()));
    }

    return QGraphicsItem::itemChange(change, value);
}

void base_design_item::childAddedEvent(base_design_item *child)
{
    Q_UNUSED(child)
}

void base_design_item::parentChangedEvent(base_design_item *)
{

}

void base_design_item::restoreLinks()
{
#ifdef HAVE_QT5
    foreach (QObject *child, children())
    {
#else
    foreach (QObject *child, QObject::children())
    {
#endif
        base_design_item *childItem = dynamic_cast<base_design_item *>(child);
        if (childItem)
        {
            childItem->restoreLinks();
        }
    }
    restoreLinksEvent();
}

QPainterPath base_design_item::shape() const
{
    QPainterPath path;
    path.addRect(this->boundingRect());
    return path;
}

void base_design_item::moveSelectedItems(QPointF delta)
{
    QList<QGraphicsItem *> selectedItems;
    selectedItems = scene()->selectedItems();
    base_design_item *selectedItem;
    foreach (QGraphicsItem *item, selectedItems)
    {
        if (item != this)
        {
            selectedItem = dynamic_cast<base_design_item *>(item);
            if (selectedItem && !selectedItem->isBand())
            {
                if (!selectedItem->m_fixedPos)
                {
                    selectedItem->setItemPos(selectedItem->pos() - delta);
                }
            }
        }
    }
}

void base_design_item::setItemPos(qreal x, qreal y)
{
    setItemPos(QPointF(x, y));
}

void base_design_item::setItemMode(ItemMode mode)
{
    m_itemMode = mode;
    foreach (QGraphicsItem *item, childItems())
    {
        base_design_item *ri = dynamic_cast<base_design_item *>(item);
        if (ri)
        {
            ri->setItemMode(mode);
        }
    }
    initMode(mode);
    initFlags();
}
void base_design_item::setItemPos(const QPointF &newPos)
{
    QPointF oldPos = pos();
    QPointF finalPos = modifyPosForAlignedItem(newPos);
    QGraphicsItem::setPos(finalPos);
    emit posChanging(this, finalPos, oldPos);
}


QWidget *base_design_item::findRootWidget(QWidget *widget)
{
    while (widget->parentWidget())
    {
        widget = widget->parentWidget();
    }
    return widget;
}

void base_design_item::showDialog(QWidget *widget)
{
    if (!widget)
    {
        return;
    }

    QDialog *dialog = qobject_cast<QDialog *>(widget);
    if (dialog)
    {
        dialog->setAttribute(Qt::WA_DeleteOnClose);
#ifdef Q_OS_MAC
        dialog->setWindowModality(Qt::WindowModal);
#else
        dialog->setWindowModality(Qt::ApplicationModal);
#endif
        dialog->setWindowTitle(widget->windowTitle());
        dialog->exec();
    }

}

void base_design_item::showEditorDialog()
{
    showDialog(defaultEditor());
    itemSynchronization();
}

void base_design_item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
        ((itemMode()&EditMode) || (itemMode()&DesignMode)))
    {
        showEditorDialog();
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void base_design_item::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!(flags() & QGraphicsItem::ItemIsSelectable))
    {
        return;
    }

    page_scene *page = dynamic_cast<page_scene *>(scene());
    if (!page->selectedItems().contains(this))
    {
        page->clearSelection();
        this->setSelected(true);
    }
    QMenu menu(event->widget());

    QAction *lockGeometryAction = menu.addAction(tr("Lock item geometry"));
    lockGeometryAction->setCheckable(true);
    lockGeometryAction->setChecked(isGeometryLocked());
    //menu.addSeparator();

    QAction *pageHeader = menu.addAction(tr("Page header"));
    pageHeader->setCheckable(true);
    pageHeader->setChecked(masterMask() == PAGE_TOP_ID);

    QAction *pageFooter = menu.addAction(tr("Page footer"));
    pageFooter->setCheckable(true);
    pageFooter->setChecked(masterMask() == PAGE_BOTTOM_ID);

    // 编组
    QAction *groupAttribute = nullptr;
    QAction *ungroup = menu.addAction(tr("ungroup"));
    QAction *group = menu.addAction(tr("group"));
    if (isGroupItem())
    {
        // 组属性
        groupAttribute = menu.addAction(tr("group attribute"));
        groupAttribute->setCheckable(true);
    }
    QAction *copyAction = menu.addAction(QIcon(":/copy"), tr("Copy"));
    QAction *cutAction = menu.addAction(QIcon(":/cut"), tr("Cut"));
    QAction *pasteAction = menu.addAction(QIcon(":/paste"), tr("Paste"));
    pasteAction->setEnabled(false);

#if QT_VERSION >=QT_VERSION_CHECK(5,0,0)
    // lockGeometryAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    cutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_X));
    pasteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    //    if(group)
    //        group->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    //    if(ungroup)
    //        ungroup->setShortcut(QKeySequence(Qt::CTRL |  Qt::SHIFT |Qt::Key_G));
#else
    // lockGeometryAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    cutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    //    if(group)
    //        group->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    //    if(ungroup)
    //        ungroup->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
#endif

    QClipboard *clipboard = QApplication::clipboard();
    ItemsReaderIntf::Ptr reader = StringXMLreader::create(clipboard->text());
    if (reader->first() && reader->itemType() == "Object")
    {
        pasteAction->setEnabled(true);
    }
    menu.addSeparator();
    preparePopUpMenu(menu);

    //    // 获取菜单项的字体
    //    QFont font = menu.font();
    //    // 获取菜单项中最长文本的宽度
    //    QFontMetrics fontMetrics(font);
    //    int maxWidth = 0;
    //    for(QAction* act : menu.actions())
    //    {
    //       maxWidth = qMax(maxWidth, fontMetrics.width(act->text()));
    //    }
    //menu.setFixedWidth(200);

    QAction *a = menu.exec(event->screenPos());
    if (a)
    {
        if (a == cutAction)
        {
            page->cut();
            return;
        }
        else if (a == copyAction)
        {
            page->copy();
        }
        else if (a == pasteAction)
        {
            page->paste();
        }
        else if (a == lockGeometryAction)
        {
            if (a->isChecked())
            {
                page->lockSelectedItems();
            }
            else
            {
                page->unlockSelectedItems();
            }
        }
        else if (a == pageHeader)
        {
            if (a->isChecked())
            {
                page->setPageHeader();
            }
            else
            {
                page->unheader();
            }
        }
        else if (a == pageFooter)
        {
            if (a->isChecked())
            {
                page->setPageFooter();
            }
            else
            {
                page->unfooter();
            }
        }
        else if (a == group)
        {
            bool ret = page->groupSelectItems(genGroupID());
            if (ret)
            {
                page->setGroupVariant();
            }
        }
        else if (a == ungroup)
        {
            page->ungroupSelectItems();
        }
        else if (a == groupAttribute)
        {
            page->setGroupVariant();
        }
        processPopUpAction(a);
    }
}

int base_design_item::possibleMoveDirectionFlags() const
{
    return m_possibleMoveDirectionFlags;
}

void base_design_item::setPossibleMoveFlags(int directionsFlags)
{
    m_possibleMoveDirectionFlags = directionsFlags;
}

int base_design_item::marginSize() const
{
    return m_margin;
}

void base_design_item::setMarginSize(int value)
{
    if (m_margin != value)
    {
        int oldValue = m_margin;
        m_margin = value;
        if (!isLoading())
        {
            update(rect());
            notify("margin", oldValue, value);
        }
    }
}

void base_design_item::drawResizeZone(QPainter *painter)
{
    painter->save();
    painter->setPen(QPen(RESIZE_ZONE_COLOR));
    (isSelected()) ? painter->setOpacity(SELECTED_RESIZE_ZONE_OPACITY) : painter->setOpacity(RESIZE_ZONE_OPACITY);
    painter->setBrush(QBrush(Qt::green, Qt::SolidPattern));
    DrawResizeRects(painter);
    painter->restore();
}

void base_design_item::DrawResizeRects(QPainter *painter)
{
    if (m_resizeDirectionFlags & ResizeBottom)
    {
        painter->drawRect(m_bottomRect);
    }

    if (m_resizeDirectionFlags & ResizeTop)
    {
        painter->drawRect(m_topRect);
    }

    if (m_resizeDirectionFlags & ResizeLeft)
    {
        painter->drawRect(m_leftRect);
    }

    if (m_resizeDirectionFlags & ResizeRight)
    {
        painter->drawRect(m_rightRect);
    }
}

void base_design_item::invalidateRect(const QRectF &rect)
{
    if (scene())
    {
        scene()->update(mapToScene(rect).boundingRect());
    }
}

//ReportEnginePrivate *base_design_item::reportEditor()
//{
//    page_scene *page = dynamic_cast<page_scene *>(scene());
//    if (page) return page->reportEditor();
//    else return 0;
//}

void base_design_item::updateItemSize(RenderPass pass, int maxHeight)
{
    Q_UNUSED(maxHeight);
    m_currentPass = pass;
    updateItemAlign();
}

bool base_design_item::isNeedUpdateSize(RenderPass /*pass*/) const
{
    return false;
}

QObject *base_design_item::createElement(const QString & /*collectionName*/, const QString &elementType)
{
    base_design_item *obj = 0;
    try
    {
        if (DesignElementsFactory::instance().objectCreator(elementType))
        {
            obj = DesignElementsFactory::instance().objectCreator(elementType)(this, this);
            if (page())
            {
                page()->registerItem(obj);
            }
        }
    }
    catch (ReportError &error)
    {
        qDebug() << error.what();
    }
    return obj;
}

int base_design_item::elementsCount(const QString & /*collectionName*/)
{
    return QObject::children().count();
}

QObject *base_design_item::elementAt(const QString & /*collectionName*/, int index)
{
    return QObject::children().at(index);
}

void base_design_item::collectionLoadFinished(const QString &collectionName)
{
    if (collectionName.compare("children", Qt::CaseInsensitive) == 0)
    {
        if (page())
        {
#ifdef HAVE_QT5
            foreach (QObject *obj, children())
            {
#else
            foreach (QObject *obj, QObject::children())
            {
#endif
                base_design_item *item = dynamic_cast<base_design_item *>(obj);
                if (item && page())
                {
                    page()->registerItem(item);
                }
            }
        }
    }
    emit loadCollectionFinished(collectionName);
}

base_design_item *base_design_item::cloneItem(ItemMode mode, QObject *owner, QGraphicsItem *parent)
{
    base_design_item *clone = cloneItemWOChild(mode, owner, parent);
    clone->setPatternName(this->objectName());
    clone->setPatternItem(this);
#ifdef HAVE_QT5
    foreach (QObject *child, children())
    {
#else
    foreach (QObject *child, QObject::children())
    {
#endif
        base_design_item *childItem = dynamic_cast<base_design_item *>(child);
        if (childItem)
        {
            clone->childAddedEvent(childItem->cloneItem(mode, clone, clone));
        }
    }
    return clone;
}

base_design_item *base_design_item::cloneItemWOChild(ItemMode mode, QObject *owner, QGraphicsItem *parent)
{
    base_design_item *clone = createSameTypeItem(owner, parent);
    clone->setObjectName(this->objectName());
    clone->setItemMode(mode);
    clone->objectLoadStarted();
    clone->setReportSettings(this->reportSettings());
    for (int i = 0; i < clone->metaObject()->propertyCount(); i++)
    {
        if (clone->metaObject()->property(i).isWritable())
        {
            clone->setProperty(clone->metaObject()->property(i).name(), property(clone->metaObject()->property(i).name()));
        }
    }
    clone->objectLoadFinished();
    return clone;
}

void base_design_item::initFromItem(base_design_item *source)
{
    objectLoadStarted();
    for (int i = 0; i < metaObject()->propertyCount(); i++)
    {
        if (strcmp(metaObject()->property(i).name(), "objectName") != 0)
            if (source->property(metaObject()->property(i).name()).isValid())
            {
                if (metaObject()->property(i).isWritable())
                {
                    setProperty(metaObject()->property(i).name(), source->property(metaObject()->property(i).name()));
                }
            }
    }
    objectLoadFinished();
}

bool base_design_item::canBeSplitted(int height) const
{
    Q_UNUSED(height);
    return false;
}

bool base_design_item::isEmpty() const
{
    return false;
}

base_design_item *base_design_item::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    Q_UNUSED(height);
    Q_UNUSED(owner);
    Q_UNUSED(parent);
    return 0;
}

base_design_item *base_design_item::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    Q_UNUSED(height);
    Q_UNUSED(owner);
    Q_UNUSED(parent);
    return 0;
}

base_design_item *base_design_item::cloneEmpty(int height, QObject *owner, QGraphicsItem *parent)
{
    Q_UNUSED(height);
    Q_UNUSED(owner);
    Q_UNUSED(parent);
    return 0;
}


void base_design_item::objectLoadStarted()
{
    m_objectState = ObjectLoading;
}

void base_design_item::objectLoadFinished()
{
    m_objectState = ObjectLoaded;
    emit objectLoaded(this);
}

void base_design_item::parentObjectLoadFinished()
{

}

QList<base_design_item *> base_design_item::childBaseItems() const
{
    QList<base_design_item *> resList;
    foreach (QGraphicsItem *item, childItems())
    {
        base_design_item *baseItem = dynamic_cast<base_design_item *>(item);
        if (baseItem)
        {
            resList << baseItem;
        }
    }
    return resList;
}


void base_design_item::addChildItems(QList<base_design_item *> *list)
{
    foreach (base_design_item *item, childBaseItems())
    {
        list->append(item);
        item->addChildItems(list);
    }
}

qreal base_design_item::calcAbsolutePosY(qreal currentOffset, base_design_item *item)
{
    base_design_item *parent = dynamic_cast<base_design_item *>(item->parent());
    if (parent)
    {
        return calcAbsolutePosY(currentOffset + item->getItemPosY(), parent);
    }
    else
    {
        return currentOffset + item->getItemPosY();
    }
}

qreal base_design_item::calcAbsolutePosX(qreal currentOffset, base_design_item *item)
{
    base_design_item *parent = dynamic_cast<base_design_item *>(item->parent());
    if (parent)
    {
        return calcAbsolutePosX(currentOffset + item->getItemPosX(), parent);
    }
    else
    {
        return currentOffset + item->getItemPosX();
    }
}

QList<base_design_item *> base_design_item::allChildBaseItems()
{
    QList<base_design_item *> resList;
    addChildItems(&resList);
    return resList;
}

base_design_item *base_design_item::childByName(const QString &name)
{
    foreach (base_design_item *item, childBaseItems())
    {
        if (item->objectName().compare(name, Qt::CaseInsensitive) == 0)
        {
            return item;
        }
        else
        {
            base_design_item *child = item->childByName(name);
            if (child)
            {
                return child;
            }
        }
    }
    return 0;
}

QWidget *base_design_item::defaultEditor()
{
    return 0;
}

void base_design_item::notify(const QString &propertyName, const QVariant &oldValue, const QVariant &newValue)
{
    if (!isLoading())
    {
        emit propertyChanged(propertyName, oldValue, newValue);
    }
}

void base_design_item::notify(const QVector<QString> &propertyNames)
{
    if (!isLoading())
    {
        emit propertyesChanged(propertyNames);
    }
}


QMap<QString, QString> base_design_item::getStringForTranslation()
{
    return QMap<QString, QString>();
}

QRectF Marker::boundingRect() const
{
    return m_rect.adjusted(-15, -15, 15, 15);
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen;
    const int markerSize = 5;
    pen.setColor(color());
    pen.setWidth(SELECTION_LINE_WIDTH);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->setOpacity(SELECTION_COLOR_OPACITY);
    painter->drawRect(rect());
    painter->setBrush(color());
    painter->setPen(Qt::transparent);

    painter->setOpacity(1);

    painter->drawRect(QRectF(-markerSize, -markerSize, markerSize * 2, markerSize * 2));
    painter->drawRect(QRectF(rect().right() - markerSize, rect().bottom() - markerSize, markerSize * 2, markerSize * 2));
    painter->drawRect(QRectF(rect().right() - markerSize, rect().top() - markerSize, markerSize * 2, markerSize * 2));
    painter->drawRect(QRectF(rect().left() - markerSize, rect().bottom() - markerSize, markerSize * 2, markerSize * 2));
}

QColor Marker::color() const
{
    return m_color;
}

SelectionMarker::SelectionMarker(QGraphicsItem *parent, base_design_item *owner)
    : Marker(parent, owner)
{
    setAcceptHoverEvents(true);
}

QColor SelectionMarker::color() const
{
    return owner()->isGeometryLocked() ? Qt::darkGray : Marker::color();
}

void SelectionMarker::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (owner())
    {
        owner()->hoverMoveEvent(event);
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void SelectionMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (owner())
    {
        owner()->setSelected(true);
        owner()->mousePressEvent(event);
    }
    QGraphicsItem::mousePressEvent(event);
}

void SelectionMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (owner())
    {
        owner()->mouseReleaseEvent(event);
    }
}

void SelectionMarker::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (owner())
    {
        owner()->mouseDoubleClickEvent(event);
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void SelectionMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (owner())
    {
        owner()->mouseMoveEvent(event);
    }
}

void base_design_item::processPopUpAction(QAction *action)
{
    if (page())
    {
        if (action->text().compare(tr("Lock item geometry")) == 0)
        {
            page()->setPropertyToSelectedItems("geometryLocked", action->isChecked());
        }
    }
}

void base_design_item::itemSynchronization()
{
    if (isMasterMask())
    {
        emit synchronization();
    }
}

void base_design_item::itemDelete()
{
    if (isMasterMask())
    {
        emit deleteItem();
    }
}

int base_design_item::genGroupID()
{
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    // 返回时间戳作为ID，确保为正整数
    return static_cast<int>(timestamp);
}

void base_design_item::setGroupID(int groupID)
{
    if (groupID == -1)
    {
        generateGroupColor(m_groupID, true);
        if (!isMasterMask())
        {
            setMasterMarkColor(Qt::white);
        }
    }
    else
    {
        QColor color = generateGroupColor(groupID);
        if (color.isValid())
        {
            setMasterMarkColor(color);
        }
        else// 无可使用的颜色，组数量已经超过最大限制
        {

        }
    }
    m_groupID = groupID;
}


} //namespace ReportDesign
