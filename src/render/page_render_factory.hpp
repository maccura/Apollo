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
#ifndef PAGERENDERFACTOR_H
#define PAGERENDERFACTOR_H
#include "abstract_render.hpp"
#include <QMap>
#include <memory>
#include <functional>
namespace ReportDesign
{
class PageRenderFactor
{
public:
    using RenderCreator = std::function<abstract_render*(void)>;
    static PageRenderFactor &instance();
    bool registerRender(QString Type, RenderCreator creator);
    abstract_render::renderPtr creator(QString Type);
private:
    PageRenderFactor();
    PageRenderFactor(const PageRenderFactor &obj) = delete;
    PageRenderFactor &operator= (const PageRenderFactor &obj) = delete;
private:
    QMap<QString, RenderCreator> m_renderCreatorMap;
};

}
#endif // PAGERENDERFACTOR_H
