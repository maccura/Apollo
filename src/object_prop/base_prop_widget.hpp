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
#ifndef BASE_PROP_WIDGET_H
#define BASE_PROP_WIDGET_H

#include <QWidget>
#include <functional>
#include "./base/attribs_abstract_factory.hpp"
#include "global.hpp"

namespace ReportDesign
{
class base_prop_widget: public QWidget
{
    Q_OBJECT
public:
    base_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    // 根据不同的属性名更新不同的控件值，一个属性编辑窗口可能有多个属性
    virtual void update(QString PropertyName) = 0;
    virtual void translateEnumItemName() {}
protected:
    QObject *object()
    {
        return m_object;
    }
    QList<QObject *> objects()
    {
        return *m_objects;
    }
private:
    QObject *m_object;
    QList<QObject *> *m_objects;
};
using CreatePropWidget = base_prop_widget * (*)(QObject *, QList<QObject *> *, QWidget *);
}
#endif // BASE_PROP_WIDGET_H
