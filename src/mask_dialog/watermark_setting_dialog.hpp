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
#ifndef WATERMARK_SETTING_DIALOG_HPP
#define WATERMARK_SETTING_DIALOG_HPP
#include "base_push_button.hpp"
#include "base_mask_dialog.hpp"
#include <QRadioButton>
#include <QTextEdit>
namespace ReportDesign
{
class watermark_setting_dialog: public base_mask_dialog
{
    Q_OBJECT
public:
    explicit watermark_setting_dialog(QWidget *parent = nullptr);
    ~watermark_setting_dialog();
    void init();
    void hide();
public slots:
    void slotOk();
signals:
    void watermarkSetting(QString watermarkText, QString imageFilePath);

private:
    image_Radio_button *m_textRadioButton;
    image_Radio_button *m_imageRadioButton;
    base_push_button *m_cancel;
    gbase_push_button *m_ok;
    QTextEdit *m_textEdit;
    QPushButton *m_loadImage;
};
}
#endif // WATERMARK_SETTING_DIALOG_HPP
