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
#ifndef BASE_MASK_DIALOG_H
#define BASE_MASK_DIALOG_H
#include <QDialog>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include "dialog_background.hpp"
#include "base_push_button.hpp"
namespace ReportDesign
{
class base_mask_dialog: public QDialog
{
    Q_OBJECT
public:
    base_mask_dialog(QWidget *parent = nullptr, bool menuBar = true, bool mask = true);
    ~base_mask_dialog();
    QVBoxLayout *VLayout();
    void SetTitle(QString Title);
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
protected:
    QMenuBar *m_menuBar;
    QToolBar *m_closeToolBar;
    QAction *m_closeWindow;
    QAction *m_TitleWindow;
    QLabel *m_leftWidget;
    dialog_background *m_mask;
    QVBoxLayout *m_VLayout;
};

class base_mask_widget: public QWidget
{
    Q_OBJECT
public:
    base_mask_widget(QWidget *parent = nullptr, bool menuBar = true, bool mask = true);
    ~base_mask_widget();
    QVBoxLayout *VLayout();
    void SetTitle(QString Title);

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
protected:
    QMenuBar *m_menuBar;
    QToolBar *m_closeToolBar;
    QAction *m_closeWindow;
    QAction *m_TitleWindow;
    QLabel *m_leftWidget;
    dialog_background *m_mask;
    QVBoxLayout *m_VLayout;
};

}
#endif // BASE_MASK_DIALOG_H
