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
#ifndef REPEAT_RENDER_H
#define REPEAT_RENDER_H
#include "abstract_render.hpp"
namespace ReportDesign
{
class RepeatRender: public abstract_render
{
public:
    RepeatRender();
    void pageRender(ReportPages &pages, PrintDataMap &data) override;
private:
    int repeatCount(QMap<QString, QVariant> &gourpMap);

    // 返回增加后的deltaY
    void copyGroupItemAtBottom(int repeatNum, QList<base_design_item *> &groupItems, page_item::Ptr page);

    // 位于groupItems下方的item全部增加deltaY;
    void AddDeltaY(QList<base_design_item *> &groupItems, int deltaY, page_item::Ptr page);

    QList<base_design_item *> copyItems(QList<base_design_item *> &orgItems, int groupDataIndex);

    // 在page中pos y值小于该y的所有元件
    QList<base_design_item *> belowTheYItems(int Y, page_item::Ptr page);

    // 移至新起页
    page_item::Ptr moveToNewPage(QList<base_design_item *> &items, page_item::Ptr beCopiedPage, PrintDataMap &PrintData);

    void insertFrontPage(page_item::Ptr posPage, page_item::Ptr insertPage);
private:
    PrintDataMap m_printData;
    ReportPages  m_newPages;
};
}
#endif // REPEAT_RENDER_H
