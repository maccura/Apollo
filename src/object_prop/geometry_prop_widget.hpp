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
#ifndef GEOMETRY_PROP_WIDGET_H
#define GEOMETRY_PROP_WIDGET_H
#include "base_prop_widget.hpp"
#include "prop_base_widget/prop_line_edit.hpp"
#include "mask_dialog/base_push_button.hpp"
namespace ReportDesign
{
class geometry_prop_widget: public base_prop_widget
{
    Q_OBJECT
public:
    explicit geometry_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    ~geometry_prop_widget();
    virtual void update(QString PropertyName) override;
public slots:
    void slotGeometryLock(bool cheaked);
    void slotGeometryChange();
    void slotPosChange();
private:
    void UpdateGeometryLock(bool cheaked);
    void UpdateGeometry(QRect rect);
private:
    void init();
    QRectF rectInUnits(QRectF rect);
    qreal  valueInUnits(qreal value);
    qreal  valueInReportUnits(qreal value);

    bool isNumber();
private:
    prop_line_edit *m_XLineEdit;
    prop_line_edit *m_YLineEdit;
    prop_line_edit *m_WLineEdit;
    prop_line_edit *m_HLineEdit;
    image_button *m_lockButton;

};
}
#endif // GEOMETRY_PROP_WIDGET_H
