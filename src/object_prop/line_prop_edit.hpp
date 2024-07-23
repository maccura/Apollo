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
#ifndef LINE_PROP_EDIT_H
#define LINE_PROP_EDIT_H
#include "base_prop_widget.hpp"
#include "prop_base_widget/prop_color_button.hpp"
#include "prop_base_widget/prop_combox.hpp"
#include "prop_base_widget/prop_spinbox_widget.hpp"
#include "mask_dialog/base_push_button.hpp"
#include <QFontComboBox>
namespace ReportDesign
{
class line_prop_edit: public base_prop_widget
{
    Q_OBJECT
public:
    explicit line_prop_edit(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    virtual void update(QString PropertyName) override;
public slots:
    void slotLineSizeChange(QString size);
private:
    prop_color_button *m_lineColorButton;
    prop_spinbox_widget *m_lineWidthSpinbox;
};
}
#endif // LINE_PROP_EDIT_H
