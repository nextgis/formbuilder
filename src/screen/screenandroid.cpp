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

#include "screen.h"

FBScreenAndroid::FBScreenAndroid (QWidget* parent):
    FBScreenMobile (parent)
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

void FBScreenAndroid::updateStyle ()
{
    // Delete all decor widgets. Scrollarea and form are not touched.
    for (int i=0; i<labsScreenDecorVert.size(); i++)
    {
        lvMain1->removeWidget(labsScreenDecorVert[i]);
        delete labsScreenDecorVert[i];
    }
    labsScreenDecorVert.clear();
    for (int i=0; i<labsScreenDecorHor.size(); i++)
    {
        lhMain->removeWidget(labsScreenDecorHor[i]);
        delete labsScreenDecorHor[i];
    }
    labsScreenDecorHor.clear();

    // Clear old style.
    // Reset general stylesheet of the screen.
    // Grey background.
    this->setStyleSheet("QWidget {background-color: "
                           +QString(FB_COLOR_LIGHTGREY)+";"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;}");

    // Top black decor line.
    QLabel *labDecorTop = new QLabel(this);
    lvMain1->insertWidget(0,labDecorTop);
    labsScreenDecorVert.append(labDecorTop);
    labDecorTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labDecorTop->setFixedHeight(15);
    QHBoxLayout *layDecorTop = new QHBoxLayout(labDecorTop);
    layDecorTop->setContentsMargins(0,0,0,0);
    layDecorTop->setSpacing(0);
    QLabel *labDecorTop10 = new QLabel(labDecorTop);
    labDecorTop10->setPixmap(QPixmap(":/img/decor_andr_5.png"));
    labDecorTop10->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop11 = new QLabel(labDecorTop);
    labDecorTop11->setPixmap(QPixmap(":/img/decor_andr_6.png"));
    labDecorTop11->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layDecorTop->addSpacing(5);
    layDecorTop->addWidget(labDecorTop10);
    layDecorTop->addStretch();
    layDecorTop->addWidget(labDecorTop11);
    layDecorTop->addSpacing(5);

    // Top blue decor line.
    QLabel *labDecorTop2 = new QLabel(this);
    lvMain1->insertWidget(1,labDecorTop2);
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
    labDecorTop20->setPixmap(QPixmap(":/img/decor_andr_4.png"));
    labDecorTop20->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop21 = new QLabel(labDecorTop2);
    labDecorTop21->setPixmap(QPixmap(":/img/decor_andr_3.png"));
    labDecorTop21->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop22 = new QLabel(labDecorTop2);
    labDecorTop22->setPixmap(QPixmap(":/img/decor_andr_2.png"));
    labDecorTop22->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop23 = new QLabel(labDecorTop2);
    labDecorTop23->setPixmap(QPixmap(":/img/decor_andr_1.png"));
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
        lvMain1->insertWidget(3,labDecorBottom);
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
        labDecorBottom1->setPixmap(QPixmap(":/img/decor_andr_7.png"));
        labDecorBottom1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorBottom2 = new QLabel(labDecorBottom);
        labDecorBottom2->setPixmap(QPixmap(":/img/decor_andr_8.png"));
        labDecorBottom2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorBottom3 = new QLabel(labDecorBottom);
        labDecorBottom3->setPixmap(QPixmap(":/img/decor_andr_9.png"));
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
        lhMain->addWidget(labDecorRight);
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
        labDecorRight1->setPixmap(QPixmap(":/img/decor_andr_9_rot.png"));
        labDecorRight1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorRight2 = new QLabel(labDecorRight);
        labDecorRight2->setPixmap(QPixmap(":/img/decor_andr_8_rot.png"));
        labDecorRight2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QLabel *labDecorRight3 = new QLabel(labDecorRight);
        labDecorRight3->setPixmap(QPixmap(":/img/decor_andr_7_rot.png"));
        labDecorRight3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        layDecorRight->addStretch();
        layDecorRight->addWidget(labDecorRight1);
        layDecorRight->addStretch();
        layDecorRight->addWidget(labDecorRight2);
        layDecorRight->addStretch();
        layDecorRight->addWidget(labDecorRight3);
        layDecorRight->addStretch();
    }

    // Signalize to elems of the form to change style accordingly.
    if (formPtr != NULL)
        formPtr->updateStyle(FB_STYLENAME_ANDROID);
}


