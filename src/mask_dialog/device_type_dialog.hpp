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
#ifndef DEVICE_TYPE_DIALOG_H
#define DEVICE_TYPE_DIALOG_H
#include "design_main_window.hpp"
#include "base_mask_dialog.hpp"
#include "base_push_button.hpp"
namespace Ui
{
class device_type_dialog;
}
namespace ReportDesign
{

class device_type_dialog : public base_mask_dialog
{
    Q_OBJECT
public:
    explicit device_type_dialog(QWidget *parent = nullptr);
    ~device_type_dialog();
    void init();
public slots:
    void slotOkButton();
signals:
    void DeviceType(QString);

private:
    Ui::device_type_dialog *ui;
    base_push_button *m_cancelButton;
    gbase_push_button *m_okButton;
};
}
#endif // DEVICE_TYPE_DIALOG_H
