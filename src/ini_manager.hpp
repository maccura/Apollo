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
#ifndef INI_MANAGER_HPP
#define INI_MANAGER_HPP
#include<QSettings>
#include <QMenu>
#include "base/singleton.hpp"
#include "mask_dialog/language_type_dialog.hpp"
#include "global.hpp"

namespace ReportDesign
{

class ini_manager: public QObject, public singleton<ini_manager>
{
    Q_OBJECT
public:
    enum Languages
    {
        chinese = 0,
        english = 1,
        russian = 2,
        japanese = 3,
        french = 4,
        latin = 5,
        german = 6,
        spanish = 7,
        italian = 8
    };
    Q_ENUM(Languages)
    ini_manager(QObject *p = nullptr);
    ~ini_manager();
    ini_manager::Languages language();
    void setLanguage(int index);
    QString curLangFile(QString filePath, ini_manager::Languages lang);

private:
    bool isCurrentLanguage(QString file, ini_manager::Languages language);
    void init();
private:
    QSettings *m_configIni;
};
}

#endif // INI_MANAGER_HPP
