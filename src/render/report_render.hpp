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
#ifndef REPORT_RENDER_HPP
#define REPORT_RENDER_HPP
#include <QObject>
#include <QList>
#include <QVector>
#include <QString>
#include "item/page_scene.hpp"
#include "data_manager/print_data_manager.hpp"
#include "item/table_item.hpp"
namespace ReportDesign
{

class report_render
{
public:
    report_render();
    ~report_render();
    static void renderPage(ReportPages &pages, PrintDataMap &data);
    // 创建母版控件的Page页
    static page_item::Ptr createMasterMaskPage(page_item::Ptr page, const PrintDataMap &PrintData);
public:
    static bool emptyPage(page_item::Ptr page);
    static int  pageSpliceY(page_item::Ptr page);
    static void renderSinglePage(QList<page_item::Ptr> &newPageList, page_item::Ptr page, QList<base_design_item *> &posTable, PrintDataMap &data);
    // 去除母版控件的控件位置表，按页分
    static QList<QList<base_design_item *>> genPosTable(const ReportPages &pages);
    static void addDeltaY(QList<base_design_item *> table, base_design_item *item, int deltaY);
    static void addDeltaY(QMap<qreal, qreal> deltaYMap, base_design_item *item);

    // 表格翻页时，新增关联表格
    static table_item *separateTableItem(QRectF edgeRect, table_item *item, bool flag, int &deltaY);
    // 计算外围矩形框
    static QRect getBoundingRect(QVector<base_design_item *> &items);
    static QRect getBoundingRect(QList<base_design_item *> &items);
    // 是否存在越界控件
    static bool isOverPage(page_item::Ptr page);
    // 超出page的控件集合,y升序排序
    static QList<base_design_item *> separateOverPageItems(page_item::Ptr page);

    // 模板顶部（页眉）矩形区域
    static QRect masterMaskTop(page_item::Ptr page);
    // 模板底部（页脚）矩形区域
    static QRect masterMaskBottom(page_item::Ptr page);

    // 模板顶部（页眉）矩形区域
    static QRect masterMaskTop(page_item *page);
    // 模板底部（页脚）矩形区域
    static QRect masterMaskBottom(page_item *page);

    static bool itemIsValid(base_design_item *item);

    static void moveValidItem(QList<base_design_item *> &itemList);

    static  page_item::Ptr findPage(ReportPages &pages, page_item *page);
};
}
#endif
