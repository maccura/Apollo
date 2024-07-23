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
#ifndef PAGE_VIEW_HPP
#define PAGE_VIEW_HPP
#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include "item/page_scene.hpp"
#include "mask_dialog/preview_report_widget.hpp"
namespace ReportDesign
{
class Ruler: public QWidget
{
public:
    enum RulerType {Horizontal, Vertical};
    Ruler(RulerType type, QWidget *parent = 0): QWidget(parent), m_page(0), m_type(type) {}
    void setPage(page_item *page);
    void setMousePos(QPoint mousePos)
    {
        m_mousePos = mousePos;
    }
protected:
    void paintEvent(QPaintEvent *event);
    void drawItemWithChildren(QPainter *painter, base_design_item *item);
    void drawItem(QPainter *painter, base_design_item *item);
private:
    page_item *m_page;
    RulerType m_type;
    QPoint m_mousePos;
};

class page_view: public preview_page_view
{
public:
    page_view(QWidget *parent = NULL);
    page_view(QGraphicsScene *scene, QWidget *parent = NULL);
    void setPageItem(page_item *pageItem);
protected:
    bool viewportEvent(QEvent *event);

private:
    page_item *m_pageItem;
    Ruler *m_horizontalRuller;
    Ruler *m_verticalRuller;
    QPoint m_rubberBandOrigin;
};
}
#endif // PAGE_VIEW_HPP
