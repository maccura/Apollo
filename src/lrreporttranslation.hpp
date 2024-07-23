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
#ifndef REPORTTRANSLATION_H
#define REPORTTRANSLATION_H

#include <QString>
#include <QList>
#include <QLocale>
#include <QMetaType>
#include "item/page_scene.hpp"


class ATranslationProperty
{
public:
    ATranslationProperty() {}
    // ATranslationProperty(const ACollectionProperty& ){}
    virtual ~ATranslationProperty() {}
};

Q_DECLARE_METATYPE(ATranslationProperty)
const int TRANSLATION_TYPE_ID = qMetaTypeId<ATranslationProperty>();

namespace ReportDesign
{

struct PropertyTranslation
{
    QString propertyName;
    QString value;
    QString sourceValue;
    bool    checked;
    bool    sourceHasBeenChanged;
};

struct ItemTranslation
{
    QString itemName;
    bool checked;
    PropertyTranslation *findProperty(const QString &propertyName);
    ~ItemTranslation();
    QList<PropertyTranslation *> propertyesTranslation;
};

struct PageTranslation
{
    QString pageName;
    bool checked;
    ~PageTranslation();
    void renameItem(const QString &oldName, const QString &newName);
    QHash<QString, ItemTranslation *> itemsTranslation;
};

class ReportTranslation
{
public:
    ReportTranslation(QLocale::Language language) : m_language(language) {}
    ReportTranslation(QLocale::Language language, QList<page_scene *> pages);
    ReportTranslation(const ReportTranslation &reportTranslation);
    ~ReportTranslation();
    QLocale::Language language() const;
    QList<PageTranslation *> &pagesTranslation();
    PageTranslation *createEmptyPageTranslation();
    void updatePageTranslation(page_scene *page);
    PageTranslation *findPageTranslation(const QString &pageName);
    void renamePage(const QString &oldName, const QString &newName);
    void invalidatePages();
    void clearInvalidPages();
private:
    void createItemTranslation(base_design_item *item, PageTranslation *pageTranslation);
    PageTranslation *createPageTranslation(page_scene *page);
private:
    QLocale::Language m_language;
    QLocale::Script m_script;
    QList<PageTranslation *> m_pagesTranslation;
};


typedef QMap<QLocale::Language, ReportTranslation *> Translations;

class ITranslationContainer
{
public:
    virtual ~ITranslationContainer() {}
    virtual Translations *translations() = 0;
    virtual void updateTranslations() = 0;
    virtual bool addTranslationLanguage(QLocale::Language language) = 0;
    virtual bool removeTranslationLanguage(QLocale::Language language) = 0;
    virtual QList<QLocale::Language> aviableLanguages() = 0;
};

} // namespace ReportDesign

//Q_DECLARE_METATYPE(ReportTranslation)

#endif // REPORTTRANSLATION_H
