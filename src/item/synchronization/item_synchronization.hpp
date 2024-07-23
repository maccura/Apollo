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
#ifndef ITEM_SYNCHRONIZATION_HPP
#define ITEM_SYNCHRONIZATION_HPP
#include "item/base_design_item.hpp"
#include "item/page_item.hpp"
#include <QObject>
namespace ReportDesign
{
class item_synchronization: public QObject
{
    Q_OBJECT
public:
    item_synchronization(QObject *parent = nullptr);
    QList<base_design_item *> itemSynchronization(base_design_item *item, const ReportPages &pageList);
    void  synchronizationItemDestroyed(base_design_item *item, const ReportPages &pageList);
private:
    void  deleteSameMastermarkIDItem(base_design_item *item, const page_item::Ptr page);
};
}
#endif
