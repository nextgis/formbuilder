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

#include "translations_dialog.h"

#include "util/settings.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

using namespace Fb;
using namespace Gui;


LanguageSelectDialog::LanguageSelectDialog (QWidget *parent, QMap<QString, LangInfo> languages):
    QDialog(parent)
{
    this->setWindowTitle(tr("New language"));

    QLabel *lb_msg = new QLabel(this);
    lb_msg->setText(tr("Select language to add:"));

    m_cmb = new QComboBox(this);
    for (auto lang_key: languages.keys())
        m_cmb->addItem(/*languages[lang_key].icon, */languages[lang_key].display_name, lang_key);

    m_btn_ok = new QPushButton(this);
    m_btn_ok->setText(tr("OK"));
    QObject::connect(m_btn_ok, SIGNAL(clicked()), this, SLOT(accept()));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(lb_msg);
    l->addWidget(m_cmb);
    l->addWidget(m_btn_ok);
}

LanguageSelectDialog::~LanguageSelectDialog ()
{

}

QString LanguageSelectDialog::getSelectedLanguageKey () const
{
    return m_cmb->currentData(Qt::UserRole).toString();
}


// ============================================================================================== //


QMap<QString, LangInfo> TranslationsDialog::getAllLanguages ()
{
    QMap<QString, LangInfo> languages;
    languages.insert("en_US", {tr("English (United States)"), ""});
    languages.insert("ru_RU", {tr("Russian"), ""});
    languages.insert("tg_Tj", {tr("Tajik"), ""});
    return languages;
}


TranslationsDialog::TranslationsDialog (QWidget *parent):
    QDialog(parent)
{
    this->setWindowTitle(tr("Edit translations"));

    int w = g_getSettings()->value(FB_STS_TRANSLATIONSDLG_W, 0).toInt();
    int h = g_getSettings()->value(FB_STS_TRANSLATIONSDLG_H, 0).toInt();
    if (w != 0 && h != 0)
        this->resize({w, h});

    m_btn_add = new QToolButton(this);
    m_btn_add->setIcon(QIcon(":/images/add.svg"));
    m_btn_add->setToolTip(tr("Add new language"));
    QObject::connect(m_btn_add, SIGNAL(clicked()), this, SLOT(onLanguageSelectBtnClicked()));

    m_tbl = new QTableWidget(this);
    m_tbl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_tbl->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_tbl->setColumnCount(0);
    m_tbl->setRowCount(0);

    m_btn_ok = new QPushButton(this);
    m_btn_ok->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    m_btn_ok->setText(tr("OK"));
    QObject::connect(m_btn_ok, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *hl_top = new QHBoxLayout();
    hl_top->addWidget(m_btn_add);
    hl_top->addStretch();

    QHBoxLayout *hl_bot = new QHBoxLayout();
    hl_bot->addStretch();
    hl_bot->addWidget(m_btn_ok);
    hl_bot->addStretch();

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addLayout(hl_top);
    l->addWidget(m_tbl);
    l->addLayout(hl_bot);
}

TranslationsDialog::~TranslationsDialog ()
{
    g_getSettings()->setValue(FB_STS_TRANSLATIONSDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_TRANSLATIONSDLG_H, this->height());
}


void TranslationsDialog::loadTranslations (const QStringList &language_keys,
                                           const QMap<QString, QStringList> &values)
{
    m_tbl->setColumnCount(1);
    m_tbl->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Key phrase")));
    for (int col = 0; col < language_keys.size(); col++)
        u_addLangKeyColumn(language_keys[col]);

    auto phrase_keys = values.keys();
    for (auto phrase_key: phrase_keys)
    {
        m_tbl->setRowCount(m_tbl->rowCount() + 1);
        m_tbl->setItem(m_tbl->rowCount() - 1, 0, new QTableWidgetItem(phrase_key));

        for (int col = 0; col < language_keys.size(); col++)
        {
            QString value_text = "";
            if (!values[phrase_key].isEmpty() && col < values[phrase_key].size()) // not a new or missed entry in the map of key phrases
                value_text = values[phrase_key][col];
            m_tbl->setItem(m_tbl->rowCount() - 1, col + 1, new QTableWidgetItem(value_text));
        }
    }

    m_tbl->resizeColumnsToContents();
}

QStringList TranslationsDialog::getLanguageKeys () const
{
    return u_getAddedLangKeys();
}

QMap<QString, QStringList> TranslationsDialog::getValues () const
{
    QMap<QString, QStringList> all_values;
    for (int row = 0; row < m_tbl->rowCount(); row++)
    {
        QString phrase_key = m_tbl->item(row, 0)->text();
        QStringList values;
        for (int col = 1; col < m_tbl->columnCount(); col++)
            values += m_tbl->item(row, col)->text();
        all_values.insert(phrase_key, values);
    }
    return all_values;
}


void TranslationsDialog::onLanguageSelectBtnClicked ()
{
    // Remove all languages which were already added.
    QMap<QString, LangInfo> langs = getAllLanguages();
    QStringList added_keys = u_getAddedLangKeys();
    for (auto lang_key: added_keys)
    {
        if (langs.contains(lang_key))
            langs.remove(lang_key);
    }

    LanguageSelectDialog dialog(this, langs);
    if (!dialog.exec())
        return;

    QString selected_key = dialog.getSelectedLanguageKey();
    u_addLangKeyColumn(selected_key);
}


void TranslationsDialog::u_addLangKeyColumn (QString lang_key)
{
    auto all_languages = getAllLanguages();
    if (all_languages.contains(lang_key)) // get display name for language key
    {
        QTableWidgetItem *header_item = new QTableWidgetItem();
        header_item->setText(all_languages[lang_key].display_name);
        header_item->setData(Qt::UserRole, lang_key);

        m_tbl->setColumnCount(m_tbl->columnCount() + 1);
        m_tbl->setHorizontalHeaderItem(m_tbl->columnCount() - 1, header_item);
        for (int r = 0; r < m_tbl->rowCount(); r++)
            m_tbl->setItem(r, m_tbl->columnCount() - 1, new QTableWidgetItem(""));
    }
}


QStringList TranslationsDialog::u_getAddedLangKeys () const
{
    QStringList keys;
    for (int col = 1; col < m_tbl->columnCount(); col++)
        keys.append(m_tbl->horizontalHeaderItem(col)->data(Qt::UserRole).toString());
    return keys;
}


