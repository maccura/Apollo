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
#ifndef LRXMLBASETYPESSERIALIZATORS_H
#define LRXMLBASETYPESSERIALIZATORS_H

#include "lrserializatorintf.hpp"
#include <QtXml/QDomDocument>
#include <QDebug>

namespace ReportDesign
{

class XmlBaseSerializator : public virtual SerializatorIntf
{
public:
    XmlBaseSerializator(QDomDocument *doc, QDomElement *node): m_doc(doc), m_node(node) {}
protected:
    QDomDocument   *doc()
    {
        return m_doc;
    }
    QDomElement    *node()
    {
        if (!m_node)
        {
            qDebug() << QLatin1String("Warning node is null");
        }
        return m_node;
    }
    void saveBool(QDomElement node, QString name, bool value);
private:
    QDomDocument    *m_doc;
    QDomElement     *m_node;
};

class XmlCryptedSerializator : public XmlBaseSerializator, public virtual CryptedSerializator
{
public:
    XmlCryptedSerializator(QDomDocument *doc, QDomElement *node)
        : XmlBaseSerializator(doc, node) {}
    virtual void setPassPhrase(const QString &passPhrase)
    {
        m_passPhrase = passPhrase;
    }
protected:
    QString passPhrase()
    {
        return m_passPhrase;
    }
private:
    QString m_passPhrase;
};

class XmlQStringSerializator : public XmlCryptedSerializator
{
public:
    XmlQStringSerializator(QDomDocument *doc, QDomElement *node): XmlCryptedSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlIntSerializator : public XmlBaseSerializator
{
public:
    XmlIntSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlUnsignedIntProperty : public XmlBaseSerializator
{
public:
    XmlUnsignedIntProperty(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlQRealSerializator : public XmlBaseSerializator
{
public:
    XmlQRealSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlEnumAndFlagsSerializator : public XmlBaseSerializator
{
public:
    XmlEnumAndFlagsSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlBoolSerializator : public XmlBaseSerializator
{
public:
    XmlBoolSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlFontSerializator : public XmlBaseSerializator
{
public:
    XmlFontSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlQSizeFSerializator : public XmlBaseSerializator
{
public:
    XmlQSizeFSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlColorSerializator : public XmlBaseSerializator
{
public:
    XmlColorSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlQImageSerializator : public XmlBaseSerializator
{
public:
    XmlQImageSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlQByteArraySerializator : public XmlBaseSerializator
{
public:
    XmlQByteArraySerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
private:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlQVariantSerializator : public XmlBaseSerializator
{
public:
    XmlQVariantSerializator(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};


class XmlTableColumnProperty : public XmlBaseSerializator
{
public:
    XmlTableColumnProperty(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlTableRowProperty : public XmlBaseSerializator
{
public:
    XmlTableRowProperty(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlTableProperty : public XmlBaseSerializator
{
public:
    XmlTableProperty(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlTableCellProperty : public XmlBaseSerializator
{
public:
    XmlTableCellProperty(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlVariantInfo : public XmlBaseSerializator
{
public:
    XmlVariantInfo(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlTableVariantInfo : public XmlBaseSerializator
{
public:
    XmlTableVariantInfo(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};

class XmlConnectTableMap : public XmlBaseSerializator
{
public:
    XmlConnectTableMap(QDomDocument *doc, QDomElement *node): XmlBaseSerializator(doc, node) {}
public:
    virtual void save(const QVariant &value, QString name);
    virtual QVariant loadValue();
};



}
#endif // LRXMLBASETYPESSERIALIZATORS_H
