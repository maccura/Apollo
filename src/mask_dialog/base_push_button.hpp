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
#ifndef BASE_PUSH_BUTTON_HPP
#define BASE_PUSH_BUTTON_HPP
#include <QPushButton>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QSharedPointer>
#include <QEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QRadioButton>
#include <QFocusEvent>
#include "global.hpp"
namespace ReportDesign
{
class base_Label: public QLabel
{
    Q_OBJECT
public:
    base_Label(QWidget *p = nullptr);
    base_Label(QString text, QWidget *p = nullptr);
    void setText(QString text);
    QString text();
private:
    QString m_text;
};

class base_push_button: public QPushButton
{
    Q_OBJECT
public:
    base_push_button(QWidget *p = nullptr);
};

class gbase_push_button: public QPushButton
{
    Q_OBJECT
public:
    gbase_push_button(QWidget *p = nullptr);
};

class image_button: public QPushButton
{
    Q_OBJECT
public:
    image_button(QWidget *p = nullptr, QString Image = "", QString hoverUrl = "", QString Checked = "");
    void SetImage(QString Image, QString hoverUrl = "", QString Checked = "");
    QSharedPointer<QPixmap> pixmap()
    {
        return m_pixmap;
    }
    QString imageUrl()
    {
        return m_imageUrl;
    }
    QString hoverUrl()
    {
        return m_hoverUrl;
    }
    QString cheakUrl()
    {
        return m_cheakUrl;
    }
private:
    QSharedPointer<QPixmap> m_pixmap;
    QString m_imageUrl;
    QString m_hoverUrl;
    QString m_cheakUrl;
    QString m_StyleSheet;
};

class image_Radio_button : public QRadioButton
{
public:
    image_Radio_button(QWidget *parent = nullptr, QString Image = "", QString hoverUrl = "", QString Checked = "") : QRadioButton(parent)
    {
        Q_UNUSED(hoverUrl);
        // 加载选中和未选中状态下的图片
        m_checkedPixmap.load(Checked);
        m_uncheckedPixmap.load(Image);
        setFixedSize(m_uncheckedPixmap.size());
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        // 根据当前按钮的选中状态绘制相应的图片
        if (isChecked())
        {
            painter.drawPixmap(rect(), m_checkedPixmap);
        }
        else
        {
            painter.drawPixmap(rect(), m_uncheckedPixmap);
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        Q_UNUSED(event);
        setChecked(true);
        emit clicked(true);
        emit pressed();
        QRadioButton::mousePressEvent(event);
    }

private:
    QPixmap m_checkedPixmap;
    QPixmap m_uncheckedPixmap;
};


class CircularButton : public QPushButton
{
public:
    CircularButton(QWidget *parent = nullptr, QString Image = "");

    void SetImage(QString Image);
    QString ImageUrl()
    {
        return m_imageUrl;
    }
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QSharedPointer<QPixmap> m_pixmap;
    QString m_imageUrl;
};

class switchButton: public image_button
{
    Q_OBJECT
public:
    switchButton(QWidget *p = nullptr);
    void setTurnOn();
    void setTurnOff();
    bool isTurnOn();
signals:
    void turnOff();
    void turnOn();
private:
    bool switchFlag;
};

// 模板文件选择按钮
class module_file_button: public QWidget
{
    Q_OBJECT
public:
    explicit module_file_button(QString ItemName, QWidget *parent = nullptr);
    void setImage(QString url);
    void setFileName(QString fileName);
    QString fileName()
    {
        return m_fileNameStr;
    }
    QString filePath();
    virtual void paintEvent(QPaintEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    void showBorder(bool flag);
signals:
    void clicked(QString);
private:
    void init();
private:
    QVBoxLayout *m_VLayout;
    QLabel *m_image;
    base_Label *m_fileName;
    QString m_fileNameStr;
    bool m_boldFlag;
};

// item按钮
class Item_button: public QLabel
{
    Q_OBJECT
public:
    explicit Item_button(QString ItemName, QWidget *parent = nullptr);
    void setImage(QString url);
    void setClickGray(bool status);
    void setItemName(QString ItemName);
    QString ItemName()
    {
        return m_itemName->text();
    }
    QString imageUrl()
    {
        return m_imageUrl;
    }
    QImage dropImage();
    QSize textSize()
    {
        return m_itemName->size();
    }
signals:
    void pressed();
protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    void init();
private:
    QVBoxLayout *m_VLayout;
    QLabel *m_image;
    base_Label *m_itemName;
    QString m_imageUrl;
    QLabel *m_mask;
};

}
#endif // BASE_PUSH_BUTTON_HPP
