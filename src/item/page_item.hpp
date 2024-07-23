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
#ifndef PAGEITEM_H
#define PAGEITEM_H
#include "base_design_item.hpp"
#include "watermark_item.hpp"
#include <QList>
#include <QColor>
#include <QtPrintSupport/QPrinter>
namespace ReportDesign
{

class ReportRender;
class  page_item : public base_design_item
{
    Q_OBJECT
    // 上下左右边距
    Q_PROPERTY(int topMargin READ topMargin WRITE setTopMargin DESIGNABLE false)
    Q_PROPERTY(int bottomMargin READ bottomMargin WRITE setBottomMargin DESIGNABLE false)
    Q_PROPERTY(int rightMargin READ rightMargin WRITE setRightMargin DESIGNABLE false)
    Q_PROPERTY(int leftMargin READ leftMargin WRITE setLeftMargin DESIGNABLE false)
    // 模块名(数据匹配，数据源模块名),不可在属性栏中修改，不提供WRITE
    Q_PROPERTY(QString module READ module WRITE setModule DESIGNABLE false)
    // 横向、纵向打印
    Q_PROPERTY(Orientation PAGE_ORIENTATION_PROPERTY READ pageOrientation WRITE setPageOrientation)
    // 纸张尺寸
    Q_PROPERTY(PageSize PAGE_SIZE_PROPERTY READ pageSize WRITE setPageSize)
    friend class ReportRender;
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    enum Orientation { Portrait = QPrinter::Portrait, Landscape = QPrinter::Landscape };

    enum PrintBehavior {Scale, Split};

    enum PageSize
    {
        A4 = QPrinter::A4, B5 = QPrinter::B5, Letter = QPrinter::Letter,
        Legal = QPrinter::Legal, Executive = QPrinter::Executive,
        A0 = QPrinter::A0, A1 = QPrinter::A1, A2 = QPrinter::A2, A3 = QPrinter::A3,
        A5 = QPrinter::A5, A6 = QPrinter::A6, A7 = QPrinter::A7, A8 = QPrinter::A8,
        A9 = QPrinter::A9, B0 = QPrinter::B0, B1 = QPrinter::B1, B10 = QPrinter::B10,
        B2 = QPrinter::B2, B3 = QPrinter::B3, B4 = QPrinter::B4, B6 = QPrinter::B6,
        B7 = QPrinter::B7, B8 = QPrinter::B8, B9 = QPrinter::B9, C5E = QPrinter::C5E,
        Comm10E = QPrinter::Comm10E, DLE = QPrinter::DLE, Folio = QPrinter::Folio,
        Ledger = QPrinter::Ledger, Tabloid = QPrinter::Tabloid, Custom = QPrinter::Custom,
        NPageSize = Custom
    };
#else
    enum Orientation { Portrait = QPageLayout::Portrait, Landscape = QPageLayout::Landscape };

    enum PrintBehavior {Scale, Split};

    enum PageSize
    {
        A4 = QPageSize::A4, B5 = QPageSize::B5, Letter = QPageSize::Letter,
        Legal = QPageSize::Legal, Executive = QPageSize::Executive,
        A0 = QPageSize::A0, A1 = QPageSize::A1, A2 = QPageSize::A2, A3 = QPageSize::A3,
        A5 = QPageSize::A5, A6 = QPageSize::A6, A7 = QPageSize::A7, A8 = QPageSize::A8,
        A9 = QPageSize::A9, B0 = QPageSize::B0, B1 = QPageSize::B1, B10 = QPageSize::B10,
        B2 = QPageSize::B2, B3 = QPageSize::B3, B4 = QPageSize::B4, B6 = QPageSize::B6,
        B7 = QPageSize::B7, B8 = QPageSize::B8, B9 = QPageSize::B9, C5E = QPageSize::C5E,
        Comm10E = QPageSize::Comm10E, DLE = QPageSize::DLE, Folio = QPageSize::Folio,
        Ledger = QPageSize::Ledger, Tabloid = QPageSize::Tabloid, Custom = QPageSize::Custom,
        NPageSize = Custom
    };
#endif

#if QT_VERSION >= 0x050500
    Q_ENUM(Orientation)
    Q_ENUM(PrintBehavior)
    Q_ENUM(PageSize)
#else
    Q_ENUMS(Orientation)
    Q_ENUMS(PrintBehavior)
    Q_ENUMS(PageSize)
#endif

    typedef QSharedPointer<page_item> Ptr;
    static page_item::Ptr create(QObject *owner);
    explicit page_item(QObject *owner = 0, QGraphicsItem *parent = 0);
    explicit page_item(const PageSize pageSize, const QRectF &rect, QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual ~page_item();
    virtual void paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual base_design_item *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0);
    virtual void geometryChangedEvent(QRectF newRect, QRectF);
    virtual QColor selectionColor() const;
    virtual QColor pageBorderColor() const;
    virtual QColor gridColor() const;
    virtual QRectF boundingRect() const;
    virtual void  reloadVariantName(const VariantNameMap &variantName) override;
    virtual void  renderPrintData(const PrintDataMap &PrintData) override;
    virtual void  clearPrintData() override;
    virtual void initFlags() override;
    virtual bool variantIsEmpty() override
    {
        return true;
    }
    PrintDataMap printData()
    {
        return m_printData;
    }

    QRect itemShowRect();

    // 可能存在不同的组设置为同一个组变量，但是一般设计中不会出现这种情况。
    QMap<int, QList<base_design_item *>> groupItemsByRepeatVariant(QString varID);
    QMap<int,  QList<base_design_item *>> repeatGroupItems();

    base_design_item *findTableItem(int ID);
    watermark_item *reportWatermark();

    bool watermarkIsEmpty();

    void setWatermark(watermark_item *watermark);
    void clearWatermark();

    void setPageNumber(int current, int total);

    void setItemMode(base_design_item::ItemMode mode);
    void clear();

    void increaseBandIndex(int startIndex);

    void relocateBands();

    //    int dataBandCount();

    QString fileName();
    void setFileName(QString fileName);

    QString module();
    void setModule(QString module);

    void setTopMargin(int value);
    int topMargin() const
    {
        return m_topMargin;
    }
    void setBottomMargin(int value);
    int bottomMargin() const
    {
        return m_bottomMargin;
    }
    void setLeftMargin(int value);
    int leftMargin() const
    {
        return m_leftMargin;
    }
    void setRightMargin(int value);
    int rightMargin() const
    {
        return m_rightMargin;
    }

    void setPageOrientation(Orientation value);
    Orientation pageOrientation() const
    {
        return m_pageOrientaion;
    }
    PageSize pageSize() const
    {
        return m_pageSize;
    }
    void setPageSize(const PageSize &size);

    void setGridStep(int value);
    int gridStep();
    void objectLoadFinished();
    bool fullPage() const;
    void setFullPage(bool fullPage);

    bool oldPrintMode() const;
    void setOldPrintMode(bool oldPrintMode);
    bool canContainChildren() const
    {
        return true;
    }
    bool canAcceptPaste() const
    {
        return true;
    }
    bool resetPageNumber() const;
    void setResetPageNumber(bool resetPageNumber);
    void updateSubItemsSize(RenderPass pass);

    void moveBandFromTo(int from, int to);


    bool isExtendedInDesignMode() const;
    void setExtendedInDesignMode(bool isExtendedInDesignMode);
    int  extendedHeight() const;
    void setExtendedHeight(int extendedHeight);

    bool isTOC() const;
    void setIsTOC(bool isTOC);
    bool getSetPageSizeToPrinter() const;
    void setSetPageSizeToPrinter(bool setPageSizeToPrinter);

    bool endlessHeight() const;
    void setEndlessHeight(bool endlessHeight);

    bool isPrintable() const;
    void setPrintable(bool printable);

    QString printerName() const;
    void setPrinterName(const QString &printerName);

    void placeTearOffBand();


    PrintBehavior printBehavior() const;
    void setPrintBehavior(const PrintBehavior &printBehavior);

    bool dropPrinterMargins() const;
    void setDropPrinterMargins(bool dropPrinterMargins);

    bool isEmpty() const;

    bool notPrintIfEmpty() const;
    void setNotPrintIfEmpty(bool notPrintIfEmpty);

    bool mixWithPriorPage() const;
    void setMixWithPriorPage(bool value);
    QSizeF  getRectByPageSize(const PageSize &size);
signals:
    void beforeFirstPageRendered();
    void afterLastPageRendered();
protected slots:
    void bandDeleted(QObject *band);
    void bandPositionChanged(QObject *object, QPointF newPos, QPointF oldPos);
    void bandGeometryChanged(QObject *object, QRectF newGeometry, QRectF oldGeometry);
    void setUnitTypeProperty(base_design_item::UnitType value);
protected:
    void    collectionLoadFinished(const QString &collectionName);
    QRectF &pageRect()
    {
        return m_pageRect;
    }
    void    updateMarginRect();
    void    initPageSize(const PageSize &size);
    void    initPageSize(const QSizeF &size);
    QColor  selectionMarkerColor()
    {
        return Qt::transparent;
    }
    void    preparePopUpMenu(QMenu &menu);
    void    processPopUpAction(QAction *action);
private:
    void paintGrid(QPainter *ppainter, QRectF rect);
    void initColumnsPos(QVector<qreal> &posByColumns, qreal pos, int dataColumnCount);
    void setWatermarkTop();
private:
    QString m_fileName;
    QString m_module;

    int m_topMargin;
    int m_bottomMargin;
    int m_leftMargin;
    int m_rightMargin;
    Orientation m_pageOrientaion;
    PageSize m_pageSize;
    QRectF m_pageRect;
    bool m_sizeChainging;

    bool m_fullPage;
    bool m_oldPrintMode;
    bool m_resetPageNumber;
    bool m_isExtendedInDesignMode;
    int  m_extendedHeight;
    bool m_isTOC;
    bool m_setPageSizeToPrinter;
    bool m_endlessHeight;
    bool m_printable;
    QString m_printerName;

    PrintBehavior m_printBehavior;
    bool m_dropPrinterMargins;
    bool m_notPrintIfEmpty;
    bool m_mixWithPriorPage;

    PrintDataMap m_printData;
};

typedef QList<page_item::Ptr> ReportPages;

}
#endif // PAGEITEM_H
