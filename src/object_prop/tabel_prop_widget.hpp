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
#ifndef TABEL_PROP_WIDGET_H
#define TABEL_PROP_WIDGET_H
#include "base_prop_widget.hpp"
#include "prop_base_widget/prop_combox.hpp"
#include "prop_base_widget/prop_spinbox_widget.hpp"
#include "prop_base_widget/prop_line_edit.hpp"
#include <QCheckBox>
namespace ReportDesign
{
class tabel_prop_widget: public base_prop_widget
{
    Q_OBJECT
public:
    explicit tabel_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    virtual void update(QString PropertyName) override;
    void init();
    void updateColCount(int colCount);
    void updateColWidth(int colWidth);
    void updateRowCount(int rowCount);
    void updateRowHeight(int rowHeight);
    void updateShowHeader(bool flag);
    void updateShowGrid(bool flag);
    void updateUnderLine(bool flag);
    void updateAutoFill(bool flag);
    void updateLock(int itemID, int connectID);
private:
    void setObjectProperty(QObject *object, QString propertyName, QVariant value);
    void setObjectsProperty(QList<QObject *> objects, QString propertyName, QVariant value);
    void connectRegister();
private:
    prop_line_edit *m_colCount;
    prop_spinbox_widget *m_colWidth;
    prop_line_edit *m_rowCount;
    prop_spinbox_widget *m_rowHeight;
    QCheckBox *m_showHeader;
    QCheckBox *m_showGrid;
private:
    //表格特有属性
    bool m_isTable;
    QCheckBox *m_underLine;
    QCheckBox *connect_lock;
    QCheckBox *m_autoFill;
};
}
#endif // TABEL_PROP_WIDGET_H
