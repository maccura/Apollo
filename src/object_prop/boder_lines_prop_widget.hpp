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
#ifndef BODER_SHOW_PROP_WIDGET_H
#define BODER_SHOW_PROP_WIDGET_H
#include "base_prop_widget.hpp"
#include "item/base_design_item.hpp"
#include <QCheckBox>
namespace ReportDesign
{
class boder_lines_prop_widget: public base_prop_widget
{
    Q_OBJECT
public:
    explicit boder_lines_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    virtual void update(QString PropertyName) override;
public slots:
    void slotCheakBox(QAbstractButton *state);
    void slotCheakBox(QObject *object, QAbstractButton *state);
    void slotCheakBox(const QList<QObject *> &object, QAbstractButton *state);
private:
    void updateBoder(base_design_item::BorderSide flag);
    void clearCheak();
private:
    QCheckBox *m_left;
    QCheckBox *m_top;
    QCheckBox *m_right;
    QCheckBox *m_bottom;
};
}
#endif // BODER_SHOW_PROP_WIDGET_H
