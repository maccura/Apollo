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
#ifndef FONT_PROP_WIDGET_H
#define FONT_PROP_WIDGET_H
#include "base_prop_widget.hpp"
#include "prop_base_widget/prop_color_button.hpp"
#include "prop_base_widget/prop_combox.hpp"
#include "prop_base_widget/prop_spinbox_widget.hpp"
#include "mask_dialog/base_push_button.hpp"
#include <QFontComboBox>
namespace ReportDesign
{
class font_prop_widget: public base_prop_widget
{
    Q_OBJECT
public:
    explicit font_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent = nullptr);
    virtual void update(QString PropertyName) override;
public slots:
    void slotFontColorProperty(QObject *object, QColor newColor);
    void slotFontColorProperty(const QList<QObject *> &objects, QColor newColor);

    void slotColorOpacityProperty(QObject *object, QString opt);
    void slotColorOpacityProperty(const QList<QObject *> &objects,  QString opt);

    void slotFontFamilyProperty(QObject *object, QFont font);
    void slotFontFamilyProperty(const QList<QObject *> &objects, QFont font);

    void slotFontBoldProperty(QObject *object, bool bold);
    void slotFontBoldProperty(const QList<QObject *> &objects, bool bold);

    void slot_pointSizeSpinbox(const QString &text);
    void slotPointSizeSpinbox(QObject *object, const QString &text);
    void slotPointSizeSpinbox(const QList<QObject *> &objects, const QString &text);

    void slotAlignButtonCheack(QObject *object, QAbstractButton *button);
    void slotAlignButtonCheack(const QList<QObject *> &objects, QAbstractButton *button);
private:
    void init();
    void UpdateAlignment(Qt::Alignment align);
    void UpdateFontColor(QColor fontColor);
    void UpdateFont(QFont font);
    void connectRegister();
private:
    QFontComboBox *m_familyCombox;
    prop_spinbox_widget *m_pointSizeSpinbox;
    prop_combox *m_boldCombox;
    prop_combox *m_opacityCombox;
    prop_color_button *m_fontColorButton;
    // 字体对齐
    QButtonGroup *m_hGroup;
    QButtonGroup *m_vGroup;
    image_Radio_button *m_HAlignLeft;
    image_Radio_button *m_HAlignCenter;
    image_Radio_button *m_HAlignRight;
    image_Radio_button *m_VAlignTop;
    image_Radio_button *m_VAlignCenter;
    image_Radio_button *m_VAlignBottom;
};
}
#endif // FONT_PROP_WIDGET_H
