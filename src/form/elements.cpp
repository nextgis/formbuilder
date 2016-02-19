/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  elements implementation
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

#include "elements.h"

#include "attributes.h"



/*****************************************************************************/
/*                                                                           */
/*                            FBElemInput                                    */
/*                                                                           */
/*****************************************************************************/

FBElemInput::FBElemInput (FBFactory *fctPtr):
    FBElem (fctPtr)
{
    attrFieldPtr = new FBAttrField(this, tr("field"),
                  tr("Target layer field"), FBImportant);
    attrs.insert(attrFieldPtr);
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemText                                    */
/*                                                                            */
/******************************************************************************/

FBElemText::FBElemText (FBFactory *fctPtr):
    FBElem (fctPtr)
{
    attrTextPtr = new FBAttrText(this, tr("text"),
                  tr("Initial text"), FBNoRole, tr("Text"));
    attrs.insert(attrTextPtr);
}

void FBElemText::changeStyle (QString styleName)
{
    this->clearContents();

    // For all styles:
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(0);

    if (styleName == FB_STYLENAME_ANDROID)
    {
        labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                               FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText->setStyleSheet("QLabel"
                               "{color: "+QString(FB_COLOR_DARKGREY)+";"
                                   "border-top: none;"
                                   "border-left: none;"
                                   "border-right: none;"
                                   "border-bottom: none;}");
        lvMain->addWidget(labText);
    }

    // ...

    else
    {
        labText = new QLabel(this);
        lvMain->addWidget(labText);
        labText->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    }

    this->updateAppearance();
}

void FBElemText::changeAttrValue ()
{
    return;
}

void FBElemText::updateAppearance ()
{
    labText->setText(" " + attrTextPtr->getValue());
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemTextedit                                */
/*                                                                            */
/******************************************************************************/

FBElemTextedit::FBElemTextedit (FBFactory *fctPtr):
    FBElemInput (fctPtr)
{
    attrTextPtr = new FBAttrText(this, tr("text"),
                  tr("Initial text"), FBNoRole, tr("Text"));
    attrs.insert(attrTextPtr);

    FBAttrNumber *attrNumberPtr = new FBAttrNumber(this, tr("max_string_count"),
                  tr("Max. string count"), FBNoRole, 1, 1, 65535);
    attrs.insert(attrNumberPtr);
}

void FBElemTextedit::changeStyle (QString styleName)
{
    this->clearContents();

    // For all styles:
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(0);

    if (styleName == FB_STYLENAME_ANDROID)
    {
        labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                               FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText->setStyleSheet("QLabel"
                               "{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
        QWidget *widDecor = new QWidget(this);
        widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        widDecor->setMinimumHeight(7);
        widDecor->setStyleSheet("QWidget"
                   "{border-top: none;"
                   "border-left: 3px solid "+QString(FB_COLOR_MEDIUMGREY)+";"
                   "border-right: 3px solid "+QString(FB_COLOR_MEDIUMGREY)+";"
                   "border-bottom: 3px solid "+QString(FB_COLOR_MEDIUMGREY)+";}");
        lvMain->addWidget(labText);
        lvMain->addWidget(widDecor);
    }

    // ...

    else // default
    {
        labText = new QLabel(this);
        lvMain->addWidget(labText);
        labText->setStyleSheet("QLabel{"
                               "border-top: 1px solid black;"
                               "border-left: 1px solid black;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;}");
    }

    this->updateAppearance();
}

void FBElemTextedit::changeAttrValue ()
{

}

void FBElemTextedit::updateAppearance ()
{
    labText->setText(" " + attrTextPtr->getValue());
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemCombobox                                */
/*                                                                            */
/******************************************************************************/

FBElemCombobox::FBElemCombobox (FBFactory *fctPtr, QWidget *appWidget):
    FBElemInput (fctPtr)
{
    attrListvalsPtr = new FBAttrListvalues(this, tr("values"),
                  tr("Values"), FBNoRole, appWidget);
    attrs.insert(attrListvalsPtr);
}

void FBElemCombobox::changeStyle (QString styleName)
{
    this->clearContents();

    // For all styles:
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(0,0,0,0);
    lvMain->setSpacing(0);

    if (styleName == FB_STYLENAME_ANDROID)
    {
        //this->setStyleSheet("QWidget"
        //                    "{background-color: rgba(0,0,0,0); "
        //                    "border: 2px dashed rgba(0,0,0,0);}");
        QHBoxLayout *hlayAll = new QHBoxLayout();
        hlayAll->setContentsMargins(2,2,2,2);
        hlayAll->setSpacing(0);
        QVBoxLayout *vlayText = new QVBoxLayout();
        vlayText->setContentsMargins(0,0,0,0);
        vlayText->setSpacing(0);
        labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                               FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText->setStyleSheet("QLabel"
                               "{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
        QWidget *widDecor = new QWidget(this);
        widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        widDecor->setMinimumHeight(7);
        widDecor->setStyleSheet("QWidget"
                    "{border-top: none;"
                    "border-left: none;"
                    "border-right: none;"
                    "border-bottom: 3px solid "+QString(FB_COLOR_MEDIUMGREY)+";}");
        vlayText->addWidget(labText);
        vlayText->addWidget(widDecor);
        QLabel *labImg = new QLabel(this);
        QPixmap pixmap = QPixmap(":/img/for_combo.png");
        labImg->setStyleSheet("QLabel{border: none;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg->setFixedWidth(pixmap.width());
        labImg->setAlignment(Qt::AlignBottom);
        labImg->setPixmap(pixmap);
        labImg->setContentsMargins(0,0,0,0);
        hlayAll->addLayout(vlayText);
        hlayAll->addWidget(labImg);
        lvMain->addLayout(hlayAll);
    }

    // ...

    else // default
    {
        //this->setStyleSheet("");
        labText = new QLabel(this);
        lvMain->addWidget(labText);
        labText->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;}");
    }

    this->updateAppearance();
}

void FBElemCombobox::changeAttrValue ()
{

}

void FBElemCombobox::updateAppearance ()
{
    //labText->setText(attrListvalsPtr->getDefDispValue());
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemDatetime                                */
/*                                                                            */
/******************************************************************************/

FBElemDatetime::FBElemDatetime (FBFactory *fctPtr):
    FBElemInput (fctPtr)
{

}

void FBElemDatetime::changeStyle (QString styleName)
{
    this->clearContents();

    // For all styles:
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(0);

    if (styleName == FB_STYLENAME_ANDROID)
    {
        labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                               FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText->setStyleSheet("QLabel"
                               "{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");

        QLabel *labImg = new QLabel(this);
        QPixmap pixmap = QPixmap(":/img/for_date.png");
        labImg->setStyleSheet("QLabel{border: none;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg->setFixedWidth(pixmap.width());
        labImg->setAlignment(Qt::AlignCenter);
        labImg->setPixmap(pixmap);
        labImg->setContentsMargins(0,0,0,0);

        QWidget *widDecor = new QWidget(this);
        widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        widDecor->setFixedHeight(3);
        widDecor->setStyleSheet("QWidget {border-top: none;"
                  "border-left: none;"
                  "border-right: none;"
                  "border-bottom: 2px solid "+QString(FB_COLOR_MEDIUMGREY)+";}");

        QHBoxLayout *hlayTop = new QHBoxLayout();
        hlayTop->setContentsMargins(0,0,0,0);
        hlayTop->setSpacing(0);
        hlayTop->addWidget(labText);
        hlayTop->addWidget(labImg);
        lvMain->addLayout(hlayTop);
        lvMain->addWidget(widDecor);
    }

    //...

    else // default
    {
        labText = new QLabel(this);
        lvMain->addWidget(labText);
        labText->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    }

    this->updateAppearance();
}

void FBElemDatetime::changeAttrValue ()
{

}

void FBElemDatetime::updateAppearance ()
{
    //labText->setText("11.08.2016");
}





