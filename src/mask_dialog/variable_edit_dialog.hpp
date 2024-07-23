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
#ifndef VARIABLE_EDIT_DIALOG_H
#define VARIABLE_EDIT_DIALOG_H

#include <QAbstractButton>
#include "data_manager/device_variant_manager.hpp"
#include "mask_dialog/base_mask_dialog.hpp"
#include "mask_dialog/base_push_button.hpp"
namespace Ui
{
class variable_edit_dialog;
}
namespace ReportDesign
{
class variable_edit_dialog : public base_mask_dialog
{
    Q_OBJECT
public:
    explicit variable_edit_dialog(bool inTable, base_homogeneous_variant *dataSource,
                                  variant_info variable, variant_info tableVariableID = variant_info());
    ~variable_edit_dialog();
    void init();
    void initVaraintList();

public slots:
    void slotLoadVaraint();
    void slotSelectVariant(int row, int col);
    void slotOk();
signals:
    void variable(variant_info variable);
    void tableVariable(variant_info itemVariable, variant_info tableVariable);

private:
    int currentVariable();
    void updateCheckStatus(variant_info variable, variant_info tableVariableID);
private:
    Ui::variable_edit_dialog *ui;
    bool m_inTable;
    base_homogeneous_variant *m_dataSource;
    gbase_push_button *m_ok;
    base_push_button *m_cancel;

};
}
#endif // VARIABLE_EDIT_DIALOG_H
