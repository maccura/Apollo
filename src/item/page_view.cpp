#include "page_view.hpp"

namespace ReportDesign
{

page_view::page_view(QWidget *parent /*= NULL*/): preview_page_view(parent),
    m_horizontalRuller(0), m_verticalRuller(0)
{
    setBackgroundBrush(QBrush(VIEW_BG_COLOR));
    setViewportMargins(20, 20, 0, 0);
    setDragMode(QGraphicsView::RubberBandDrag);
}
page_view::page_view(QGraphicsScene *scene, QWidget *parent /*= NULL*/):
    preview_page_view(scene, parent),
    m_horizontalRuller(0), m_verticalRuller(0)
{
    setViewportMargins(20, 20, 0, 0);
    setDragMode(QGraphicsView::RubberBandDrag);
}

void page_view::setPageItem(page_item *pageItem)
{
    if (!pageItem)
    {
        return;
    }
    m_pageItem = pageItem;
    if (!m_horizontalRuller)
    {
        m_horizontalRuller = new Ruler(Ruler::Horizontal, this);
        m_horizontalRuller->setPage(pageItem);
    }
    if (!m_verticalRuller)
    {
        m_verticalRuller = new Ruler(Ruler::Vertical, this);
        m_verticalRuller->setPage(pageItem);
    }
}

bool page_view::viewportEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseMove:
        if (m_horizontalRuller && m_verticalRuller)
        {
            m_horizontalRuller->setMousePos(dynamic_cast<QMouseEvent *>(event)->pos());
            m_verticalRuller->setMousePos(dynamic_cast<QMouseEvent *>(event)->pos());
            m_horizontalRuller->update();
            m_verticalRuller->update();
        }
        break;
    //case QEvent::Resize:
    case QEvent::Paint:
        if (m_horizontalRuller && m_verticalRuller)
        {
            int x = mapFromScene(m_pageItem->boundingRect().x(), m_pageItem->boundingRect().y()).x();
            int y = mapFromScene(m_pageItem->boundingRect().x(), m_pageItem->boundingRect().y()).y();
            int width = mapFromScene(m_pageItem->boundingRect().bottomRight().x(), m_pageItem->boundingRect().bottomRight().y()).x();
            int height = mapFromScene(m_pageItem->boundingRect().bottomRight().x(), m_pageItem->boundingRect().bottomRight().y()).y();

            x = x < 0 ? 0 : x;
            y = y < 0 ? 0 : y;

            m_horizontalRuller->setGeometry(x + 20, 0, (width - x), 20);
            m_verticalRuller->setGeometry(0, y + 20, 20, (height - y));
            m_verticalRuller->update();
            m_horizontalRuller->update();

        }
        break;
    default:
        break;
    }

    return QGraphicsView::viewportEvent(event);
}

void Ruler::setPage(page_item *page)
{
    m_page = page;
}

void Ruler::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::SolidLine);
    painter.drawRect(event->rect());

    if (m_page)
    {
        qreal rulerWidth = m_page->geometry().width() / m_page->unitFactor();
        qreal rulerHeight = m_page->geometry().height() / m_page->unitFactor();

        QGraphicsView *view = qobject_cast<QGraphicsView *>(parent());

        int hStartPos = view->mapFromScene(0, 0).x();
        int vStartPos = view->mapFromScene(0, 0).y();

        QFont font = painter.font();
        font.setPointSize(7);
        painter.setFont(font);

        switch (m_type)
        {
        case Horizontal:
            painter.setPen(Qt::NoPen);

            if (isColorDark(palette().window().color()))
            {
                painter.setBrush(SELECTION_COLOR);
            }
            else
            {
                painter.setBrush(SELECTION_COLOR);
            }

            drawItemWithChildren(&painter, m_page);
            painter.setPen(palette().windowText().color());

            for (int i = 0; i < rulerWidth / 10; ++i)
            {
                int hs10 = view->mapFromScene(QPointF(m_page->geometry().topLeft().x() + i * 10 * m_page->unitFactor(), 0)).x();
                int hs5 = view->mapFromScene(QPointF(m_page->geometry().topLeft().x() + i * 10 * m_page->unitFactor() + 5 * m_page->unitFactor(), 0)).x();
                if (hs10 > 0)
                {
                    if (hStartPos > 0)
                    {
                        hs10 -= hStartPos;
                        hs5  -= hStartPos;
                    }
                    painter.drawLine(hs10, 15, hs10, 20);
                    painter.drawLine(hs5, 10, hs5, 20);
                    if (i > 0)
                        painter.drawText(QPoint(hs10 - (painter.fontMetrics().boundingRect(QString::number(i)).width() / 2), 12),
                                         QString::number(i));
                }
            }
            painter.setPen(palette().windowText().color());
            painter.drawLine(m_mousePos.x() - (hStartPos > 0 ? hStartPos : 0), 0,
                             m_mousePos.x() - (hStartPos > 0 ? hStartPos : 0), 20);
            break;
        case Vertical:
            painter.setPen(Qt::NoPen);

            if (isColorDark(palette().window().color()))
            {
                painter.setBrush(SELECTION_COLOR);
            }
            else
            {
                painter.setBrush(SELECTION_COLOR);
            }

            drawItemWithChildren(&painter, m_page);
            painter.setPen(palette().windowText().color());
            for (int i = 0; i < rulerHeight / 10; ++i)
            {
                int vs10 = view->mapFromScene(QPointF(0, m_page->geometry().topLeft().y() + i * 10 * m_page->unitFactor())).y();
                int vs5  = view->mapFromScene(QPointF(0, m_page->geometry().topLeft().y() + i * 10 * m_page->unitFactor() + 5 * m_page->unitFactor())).y();
                if (vs10 > 0)
                {
                    if (vStartPos > 0)
                    {
                        vs10 -= vStartPos;
                        vs5 -= vStartPos;
                    }
                    painter.drawLine(15, vs10, 20, vs10);
                    if (i > 0)
                        painter.drawText(QPoint((15 - painter.fontMetrics().boundingRect(QString::number(i)).width()) / 2,
                                                vs10 + (painter.fontMetrics().height() / 2)), QString::number(i));
                    painter.drawLine(10, vs5, 20, vs5);
                }
            }
            painter.setPen(palette().windowText().color());
            painter.drawLine(0, m_mousePos.y() - (vStartPos > 0 ? vStartPos : 0),
                             20, m_mousePos.y() - (vStartPos > 0 ? vStartPos : 0));
            break;
        }
    }
}

void Ruler::drawItemWithChildren(QPainter *painter, base_design_item *item)
{
    foreach (base_design_item *child, item->childBaseItems())
    {
        if (!child->childBaseItems().isEmpty())
        {
            drawItemWithChildren(painter, child);
        }
        else
        {
            drawItem(painter, child);
        }

    }
    drawItem(painter, item);
}

void Ruler::drawItem(QPainter *painter, base_design_item *item)
{
    if (!item->isSelected())
    {
        return;
    }

    QGraphicsView *view = qobject_cast<QGraphicsView *>(parent());
    int hStartPos = view->mapFromScene(0, 0).x();
    int vStartPos = view->mapFromScene(0, 0).y();

    int itemWidth = view->mapFromScene(item->mapToScene(item->geometry().width(), 0).x() - item->mapToScene(0, 0).x(), 0).x() - hStartPos;
    int itemHeight = view->mapFromScene(0, item->mapToScene(0, item->geometry().height()).y() - item->mapToScene(0, 0).y()).y() - vStartPos;

    switch (m_type)
    {
    case Horizontal:
        if (item->isSelected())
            painter->drawRect(view->mapFromScene(item->mapToScene(0, 0)).x() - (hStartPos > 0 ? hStartPos : 0), 0,
                              itemWidth, 20);
        break;
    case Vertical:
        if (item->isSelected())
            painter->drawRect(0, view->mapFromScene(item->mapToScene(0, 0)).y() - (vStartPos > 0 ? vStartPos : 0),
                              20, itemHeight);
        break;
    }
}
}
