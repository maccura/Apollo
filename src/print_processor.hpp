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
#ifndef PRINTPROCESSOR_HPP
#define PRINTPROCESSOR_HPP
#include <QPrinter>
#include "item/page_scene.hpp"
#include "item/page_item.hpp"

namespace ReportDesign
{
class PrintProcessor
{
public:
    explicit PrintProcessor(QPrinter *printer);
    PrintProcessor(QPrinter *printer, page_item::Ptr page);
    ~PrintProcessor();
    bool printPage(page_item::Ptr page);
private:
    void initPrinter(page_item *page);
private:
    QPrinter *m_printer;
    QPainter *m_painter;
    page_scene *m_renderPage;
    bool m_firstPage;
};
}
#endif // PRINTPROCESSOR_HPP
