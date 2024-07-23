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
#ifndef REPORT_MANAGER_HPP
#define REPORT_MANAGER_HPP
#include <QObject>
#include "lrcollection.hpp"
#include "data_manager/print_data_manager.hpp"
#include <QList>
#include <QString>
#include <functional>
#include "item/page_scene.hpp"
#include "item/watermark_item.hpp"
#include "item/synchronization/item_synchronization.hpp"
namespace ReportDesign
{

class report_manager: public QObject, public ICollectionContainer
{
    Q_OBJECT
    Q_PROPERTY(ACollectionProperty pages READ fakeCollectionReader)
    Q_PROPERTY(QString module READ module WRITE setModule)
    Q_PROPERTY(QString moduleName READ moduleName WRITE setModuleName)
    Q_PROPERTY(bool translate READ translate WRITE setTranslate)
    Q_PROPERTY(QString deviceID READ deviceID WRITE setDeviceID)
    Q_PROPERTY(QString reportVesion READ reportVesion WRITE setReportVesion)
    Q_PROPERTY(QString printDataVesion READ printDataVesion WRITE setPrintDataVesion)
    Q_PROPERTY(QObject *printDataManager READ printDataManager)
public:
    using LogRecall = std::function<void(const QString &)>;
public:
    explicit report_manager(QString deviceID = "", QString module = "", QString moduleName = "", QObject *parent = 0);
    ~report_manager();
    page_scene      *appendPage(const QString &pageName = "");
    page_scene      *appendPage(page_item *basePage, const QString &pageName = "");
    bool deletePage(page_scene *page);
    page_scene      *pageAt(int index)
    {
        return (index <= (m_pages.count() - 1)) ? m_pages.at(index) : 0;
    }
    int    pageCount()
    {
        return m_pages.count();
    }

    virtual QObject    *createElement(const QString &, const QString &)override;
    virtual int         elementsCount(const QString &)               override;
    virtual QObject    *elementAt(const QString &, int index)        override;
    virtual void        collectionLoadFinished(const QString &)      override;

    bool isNeedToSave();
    bool saveToFile(const QString &fileName);

    QString module();
    void setModule(QString module);

    QString moduleName();
    void setModuleName(QString moduleName);

    bool translate()
    {
        return m_translate;
    }
    void setTranslate(bool translate)
    {
        m_translate = translate;
    }

    QString deviceID()
    {
        return m_deviceID;
    }
    void setDeviceID(QString deviceID)
    {
        m_deviceID = deviceID;
    }

    QString reportVesion()
    {
        return m_reportVesion;
    }
    void setReportVesion(QString reportVesion)
    {
        m_reportVesion = reportVesion;
    }

    QString printDataVesion()
    {
        return m_printDataVesion;
    }
    void setPrintDataVesion(QString printDataVesion)
    {
        m_printDataVesion = printDataVesion;
    }

    // 注册日志回调，用于打印接口库打印日志
    static void registerLogReall(LogRecall logRecall);

    // 记录日志
    static void logInfo(const QString &message);

    // 非自适应page,用于编辑模式下的page展示，在编辑区控件不做变长处理，不会新增页，显示效果和定长一致
    ReportPages editePages();

    // 自适应page,用于实际打印或示例数据预览等不可编辑的场景,控件和page会自适应.
    ReportPages renderPages();
    void reloadPages();

    // 语言翻译
    static VariantNameMap  genVariantNameMap(QString module, QString json);
    void reloadVariantName(const VariantNameMap &variantNameMap);

    //  渲染打印数据,传入打印数据
    void setRenderPrintData(QString json);

    //  清除打印数据
    void clearPrintData();

    //  清除所有指令队列，（撤销）
    void removeAllCmd();

    void setWatermark(watermark_item *watermark);
    void clearWatermark();

    bool watermarkIsEmpty();
    watermark_item *watermarkItem();

    print_data_manager *printDataManager()
    {
        return m_printDataManager;
    }
signals:
    void addConnectTableItem(int addTableItemID, int ConnectTableItemID);
    void deleteConnectTableItem(int deleteTableItemID, int ConnectTableItemID);

public slots:
    bool  slotLoadFromFile(const QString &fileName);
    void  slotIdFindTableItem(int tableItemID, base_design_item *&tableItem);
    void  slotItemSynchronization();
    void  slotSynchronizationItemDestroyed();
protected:
    //page_scene *createPage(const QString &pageName = "");
    page_scene *createPage(const QString &pageName = "", page_item *pageItem = nullptr);
    page_item  *createMasterMaskPage(page_item *pageItem);
private:
    QList<page_scene *> m_pages;
    QString m_module;
    QString m_moduleName;
    QString m_deviceID;
    // json数据协议版本号
    QString m_printDataVesion;
    print_data_manager *m_printDataManager;
    item_synchronization *m_item_synchronization;
    bool m_translate;
    ReportPages m_adaptivePages;
    // 模板报告协议
    QString m_reportVesion;
    // 日志回调对象
    static LogRecall m_logRecall;
};
}
#endif // REPORT_MANAGER_HPP
