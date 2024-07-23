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
#ifndef DESIGN_WIDGET_HPP
#define DESIGN_WIDGET_HPP

#include <QWidget>
#include <QTabWidget>
#include <QSettings>
#include <QPushButton>
#include <QGraphicsView>
#include "item/page_scene.hpp"
#include "item/page_view.hpp"
#include "report_manager.hpp"

namespace ReportDesign
{

class design_widget : public QWidget
{
    Q_OBJECT
public:
    explicit design_widget(QWidget *parent = nullptr);
    void startInsertMode(const QString &itemType);

    // 缩放
    void scale(qreal sx, qreal sy);
    // 拷贝
    void copy();
    //
    void variantLock();
    // 粘贴
    void paste();
    // 剪切
    void cut();
    // 撤销
    void undo();
    // 反撤销
    void redo();

    // 页面设置
    void PageSeting();

    bool isCanUndo();
    bool isCanRedo();

    // 创建新模板文件
    void newReport(QString module, QString moduleName);
    void addPage();
    bool isNeedToSave();
    void clear();
    void newReportButtonHide();
    report_manager *report();

    // 预览
    void previewReport();

    // 导出（PDF）
    void exportReport();

    // 示例数据
    void SimulatedDataOn();
    void SimulatedDataOff();

    // 水印
    void setWatermarkData(QString watermarkText, const QImage &image);
    void watermarkOn();
    void watermarkOff();
    bool reportWatermarkIsEmpty();

    void SetDefaultFileName(QString FileName)
    {
        m_defaultFileName = FileName;
    }
    void setFileNameEmpty()
    {
        m_fileName.clear();
    }

    int pageCount();

    void deleteCurrentPage();

    QString fileName();
    QString module();
    QString moduleName();
    void setBackgroundColor(QColor color);
private:
    void initTabWidget();
    void createTabs();
    void connectPage(page_scene *page);
    page_view *createPageView(page_scene *page);
    page_scene *createStartPage(QString module, QString moduleName);

public:
    page_scene *activePage();
    QGraphicsView *activeView();

signals:
    void pageAdded(page_scene *page);
    void activePageChanged();
    void itemInserted(page_scene *, QPointF, QString);

    // 都不选中
    void noItemSelected();

    // 选中一个
    void itemSelected(base_design_item *item);

    // 选中多个
    void multiItemSelected();

    //
    void commandHistoryChanged();

    // 新建按钮按下
    void newReportButtonClicked();

    void pageDeleted();

public slots:
    // 对象选中发生改变时调用，用于选中item时,通知属性窗口加载item属性
    void slotSelectionChanged();

    // 页眉页脚同步操作
    void slotSynchronization();

    // 删除选中的元件
    void deleteSelectedItems();

    void bringToFront();
    void sendToBack();
    void alignToLeft();
    void alignToRight();
    void alignToVCenter();
    void alignToTop();
    void alignToBottom();
    void alignToHCenter();
    void sameHeight();
    void sameWidth();
    bool save();
    bool saveToFile(const QString &fileName);
    // 0为成功，非0为错误码
    int loadFile(const QString &fileName);
    int loadDefalutFile(const QString &fileName, const QString &module, const QString &deviceID);
    void slotPreviewReportWidgetDesdroy();
private:
    QTabWidget *m_tabWidget;
    QPushButton *m_newReport;
    QSettings *m_settings;
    report_manager *m_report;
    // page_scene* m_pages;

    // 刻度单位
    base_design_item::UnitType m_defaultUnits;

    // 模板文件名
    QString m_fileName;

    // 用户输入的
    QString m_defaultFileName;

    // 当前选择模块名
    //QString Module;

    // 当前设置的水印数据。
    watermark_item *m_watermark;
};
}
#endif // DESIGN_WIDGET_H
