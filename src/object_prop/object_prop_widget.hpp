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
#ifndef OBJECT_PROP_WIDGET_H
#define OBJECT_PROP_WIDGET_H
#include <QWidget>
#include <QVariant>
#include <QMutex>
#include <QVBoxLayout>
#include "base_prop_widget.hpp"
#include "item/base_design_item.hpp"

namespace ReportDesign
{
class object_prop_widget: public QWidget
{
    Q_OBJECT
public:
    object_prop_widget(QWidget *parent = nullptr);
    void setObject(QObject *object);
    const QObject *object();
    QList<QObject *> objects()
    {
        return m_objects;
    }
    void setMultiObjects(QList<QObject *> *list);
    void clearObjectsList();
    void updateProperty(const QString &propertyName);
public slots:
    void slotPropertyChanged(const QString &propertyName, const QVariant &oldValue, const QVariant &newValue);
    void slotPropertyUpdate(const QString &propertyName);
    void slotObjectDestroyed(QObject *obj);
private:
    void init();
    QVector<base_prop_widget *> genPropWidgets(QObject *object, QList<QObject *> *objects, QWidget *parent);
    CreatePropWidget propWidgetCreateor(QString property);
    void updatePropWidget();
    void clearPropWidget();
    bool creatorContains(QVector<CreatePropWidget> vector, CreatePropWidget Creator);
    // 按照Ui顺序排序
    QVector<base_prop_widget *> sortPropWidget(QMap<QString, base_prop_widget *> propWidgetMap);
    void setPropMap(QMap<QString, base_prop_widget *> map);
    void clearPropMap();
    void updatePropMap(QString property);
    bool propFilter(QObject *obj, QString property);
private:
    // 单选元件
    QObject *m_object;
    // 多选元件
    QList<QObject *> m_objects;
    // 属性-属性编辑部件
    QMutex m_propWidgetMapMutex;
    QMap<QString, base_prop_widget *> m_propWidgetMap;

    //
    QVBoxLayout *m_VLayout;

    // 属性窗口顺序表
    QVector<QString> m_propSortTable;
};
}
#endif // OBJECT_PROP_WIDGET_H
