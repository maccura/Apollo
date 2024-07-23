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
#include "lrxmlbasetypesserializators.hpp"
#include "serializators/lrxmlserializatorsfactory.hpp"
#include "lrsimplecrypt.hpp"
#include "item/table_base_item.hpp"
#include "data_manager/base_homogeneous_variant.hpp"
#include <QFont>
#include <QImage>
#include <QColor>
#include <QBuffer>

namespace
{

ReportDesign::SerializatorIntf *createIntSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlIntSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQRealSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlQRealSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQStringSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlQStringSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createEnumAndFlagsSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlEnumAndFlagsSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createBoolSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlBoolSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createFontSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlFontSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQSizeFSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlQSizeFSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQImageSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlQImageSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQColorSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlColorSerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQByteArraySerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlQByteArraySerializator(doc, node);
}

ReportDesign::SerializatorIntf *createQVariantSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlQVariantSerializator(doc, node);
}
ReportDesign::SerializatorIntf *createTableColumnSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlTableColumnProperty(doc, node);
}
ReportDesign::SerializatorIntf *createlTableRowSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlTableRowProperty(doc, node);
}
ReportDesign::SerializatorIntf *createlUnsignedIntSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlUnsignedIntProperty(doc, node);
}

ReportDesign::SerializatorIntf *createlTablePropertySerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlTableProperty(doc, node);
}

ReportDesign::SerializatorIntf *createlTableCellPropertySerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlTableCellProperty(doc, node);
}

ReportDesign::SerializatorIntf *createlTableVariantInfoSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlTableVariantInfo(doc, node);
}

ReportDesign::SerializatorIntf *createlVariantInfoSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlVariantInfo(doc, node);
}

ReportDesign::SerializatorIntf *createlConnectTableMapSerializator(QDomDocument *doc, QDomElement *node)
{
    return new ReportDesign::XmlConnectTableMap(doc, node);
}

bool  registredQString = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QString", createQStringSerializator);
bool  registredInt = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("int", createIntSerializator);
bool  registredEnumAndFlags = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("enumAndFlags", createEnumAndFlagsSerializator);
bool  registredBool = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("bool", createBoolSerializator);
bool  registredQFont = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QFont", createFontSerializator);
bool  registredQSizeF = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QSizeF", createQSizeFSerializator);
bool  registredQImage = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QImage", createQImageSerializator);
bool  registredQReal = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("qreal", createQRealSerializator);
bool  registerDouble = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("double", createQRealSerializator);
bool  registerQColor = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QColor", createQColorSerializator);
bool  registerQByteArray = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QByteArray", createQByteArraySerializator);
bool  registerQVariant = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QVariant", createQVariantSerializator);
bool  registerTableColumn = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QList<ReportDesign::column_property>", createTableColumnSerializator);
bool  registerTableRow = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QList<ReportDesign::row_property>", createlTableRowSerializator);
bool  registerUnsignedInt = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("uint", createlUnsignedIntSerializator);
bool  registerTableProperty = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("ReportDesign::content_property", createlTablePropertySerializator);
bool  registerTableCellProperty = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QMap<QPair<int,int>,ReportDesign::content_property>", createlTableCellPropertySerializator);
bool  registerVariantInfo = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("ReportDesign::variant_info", createlVariantInfoSerializator);
bool  registerTableVariantInfo = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QMap<QPair<int,int>,ReportDesign::variant_info>", createlTableVariantInfoSerializator);
bool  registerConnectTableMap = ReportDesign::XMLAbstractSerializatorFactory::instance().registerCreator("QMap<int,ReportDesign::ConnectTableInfo>", createlConnectTableMapSerializator);

}


namespace ReportDesign
{

void XmlBaseSerializator::saveBool(QDomElement node, QString name, bool value)
{
    if (value)
    {
        node.setAttribute(name, 1);
    }
    else
    {
        node.setAttribute(name, 0);
    }
}

void XmlQStringSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QString");
    if (name.compare("password") == 0)
    {
        Chipper chipper(passPhrase());
        QByteArray ba = chipper.cryptString(value.toString());
        //ba.append();
        _node.setAttribute("Value", QString(ba.toBase64()));
    }
    else
    {
        _node.appendChild(doc()->createTextNode(value.toString()));
    }
    node()->appendChild(_node);
}

QVariant XmlQStringSerializator::loadValue()
{
    if (node()->tagName().compare("password") == 0)
    {
        QByteArray ba;
        Chipper chipper(passPhrase());
        ba.append(node()->attribute("Value").toLatin1());
        return chipper.decryptByteArray(QByteArray::fromBase64(ba));
    }
    else
    {
        return node()->text();
    }
}

void XmlIntSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "int");
    _node.setAttribute("Value", value.toInt());
    node()->appendChild(_node);
}

QVariant XmlIntSerializator::loadValue()
{
    return node()->attribute("Value").toInt();
}

void XmlUnsignedIntProperty::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "uint");
    _node.setAttribute("Value", value.toInt());
    node()->appendChild(_node);
}

QVariant XmlUnsignedIntProperty::loadValue()
{
    return node()->attribute("Value").toUInt();
}

void XmlEnumAndFlagsSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "enumAndFlags");

    //Qt::Alignment 目前转int为0,还未解决该问题
    if (name == "alignment")
    {
        _node.setAttribute("Value", value.value<Qt::Alignment>());
    }
    else
    {
        _node.setAttribute("Value", value.toInt());
    }
    node()->appendChild(_node);
}

QVariant XmlEnumAndFlagsSerializator::loadValue()
{
    return node()->attribute("Value").toInt();
}

void XmlBoolSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "bool");
    if (value.toBool())
    {
        _node.setAttribute("Value", 1);
    }
    else
    {
        _node.setAttribute("Value", 0);
    }

    node()->appendChild(_node);
}

QVariant XmlBoolSerializator::loadValue()
{
    return node()->attribute("Value").toInt();
}

void XmlFontSerializator::save(const QVariant &value, QString name)
{
    QFont font = value.value<QFont>();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QFont");
    _node.setAttribute("family", font.family());
    _node.setAttribute("pointSize", font.pointSize());
    //#if QT_VERSION>0x040800
    //    _node.setAttribute("stylename",font.styleName());
    //#endif
    _node.setAttribute("weight", font.weight());
    //saveBool(_node,"bold",font.bold());
    saveBool(_node, "italic", font.italic());
    saveBool(_node, "underline", font.underline());
    node()->appendChild(_node);
}

QVariant XmlFontSerializator::loadValue()
{
    QFont font;
    font.setFamily(node()->attribute("family"));
    font.setPointSize(node()->attribute("pointSize").toInt());
    //#if QT_VERSION>0x040800
    //    font.setStyleName(node()->attribute("stylename"));
    //#endif
    if (node()->attribute("weight").toInt() > 0)
    {
        font.setWeight(static_cast<QFont::Weight>(node()->attribute("weight").toInt()));
    }
    if (!node()->attribute("bold").isEmpty())
    {
        font.setBold(node()->attribute("bold").toInt());
    }
    font.setItalic(node()->attribute("italic").toInt());
    font.setUnderline(node()->attribute("underline").toInt());
    return font;
}

void XmlQSizeFSerializator::save(const QVariant &value, QString name)
{
    QSizeF size = value.toSizeF();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QSizeF");
    _node.setAttribute("width", QString::number(size.width()));
    _node.setAttribute("height", QString::number(size.height()));
    node()->appendChild(_node);
}

QVariant XmlQSizeFSerializator::loadValue()
{
    QSizeF size;
    size.setWidth(node()->attribute("width").toDouble());
    size.setHeight(node()->attribute("height").toDouble());
    return size;
}

void XmlQImageSerializator::save(const QVariant &value, QString name)
{
    QImage image = value.value<QImage>();
    QByteArray ba;
    QBuffer buff(&ba);
    buff.open(QIODevice::WriteOnly);
    image.save(&buff, "PNG");
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QImage");
    _node.appendChild(doc()->createTextNode(ba.toHex()));
    node()->appendChild(_node);
}

QVariant XmlQImageSerializator::loadValue()
{
    QImage img;
    img.loadFromData(QByteArray::fromHex(node()->text().toLatin1()), "PNG");
    return img;
}

void XmlQRealSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "qreal");
    _node.setAttribute("Value", QString::number(value.toDouble()));
    node()->appendChild(_node);
}

QVariant XmlQRealSerializator::loadValue()
{
    return node()->attribute("Value").toDouble();
}

void XmlColorSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QColor");
    _node.setAttribute("Value", value.value<QColor>().name());
    node()->appendChild(_node);
}

QVariant XmlColorSerializator::loadValue()
{
    return QColor(node()->attribute("Value"));
}

void XmlQByteArraySerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QByteArray");
    _node.setAttribute("Value", QString(value.toByteArray().toBase64()));
    node()->appendChild(_node);
}

QVariant XmlQByteArraySerializator::loadValue()
{
    QByteArray ba;
    ba.append(node()->attribute("Value").toLatin1());
    return QByteArray::fromBase64(ba);
}

void XmlQVariantSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QVariant");
    _node.setAttribute("Value", QString(value.toByteArray().toBase64()));
    node()->appendChild(_node);
}

QVariant XmlQVariantSerializator::loadValue()
{
    QByteArray ba;
    ba.append(node()->attribute("Value").toLatin1());
    return QByteArray::fromBase64(ba);
}

void XmlTableColumnProperty::save(const QVariant &value, QString name)
{
    QList<column_property> columnPropertyList = value.value<QList<column_property>>();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QList<ReportDesign::column_property>");
    for (int i = 0; i < columnPropertyList.count(); ++i)
    {
        QDomElement columnPropertyNode = doc()->createElement("ColumnPropertyNode");
        columnPropertyNode.setAttribute("columnPosx", columnPropertyList[i].columnPosx());
        columnPropertyNode.setAttribute("width", columnPropertyList[i].width());

        // [Column]Font
        QFont font = columnPropertyList[i].contentProperty().font();
        QDomElement contentFont = doc()->createElement("font");
        contentFont.setAttribute("Type", "QFont");
        contentFont.setAttribute("family", font.family());
        contentFont.setAttribute("pointSize", font.pointSize());
        contentFont.setAttribute("weight", font.weight());
        saveBool(contentFont, "italic", font.italic());
        saveBool(contentFont, "underline", font.underline());
        columnPropertyNode.appendChild(contentFont);

        QDomElement fontColor = doc()->createElement("fontColor");
        fontColor.setAttribute("Type", "QColor");
        fontColor.setAttribute("Value", columnPropertyList[i].contentProperty().fontColor().name());
        columnPropertyNode.appendChild(fontColor);

        columnPropertyNode.setAttribute("alignment", columnPropertyList[i].contentProperty().alignment());
        saveBool(columnPropertyNode, "isNull", columnPropertyList[i].contentProperty().isNull());
        columnPropertyNode.setAttribute("timeCount", columnPropertyList[i].timeCount());
        _node.appendChild(columnPropertyNode);
    }
    node()->appendChild(_node);
}

QVariant XmlTableColumnProperty::loadValue()
{
    QDomNodeList arrayElements = node()->elementsByTagName("ColumnPropertyNode");
    QList<column_property> columnPropertyList;
    for (int i = 0; i < arrayElements.count(); ++i)
    {
        QDomElement ColumnPropertyNode = arrayElements.at(i).toElement();
        column_property columnProperty(ColumnPropertyNode.attribute("columnPosx").toInt(), ColumnPropertyNode.attribute("width").toInt());
        content_property contentProperty;
        columnProperty.setTimeCount(ColumnPropertyNode.attribute("timeCount").toInt());
        contentProperty.setAlignment(Qt::Alignment(ColumnPropertyNode.attribute("alignment").toInt()));

        QDomElement fontNode = ColumnPropertyNode.elementsByTagName("font").at(0).toElement();
        QFont font;
        font.setFamily(fontNode.attribute("family"));
        font.setPointSize(fontNode.attribute("pointSize").toInt());
        if (fontNode.attribute("weight").toInt() > 0)
        {
            font.setWeight(static_cast<QFont::Weight>(fontNode.attribute("weight").toInt()));
        }
        if (!fontNode.attribute("bold").isEmpty())
        {
            font.setBold(fontNode.attribute("bold").toInt());
        }
        font.setItalic(fontNode.attribute("italic").toInt());
        font.setUnderline(fontNode.attribute("underline").toInt());
        contentProperty.setFont(font);
        contentProperty.setIsNull(ColumnPropertyNode.attribute("isNull").toInt());

        QDomElement fontColorNode = ColumnPropertyNode.elementsByTagName("fontColor").at(0).toElement();
        contentProperty.setFontColor(QColor(fontColorNode.attribute("Value")));

        columnProperty.setContentProperty(contentProperty);
        columnPropertyList.append(columnProperty);
    }

    QVariant variant;
    variant.setValue(columnPropertyList);
    return variant;
}

void XmlTableRowProperty::save(const QVariant &value, QString name)
{
    QList<row_property> rowPropertyList = value.value<QList<row_property>>();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QList<ReportDesign::row_property>");
    for (int i = 0; i < rowPropertyList.count(); ++i)
    {
        QDomElement RowPropertyNode = doc()->createElement("RowPropertyNode");
        RowPropertyNode.setAttribute("rowPosx", rowPropertyList[i].rowPosY());
        RowPropertyNode.setAttribute("width", rowPropertyList[i].height());

        // [row]Font
        QFont font = rowPropertyList[i].contentProperty().font();
        QDomElement contentFont = doc()->createElement("font");
        contentFont.setAttribute("Type", "QFont");
        contentFont.setAttribute("family", font.family());
        contentFont.setAttribute("pointSize", font.pointSize());
        contentFont.setAttribute("weight", font.weight());
        saveBool(contentFont, "italic", font.italic());
        saveBool(contentFont, "underline", font.underline());
        RowPropertyNode.appendChild(contentFont);


        QDomElement fontColor = doc()->createElement("fontColor");
        fontColor.setAttribute("Type", "QColor");
        fontColor.setAttribute("Value", rowPropertyList[i].contentProperty().fontColor().name());
        RowPropertyNode.appendChild(fontColor);

        RowPropertyNode.setAttribute("alignment", rowPropertyList[i].contentProperty().alignment());
        saveBool(RowPropertyNode, "isNull", rowPropertyList[i].contentProperty().isNull());
        RowPropertyNode.setAttribute("timeCount", rowPropertyList[i].timeCount());
        _node.appendChild(RowPropertyNode);
    }
    node()->appendChild(_node);
}

QVariant XmlTableRowProperty::loadValue()
{
    QDomNodeList arrayElements = node()->elementsByTagName("RowPropertyNode");
    QList<row_property> rowPropertyList;
    for (int i = 0; i < arrayElements.count(); ++i)
    {
        QDomElement RowPropertyNode = arrayElements.at(i).toElement();
        row_property rowProperty(RowPropertyNode.attribute("rowPosx").toInt(), RowPropertyNode.attribute("width").toInt());
        content_property contentProperty;
        rowProperty.setTimeCount(RowPropertyNode.attribute("timeCount").toInt());

        contentProperty.setAlignment(Qt::Alignment(RowPropertyNode.attribute("alignment").toInt()));
        QDomElement fontNode = RowPropertyNode.elementsByTagName("font").at(0).toElement();
        QFont font;
        font.setFamily(fontNode.attribute("family"));
        font.setPointSize(fontNode.attribute("pointSize").toInt());
        if (fontNode.attribute("weight").toInt() > 0)
        {
            font.setWeight(static_cast<QFont::Weight>(fontNode.attribute("weight").toInt()));
        }

        if (!fontNode.attribute("bold").isEmpty())
        {
            font.setBold(fontNode.attribute("bold").toInt());
        }

        font.setItalic(fontNode.attribute("italic").toInt());
        font.setUnderline(fontNode.attribute("underline").toInt());
        contentProperty.setFont(font);

        QDomElement fontColorNode = RowPropertyNode.elementsByTagName("fontColor").at(0).toElement();
        contentProperty.setFontColor(QColor(fontColorNode.attribute("Value")));

        contentProperty.setIsNull(RowPropertyNode.attribute("isNull").toInt());
        rowProperty.setContentProperty(contentProperty);
        rowPropertyList.append(rowProperty);
    }

    QVariant variant;
    variant.setValue(rowPropertyList);
    return variant;
}

void XmlTableProperty::save(const QVariant &value, QString name)
{
    content_property TableProperty = value.value<content_property>();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "ReportDesign::content_property");

    // [列]Font
    QFont font = TableProperty.font();
    QDomElement contentFont = doc()->createElement("font");
    contentFont.setAttribute("Type", "QFont");
    contentFont.setAttribute("family", font.family());
    contentFont.setAttribute("pointSize", font.pointSize());
    contentFont.setAttribute("weight", font.weight());
    saveBool(contentFont, "italic", font.italic());
    saveBool(contentFont, "underline", font.underline());

    //saveBool(_node,"isNull",TableProperty.isNull());
    _node.appendChild(contentFont);
    _node.setAttribute("alignment", TableProperty.alignment());

    QDomElement fontColor = doc()->createElement("fontColor");
    fontColor.setAttribute("Type", "QColor");
    fontColor.setAttribute("Value", value.value<QColor>().name());
    _node.appendChild(fontColor);

    node()->appendChild(_node);
}

QVariant XmlTableProperty::loadValue()
{
    content_property TableProperty;
    TableProperty.setAlignment(Qt::Alignment(node()->attribute("alignment").toInt()));
    QDomElement fontNode = node()->elementsByTagName("font").at(0).toElement();
    QFont font;
    font.setFamily(fontNode.attribute("family"));
    font.setPointSize(fontNode.attribute("pointSize").toInt());
    if (fontNode.attribute("weight").toInt() > 0)
    {
        font.setWeight(static_cast<QFont::Weight>(fontNode.attribute("weight").toInt()));
    }

    if (!fontNode.attribute("bold").isEmpty())
    {
        font.setBold(fontNode.attribute("bold").toInt());
    }
    font.setItalic(fontNode.attribute("italic").toInt());
    font.setUnderline(fontNode.attribute("underline").toInt());
    TableProperty.setFont(font);
    //TableProperty.setIsNull(node()->attribute("isNull").toInt());
    QDomElement fontColorNode = node()->elementsByTagName("fontColor").at(0).toElement();
    TableProperty.setFontColor(QColor(fontColorNode.attribute("Value")));

    QVariant variant;
    variant.setValue(TableProperty);
    return variant;
}

void XmlTableCellProperty::save(const QVariant &value, QString name)
{
    QMap<QPair<int, int>, content_property> TableCellProperty = value.value<QMap<QPair<int, int>, content_property>>();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type", "QMap<QPair<int,int>,ReportDesign::content_property>");

    for (auto it = TableCellProperty.begin(); it != TableCellProperty.end(); ++it)
    {
        QDomElement CellProperty = doc()->createElement("CellProperty");
        CellProperty.setAttribute("rowIndex", it.key().first);
        CellProperty.setAttribute("columnIndex", it.key().second);

        // [row]Font
        QFont font =  it.value().font();
        QColor fColor =  it.value().fontColor();
        QDomElement contentFont = doc()->createElement("font");
        contentFont.setAttribute("Type", "QFont");
        contentFont.setAttribute("family", font.family());
        contentFont.setAttribute("pointSize", font.pointSize());
        contentFont.setAttribute("weight", font.weight());
        saveBool(contentFont, "italic", font.italic());
        saveBool(contentFont, "underline", font.underline());
        QDomElement fontColor = doc()->createElement("fontColor");
        fontColor.setAttribute("Type", "QColor");
        fontColor.setAttribute("Value", fColor.name());
        CellProperty.appendChild(fontColor);
        CellProperty.appendChild(contentFont);
        CellProperty.setAttribute("alignment", it.value().alignment());
        //saveBool(CellProperty,"isNull",it.value().isNull());
        _node.appendChild(CellProperty);
    }
    node()->appendChild(_node);
}

QVariant XmlTableCellProperty::loadValue()
{
    QMap<QPair<int, int>, content_property> TableCellProperty;
    QDomNodeList arrayElements = node()->elementsByTagName("CellProperty");

    for (int i = 0; i < arrayElements.count(); ++i)
    {
        QDomElement CellPropertyNode = arrayElements.at(i).toElement();
        QPair<int, int> TableIndex(CellPropertyNode.attribute("rowIndex").toInt(), CellPropertyNode.attribute("columnIndex").toInt());
        content_property contentProperty;

        contentProperty.setAlignment(Qt::Alignment(CellPropertyNode.attribute("alignment").toInt()));
        QDomElement fontNode = CellPropertyNode.elementsByTagName("font").at(0).toElement();
        QFont font;
        font.setFamily(fontNode.attribute("family"));
        font.setPointSize(fontNode.attribute("pointSize").toInt());
        if (fontNode.attribute("weight").toInt() > 0)
        {
            font.setWeight(static_cast<QFont::Weight>(fontNode.attribute("weight").toInt()));
        }

        if (!fontNode.attribute("bold").isEmpty())
        {
            font.setBold(fontNode.attribute("bold").toInt());
        }

        font.setItalic(fontNode.attribute("italic").toInt());
        font.setUnderline(fontNode.attribute("underline").toInt());
        contentProperty.setFont(font);

        QDomElement fontColorNode = CellPropertyNode.elementsByTagName("fontColor").at(0).toElement();
        contentProperty.setFontColor(QColor(fontColorNode.attribute("Value")));

        //contentProperty.setIsNull(CellPropertyNode.attribute("isNull").toInt());
        TableCellProperty.insert(TableIndex, contentProperty);
    }
    QVariant variant;
    variant.setValue(TableCellProperty);
    return variant;
}

void XmlVariantInfo::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    variant_info data = value.value<variant_info>();
    _node.setAttribute("Type", "ReportDesign::variant_info");
    _node.setAttribute("ID", data.m_ID);
    _node.setAttribute("Name", data.m_name);
    _node.setAttribute("VariantType", data.m_type);
    saveBool(_node, "NameChange", data.m_nameChange);
    node()->appendChild(_node);
}

QVariant XmlVariantInfo::loadValue()
{
    variant_info data;
    data.m_ID = node()->attribute("ID");
    data.m_name = node()->attribute("Name");
    data.m_type = node()->attribute("VariantType");
    data.m_nameChange =  node()->attribute("NameChange").toInt();
    QVariant variant;
    variant.setValue(data);
    return variant;
}

void XmlTableVariantInfo::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    QMap<QPair<int, int>, variant_info> TableVariantInfo = value.value<QMap<QPair<int, int>, variant_info>>();
    _node.setAttribute("Type", "QMap<QPair<int,int>,ReportDesign::variant_info>");

    for (auto it = TableVariantInfo.begin(); it != TableVariantInfo.end(); ++it)
    {
        QDomElement CellVariant = doc()->createElement("CellVariant");
        CellVariant.setAttribute("rowIndex", it.key().first);
        CellVariant.setAttribute("columnIndex", it.key().second);
        CellVariant.setAttribute("ID", it.value().m_ID);
        CellVariant.setAttribute("Name", it.value().m_name);
        CellVariant.setAttribute("VariantType", it.value().m_type);
        saveBool(CellVariant, "NameChange", it.value().m_nameChange);
        //saveBool(CellProperty,"isNull",it.value().isNull());
        _node.appendChild(CellVariant);
    }
    node()->appendChild(_node);
}

QVariant XmlTableVariantInfo::loadValue()
{
    QMap<QPair<int, int>, variant_info> TableCellVariant;
    QDomNodeList arrayElements = node()->elementsByTagName("CellVariant");

    for (int i = 0; i < arrayElements.count(); ++i)
    {
        QDomElement CellVariantNode = arrayElements.at(i).toElement();
        QPair<int, int> TableIndex(CellVariantNode.attribute("rowIndex").toInt(), CellVariantNode.attribute("columnIndex").toInt());
        variant_info VariantInfo;

        VariantInfo.m_ID = CellVariantNode.attribute("ID");
        VariantInfo.m_name = CellVariantNode.attribute("Name");
        VariantInfo.m_type = CellVariantNode.attribute("VariantType");
        VariantInfo.m_nameChange = CellVariantNode.attribute("NameChange").toInt();
        TableCellVariant.insert(TableIndex, VariantInfo);
    }

    QVariant variant;
    variant.setValue(TableCellVariant);
    return variant;
}

void XmlConnectTableMap::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    QMap<int, ConnectTableInfo> ConnectTableMap = value.value<QMap<int, ConnectTableInfo>>();
    _node.setAttribute("Type", "QMap<int,ReportDesign::ConnectTableInfo>");

    for (auto it = ConnectTableMap.begin(); it != ConnectTableMap.end(); ++it)
    {
        QDomElement ConnectTableElement = doc()->createElement("ConnectTableList");
        ConnectTableInfo ConnectTableList = it.value();
        for (auto tableID : ConnectTableList.tableIDList)
        {
            QDomElement TableIDElement = doc()->createElement("TableID");
            TableIDElement.setAttribute("ID", tableID);
            ConnectTableElement.appendChild(TableIDElement);
        }
        _node.appendChild(ConnectTableElement);
    }
    node()->appendChild(_node);
}

QVariant XmlConnectTableMap::loadValue()
{
    QDomNodeList ConnectTableList = node()->elementsByTagName("ConnectTableList");
    QMap<int, ConnectTableInfo> ConnectTableMap;

    // 被关联的table集合
    for (int i = 0; i < ConnectTableList.count(); ++i)
    {
        // 关联的table集合
        QDomNodeList tableList = ConnectTableList.at(i).toElement().elementsByTagName("TableID");
        ConnectTableInfo tableIDList;
        for (int j = 0; j < tableList.count(); ++j)
        {
            tableIDList.tableIDList.push_back(tableList.at(j).toElement().attribute("ID").toInt());
        }
        ConnectTableMap.insert(tableIDList.tableIDList.front(), tableIDList);
    }

    QVariant variant;
    variant.setValue(ConnectTableMap);
    return variant;
}

}



