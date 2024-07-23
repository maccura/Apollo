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
#ifndef DESIGN_MAIN_WINDOW_HPP
#define DESIGN_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QSignalMapper>
#include <QToolButton>
#include <QToolBar>
#include <QDockWidget>
#include <QSignalMapper>
#include <QSizeGrip>
#include <QHash>
#include "mask_dialog/base_push_button.hpp"
#include "design_widget.hpp"
#include "mask_dialog/watermark_setting_dialog.hpp"
#include "mask_dialog/base_push_button.hpp"
#include "object_prop/object_prop_widget.hpp"
namespace ReportDesign
{
class design_main_window : public QMainWindow
{
    Q_OBJECT
public:
    design_main_window(QWidget *parent = 0);
    ~design_main_window();
    void DeviceTypeSelect();
    virtual void paintEvent(QPaintEvent *event) override;
public slots:
    //
    void slotDeviceType(QString DeviceType);
    void slotModuleType(variant_info DeviceType, QString fileName, QString moduleFileName);
    void slotAddNewItem();
    void slotCloseWindow();
    void slotShowAbout();

    // 保存文件
    void slotSaveReport();
    void slotSaveReportAs();
    void slotLoadReport();

    void slotNewPage();
    void slotAddPage();
    void slotLanguage();

    // 点击item创建完成后，tool action to setCheckable(false);
    void slotItemInserted(page_scene *, QPointF, const QString &ItemType);
    // 处理选中一个元件
    void slotItemSelected(base_design_item *item);
    // 处理选中多个元件
    void slotMultiItemSelected();
    // 都不选中
    void slotNoItemSelected();
    // 删除选中元件
    void slotDelete();

    // 删除当前文件页
    void slotDeletePage();

    // 视图放大
    void slotZoomIn();
    // 视图缩小
    void slotZoomOut();
    // 预览
    void slotPreviewReport();
    // 导出(PDF)
    void slotExportReport();

    // 粘贴
    void slotPaste();
    // 复制
    void slotCopy();
    // 关联复制
    void slotVariantLock();

    // 剪切
    void slotCut();
    // 撤销
    void slotUndo();
    // 反撤销
    void slotRedo();
    // 命令提交
    void slotCommandHistoryChanged();

    // 示例数据开关
    void slotSimulatedDataOn();
    void slotSimulatedDataOff();

    // 水印开关
    void slotWatermarkOn();
    void slotWatermarkOff();

    // 水印设置
    void slotWatermarkWidgetOpen();
    void slotWatermarkSeting(QString text, QString imagePath);


    void slotPageDeleted();

    void slotPageAdded(page_scene *);
    void slotActivePageChanged();
    void slotLoadRecentFile(const QString fileName);

    QColor artboardBackground();
    void setArtboardBackground(QColor color);
private:
    void initMainWindow();
    void initReportEditor();
    void createReportItemDock();
    void createMainToolDock();
    void createObjectInspectorDock();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void createMainMenuActions();
    void createMainToolButton();
    void createActions();
    QList<QLabel *> createReportToolActions();
    bool checkNeedToSave();
    void updateRedoUndo();
    // 加载系统提供的默认模板（提前设计好的样板）
    void loadDefauleReport(QString fileName, QString preFileName, QString ModuleType);

    void setReportInfoToTitle(QString moduleName, QString fileName);

    QSettings *settings();
    void setSettings(QSettings *value);
    void restoreSetting();
    void addRecentFile(const QString &fileName);
    void createRecentFilesMenu();
    void removeNotExistedRecentFiles();
    void removeNotExistedRecentFilesFromMenu(const QString &fileName);
    void writeState();
    void setPageItemObject(QObject *object);
    void setItemObject(QObject *object);
    void setItemObjects(QList<QObject *> object);
private:
    // 设计编辑的主区域
    design_widget *m_design_widget;

    // page屬性窗口
    object_prop_widget *m_page_prop_widget;
    // item属性窗口
    object_prop_widget *m_item_prop_widget;

    // 示例数据
    switchButton *m_simulatedData;

    // 水印设置
    switchButton *m_watermark;
    QPushButton *m_watermarkSeting;
    // 水印設置窗口
    watermark_setting_dialog *m_watermarkSetingDlg;

    // 顶部dock部件
    QDockWidget *m_mainTooldock;
    QToolButton *m_infoButton;
    QMenu *m_infoMenu;
    base_Label *m_reportInfo;
    image_button *m_zoomIn;
    image_button *m_zoomOut;
    image_button *m_copy;
    image_button *m_connectCopy;
    image_button *m_paste;
    image_button *m_cut;
    image_button *m_undo;
    image_button *m_delete;
    image_button *m_pageSet;
    image_button *m_addPage;
    image_button *m_deletePage;
    base_push_button *m_previewReport;
    gbase_push_button *m_exportReport;
    image_button *m_widgetMin;
    image_button *m_widgetMax;
    image_button *m_widgetClose;

    // m_infoMenu 菜单选项
    QAction *m_about;
    QAction *m_saveReportAction;
    QAction *m_saveReportAsAction;
    QAction *m_loadReportAction;
    QAction *m_exitAction;
    QAction *m_newPageAction;
    QAction *m_Language;
    QMenu *m_recentFilesMenu;
    QSignalMapper *m_recentFilesSignalMap;

    // 左边dock item选择按钮
    base_Label *m_ReportItemDockTitle;
    Item_button *m_newTextItemAction;
    Item_button *m_newGroupTextItemAction;
    Item_button *m_newShapeItemAction;
    Item_button *m_newTableItemAction;
    Item_button *m_newSVGItemAction;
    Item_button *m_newImageItemAction;
    Item_button *m_newPageNumberItemAction;
    Item_button *m_colHeaderTableAction;

    bool m_reportItemIsLocked;
    QVector<QDockWidget *> m_pageEditors;
    QMap<QString, QDateTime> m_recentFiles;
    QString m_deviceType;
    bool dragging;
    Item_button *m_cruItem;
    QSettings *m_settings;
    bool m_ownedSettings;

    // 窗口移动
    // 鼠标按下时的位置
    QPoint mousePressPos;
    // 窗口左上角的位置
    QPoint windowPos;

    // resize
    QSizeGrip *topLeftGrip;
    QSizeGrip *topRightGrip;
    QSizeGrip *bottomLeftGrip;
    QSizeGrip *bottomRightGrip;
    QSizeGrip *rightEdgeGrip;
    QSizeGrip *bottomEdgeGrip;
    QSize cornerOffset;
    QSize edgeOffset;

    QColor m_backColor;

};
}
#endif // DESIGN_MAIN_WINDOW_HPP
