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
#ifndef PAGE_PROP_EDITOR_WIDGET_H
#define PAGE_PROP_EDITOR_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QMetaEnum>
#include "base_prop_widget.hpp"
#include "prop_base_widget/prop_combox.hpp"
namespace ReportDesign
{
class page_prop_editor_widget: public base_prop_widget
{
    Q_OBJECT
public:
    page_prop_editor_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    virtual void update(QString PropertyName);
    virtual void translateEnumItemName() override;
public slots:
    void slotPageSizeChange(int index);
    void slotPageOrientationChange(int index);
private:
    void init();
    void initCombox(prop_combox *comBox, QMetaEnum propEnum, int curentPropEnum);
    void updatePageProperty(prop_combox *comBox, int Property);
private:
    QLabel *m_pageSizeLabel;
    QLabel *m_pageOrientationLabel;
    prop_combox *m_pageSize;
    prop_combox *m_pageOrientation;
};
}
#endif // PAGE_PROP_EDITOR_WIDGET_H
