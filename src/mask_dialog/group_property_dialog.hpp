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
#ifndef GROUP_PROPERTY_DIALOG_HPP
#define GROUP_PROPERTY_DIALOG_HPP
#include "base_mask_dialog.hpp"
#include <QCheckBox>
#include "object_prop/prop_base_widget/prop_combox.hpp"
#include "data_manager/base_homogeneous_variant.hpp"
#include "item/page_scene.hpp"
#include "base_mask_dialog.hpp"
#include "base_push_button.hpp"
namespace ReportDesign
{
class group_property_dialog: public base_mask_dialog
{
    Q_OBJECT
public:
    group_property_dialog(page_scene *pageScene, QWidget *parent = nullptr);

signals:
    void GroupProperty(bool, variant_info);
private:
    void slotOkButton();
    void slotRepeatCheck(bool cheaked);
    void setComBoxToID(QString ID);
    void setComBoxToEmpty();
private:
    prop_combox *m_variantCombox;
    QCheckBox *m_repeatCheckBox;
    QCheckBox *m_groupPageBreakBox;
    page_scene *m_pageScene;
    base_push_button *m_cancelButton;
    gbase_push_button *m_okButton;
};
}
#endif // GROUP_PROPERTY_DIALOG_HPP
