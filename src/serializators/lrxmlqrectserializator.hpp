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
#ifndef LRXMLQRECTSERIALIZATOR_H
#define LRXMLQRECTSERIALIZATOR_H

#include "lrserializatorintf.hpp"
#include <QtXml/QDomDocument>
#include <QVariant>
namespace ReportDesign
{

class XMLQRectSerializator : public SerializatorIntf
{
public:
    XMLQRectSerializator(QDomDocument *doc, QDomElement *node): m_doc(doc), m_node(node) {}
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
private:
    QDomDocument *m_doc;
    QDomElement *m_node;
};

class XMLQRectFSerializator : public SerializatorIntf
{
public:
    XMLQRectFSerializator(QDomDocument *doc, QDomElement *node): m_doc(doc), m_node(node) {}
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
private:
    QDomDocument *m_doc;
    QDomElement *m_node;
};

}
#endif // LRXMLQRECTSERIALIZATOR_H
