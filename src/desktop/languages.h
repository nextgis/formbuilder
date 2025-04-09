/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
 *                                                                                                *
 *    This program is free software: you can redistribute it and/or modify it under the terms     *
 *    of the GNU General Public License as published by the Free Software Foundation, either      *
 *    version 2 of the License, or (at your option) any later version.                            *
 *                                                                                                *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 *    See the GNU General Public License for more details.                                        *
 *                                                                                                *
 *    You should have received a copy of the GNU General Public License along with this program.  *
 *    If not, see http://www.gnu.org/licenses/.                                                   *
 *                                                                                                *
 **************************************************************************************************/

#pragma once

#include <QObject>
#include <QMap>


enum class Language
{
    Undefined,
    en_GB,
    ru_RU
};

struct LanguageData
{
    QString display_name_en;
    QString display_name;
    QString code;
    QString sys_code;
    QString logo_path;
    QString app_link;
    QString subscr_link;
    QString help_link;
    QString restart_message;
};

const QMap<Language, LanguageData> LANGUAGES =
{
    {{Language::en_GB}, {"English", "English", "en_GB", "en", "",
                         "http://nextgis.com/nextgis-formbuilder",
                         "http://nextgis.com/pricing/",
                         "http://docs.nextgis.com/docs_formbuilder/source/index.html",
                         "Please restart application in order to change language"}},

    {{Language::ru_RU}, {"Russian", "Русский", "ru_RU", "ru", "",
                         "http://nextgis.ru/nextgis-formbuilder",
                         "http://nextgis.ru/pricing/",
                         "http://docs.nextgis.ru/docs_formbuilder/source/index.html",
                         "Пожалуйста, перезагрузите приложение чтобы изменение языка вступило в силу"}}
};

inline Language g_findLanguage (QString code)
{
    for (auto &key: LANGUAGES.keys())
    {
        auto &data = LANGUAGES.value(key);
        if (data.code == code)
            return key;
    }
    return Language::Undefined;
}

Q_DECLARE_METATYPE(Language)



