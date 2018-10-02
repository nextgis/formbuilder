/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  mobile screens implementations
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
#include "devices.h"

#define FB_ANDR_FONTSIZE_NORMAL 12
#define FB_ANDR_FONTTYPE "Segoe UI"
#define FB_ANDR_SIZE_STDHEIGHT 30
#define FB_ANDR_SIZE_STDHEIGHTX2 60
#define FB_ANDR_SIZE_BUTTONHEIGHT 36
#define FB_ANDR_SIZE_LARGESQUARE 52
#define FB_ANDR_COLOR_LIGHTGREY "rgb(238,238,238)"
#define FB_ANDR_COLOR_LIGHTMEDIUMGREY "rgb(210,210,210)"
#define FB_ANDR_COLOR_MEDIUMGREY "rgb(170,170,170)"
#define FB_ANDR_COLOR_DARKGREY "rgb(100,100,100)"
#define FB_ANDR_COLOR_VERYDARKGREY "rgb(46,46,46)"
#define FB_ANDR_COLOR_LIGHTBLUE "rgb(139,183,224)"
#define FB_ANDR_COLOR_DARKBLUE "rgb(23,111,193)"

FBScreenAndroid::FBScreenAndroid (QWidget* parent, float sizeFactor):
    FBScreenMobile (parent,sizeFactor)
{ 
    // TODO: read list of devices from csv file.
    devices.append(FBDeviceMobile(
                   QPair<int,int>(480,800),4.0,-1.0,"Samsung Ace 4",""));
    devices.append(FBDeviceTablet(
                   QPair<int,int>(1280,800),9.6,-1.0,"Galaxy Tab E",""));
    devices.append(FBDeviceMobile(
                   QPair<int,int>(720,1280),4.6,-1.0,"Sony Xperia Z5",""));
}

FBScreenAndroid::~FBScreenAndroid ()
{
}

void FBScreenAndroid::installDecor ()
{
    // Reset general stylesheet of the screen: grey background.
    this->setStyleSheet("background-color: "
                           +QString(FB_ANDR_COLOR_LIGHTGREY)+";"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;");
                           //"border: 1px solid black;"
                           //"margin: 1;");

    // Top black decor line.
    QLabel *labDecorTop = new QLabel(this);
    lvMain2->insertWidget(0,labDecorTop);
    labsScreenDecorVert.append(labDecorTop);
    labDecorTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labDecorTop->setFixedHeight(15);
    QHBoxLayout *layDecorTop = new QHBoxLayout(labDecorTop);
    layDecorTop->setContentsMargins(0,0,0,0);
    layDecorTop->setSpacing(0);
    QLabel *labDecorTop10 = new QLabel(labDecorTop);
    labDecorTop10->setPixmap(QPixmap(":screen/img/android/decor_andr_5.png"));
    labDecorTop10->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop11 = new QLabel(labDecorTop);
    labDecorTop11->setPixmap(QPixmap(":screen/img/android/decor_andr_6.png"));
    labDecorTop11->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layDecorTop->addSpacing(5);
    layDecorTop->addWidget(labDecorTop10);
    layDecorTop->addStretch();
    layDecorTop->addWidget(labDecorTop11);
    layDecorTop->addSpacing(5);

    // Top blue decor line.
    QLabel *labDecorTop2 = new QLabel(this);
    lvMain2->insertWidget(1,labDecorTop2);
    labsScreenDecorVert.append(labDecorTop2);
    labDecorTop2->setStyleSheet("QLabel{background-color: rgb(3,169,244);"
                                "border-top-left-radius: 0px;"
                                "border-top-right-radius: 0px;"
                                "border-bottom-left-radius: 0px;"
                                "border-bottom-right-radius: 0px;}");
    labDecorTop2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labDecorTop2->setFixedHeight(35);
    QHBoxLayout *layDecorTop2 = new QHBoxLayout(labDecorTop2);
    layDecorTop2->setContentsMargins(0,0,0,0);
    layDecorTop2->setSpacing(0);
    QLabel *labDecorTop20 = new QLabel(labDecorTop2);
    labDecorTop20->setPixmap(QPixmap(":screen/img/android/decor_andr_4.png"));
    labDecorTop20->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop21 = new QLabel(labDecorTop2);
    labDecorTop21->setPixmap(QPixmap(":screen/img/android/decor_andr_3.png"));
    labDecorTop21->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop22 = new QLabel(labDecorTop2);
    labDecorTop22->setPixmap(QPixmap(":screen/img/android/decor_andr_2.png"));
    labDecorTop22->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop23 = new QLabel(labDecorTop2);
    labDecorTop23->setPixmap(QPixmap(":screen/img/android/decor_andr_1.png"));
    labDecorTop23->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layDecorTop2->addSpacing(5);
    layDecorTop2->addWidget(labDecorTop20);
    layDecorTop2->addStretch();
    layDecorTop2->addWidget(labDecorTop21);
    layDecorTop2->addSpacing(9);
    layDecorTop2->addWidget(labDecorTop22);
    layDecorTop2->addSpacing(5);
    layDecorTop2->addWidget(labDecorTop23);
    layDecorTop2->addSpacing(5);

    // If portrait or maximized orientation.
    if (curState == 0)
    {
        labDecorTop->setStyleSheet("QLabel"
                                   "{background-color: black;"
                                       "border-top-left-radius: 4px;"
                                       "border-top-right-radius: 4px;"
                                       "border-bottom-left-radius: 0px;"
                                       "border-bottom-right-radius: 0px;}");

        // Bottom black decor line.
        QLabel *labDecorBottom = new QLabel(this);
        lvMain2->insertWidget(3,labDecorBottom);
        labsScreenDecorVert.append(labDecorBottom);
        labDecorBottom->setStyleSheet("QLabel"
                                      "{background-color: black;"
                                      "border-top-left-radius: 0px;"
                                      "border-top-right-radius: 0px;"
                                      "border-bottom-left-radius: 4px;"
                                      "border-bottom-right-radius: 4px;}");
        labDecorBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labDecorBottom->setFixedHeight(30);
        QHBoxLayout *layDecorBottom = new QHBoxLayout(labDecorBottom);
        layDecorBottom->setContentsMargins(0,0,0,0);
        layDecorBottom->setSpacing(0);
        QLabel *labDecorBottom1 = new QLabel(labDecorBottom);
        labDecorBottom1->setPixmap(QPixmap(":screen/img/android/decor_andr_7.png"));
        labDecorBottom1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorBottom2 = new QLabel(labDecorBottom);
        labDecorBottom2->setPixmap(QPixmap(":screen/img/android/decor_andr_8.png"));
        labDecorBottom2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorBottom3 = new QLabel(labDecorBottom);
        labDecorBottom3->setPixmap(QPixmap(":screen/img/android/decor_andr_9.png"));
        labDecorBottom3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        layDecorBottom->addStretch();
        layDecorBottom->addWidget(labDecorBottom1);
        layDecorBottom->addStretch();
        layDecorBottom->addWidget(labDecorBottom2);
        layDecorBottom->addStretch();
        layDecorBottom->addWidget(labDecorBottom3);
        layDecorBottom->addStretch();
    }

    // If landscape orientation.
    else // if (curState == 1)
    {
        labDecorTop->setStyleSheet("QLabel"
                                   "{background-color: black;"
                                   "border-top-left-radius: 4px;"
                                   "border-top-right-radius: 0px;"
                                   "border-bottom-left-radius: 0px;"
                                   "border-bottom-right-radius: 0px;}");

        // Right black decor line.
        QLabel *labDecorRight = new QLabel(this);
        lhMain1->addWidget(labDecorRight);
        labsScreenDecorHor.append(labDecorRight);
        labDecorRight->setStyleSheet("QLabel"
                                      "{background-color: black;"
                                      "border-top-left-radius: 0px;"
                                      "border-top-right-radius: 4px;"
                                      "border-bottom-left-radius: 0px;"
                                      "border-bottom-right-radius: 4px;}");
        labDecorRight->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labDecorRight->setFixedWidth(30);
        QVBoxLayout *layDecorRight = new QVBoxLayout(labDecorRight);
        layDecorRight->setContentsMargins(0,0,0,0);
        layDecorRight->setSpacing(0);
        QLabel *labDecorRight1 = new QLabel(labDecorRight);
        labDecorRight1->setPixmap(QPixmap(":screen/img/android/decor_andr_9_rot.png"));
        labDecorRight1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorRight2 = new QLabel(labDecorRight);
        labDecorRight2->setPixmap(QPixmap(":screen/img/android/decor_andr_8_rot.png"));
        labDecorRight2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorRight3 = new QLabel(labDecorRight);
        labDecorRight3->setPixmap(QPixmap(":screen/img/android/decor_andr_7_rot.png"));
        labDecorRight3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        layDecorRight->addStretch();
        layDecorRight->addWidget(labDecorRight1);
        layDecorRight->addStretch();
        layDecorRight->addWidget(labDecorRight2);
        layDecorRight->addStretch();
        layDecorRight->addWidget(labDecorRight3);
        layDecorRight->addStretch();
    }
}


void FBDecoratorAndroid::clearWithDefaults (FBElem* elem)
{
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_ANDR_SIZE_STDHEIGHT);
}


void FBDecAndrText::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
}
void FBDecAndrText::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
}


void FBDecAndrSpace::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
}


void FBDecAndrTextedit::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");
    QWidget *widDecor = new QWidget(elem);
    widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor->setMinimumHeight(7);
    widDecor->setStyleSheet("QWidget"
               "{border-top: none;"
               "border-left: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";"
               "border-right: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";"
               "border-bottom: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
    elem->addAsDecor(widDecor);
}
void FBDecAndrTextedit::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
}


void FBDecAndrCombobox::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    elem->registerAsDecor(labText,FB_NAMEDDECOR_TEXT);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");
    QWidget *widDecor = new QWidget(elem);
    widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor->setMinimumHeight(7);
    widDecor->setStyleSheet("QWidget"
                "{border-top: none;"
                "border-left: none;"
                "border-right: none;"
              "border-bottom: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");
    QLabel *labImg = new QLabel(elem);
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pixCombo.width());
    labImg->setAlignment(Qt::AlignBottom);
    labImg->setPixmap(pixCombo);
    labImg->setContentsMargins(0,0,0,0);
    QHBoxLayout *hlayAll = new QHBoxLayout();
    //hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);
    QVBoxLayout *vlayText = new QVBoxLayout();
    vlayText->setContentsMargins(0,0,0,0);
    vlayText->setSpacing(0);
    vlayText->addWidget(labText);
    vlayText->addWidget(widDecor);
    hlayAll->addLayout(vlayText);
    hlayAll->addWidget(labImg);
    elem->addAsDecor(hlayAll);
}
void FBDecAndrCombobox::update (FBElem* elem)
{
    this->updateCombobox(elem);
}


void FBDecAndrDoubCombobox::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    elem->setFixedHeight(FB_ANDR_SIZE_STDHEIGHTX2);
    QLabel *labText1 = new QLabel(elem);
    elem->registerAsDecor(labText1,FB_NAMEDDECOR_TEXT);
    labText1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText1->setFont(QFont(FB_ANDR_FONTTYPE,
                            FB_ANDR_FONTSIZE_NORMAL));
    labText1->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");
    QLabel *labText2 = new QLabel(elem);
    elem->registerAsDecor(labText2,FB_NAMEDDECOR_TEXT2);
    labText2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText2->setFont(QFont(FB_ANDR_FONTTYPE,
                            FB_ANDR_FONTSIZE_NORMAL));
    labText2->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");
    QWidget *widDecor1 = new QWidget(elem);
    widDecor1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor1->setMinimumHeight(7);
    widDecor1->setStyleSheet("QWidget"
                            "{border-top: none;"
                            "border-left: none;"
                            "border-right: none;"
                            "border-bottom: 2px solid "
                             +QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");
    QWidget *widDecor2 = new QWidget(elem);
    widDecor2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor2->setMinimumHeight(7);
    widDecor2->setStyleSheet("QWidget"
                            "{border-top: none;"
                            "border-left: none;"
                            "border-right: none;"
                            "border-bottom: 2px solid "
                             +QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");
    QLabel *labImg1 = new QLabel(elem);
    labImg1->setStyleSheet("QLabel{border: none;}");
    labImg1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg1->setFixedWidth(pixCombo.width());
    labImg1->setAlignment(Qt::AlignBottom);
    labImg1->setPixmap(pixCombo);
    labImg1->setContentsMargins(0,0,0,0);
    QLabel *labImg2 = new QLabel(elem);
    labImg2->setStyleSheet("QLabel{border: none;}");
    labImg2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg2->setFixedWidth(pixCombo.width());
    labImg2->setAlignment(Qt::AlignBottom);
    labImg2->setPixmap(pixCombo);
    labImg2->setContentsMargins(0,0,0,0);
    QHBoxLayout *hlay1 = new QHBoxLayout();
    hlay1->setContentsMargins(0,0,0,0);
    hlay1->setSpacing(0);
    QHBoxLayout *hlay2 = new QHBoxLayout();
    hlay2->setContentsMargins(0,0,0,0);
    hlay2->setSpacing(0);
    QVBoxLayout *vlayText1 = new QVBoxLayout();
    vlayText1->setContentsMargins(0,0,0,0);
    vlayText1->setSpacing(0);
    QVBoxLayout *vlayText2 = new QVBoxLayout();
    vlayText2->setContentsMargins(0,0,0,0);
    vlayText2->setSpacing(0);
    vlayText1->addWidget(labText1);
    vlayText1->addWidget(widDecor1);
    vlayText2->addWidget(labText2);
    vlayText2->addWidget(widDecor2);
    hlay1->addLayout(vlayText1);
    hlay1->addWidget(labImg1);
    hlay2->addLayout(vlayText2);
    hlay2->addWidget(labImg2);
    elem->addAsDecor(hlay1);
    elem->addAsDecor(hlay2);
}
void FBDecAndrDoubCombobox::update (FBElem* elem)
{
    this->updateDoubCombobox(elem);
}


void FBDecAndrCheckbox::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labImg = new QLabel(elem);
    elem->registerAsDecor(labImg,FB_NAMEDDECOR_IMG);
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pixCheckOn.width()); // sizes of pixmaps are equal
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setContentsMargins(0,0,0,0);
    QLabel *labText = new QLabel(elem);
    elem->registerAsDecor(labText,FB_NAMEDDECOR_TEXT);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel{color: "
                           +QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    QHBoxLayout *hlayAll = new QHBoxLayout();
    hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);
    hlayAll->addWidget(labImg);
    hlayAll->addWidget(labText);
    elem->addAsDecor(hlayAll);
}
void FBDecAndrCheckbox::update (FBElem* elem)
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
        labImg->setPixmap(pixCheckOn);
    }
    else
    {
        labImg->setPixmap(pixCheckOff);
    }
}


void FBDecAndrRadiogroup::redecor (FBElem* elem)
{
    // See comments in FBDecoratorRadiogroup.
    return;
}
void FBDecAndrRadiogroup::update (FBElem* elem)
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
        // See comments in FBDecoratorRadiogroup.
        QLabel *labText = new QLabel(elem);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont(FB_ANDR_FONTTYPE,
                               FB_ANDR_FONTSIZE_NORMAL));
        labText->setStyleSheet("QLabel{color: "
                               +QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                                   "border-top: none;"
                                   "border-left: none;"
                                   "border-right: none;"
                                   "border-bottom: none;}");
        labText->setText(val.first[i].second);
        QLabel *labImg = new QLabel(elem);
        labImg->setStyleSheet("QLabel{border: none;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg->setFixedWidth(pixRadioOn.width()); // sizes are equal
        labImg->setAlignment(Qt::AlignCenter);
        labImg->setContentsMargins(0,0,0,0);
        if (i == val.second)
        {
            labImg->setPixmap(pixRadioOn);
        }
        else
        {
            labImg->setPixmap(pixRadioOff);
        }
        QHBoxLayout *hlay = new QHBoxLayout();
        hlay->setContentsMargins(0,3,0,3);
        hlay->setSpacing(6);
        hlay->addWidget(labImg);
        hlay->addWidget(labText);
        elem->addAsDecor(hlay);
    }
}


void FBDecAndrDatetime::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    elem->registerAsDecor(labText,FB_NAMEDDECOR_TEXT);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");
    QLabel *labImg = new QLabel(elem);
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pixDate.width());
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setPixmap(pixDate);
    labImg->setContentsMargins(0,0,0,0);
    QWidget *widDecor = new QWidget(elem);
    widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor->setFixedHeight(3);
    widDecor->setStyleSheet("QWidget {border-top: none;"
              "border-left: none;"
              "border-right: none;"
           "border-bottom: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");
    QHBoxLayout *hlayTop = new QHBoxLayout();
    hlayTop->setContentsMargins(0,0,0,0);
    hlayTop->setSpacing(0);
    hlayTop->addWidget(labText);
    hlayTop->addWidget(labImg);
    elem->addAsDecor(hlayTop);
    elem->addAsDecor(widDecor);
}
void FBDecAndrDatetime::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_INITVALUE_datetime);
}


void FBDecAndrButton::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_ANDR_SIZE_BUTTONHEIGHT);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setAlignment(Qt::AlignCenter);
    labText->setStyleSheet("QLabel{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: 1px solid "+FB_ANDR_COLOR_DARKGREY+";"
                           "background-color: "+FB_ANDR_COLOR_LIGHTMEDIUMGREY+";"
                           "border-top-left-radius: 1px;"
                           "border-top-right-radius: 1px;"
                           "border-bottom-left-radius: 1px;"
                           "border-bottom-right-radius: 1px;}");
    elem->addAsDecor(labText,FB_NAMEDDECOR_TEXT);
}
void FBDecAndrButton::update (FBElem* elem)
{
    this->updateTextFromStringAttr(elem,FB_ATTRNAME_TEXT);
}


void FBDecAndrPhoto::redecor (FBElem* elem)
{
    return;
}
void FBDecAndrPhoto::update (FBElem* elem)
{
    if (elem == NULL) return;
    FBAttr *attr = elem->getAttr(FB_ATTRNAME_GALLERYSIZE);
    if (attr == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_ANDR_SIZE_STDHEIGHTX2);
    int count = attr->getValue().toInt();
    // Clear and recrate all widgets.
    QHBoxLayout *lhItems = new QHBoxLayout();
    lhItems->setContentsMargins(2,2,2,2);
    lhItems->setSpacing(10);
    lhItems->addStretch();
    QLabel *labImg = new QLabel(elem);
    labImg->setStyleSheet("QLabel"
                          "{border: none;"
               "background-color: "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";"
                          "border-top-left-radius: 2px;"
                          "border-top-right-radius: 2px;"
                          "border-bottom-left-radius: 2px;"
                          "border-bottom-right-radius: 2px;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    labImg->setFixedWidth(FB_ANDR_SIZE_LARGESQUARE);
    labImg->setFixedHeight(FB_ANDR_SIZE_LARGESQUARE);
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setPixmap(pixPhoto);
    labImg->setContentsMargins(0,0,0,0);
    lhItems->insertWidget(0,labImg);
    for (int i=0; i<count; i++)
    {
        QLabel *labImg2 = new QLabel(elem);
        labImg2->setObjectName("lab_photo_"+QString::number(i));
        labImg2->setStyleSheet("QLabel"
             "{border: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";"
                    "background-color: rgba(0,0,0,0);"
                    "border-top-left-radius: 2px;"
                    "border-top-right-radius: 2px;"
                    "border-bottom-left-radius: 2px;"
                    "border-bottom-right-radius: 2px;}");
        labImg2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labImg2->setFixedWidth(FB_ANDR_SIZE_LARGESQUARE);
        labImg2->setFixedHeight(FB_ANDR_SIZE_LARGESQUARE);
        lhItems->insertWidget(1,labImg2);
    }
    elem->addAsDecor(lhItems);
}


void FBDecAndrSignature::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_ANDR_SIZE_STDHEIGHTX2);
    QLabel *labImg = new QLabel(elem);
    labImg->setStyleSheet("QLabel"
                "{border: 2px solid "+QString(FB_ANDR_COLOR_MEDIUMGREY)+";"
                          "background-color: white;"
                          "border-top-left-radius: 1px;"
                          "border-top-right-radius: 1px;"
                          "border-bottom-left-radius: 1px;"
                          "border-bottom-right-radius: 1px;}");
    labImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labImg->setFixedHeight(FB_ANDR_SIZE_LARGESQUARE);
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setPixmap(pixSign);
    labImg->setContentsMargins(0,0,0,0);
    elem->addAsDecor(labImg);
}


void FBDecAndrCounter::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labImg = new QLabel(elem);
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pix.width()); // sizes of pixmaps are equal
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setContentsMargins(0,0,0,0);
    labImg->setPixmap(pix);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    elem->registerAsDecor(labText,FB_NAMEDDECOR_TEXT);
    QHBoxLayout *hlayAll = new QHBoxLayout();
    hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);
    hlayAll->addWidget(labImg);
    hlayAll->addWidget(labText);
    elem->addAsDecor(hlayAll);
}
void FBDecAndrCounter::update (FBElem* elem)
{
    /*
    // Appearance depends on format and initial  value.
    if (elem == NULL) return;
    FBAttr *attrIv = elem->getAttr(FB_ATTRNAME_INITVALUE);
    FBAttr *attrPr = elem->getAttr(FB_ATTRNAME_PREFIX);
    FBAttr *attrSu = elem->getAttr(FB_ATTRNAME_SUFFIX);
    FBAttr *attrPrList = elem->getAttr(FB_ATTRNAME_PREFIX_FROM_LIST);
    FBAttr *attrSuList = elem->getAttr(FB_ATTRNAME_SUFFIX_FROM_LIST);
    if (attrIv == NULL || attrPr == NULL || attrSu == NULL
            || attrPrList == NULL || attrSuList == NULL) return;
    QString strIv = QString::number(attrIv->getValue().toInt());
    QString strPr = attrPr->getValue().toString();
    QString strSu = attrSu->getValue().toString();
    QString strPrList = attrPrList->getValue().toInt();
    QString strSuList = attrSuList->getValue().toInt();
    QLabel *labText = (QLabel*)elem->findAsDecor(FB_NAMEDDECOR_TEXT);
    if (labText == NULL) return;
    labText->setText(" " + strPr + strIv + strSu);
    */

    // TEMPORARY:
    QLabel *labText = (QLabel*)elem->findAsDecor(FB_NAMEDDECOR_TEXT);
    if (labText == NULL) return;
    labText->setText(QObject::tr("Counter"));
}


void FBDecAndrCoordinates::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labImg = new QLabel(elem);
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pix.width()); // sizes of pixmaps are equal
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setContentsMargins(0,0,0,0);
    labImg->setPixmap(pix);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    labText->setText("  0.000000  0.000000");
    QHBoxLayout *hlayAll = new QHBoxLayout();
    hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);
    hlayAll->addWidget(labImg);
    hlayAll->addWidget(labText);
    elem->addAsDecor(hlayAll);
}
void FBDecAndrCoordinates::update (FBElem* elem)
{
    return;
}


void FBDecAndrSplitCombobox::redecor (FBElem* elem)
{
    if (elem == NULL) return;

    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    elem->setFixedHeight(FB_ANDR_SIZE_STDHEIGHTX2);

    QLabel *lab1 = new QLabel(elem);
    lab1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    lab1->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    lab1->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    elem->registerAsDecor(lab1,"caption1");

    QLabel *lab2 = new QLabel(elem);
    lab2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    lab2->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    lab2->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    elem->registerAsDecor(lab2,"caption2");

    QLabel *labText1 = new QLabel(elem);
    elem->registerAsDecor(labText1,"combo1");
    labText1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText1->setFont(QFont(FB_ANDR_FONTTYPE,
                            FB_ANDR_FONTSIZE_NORMAL));
    labText1->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");

    QLabel *labText2 = new QLabel(elem);
    elem->registerAsDecor(labText2,"combo2");
    labText2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText2->setFont(QFont(FB_ANDR_FONTTYPE,
                            FB_ANDR_FONTSIZE_NORMAL));
    labText2->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_VERYDARKGREY)+";"
                           "border-top: none;"
                           "border-left: none;"
                           "border-right: none;"
                           "border-bottom: none;}");

    QWidget *widDecor1 = new QWidget(elem);
    widDecor1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor1->setMinimumHeight(7);
    widDecor1->setStyleSheet("QWidget"
                            "{border-top: none;"
                            "border-left: none;"
                            "border-right: none;"
                            "border-bottom: 2px solid "
                             +QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");

    QWidget *widDecor2 = new QWidget(elem);
    widDecor2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor2->setMinimumHeight(7);
    widDecor2->setStyleSheet("QWidget"
                            "{border-top: none;"
                            "border-left: none;"
                            "border-right: none;"
                            "border-bottom: 2px solid "
                             +QString(FB_ANDR_COLOR_MEDIUMGREY)+";}");

    QLabel *labImg1 = new QLabel(elem);
    labImg1->setStyleSheet("QLabel{border: none;}");
    labImg1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg1->setFixedWidth(pixCombo.width());
    labImg1->setAlignment(Qt::AlignBottom);
    labImg1->setPixmap(pixCombo);
    labImg1->setContentsMargins(0,0,0,0);

    QLabel *labImg2 = new QLabel(elem);
    labImg2->setStyleSheet("QLabel{border: none;}");
    labImg2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg2->setFixedWidth(pixCombo.width());
    labImg2->setAlignment(Qt::AlignBottom);
    labImg2->setPixmap(pixCombo);
    labImg2->setContentsMargins(0,0,0,0);

    QHBoxLayout *hlay1 = new QHBoxLayout();
    hlay1->setContentsMargins(0,0,0,0);
    hlay1->setSpacing(0);
    QHBoxLayout *hlay2 = new QHBoxLayout();
    hlay2->setContentsMargins(0,0,0,0);
    hlay2->setSpacing(0);
    QVBoxLayout *vlayText1 = new QVBoxLayout();
    vlayText1->setContentsMargins(0,0,0,0);
    vlayText1->setSpacing(0);
    QVBoxLayout *vlayText2 = new QVBoxLayout();
    vlayText2->setContentsMargins(0,0,0,0);
    vlayText2->setSpacing(0);
    QGridLayout *glayAll = new QGridLayout();
    glayAll->setContentsMargins(0,0,0,0);
    glayAll->setSpacing(8);

    vlayText1->addWidget(labText1);
    vlayText1->addWidget(widDecor1);
    vlayText2->addWidget(labText2);
    vlayText2->addWidget(widDecor2);
    hlay1->addLayout(vlayText1);
    hlay1->addWidget(labImg1);
    hlay2->addLayout(vlayText2);
    hlay2->addWidget(labImg2);
    glayAll->addWidget(lab1,0,0);
    glayAll->addWidget(lab2,0,1);
    glayAll->addLayout(hlay1,1,0);
    glayAll->addLayout(hlay2,1,1);

    elem->addAsDecor(glayAll);
}
void FBDecAndrSplitCombobox::update (FBElem* elem)
{
    if (elem == NULL) return;

    FBAttr *attrVals = elem->getAttr(FB_ATTRNAME_VALUE_mult);
    if (attrVals == NULL) return;
    QVariant varVals = attrVals->getValue();
    FBListValue2 tupleVals = varVals.value<FBListValue2>();

    FBAttr *attrCap1 = elem->getAttr(FB_ATTRNAME_LABEL1);
    if (attrCap1 == NULL) return;
    QVariant varCap1 = attrCap1->getValue();
    QString strCap1 = varCap1.value<QString>();

    FBAttr *attrCap2 = elem->getAttr(FB_ATTRNAME_LABEL2);
    if (attrCap2 == NULL) return;
    QVariant varCap2 = attrCap2->getValue();
    QString strCap2 = varCap2.value<QString>();

    QLabel *wCap1 = (QLabel*)elem->findAsDecor("caption1");
    QLabel *wCap2 = (QLabel*)elem->findAsDecor("caption2");
    QLabel *wCombo1 = (QLabel*)elem->findAsDecor("combo1");
    QLabel *wCombo2 = (QLabel*)elem->findAsDecor("combo2");
    if (wCap1 == NULL || wCap2 == NULL || wCombo1 == NULL || wCombo2 == NULL) return;

    QString strVals1 = "";
    QString strVals2 = "";
    QList<QPair<QString,QString> > listVals1 = std::get<0>(tupleVals);
    int nDefVal = std::get<1>(tupleVals);
    QList<QString> listVals2 = std::get<2>(tupleVals);
    if (nDefVal >= 0 && nDefVal < listVals1.size())
    {
        strVals1 = listVals1[nDefVal].second;
        strVals2 = listVals2[nDefVal];
    }

    wCap1->setText(" " + strCap1);
    wCap2->setText(" " + strCap2);
    wCombo1->setText(" " + strVals1);
    wCombo2->setText(" " + strVals2);
}


void FBDecAndrDistance::redecor (FBElem* elem)
{
    if (elem == NULL) return;
    this->clearWithDefaults(elem);
    QLabel *labText = new QLabel(elem);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setFont(QFont(FB_ANDR_FONTTYPE,
                           FB_ANDR_FONTSIZE_NORMAL));
    labText->setStyleSheet("QLabel"
                           "{color: "+QString(FB_ANDR_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
    labText->setText(QObject::tr("0 m"));
    elem->addAsDecor(labText, "");
}


// REGISTRAR:
void FBScreenAndroid::registerDecorators ()
{
    this->registerDecorator(FB_ELEMNAME_TEXT_LABEL, new FBDecAndrText());
    this->registerDecorator(FB_ELEMNAME_SPACE, new FBDecAndrSpace());
    this->registerDecorator(FB_ELEMNAME_TEXT_EDIT, new FBDecAndrTextedit());
    this->registerDecorator(FB_ELEMNAME_COMBOBOX, new FBDecAndrCombobox());
    this->registerDecorator(FB_ELEMNAME_DOUBLE_COMBOBOX, new FBDecAndrDoubCombobox());
    this->registerDecorator(FB_ELEMNAME_CHECKBOX, new FBDecAndrCheckbox());
    this->registerDecorator(FB_ELEMNAME_RADIOGROUP, new FBDecAndrRadiogroup());
    this->registerDecorator(FB_ELEMNAME_DATE_TIME, new FBDecAndrDatetime());
    this->registerDecorator(FB_ELEMNAME_BUTTON, new FBDecAndrButton());
    this->registerDecorator(FB_ELEMNAME_PHOTO, new FBDecAndrPhoto());
    this->registerDecorator(FB_ELEMNAME_SIGNATURE, new FBDecAndrSignature());
    this->registerDecorator(FB_ELEMNAME_COUNTER, new FBDecAndrCounter());
    this->registerDecorator(FB_ELEMNAME_COORDINATES, new FBDecAndrCoordinates());
    this->registerDecorator(FB_ELEMNAME_SPLIT_COMBOBOX, new FBDecAndrSplitCombobox());
    this->registerDecorator(FB_ELEMNAME_DISTANCE, new FBDecAndrDistance());
}
