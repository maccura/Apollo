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
#ifndef PROP_SPINBOX_WIDGET_H
#define PROP_SPINBOX_WIDGET_H
#include <QSpinBox>
#include <QLabel>
namespace ReportDesign
{

class CustomSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    CustomSpinBox(QWidget *parent = nullptr) : QSpinBox(parent)
    {

        connect(this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChange()));
    }
signals:
    void beginEdit();
protected:
    void focusInEvent(QFocusEvent *event) override
    {
        QSpinBox::focusInEvent(event);
        // 判断QLineEdit开始编辑
        emit beginEdit();
    }
private slots:
    void slotValueChange()
    {
        update();
    }
};

class prop_spinbox_widget: public QWidget
{
    Q_OBJECT
public:
    prop_spinbox_widget(QWidget *parent = nullptr, QString name = "");
    void setValue(int value);
    void setName(QString name);
    void setMinimumWidth(int width);
    void setMinimumHeight(int height);
    void setMaximum(int mun);
    int value();
protected:
    void paintEvent(QPaintEvent *event) override;
signals:
    void valueChanged(const QString &);
private:
    bool m_editFlag;
    CustomSpinBox *m_spinBox;
    QLabel *m_name;
};
}
#endif // PROP_SPINBOX_WIDGET_H
