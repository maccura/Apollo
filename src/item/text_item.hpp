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
#ifndef TEXTITEM_H
#define TEXTITEM_H
#include <QGraphicsTextItem>
#include <QtGui>
#include <QLabel>
#include <QTextDocument>
#include <QtGlobal>
#include "data_manager/base_homogeneous_variant.hpp"
#include "base_design_item.hpp"
namespace ReportDesign
{

class Tag;
class TextItem : public base_design_item
{
    Q_OBJECT
    // 文本对齐水平对齐：左对齐，右对齐，居中，垂直对齐：底端对齐，顶端对齐，垂直居中
    Q_PROPERTY(Qt::Alignment ALIGNMENT_PROPERTY READ alignment WRITE setAlignment)
    // 字体
    Q_PROPERTY(QFont FONT_PROPERTY READ font WRITE setTextItemFont)
    // 字体颜色
    Q_PROPERTY(QColor FONT_COLOR_PROPERTY READ fontColor WRITE setFontColorProperty)
    // 变量, content目前是存在variant.name当中的
    Q_PROPERTY(variant_info variant READ variant WRITE setVariant)
    // 上下左右边框
    Q_PROPERTY(BorderLines BORDER_PROPERTY READ borderLines WRITE setBorderLinesFlags)
    Q_PROPERTY(qreal BORDERLINESIZE_PROPERTY READ borderLineSize WRITE setBorderLineSize)
    Q_PROPERTY(QColor BORDERCOLOR_PROPERTY READ borderColor WRITE setBorderColor)
public:
    enum AutoWidth {NoneAutoWidth, MaxWordLength, MaxStringLength};
    enum AngleType {Angle0, Angle90, Angle180, Angle270, Angle45, Angle315};
    enum ValueType {Default, DateTime, Double};
#if QT_VERSION >= 0x050500
    Q_ENUM(AutoWidth)
    Q_ENUM(AngleType)
    Q_ENUM(ValueType)
#else
    Q_ENUMS(AutoWidth)
    Q_ENUMS(AngleType)
    Q_ENUMS(ValueType)
#endif

    void Init();
    TextItem(QString itemTypeName = TEXT_ITEM_TYPE, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~TextItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QString content() const;
    void setContent(const QString &value);

    void setAlignment(Qt::Alignment value);
    Qt::Alignment alignment()
    {
        return m_alignment;
    }
    void setFontColorProperty(QColor value);
    void geometryChangedEvent(QRectF, QRectF);

    bool canBeSplitted(int height) const;
    bool isSplittable() const
    {
        return true;
    }
    bool isEmpty() const
    {
        return m_strText.trimmed().isEmpty();
    }
    base_design_item *cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent);
    base_design_item *cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent);
    base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0);
    base_design_item *cloneEmpty(int height, QObject *owner, QGraphicsItem *parent);
    void objectLoadFinished();

    void setTextItemFont(QFont value);
    virtual QWidget *defaultEditor() override;

    virtual void reloadVariantName(const VariantNameMap &variantName);
    virtual void renderPrintData(const PrintDataMap &PrintData) override;
    virtual bool printDataIsEmpty();
    virtual void clearPrintData() override;
    virtual bool variantIsEmpty() override;
    virtual void clearVariant() override;
    AngleType angle() const;
    void setAngle(const AngleType &value);
    int foregroundOpacity()
    {
        return m_foregroundOpacity;
    }
    void setForegroundOpacity(int value);
    bool underlines()
    {
        return m_underlines;
    }
    void setUnderlines(bool value);
    QSizeF textSize()
    {
        return m_textSize;
    }
    typedef QSharedPointer<QTextDocument> TextPtr;

    qreal textIndent() const;
    void setTextIndent(const qreal &textIndent);
    Qt::LayoutDirection textLayoutDirection() const;
    void setTextLayoutDirection(const Qt::LayoutDirection &textLayoutDirection);
    variant_info variant();
public slots:
    void setVariant(variant_info variant);

    void setVariantName(variant_info variant);
protected:
    void updateLayout();
    bool isNeedExpandContent() const;
    QString replaceBR(QString text) const;
    QString replaceReturns(QString text) const;
    int fakeMarginSize() const;
    QString getTextPart(int height, int skipHeight);
    //void restoreLinksEvent();
protected:


    void initTextSizes() const;
    void setTextFont(TextPtr text, const QFont &value) const;
    void adaptFontSize(TextPtr text) const;
    QString formatDateTime(const QDateTime &value);
    QString formatNumber(const double value);
    QString formatFieldValue();
    QString extractText(QTextBlock &curBlock, int height);
    TextPtr textDocument() const;
protected:
    QString m_strText;
    Qt::Alignment m_alignment;
    bool m_autoHeight;
    AutoWidth m_autoWidth;
    QSizeF mutable m_textSize;
    qreal  mutable m_firstLineSize;
    AngleType m_angle;
    int m_foregroundOpacity;
    bool m_underlines;
    bool m_adaptFontToSize;
    bool m_trimValue;
    int m_lineSpacing;
    int m_underlineLineSize;
    QString m_format;
    ValueType m_valueType;
    qreal m_textIndent;
    Qt::LayoutDirection m_textLayoutDirection;
    bool m_hideIfEmpty;
    int m_fontLetterSpacing;
    variant_info m_variant;
};

}
#endif // TEXTITEM_H
