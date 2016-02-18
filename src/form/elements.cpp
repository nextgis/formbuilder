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


/******************************************************************************/
/*                                                                            */
/*                              FBElemText                                    */
/*                                                                            */
/******************************************************************************/

FBElemText::FBElemText (FBFactory *fctPtr):
    FBElem(fctPtr)
{

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

}

void FBElemText::updateAppearance ()
{
    labText->setText("Label");
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemTextedit                                */
/*                                                                            */
/******************************************************************************/

FBElemTextedit::FBElemTextedit (FBFactory *fctPtr):
    FBElem(fctPtr)
{

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
    labText->setText("Text edit");
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemDatetime                                */
/*                                                                            */
/******************************************************************************/

FBElemDatetime::FBElemDatetime (FBFactory *fctPtr):
    FBElem(fctPtr)
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
        labImg->setAlignment(Qt::AlignTop);
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
    labText->setText("11.08.2016");
}





