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
#ifndef LANGUAGE_TYPE_DIALOG_HPP
#define LANGUAGE_TYPE_DIALOG_HPP
#include <QDialog>
#include <QAbstractButton>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QComboBox>
#include <QMenu>
#include "base_push_button.hpp"
#include "base_mask_dialog.hpp"

namespace ReportDesign
{
class language_type_dialog : public base_mask_dialog
{
    Q_OBJECT
public:
    explicit language_type_dialog(QWidget *parent = nullptr);
    ~language_type_dialog();
    void init();
    QString valueToKey(int index);
    int keysToValue(QString name);
private:
    int language();
public slots:
    void slotOk();

private:
    QComboBox *m_language;
    base_push_button *m_cancel;
    gbase_push_button *m_ok;
};
}

#endif // LANGUAGE_TYPE_DIALOG_HPP
