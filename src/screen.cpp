/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  basic screen implementations
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


/****************************************************************************/
/*                                FBScreen                                  */
/****************************************************************************/

FBScreen::FBScreen (QWidget *parent):
    QWidget(parent)
{
    glWorkingArea = new QGridLayout(this);
    glWorkingArea->setContentsMargins(0,0,0,0);
    glWorkingArea->setSpacing(0);

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (i == 1 && j == 1)
                continue;
            QWidget *w = new QWidget(this);
            w->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);
            glWorkingArea->addWidget(w,i,j); // will be 7 items in array
            wsWorkingArea.append(w);
        }
    }

    wScreen = new QWidget(this);
    wScreen->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);
    glWorkingArea->addWidget(wScreen,1,1);

    lvScreen= new QVBoxLayout(wScreen);
    lvScreen->setContentsMargins(0,0,0,0);
    lvScreen->setSpacing(0);

    scrollScreen = new QScrollArea(wScreen);
    scrollScreen->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    scrollScreen->setStyleSheet("QScrollArea{border: 0px;}");
    scrollScreen->setWidgetResizable(true);
    lvScreen->addWidget(scrollScreen);

    formPtr = NULL;

    // Default states: only maximized screen.
    states.append(FBState(
                  tr("Maximized"),tr("Maximize"),":/img/maximized.png"));
    curState = 0;
    curDevice = -1;
}

FBScreen::~FBScreen ()
{
}

void FBScreen::setForm (FBForm* form)
{
    if (form != NULL)
        this->removeForm();
    scrollScreen->setWidget(form);
    formPtr = form;
}

void FBScreen::removeForm ()
{
    if (formPtr == NULL)
        return;
    scrollScreen->takeWidget();
    delete formPtr;
    formPtr = NULL;
}

FBForm *FBScreen::takeForm ()
{
    if (formPtr == NULL)
        return NULL;
    FBForm *wForm = (FBForm*)scrollScreen->takeWidget(); // can cast, because it was
                                                         // set via specific method
    formPtr = NULL;
    return wForm;
}

FBForm *FBScreen::getFormPtr ()
{
    return formPtr;
}

void FBScreen::updateStyle ()
{
    // Clear old style.
    // Reset general stylesheet of the screen.
    wScreen->setStyleSheet("");

    // Grey background.
    wScreen->setStyleSheet("QWidget {background-color: "
                           +QString(FB_COLOR_LIGHTGREY)+";"
                           "border-top-left-radius: 4px;"
                           "border-top-right-radius: 4px;"
                           "border-bottom-left-radius: 4px;"
                           "border-bottom-right-radius: 4px;}");

    // Signalize to form and its elems to change style accordingly.
    if (formPtr != NULL)
        formPtr->updateStyle(FB_STYLENAME_DEFAULT);
}

void FBScreen::setState (int index)
{
    if (index != 0) // default state is only one
        return;

    // Maximize working area.
    for (int i=0; i<wsWorkingArea.size(); i++)
    {
        //wsWorkingArea[i]->setMinimumSize(0,0);
        wsWorkingArea[i]->setSizePolicy(QSizePolicy::Minimum,
                                        QSizePolicy::Minimum);
    }
}

void FBScreen::setDevice (int index)
{
    return; // no default device
}


/****************************************************************************/
/*                             FBScreenMobile                               */
/****************************************************************************/


FBScreenMobile::FBScreenMobile (QWidget *parent):
    FBScreen (parent)
{
    devices.append(FBDevice(
                   QPair<int,int>(800,480),7.0,1.0,"Default mobile",""));

    // WARNING. Do not forget to change indexes in the state setting method.
    states.append(FBState(
                  tr("Portrait"),tr("Portrait orientation"),""));
    states.append(FBState(
                  tr("Landscape"),tr("Landscape orientation"),""));

    // TODO: remove maximized screen here?

    curDevice = 0;
    curState = 1; // portrait
}

FBScreenMobile::~FBScreenMobile ()
{
}

void FBScreenMobile::setState (int index)
{
    if (index < 0 || index > states.size()-1
            || index == curState)
        return;

    if (index == 1) // portrait
    {
        for (int i=0; i<3; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        for (int i=5; i<8; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        wsWorkingArea[3]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        wsWorkingArea[4]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    else if (index == 2) // landscape
    {
        for (int i=0; i<3; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (int i=5; i<8; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        wsWorkingArea[3]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        wsWorkingArea[4]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    else // default type = maximized
    {
        FBScreen::setState(0);
    }

    curState = index;
}

void FBScreenMobile::setDevice (int index)
{
    if (index < 0 || index > devices.size()-1
            || index == curDevice)
        return;

    // Apply screen settings.


    curDevice = index;
}

/****************************************************************************/
/*                            FBScreenAndroid                               */
/****************************************************************************/

FBScreenAndroid::FBScreenAndroid (QWidget* parent):
    FBScreenMobile (parent)
{ 
    devices.append(FBDevice(
                   QPair<int,int>(1280,720),6.3f,1.5f,"Galaxy Mega",""));

    curDevice = 0;
    curState = 1; // portrait
}

FBScreenAndroid::~FBScreenAndroid ()
{
}

void FBScreenAndroid::updateStyle ()
{
    FBScreen::updateStyle();

    // Delete all decor widgets except Scrollarea+form.
    for (int i=0; i<labsScreenDecor.size(); i++)
    {
        lvScreen->removeWidget(labsScreenDecor[i]);
        delete labsScreenDecor[i];
    }
    labsScreenDecor.clear();

    // Top black decor line.
    QLabel *labDecorTop = new QLabel(wScreen);
    lvScreen->insertWidget(0,labDecorTop);
    labsScreenDecor.append(labDecorTop);
    labDecorTop->setStyleSheet("QLabel"
                               "{"
                                   "background-color: black;"
                                   "border-top-left-radius: 4px;"
                                   "border-top-right-radius: 4px;"
                               "}");
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
    QLabel *labDecorTop2 = new QLabel(wScreen);
    lvScreen->insertWidget(1,labDecorTop2);
    labsScreenDecor.append(labDecorTop2);
    labDecorTop2->setStyleSheet("QLabel{background-color: rgb(3,169,244)}");
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

    // Bottom black decor line.
    QLabel *labDecorBottom = new QLabel(wScreen);
    lvScreen->insertWidget(3,labDecorBottom);
    labsScreenDecor.append(labDecorBottom);
    labDecorBottom->setStyleSheet("QLabel"
                                  "{"
                                      "background-color: black;"
                                      "border-bottom-left-radius: 4px;"
                                      "border-bottom-right-radius: 4px;"
                                  "}");
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

    // Signalize to form and its elems to change style accordingly.
    formPtr->updateStyle(FB_STYLENAME_ANDROID);
}




/*
QList<QPair<QString,QString> > FBWorkingArea::STYLES;
QList<QPair<QString,QString> > FBWorkingArea::TYPES;
QList<QString> FBWorkingArea::RATIOS;
QList<QString> FBWorkingArea::RESOLS;

void FBWorkingArea::initAll ()
{
    STYLES.append(QPair<QString,QString>(FB_SCREEN_ANDROID,
                                         ":/img/android.png"));
    STYLES.append(QPair<QString,QString>(FB_SCREEN_QGIS,
                                         ":/img/qgis.png"));
    STYLES.append(QPair<QString,QString>(FB_SCREEN_WEB,
                                         ":/img/web.png"));

    TYPES.append(QPair<QString,QString>(FB_SCREEN_MAXIMIZED,
                                        ":/img/maximized.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_PHONEPORT,
                                        ":/img/phone_port.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_PHONELAND,
                                        ":/img/phone_land.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_TABLETPORT,
                                        ":/img/tablet_port.png"));
    TYPES.append(QPair<QString,QString>(FB_SCREEN_TABLETLAND,
                                        ":/img/tablet_land.png"));

    RATIOS.append("15:9");
    RATIOS.append("16:9");
    RATIOS.append("3:4");

    RESOLS.append("480x800");
    RESOLS.append("1366x768");
    RESOLS.append("1280x1024");
}
 

FBWorkingArea::FBWorkingArea (QWidget *parent): QWidget(parent)
{
    glWorkingArea = new QGridLayout(this);
    glWorkingArea->setContentsMargins(0,0,0,0);
    glWorkingArea->setSpacing(0);

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (i == 1 && j == 1)
                continue;
            QWidget *w = new QWidget(this);
            w->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);
            glWorkingArea->addWidget(w,i,j); // will be 7 items in array
            wsWorkingArea.append(w);
        }
    }

    wScreen = new QWidget(this);
    wScreen->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);
    glWorkingArea->addWidget(wScreen,1,1);

    vlScreen = new QVBoxLayout(wScreen);
    vlScreen->setContentsMargins(0,0,0,0);
    vlScreen->setSpacing(0);

    scrollScreen = new QScrollArea(wScreen);
    scrollScreen->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    scrollScreen->setStyleSheet("QScrollArea{border: 0px;}");
    scrollScreen->setWidgetResizable(true);
    vlScreen->addWidget(scrollScreen);

    form = NULL;
    scrollScreen->setWidget(form);

    // We must set default style at the construction time, becuse
    // it will be no project rendering at first and at the same time
    // no style of target device specified.
    this->changeStyle("default"); // just any string to set defaults
    this->changeType("default");
}


// Change only decoration and do not touch form.
void FBWorkingArea::changeStyle (QString styleStr)
{
    //----------------------------------------------------------------------
    //                      Clear previous screen
    //----------------------------------------------------------------------

    // Reset general stylesheet of the screen.
    wScreen->setStyleSheet("");

    // Delete all decor widgets except Scrollarea+form.
    for (int i=0; i<labsScreenDecor.size(); i++)
    {
        vlScreen->removeWidget(labsScreenDecor[i]);
        delete labsScreenDecor[i];
    }
    labsScreenDecor.clear();

    //----------------------------------------------------------------------
    //                           Android screen
    //----------------------------------------------------------------------
    if (style == FB_SCREEN_ANDROID)
    {

        // The background.
        wScreen->setStyleSheet("QWidget {background-color: "
                               +QString(FB_COLOR_LIGHTGREY)+";"
                               "border-top-left-radius: 4px;"
                               "border-top-right-radius: 4px;"
                               "border-bottom-left-radius: 4px;"
                               "border-bottom-right-radius: 4px;}");

        // Top black decor line.
        QLabel *labDecorTop = new QLabel();
        labDecorTop->setStyleSheet("QLabel"
                                   "{"
                                       "background-color: black;"
                                       "border-top-left-radius: 4px;"
                                       "border-top-right-radius: 4px;"
                                   "}");
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
        labsScreenDecor.append(labDecorTop);

        // Top blue decor line.
        QLabel *labDecorTop2 = new QLabel();
        labDecorTop2->setStyleSheet("QLabel{background-color: rgb(3,169,244)}");
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
        labsScreenDecor.append(labDecorTop2);

        // Bottom black decor line.
        QLabel *labDecorBottom = new QLabel();
        labDecorBottom->setStyleSheet("QLabel"
                                      "{"
                                          "background-color: black;"
                                          "border-bottom-left-radius: 4px;"
                                          "border-bottom-right-radius: 4px;"
                                      "}");
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
        labsScreenDecor.append(labDecorBottom);

        // Add decorations in required order

            ...
    }

    //----------------------------------------------------------------------
    //                           Default screen
    //----------------------------------------------------------------------

    else
    {
        // Just simple grey screen for default.
        wScreen->setStyleSheet("QWidget {background-color: "
                               +QString(FB_COLOR_LIGHTGREY)+";"
                               "border-top-left-radius: 4px;"
                               "border-top-right-radius: 4px;"
                               "border-bottom-left-radius: 4px;"
                               "border-bottom-right-radius: 4px;}");
    }

    this->style = styleStr;
}


void FBWorkingArea::changeType (QString typeStr)
{
    if (typeStr == FB_SCREEN_PHONEPORT
            || typeStr == FB_SCREEN_TABLETPORT)
    {
        for (int i=0; i<3; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        for (int i=5; i<8; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        wsWorkingArea[3]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        wsWorkingArea[4]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    else if (typeStr == FB_SCREEN_PHONELAND)
    {
        for (int i=0; i<3; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (int i=5; i<8; i++) wsWorkingArea[i]->setSizePolicy(
                    QSizePolicy::Expanding, QSizePolicy::Expanding);
        wsWorkingArea[3]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
        wsWorkingArea[4]->setSizePolicy(
                    QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    //...

    else // default type = FB_SCREEN_MAXIMIZED
    {
        for (int i=0; i<wsWorkingArea.size(); i++)
        {
            //wsWorkingArea[i]->setMinimumSize(0,0);
            wsWorkingArea[i]->setSizePolicy(QSizePolicy::Minimum,
                                            QSizePolicy::Minimum);
        }
    }

    this->type = typeStr;
}


void FBWorkingArea::changeRatio (int index)
{

}


void FBWorkingArea::changeResol (int index)
{

}

*/



