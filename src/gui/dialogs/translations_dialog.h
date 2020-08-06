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

#include "gui/custom_dialog.h"

#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>

namespace Fb
{
namespace Gui
{


struct LangInfo
{
    QString display_name;
    QString icon;
};


class LanguageSelectDialog: public QDialog
{
    Q_OBJECT

    public:

     LanguageSelectDialog (QWidget *parent, QMap<QString, LangInfo> languages);
     ~LanguageSelectDialog ();

     QString getSelectedLanguageKey () const;

    protected:

     QComboBox *m_cmb;
     QPushButton *m_btn_ok;
};


class TranslationsDialog: public QDialog
{
    Q_OBJECT

    public:

     static QMap<QString, LangInfo> getAllLanguages ();

     TranslationsDialog (QWidget *parent);
     ~TranslationsDialog ();

     void loadTranslations (const QStringList &language_keys, const QMap<QString, QStringList> &values);
     QStringList getLanguageKeys () const;
     QMap<QString, QStringList> getValues () const;

    protected slots:

     void onLanguageSelectBtnClicked ();

    protected:

     void u_addLangKeyColumn (QString lang_key);
     QStringList u_getAddedLangKeys () const;

     QToolButton *m_btn_add;
     QTableWidget *m_tbl;
     QPushButton *m_btn_ok;
};


}
}



