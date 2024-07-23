/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
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
#include <QtGui>
#include <QTextLayout>
#include <QLocale>
#include <QMessageBox>
#include <QMenu>
#include <math.h>
#include <QVariant>
#include "page_scene.hpp"
#include "text_item.hpp"
#include "design_elements_factory.hpp"
#include "global.hpp"
#include "mask_dialog/variable_edit_dialog.hpp"

namespace
{

ReportDesign::base_design_item *createTextItem(QObject *owner, ReportDesign::base_design_item  *parent)
{
    return new ReportDesign::TextItem(ReportDesign::TEXT_ITEM_TYPE, owner, parent);
}
bool registred = ReportDesign::DesignElementsFactory::instance().
                 registerCreator(ReportDesign::TEXT_ITEM_TYPE, ReportDesign::ItemAttribs(QObject::tr("Text Item"),
                                 ReportDesign::TEXT_ITEM_TYPE), createTextItem);
}

namespace ReportDesign
{

TextItem::TextItem(QString itemTypeName, QObject *owner, QGraphicsItem *parent)
    : base_design_item(itemTypeName, owner, parent), m_angle(Angle0), m_trimValue(true),
      m_textIndent(0),
      m_textLayoutDirection(Qt::LayoutDirectionAuto), m_hideIfEmpty(false), m_fontLetterSpacing(0)
{
    // 找到Page节点，保持和Page的font一致
    page_item *pageItem = dynamic_cast<page_item *>(parent);
    base_design_item *parentItem = dynamic_cast<base_design_item *>(parent);
    while (!pageItem && parentItem)
    {
        parentItem = dynamic_cast<base_design_item *>(parentItem->parentItem());
        pageItem = dynamic_cast<page_item *>(parentItem);
    }

    if (pageItem)
    {
        QFont defaultFont = pageItem->font();
        setFont(defaultFont);
    }
    Init();
}

TextItem::~TextItem() {}

int TextItem::fakeMarginSize() const
{
    return marginSize() + 5;
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(style);


    TextPtr text = textDocument();

    painter->save();

    setupPainter(painter);
    prepareRect(painter, style, widget);

    QSizeF tmpSize = rect().size() - text->size();

    if (!painter->clipRegion().isEmpty())
    {
        QRegion clipReg = painter->clipRegion().xored(painter->clipRegion().subtracted(rect().toRect()));
        painter->setClipRegion(clipReg);
    }
    else
    {
        painter->setClipRect(rect());
    }

    qreal hOffset = 0, vOffset = 0;
    switch (m_angle)
    {
    case Angle0:
        hOffset = fakeMarginSize();
        if ((tmpSize.height() > 0) && (m_alignment & Qt::AlignVCenter))
        {
            vOffset = tmpSize.height() / 2;
        }
        if ((tmpSize.height() > 0) && (m_alignment & Qt::AlignBottom)) // allow html
        {
            vOffset = tmpSize.height();
        }
        painter->translate(hOffset, vOffset);
        break;
    case Angle90:
        hOffset = width() - fakeMarginSize();
        vOffset = fakeMarginSize();
        if (m_alignment & Qt::AlignVCenter)
        {
            hOffset = (width() - text->size().height()) / 2 + text->size().height();
        }

        if (m_alignment & Qt::AlignBottom)
        {
            hOffset = (text->size().height());
        }
        painter->translate(hOffset, vOffset);
        painter->rotate(90);
        break;
    case Angle180:
        hOffset = width() - fakeMarginSize();
        vOffset = height() - fakeMarginSize();
        if ((tmpSize.width() > 0) && (m_alignment & Qt::AlignVCenter))
        {
            vOffset = tmpSize.height() / 2 + text->size().height();
        }
        if ((tmpSize.height() > 0) && (m_alignment & Qt::AlignBottom))
        {
            vOffset = (text->size().height());
        }
        painter->translate(hOffset, vOffset);
        painter->rotate(180);
        break;
    case Angle270:
        hOffset = fakeMarginSize();
        vOffset = height() - fakeMarginSize();
        if (m_alignment & Qt::AlignVCenter)
        {
            hOffset = (width() - text->size().height()) / 2;
        }

        if (m_alignment & Qt::AlignBottom)
        {
            hOffset = (width() - text->size().height());
        }
        painter->translate(hOffset, vOffset);
        painter->rotate(270);
        break;
    case Angle45:
        painter->translate(width() / 2, 0);
        painter->rotate(45);
        text->setTextWidth(sqrt(2 * (pow(width() / 2, 2))));
        break;
    case Angle315:
        painter->translate(0, height() / 2);
        painter->rotate(315);
        text->setTextWidth(sqrt(2 * (pow(height() / 2, 2))));
        break;
    }

    int lineHeight = painter->fontMetrics().height();
    qreal curpos = 0;

    if (m_underlines)
    {
        QPen pen = painter->pen();
        pen.setWidth(m_underlineLineSize);
        painter->setPen(pen);
    }

    painter->setOpacity(qreal(foregroundOpacity()) / 100);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, fontColor());

    for (QTextBlock it = text->begin(); it != text->end(); it = it.next())
    {
        it.blockFormat().setLineHeight(m_lineSpacing, QTextBlockFormat::LineDistanceHeight);
        for (int i = 0; i < it.layout()->lineCount(); i++)
        {
            QTextLine line = it.layout()->lineAt(i);
            if (m_underlines)
            {
                painter->drawLine(QPointF(0, line.rect().bottomLeft().y()), QPoint(rect().width(), line.rect().bottomRight().y()));
                lineHeight = line.height() + m_lineSpacing;
                curpos = line.rect().bottom();
            }
        }
    }

    text->documentLayout()->draw(painter, ctx);

    if (m_underlines)
    {
        if (lineHeight < 0)
        {
            lineHeight = painter->fontMetrics().height();
        }
        for (curpos += lineHeight; curpos < rect().height(); curpos += lineHeight)
        {
            painter->drawLine(QPointF(0, curpos), QPoint(rect().width(), curpos));
        }
    }

    painter->restore();
    base_design_item::paint(painter, style, widget);
    drawMasterMarkColor(painter);
}

void TextItem::Init()
{
    m_autoWidth = NoneAutoWidth;
    m_alignment = Qt::AlignLeft | Qt::AlignVCenter;
    m_autoHeight = false;
    m_textSize = QSizeF();
    m_firstLineSize = 0;
    m_foregroundOpacity = 100;
    m_underlines = false;
    m_adaptFontToSize = false;
    m_underlineLineSize = 1;
    m_lineSpacing = 1;
    m_valueType = Default;
    m_variant.m_name = tr("TextItem");
    m_strText = m_variant.m_name;
}

void TextItem::setContent(const QString &value)
{
    if (m_strText.compare(value) != 0)
    {
        QString oldValue = m_strText;

        m_strText = value;

        if (!isLoading())
        {
            update(rect());
            notify("content", oldValue, value);
        }
    }
}

QString TextItem::content() const
{
    return m_strText;
}

void TextItem::updateLayout()
{

}

bool TextItem::isNeedExpandContent() const
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rx("$*\\{[^{]*\\}");
#else
    bool result = false;
    QRegularExpression rx("\\$*\\{[^{]*\\}");
    result = content().contains(rx) || isContentBackedUp();
    return result;
#endif
    return content().contains(rx); /*|| isContentBackedUp();*/
}

QString TextItem::replaceBR(QString text) const
{
    return text.replace("<br/>", "\n");
}

QString TextItem::replaceReturns(QString text) const
{
    QString result = text.replace("\r\n", "<br/>");
    result = result.replace("\n", "<br/>");
    return result;
}

void TextItem::setTextFont(TextPtr text, const QFont &value) const
{
    text->setDefaultFont(value);
    if ((m_angle == Angle0) || (m_angle == Angle180))
    {
        text->setTextWidth(rect().width() - fakeMarginSize() * 2);
    }
    else
    {
        text->setTextWidth(rect().height() - fakeMarginSize() * 2);
    }
}

variant_info TextItem::variant()
{
    return m_variant;
}

void TextItem::setVariant(variant_info variant)
{
    m_variant = variant;
    m_strText = m_variant.m_name;
}

void TextItem::adaptFontSize(TextPtr text) const
{
    QFont _font = transformToSceneFont(font());
    do
    {
        //        qApp->processEvents();
        setTextFont(text, _font);
        if (_font.pixelSize() > 2)
        {
            _font.setPixelSize(_font.pixelSize() - 1);
        }
        else
        {
            break;
        }
    }
    while (text->size().height() > this->height() || text->size().width() > (this->width()) - fakeMarginSize() * 2);
}

void TextItem::initTextSizes() const
{
    TextPtr text = textDocument();
    m_textSize = text->size();
    if (text->begin().isValid() && text->begin().layout()->lineAt(0).isValid())
    {
        m_firstLineSize = text->begin().layout()->lineAt(0).height();
    }
}

QString TextItem::formatDateTime(const QDateTime &value)
{
    if (m_format.isEmpty())
    {
        return value.toString();
    }

    return value.toString(m_format);
}

QString TextItem::formatNumber(const double value)
{
    QString str = QString::number(value);

    if (m_format.contains("%"))
    {
#if QT_VERSION < 0x050500
        str.sprintf(m_format.toStdString().c_str(), value);
#else
        str.asprintf(m_format.toStdString().c_str(), value);
#endif
        str = str.replace(",", QLocale::system().groupSeparator());
        str = str.replace(".", QLocale::system().decimalPoint());
    }

    return str;
}

QString TextItem::formatFieldValue()
{
    if (m_format.isEmpty())
    {
        return m_varValue.toString();
    }

    QVariant value = m_varValue;

    if (m_valueType != Default)
    {
        switch (m_valueType)
        {
        case DateTime:
        {
            QDateTime dt = QDateTime::fromString(value.toString(), Qt::ISODate);
            value = (dt.isValid() ? QVariant(dt) : m_varValue);
            break;
        }
        case Double:
        {
            bool bOk = false;
            double dbl = value.toDouble(&bOk);
            value = (bOk ? QVariant(dbl) : m_varValue);
        }
        default:
            break;
        }
    }

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    switch (value.type())
    {
    case QVariant::Date:
    case QVariant::DateTime:
        return formatDateTime(value.toDateTime());
    case QVariant::Double:
        return formatNumber(value.toDouble());
    default:
        return value.toString();
    }
#else
    switch (value.typeId())
    {
    case QMetaType::QDate:
    case QMetaType::QDateTime:
        return formatDateTime(value.toDateTime());
    case QMetaType::Double:
        return formatNumber(value.toDouble());
    default:
        return value.toString();
    }
#endif

}

TextItem::TextPtr TextItem::textDocument() const
{
    TextPtr text(new QTextDocument);
    QString content = m_trimValue ? m_strText.trimmed() : m_strText;

    text->setPlainText(content);

    QTextOption to;
    to.setAlignment(m_alignment);
    to.setTextDirection(m_textLayoutDirection);

    if (m_autoWidth != MaxStringLength)
        if (m_adaptFontToSize && (!(m_autoHeight || m_autoWidth)))
        {
            to.setWrapMode(QTextOption::WordWrap);
        }
        else
        {
            to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        }
    else
    {
        to.setWrapMode(QTextOption::NoWrap);
    }

    text->setDocumentMargin(0);
    text->setDefaultTextOption(to);

    QFont _font = transformToSceneFont(font());
    if (m_adaptFontToSize && (!(m_autoHeight || m_autoWidth)))
    {
        adaptFontSize(text);
    }
    else
    {
        setTextFont(text, _font);
    }

    if (m_lineSpacing != 1 || m_textIndent != 0)
    {

        for (QTextBlock block = text->begin(); block.isValid(); block = block.next())
        {
            QTextCursor tc = QTextCursor(block);
            QTextBlockFormat fmt = block.blockFormat();
            fmt.setTextIndent(m_textIndent);
            if (fmt.lineHeight() != m_lineSpacing)
            {
                fmt.setLineHeight(m_lineSpacing, QTextBlockFormat::LineDistanceHeight);
            }
            tc.setBlockFormat(fmt);
        }

    }

    return text;

}

qreal TextItem::textIndent() const
{
    return m_textIndent;
}

void TextItem::setTextIndent(const qreal &textIndent)
{
    if (m_textIndent != textIndent)
    {
        qreal oldValue = m_textIndent;
        m_textIndent = textIndent;
        update();
        notify("textIndent", oldValue, textIndent);
    }
}

Qt::LayoutDirection TextItem::textLayoutDirection() const
{
    return m_textLayoutDirection;
}

void TextItem::setTextLayoutDirection(const Qt::LayoutDirection &textLayoutDirection)
{
    if (m_textLayoutDirection != textLayoutDirection)
    {
        int oldValue = int(m_textLayoutDirection);
        m_textLayoutDirection = textLayoutDirection;
        update();
        notify("textLayoutDirection", oldValue, int(textLayoutDirection));
    }
}

void TextItem::geometryChangedEvent(QRectF, QRectF)
{}

void TextItem::setAlignment(Qt::Alignment value)
{
    if (m_alignment != value)
    {
        Qt::Alignment oldValue = m_alignment;
        m_alignment = value;
        //m_layout.setTextOption(QTextOption(m_alignment));
        if (!isLoading())
        {
            update(rect());
            notify("alignment", QVariant(oldValue), QVariant(value));
        }
    }
}

bool TextItem::canBeSplitted(int height) const
{
    QFontMetrics fm(font());
    return height > m_firstLineSize;
}

QString TextItem::extractText(QTextBlock &curBlock, int height)
{
    int curLine = 0;
    int linesHeight = 0;
    QString resultText;
    for (; curBlock != curBlock.document()->end() || curLine <= curBlock.lineCount(); curBlock = curBlock.next(), curLine = 0, resultText += '\n')
    {
        linesHeight += curBlock.blockFormat().topMargin();
        for (; curLine < curBlock.layout()->lineCount(); curLine++)
        {
            linesHeight += curBlock.layout()->lineAt(curLine).height() + m_lineSpacing;
            if (height > 0 && linesHeight > (height - borderLineSize() * 2))
            {
                goto loop_exit;
            }
            resultText += curBlock.text().mid(curBlock.layout()->lineAt(curLine).textStart(),
                                              curBlock.layout()->lineAt(curLine).textLength());
        }
    }
loop_exit:
    return resultText;
}

QString TextItem::getTextPart(int height, int skipHeight)
{

    QString resultText = "";
    TextPtr text = textDocument();
    text->size().height();
    QTextBlock curBlock = text->begin();
    QTextCursor cursor(text.data());
    cursor.movePosition(QTextCursor::Start);

    if (skipHeight > 0)
    {
        resultText = extractText(curBlock, skipHeight);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, resultText.length());
    }

    resultText = extractText(curBlock, height);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, resultText.length());

    resultText = cursor.selection().toPlainText();

    return resultText;
}

base_design_item *TextItem::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem *upperPart = dynamic_cast<TextItem *>(cloneItem(itemMode(), owner, parent));
    upperPart->setContent(getTextPart(height, 0));
    upperPart->initTextSizes();
    upperPart->setHeight(upperPart->textSize().height() + borderLineSize() * 2);
    return upperPart;
}

base_design_item *TextItem::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem *bottomPart = dynamic_cast<TextItem *>(cloneItem(itemMode(), owner, parent));
    bottomPart->setContent(getTextPart(0, height));
    bottomPart->initTextSizes();
    bottomPart->setHeight(bottomPart->textSize().height() + borderLineSize() * 2);
    return bottomPart;
}

base_design_item *TextItem::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new TextItem(ReportDesign::TEXT_ITEM_TYPE, owner, parent);
}

base_design_item *TextItem::cloneEmpty(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem *empty = dynamic_cast<TextItem *>(cloneItem(itemMode(), owner, parent));
    empty->setContent("");
    empty->setHeight(height);
    return empty;
}

void TextItem::objectLoadFinished()
{
    base_design_item::objectLoadFinished();
}

void TextItem::setTextItemFont(QFont value)
{
    if (font() != value)
    {
        QFont oldValue = font();
        value.setLetterSpacing(QFont::AbsoluteSpacing, m_fontLetterSpacing);
        setFont(value);
        if (!isLoading())
        {
            update();
        }
        notify("font", oldValue, value);
    }
}

QWidget *TextItem::defaultEditor()
{
    base_homogeneous_variant *GroupVariant = nullptr;
    base_homogeneous_variant *variant = nullptr;
    if (groupVariant() != variant_info())
    {
        GroupVariant =  device_variant_manager::instance().
                        HomogeneousVariant(page()->pageItem()->module(), VariantType::Group);
    }

    if (GroupVariant != nullptr)
    {
        variant = GroupVariant->typeChildVariants(groupVariant().m_ID, VariantType::Text);
    }
    else
    {
        variant = device_variant_manager::instance().
                  HomogeneousVariant(page()->pageItem()->module(), VariantType::Text);
    }

    variable_edit_dialog *dlg = new variable_edit_dialog(false, variant, m_variant);
    connect(dlg, SIGNAL(variable(variant_info)), this, SLOT(setVariant(variant_info)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    return dlg;
}

void TextItem::reloadVariantName(const VariantNameMap &variantName)
{
    // repeat
    variant_info GroupVariant = groupVariant();
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (variantName.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupMap = variantName.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            if (groupMap.contains(m_variant.m_ID))
            {
                variant_info var = groupMap.value(m_variant.m_ID).value<variant_info>();
                setVariantName(var);
            }
        }
    }
    else
    {
        VariantNameMap::const_iterator it = variantName.find(m_variant.m_ID);
        if (it != variantName.end())
        {
            QVariant Variant  = it.value();
            variant_info variantInfo = Variant.value<variant_info>();
            setVariantName(variantInfo);
        }
    }
}

void TextItem::renderPrintData(const PrintDataMap &PrintData)
{
    variant_info GroupVariant = groupVariant();
    QString TextValue;
    if (GroupVariant != variant_info()) // repeat元件
    {
        // 从组数据中获取数据
        if (PrintData.contains(GroupVariant.m_ID))
        {
            QMap<QString, QVariant> groupMap = PrintData.value(GroupVariant.m_ID).value<QMap<QString, QVariant>>();
            if (groupMap.contains(m_variant.m_ID))
            {
                QVector<QString> valueArr = groupMap.value(m_variant.m_ID).value<QVector<QString>>();
                if (valueArr.count() > groupDataIndex())
                {
                    TextValue = valueArr[groupDataIndex()];
                }
            }
        }
    }
    else // 非repeat元件
    {
        if (PrintData.contains(m_variant.m_ID))
        {
            TextValue = PrintData.value(m_variant.m_ID).toString();
        }
    }

    if (!TextValue.isEmpty())
    {
        m_strText = m_variant.m_name + TextValue;
    }
    else
    {
        m_strText = m_variant.m_name;
    }
}

bool TextItem::printDataIsEmpty()
{
    return m_strText == m_variant.m_name;
}


void TextItem::clearPrintData()
{
    m_strText = m_variant.m_name;
}

bool TextItem::variantIsEmpty()
{
    return m_variant.m_ID.isEmpty();
}

void TextItem::setFontColorProperty(QColor value)
{
    if (value != fontColor())
    {
        QColor oldValue = fontColor();
        setFontColor(value);
        notify("fontColor", oldValue, value);
        update();
    }
}

TextItem::AngleType TextItem::angle() const
{
    return m_angle;
}

void TextItem::setAngle(const AngleType &value)
{
    if (m_angle != value)
    {
        AngleType oldValue = m_angle;
        m_angle = value;
        if (!isLoading())
        {
            update();
            notify("angle", oldValue, value);
        }
    }
}

void TextItem::setForegroundOpacity(int value)
{
    if (value > 100)
    {
        value = 100;
    }
    else if (value < 0)
    {
        value = 0;
    }
    if (m_foregroundOpacity != value)
    {
        int oldValue = m_foregroundOpacity;
        m_foregroundOpacity = value;
        update();
        notify("foregroundOpacity", oldValue, value);
    }
}

void TextItem::setUnderlines(bool value)
{
    if (m_underlines != value)
    {
        bool oldValue = m_underlines;
        m_underlines = value;
        update();
        notify("underlines", oldValue, value);
    }
}

void TextItem::setVariantName(variant_info variant)
{
    if (!m_variant.m_nameChange)
    {
        m_variant.m_name =  variant.m_name;
        m_strText = m_variant.m_name;
    }
}

void TextItem::clearVariant()
{
    setVariant(variant_info());
}

} //namespace ReportDesign



