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
#ifndef ABOUT_DIALOG_HPP
#define ABOUT_DIALOG_HPP

#include<QDialog>
#include<QTabWidget>
#include<QTextEdit>

namespace ReportDesign
{
class about_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit about_dialog(QWidget *parent = 0);
    ~about_dialog();

private:
    void init();
    void loadSoftDes();
    void loadQtDes();
    QString readDes(QString path);
private:
    QTabWidget *m_infoTable;
    QWidget *m_licenseTab;
    QTextEdit  *m_licenseText;
    QTextEdit  *m_QtVerText;
};
}
#endif // ABOUT_DIALOG_HPP
