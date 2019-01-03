/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  methods/classes for common screen 
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "screens.h"

#define FB_COMM_SIZE_STDHEIGHT 20

void FBDecoratorDefault::clearWithDefaults (FBElem* elem)
{
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_COMM_SIZE_STDHEIGHT);
}

void FBDecoratorDefault::updateTextFromStringAttr (FBElem *elem, QString attrKeyName)
{
    if (elem == NULL) return;
    FBAttr *attr = elem->getAttr(attrKeyName);
    if (attr == NULL) return;
    QString str = attr->getValue().toString();
    QLabel *labText = (QLabel*)elem->findAsDecor(FB_NAMEDDECOR_TEXT);
    if (labText == NULL) return;
    labText->setText(" " + str);
}

void FBDecoratorDefault::updateCombobox (FBElem *elem)
{
    if (elem == NULL) return;
    FBAttr *attr = elem->getAttr(FB_ATTRNAME_VALUE_mult);
    if (attr == NULL) return;
    QVariant var = attr->getValue();
    FBListValue val = var.value<FBListValue>();
    QLabel *labText = (QLabel*)elem->findAsDecor(FB_NAMEDDECOR_TEXT);
    if (labText == NULL) return;
    QString str = "";
    if (val.second != -1)
        str = val.first[val.second].second;
    labText->setText(" " + str);
}

void FBDecoratorDefault::updateDoubCombobox (FBElem *elem)
{
    if (elem == NULL) return;
    FBAttr *attr = elem->getAttr(FB_ATTRNAME_VALUE_mult);
    if (attr == NULL) return;
    QVariant var = attr->getValue();
    FBDoublelistValue val = var.value<FBDoublelistValue>();
    QLabel *labText1 = elem->findChild<QLabel*>(FB_NAMEDDECOR_TEXT);
    QLabel *labText2 = elem->findChild<QLabel*>(FB_NAMEDDECOR_TEXT2);
    if (labText1 == NULL || labText2 == NULL) return;
    QString str1 = "";
    QString str2 = "";
    if (val.first.second != -1)
    {
        str1 = val.first.first[val.first.second].second;
        if (val.second.second[val.first.second] != -1)
        {
            str2 = val.second.first[val.first.second][
                    val.second.second[val.first.second]].second;
        }
    }
    labText1->setText(" " + str1);
    labText2->setText(" " + str2);
}


void FBDecoratorText::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setStyleSheet("QLabel{"
                           "border: none;"
                           "border-radius: none;}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
}
void FBDecoratorText::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
}


void FBDecoratorSpace::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem); // will be just void elem
}


void FBDecoratorTextedit::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setStyleSheet("QLabel{"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: 1px solid black;"
                           "border-radius: none;}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
}
void FBDecoratorTextedit::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
}


void FBDecoratorCombobox::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setStyleSheet("QLabel{"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: 1px solid black;"
                           "border-bottom: 1px solid black;"
                           "border-radius: none;}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
}
void FBDecoratorCombobox::update (FBElem* elem)
{
    this->updateCombobox(elem);
}


void FBDecoratorDoubledcombobox::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_COMM_SIZE_STDHEIGHT*2);
    QLabel *labText1 = new QLabel(elem);
    labText1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText1->setStyleSheet("QLabel{"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: 1px solid black;"
                           "border-bottom: 1px solid black;"
                           "border-radius: none;}");
    elem->addAsDecor(labText1,FB_NAMEDDECOR_TEXT);
    QLabel *labText2 = new QLabel(elem);
    labText2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText2->setStyleSheet("QLabel{"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: 1px solid black;"
                           "border-bottom: 1px solid black;"
                           "border-radius: none;}");
    elem->addAsDecor(labText2,FB_NAMEDDECOR_TEXT2);
}
void FBDecoratorDoubledcombobox::update (FBElem* elem)
{
    this->updateDoubCombobox(elem);
}


void FBDecoratorCheckbox::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    elem->registerAsDecor(labText,FB_NAMEDDECOR_TEXT);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setStyleSheet("QLabel{border: none;}");
    QLabel *labImg = new QLabel(elem);
    elem->registerAsDecor(labImg,FB_NAMEDDECOR_IMG);
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    labImg->setFixedWidth(10);
    labImg->setFixedHeight(10);
    labImg->setStyleSheet("QLabel{border: 1px solid black;"
                          "background: none;"
                          "border-radius: none;}");
    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->setContentsMargins(2,0,2,0);
    hlay->setSpacing(4);
    hlay->addWidget(labImg);
    hlay->addWidget(labText);
    elem->addAsDecor(hlay);
}
void FBDecoratorCheckbox::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
    if (elem == NULL) return;
    FBAttr *attr = elem->getAttr(FB_ATTRNAME_INITVALUE);
    if (attr == NULL) return;
    bool checked = attr->getValue().toBool();
    QLabel *labImg = elem->findChild<QLabel*>(FB_NAMEDDECOR_IMG);
    if (labImg == NULL) return;
    if (checked)
    {
        labImg->setStyleSheet("QLabel{border: 1px solid black;"
                              "background: black;"
                              "border-radius: none;}");
    }
    else
    {
        labImg->setStyleSheet("QLabel{border: 1px solid black;"
                              "background: none;"
                              "border-radius: none;}");
    }
}


void FBDecoratorRadiogroup::redecor (FBElem* elem)
{
    // Nothing to redecorate because the contents of radiogroup fully depends on
    // attributes state. See update().
    return; // do not call update here so this will not be called outside twice
}
void FBDecoratorRadiogroup::update (FBElem* elem)
{
    if (elem == NULL) return;
    FBAttr *attr = elem->getAttr(FB_ATTRNAME_VALUE_mult);
    if (attr == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    elem->setMinimumHeight(0);
    elem->setMaximumHeight(QWIDGETSIZE_MAX);
    QVariant var = attr->getValue();
    FBListValue val = var.value<FBListValue>();
    for (int i=0; i<val.first.size(); i++)
    {
        // For Radiogroup we clear and recrate all widgets because we do not know
        // how much items will be in the elem each time it needs an update.
        QLabel *labText = new QLabel(elem);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setStyleSheet("QLabel{border: none;}");
        labText->setText(val.first[i].second);
        QLabel *labImg = new QLabel(elem);
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labImg->setFixedWidth(10);
        if (i == val.second) // will be found anyway, because radio-group must
        {                    // have selected value
            labImg->setStyleSheet("QLabel{border: 1px solid black;"
                                  "background: black;"
                                  "border-radius: 3px;}");
        }
        else
        {
            labImg->setStyleSheet("QLabel{border: 1px solid black;"
                                  "background: none;"
                                  "border-radius: 3px;}");
        }
        QHBoxLayout *hlay = new QHBoxLayout();
        hlay->setContentsMargins(2,2,2,2);
        hlay->setSpacing(4);
        hlay->addWidget(labImg);
        hlay->addWidget(labText);
        elem->addAsDecor(hlay);
    }
}


void FBDecoratorButton::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setAlignment(Qt::AlignCenter);
    labText->setStyleSheet("QLabel{"
                           "border: 1px solid black;"
                           "border-radius: none;}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
}
void FBDecoratorButton::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
}


// REGISTRAR:
// The screen will know which decorators it can use for decoring elems of its form.
void FBScreen::registerDecorators()
{
    this->registerDecorator(FB_ELEMNAME_TEXT_LABEL, new FBDecoratorText());
    this->registerDecorator(FB_ELEMNAME_SPACE, new FBDecoratorSpace());
    this->registerDecorator(FB_ELEMNAME_TEXT_EDIT, new FBDecoratorTextedit());
    this->registerDecorator(FB_ELEMNAME_COMBOBOX, new FBDecoratorCombobox());
    this->registerDecorator(FB_ELEMNAME_DOUBLE_COMBOBOX, new FBDecoratorDoubledcombobox());
    this->registerDecorator(FB_ELEMNAME_CHECKBOX, new FBDecoratorCheckbox());
    this->registerDecorator(FB_ELEMNAME_RADIOGROUP, new FBDecoratorRadiogroup());
    this->registerDecorator(FB_ELEMNAME_BUTTON, new FBDecoratorButton());
}

 
 
