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
#ifndef MESSAGE_INFO_DIALOG_HPP
#define MESSAGE_INFO_DIALOG_HPP
#include "base_push_button.hpp"
#include "base_mask_dialog.hpp"
#include "global.hpp"
namespace ReportDesign
{

class message_info_dialog: public base_mask_dialog
{
    Q_OBJECT
public:
    enum DlgResult
    {
        yes = 1,
        no = 2,
        cancle = 3
    };
    explicit message_info_dialog(QWidget *parent = nullptr);
    void init();
    void setMessage(QString message);
    DlgResult dlgResult()
    {
        return m_ret;
    }

    // 1 yes,2 no
    static DlgResult message(QWidget *parent, QString title, QString message);
private:
    base_push_button *m_cancel;
    base_push_button *m_no;
    gbase_push_button *m_yes;
    QLabel *m_messageLabel;
    DlgResult m_ret;
};
}
#endif // MESSAGE_INFO_DIALOG_HPP
