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
#ifndef ITEMSALIGNEDITORWIDGET_H
#define ITEMSALIGNEDITORWIDGET_H
#include "base_prop_widget.hpp"
#include "item/page_scene.hpp"
#include "mask_dialog/base_push_button.hpp"

namespace ReportDesign
{
class alignment_prop_widget: public base_prop_widget
{
    Q_OBJECT
public:
    explicit alignment_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    virtual void update(QString PropertyName) override;
private slots:
    void slotBringToFront();
    void slotSendToBack();
    void slotAlignToLeft();
    void slotAlignToRight();
    void slotAlignToVCenter();
    void slotAlignToTop();
    void slotAlignToBottom();
    void slotAlignToHCenter();
    void slotSameHeight();
    void slotSameWidth();
private:
    void initEditor();
    page_scene *page();
private:
    image_button *m_bringToFront;
    image_button *m_sendToBack;
    image_button *m_alignToLeft;
    image_button *m_alignToRight;
    image_button *m_alignToVCenter;
    image_button *m_alignToTop;
    image_button *m_alignToBottom;
    image_button *m_alignToHCenter;
    image_button *m_sameHeight;
    image_button *m_sameWidth;
};

} //namespace LimeReport

#endif // ITEMSALIGNEDITORWIDGET_H
