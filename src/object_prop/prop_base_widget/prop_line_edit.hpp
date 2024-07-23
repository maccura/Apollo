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
#ifndef PROP_LINE_EDIT_H
#define PROP_LINE_EDIT_H
#include <QLineEdit>
#include <QLabel>
namespace ReportDesign
{
class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}
signals:
    void beginEdit();
protected:
    void focusInEvent(QFocusEvent *event) override
    {
        QLineEdit::focusInEvent(event);
        // 判断QLineEdit开始编辑
        emit beginEdit();
    }
};

class prop_line_edit: public QWidget
{
    Q_OBJECT
public:
    prop_line_edit(QString name = "", QWidget *parent = nullptr, bool bodeFlag = true);
    QString name();
    void setName(QString name);
    QString content();
    void setContent(QString content);
    void setMinimumHeight(int height);
    void setReadOnly(bool flag);
signals:
    void contentEditFinished(QString);
    void beginEdit();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    void paintEditLine(QPaintEvent *event);
private:
    QLabel *m_name;
    CustomLineEdit *m_edit;
    bool m_editLineFlag;
    bool m_bodeFlag;
};
}
#endif // PROP_LINE_EDIT_H
