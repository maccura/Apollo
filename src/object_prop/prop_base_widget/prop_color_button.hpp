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
#ifndef PROP_COLOR_BUTTON_H
#define PROP_COLOR_BUTTON_H
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QColorDialog>
#include "prop_line_edit.hpp"
namespace ReportDesign
{

class color_button: public QPushButton
{
    Q_OBJECT
public:
    color_button(QWidget *parent = nullptr): QPushButton(parent) {}
    QColor color()
    {
        return m_color;
    }

    // 设置矩形颜色的函数
    void setColor(QColor color)
    {
        m_color = color;
        update(); // 更新按钮的绘制
    }

protected:
    // 重写paintEvent函数，在按钮上绘制矩形
    void paintEvent(QPaintEvent *event) override
    {
        QPushButton::paintEvent(event);
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(m_color);
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
private:
    QColor m_color;
};

class prop_color_button: public QWidget
{
    Q_OBJECT
public:
    prop_color_button(QWidget *parent = nullptr);
    void setMinimumHeight(int minHeight);
    void setColor(QColor color);
signals:
    void colorChange(QColor color);
public slots:
    void slotEditFinish();
    void slotColorButtonClicked();
protected:
    // 重写paintEvent函数，在按钮上绘制矩形
    void paintEvent(QPaintEvent *event) override;

private:
    color_button *m_colorButton;
    prop_line_edit *m_colorLineEdit;
    bool m_bodeFlag;
};
}
#endif // PROP_COLOR_BUTTON_H
