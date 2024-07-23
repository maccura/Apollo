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
#ifndef PREVIEW_REPORT_WIDGET_HPP
#define PREVIEW_REPORT_WIDGET_HPP

#include <QDialog>
#include <QSharedPointer>
#include <QGraphicsView>
#include <QPushButton>
#include <QWheelEvent>
#include <QPrinterInfo>
#include <QKeyEvent>
#include "mask_dialog/base_mask_dialog.hpp"
#include "mask_dialog/base_push_button.hpp"
#include "item/page_item.hpp"
#include "item/page_scene.hpp"
namespace ReportDesign
{

class preview_page_view: public QGraphicsView
{
    Q_OBJECT
public:
    preview_page_view(QWidget *parent = NULL);
    preview_page_view(QGraphicsScene *scene, QWidget *parent = NULL);
    void setMaxScale(qreal maxScale);
    void setMinScale(qreal minScale);
protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private:
    bool  m_ctrolFlag;
    qreal m_minScale;
    qreal m_maxScale;
};

class preview_report_widget: public base_mask_widget
{
    Q_OBJECT
public:
    preview_report_widget();
    void init();
    ~preview_report_widget();
    void setPages(ReportPages pages);
    static void printPdf(ReportPages pages);
    static bool isPrintPdf(QPrinterInfo &info);
    void setEditePage(ReportPages pages)
    {
        m_edite_reportPages = pages;
    }
public slots:
    void slotFitPage();
    void slotFitPageWidth();
    void slotPrint();
    void slotPrintPDF();
    void slotZoomIn();
    void slotZoomOut();
protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        Q_UNUSED(event);
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
        Q_UNUSED(event);
    }
private:
    void setScalePercent(qreal percent);
    static void saveToQtPDF(ReportPages pages);

private:
    page_scene *m_previewPage;
    ReportPages m_reportPages;
    preview_page_view *m_pageView;
    image_button *m_close;
    CircularButton *m_zoomInButton;
    CircularButton *m_zoomOutButton;

    // 打印
    QPushButton *m_print;

    // 导出PDF
    QPushButton *m_printPDF;

    // 返回
    QPushButton *m_back;


    ReportPages m_edite_reportPages;

};

}
#endif // PREVIEW_REPORT_WIDGET_HPP
