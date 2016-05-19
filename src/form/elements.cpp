/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  various elements implementation
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

void FBElemInput::updateFields (QStringList fieldsKeyNames) // STATIC
{
    FBAttrField::updateValues(fieldsKeyNames);
}

FBElemInput::FBElemInput (FBFactory *fctPtr):
    FBElem (fctPtr)
{
    // ATTRIBUTE
    attrFieldPtr = new FBAttrField(this, FB_ATTRNAME_FIELD,
                  tr("Layer field"), FBImportant);
    attrs.insert(attrFieldPtr);
}

void FBElemInput::resetSelectedField (QString keyname) // keyname unused here
{
    attrFieldPtr->resetValue();
}

QStringList FBElemInput::getSelectedFields ()
{
    QStringList list;
    QString selectedField = attrFieldPtr->getValue();
    if (selectedField != FB_DEFVALUE_NOTSELECTED)
    {
        list.append(selectedField); // otherwise void list will be returned
    }
    return list;
}


/*****************************************************************************/
/*                                                                           */
/*                          FBElemInputVariate                               */
/*                                                                           */
/*****************************************************************************/

FBElemInputVariate::FBElemInputVariate (FBFactory *fctPtr):
    FBElemInput (fctPtr)
{
    // ATTRIBUTE
    attrKeepLastPtr = new FBAttrBoolean(this, FB_ATTRNAME_STORELAST,
                  tr("Keep last value"), FBNoRole, false);
    attrs.insert(attrKeepLastPtr);
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemText                                    */
/*                                                                            */
/******************************************************************************/

FBElemText::FBElemText (FBFactory *fctPtr):
    FBElem(fctPtr)
{
    // ATTRIBUTE
    attrTextPtr = new FBAttrText(this, FB_ATTRNAME_TEXT,
                  tr("Text"), FBNoRole, tr("Caption"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrTextPtr);
}

void FBElemText::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);

    // ANDROID STYLE
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
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // DEVELOPERS: add style here.
    // ...

    // DEFAULT STYLE
    else
    {
        labText = new QLabel(this);
        labText->setStyleSheet("QLabel{border:none;}");
        lvMain->addWidget(labText);
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // UPDATE STYLE
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
/*                              FBElemSpace                                   */
/*                                                                            */
/******************************************************************************/

FBElemSpace::FBElemSpace (FBFactory *fctPtr):
    FBElem(fctPtr)
{
    // No attributes.
}

void FBElemSpace::changeStyle (QString styleName)
{
    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setMinimumHeight(30);
}

/******************************************************************************/
/*                                                                            */
/*                              FBElemTextedit                                */
/*                                                                            */
/******************************************************************************/

FBElemTextedit::FBElemTextedit (FBFactory *fctPtr):
    FBElemInputVariate(fctPtr)
{
    // ATTRIBUTE
    attrTextPtr = new FBAttrText(this, FB_ATTRNAME_TEXT,
                  tr("Initial text"), FBNoRole, tr("Text"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrTextPtr);

    // ATTRIBUTE
    FBAttrNumber *attrMaxCountPtr = new FBAttrNumber(this, FB_ATTRNAME_MAXSTRINGCOUNT,
                  tr("Max. string count"), FBNoRole, 1, 1, 65535);
    attrs.insert(attrMaxCountPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrOnlyNumbers = new FBAttrBoolean(this, FB_ATTRNAME_ONLYNUMBERS,
                  tr("Only numbers"), FBNoRole, false);
    attrs.insert(attrOnlyNumbers);
}

void FBElemTextedit::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);

    // ANDROID STYLE
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
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        labText = new QLabel(this);
        labText->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: 1px solid black;"
                               "border-radius: none;}");
        lvMain->addWidget(labText);
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // UPDATE STYLE
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
    FBElemInputVariate(fctPtr)
{
    // ATTRIBUTE
    attrListvalsPtr = new FBAttrListvalues(this, FB_ATTRNAME_VALUE_mult,
                  tr("Values"), FBNoRole, appWidget);
    QObject::connect(attrListvalsPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrListvalsPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrInputSearch = new FBAttrBoolean(this, FB_ATTRNAME_INPUTSEARCH,
                  tr("Input with search"), FBNoRole, false);
    attrs.insert(attrInputSearch);

    // ATTRIBUTE
    FBAttrBoolean *attrAllowVals = new FBAttrBoolean(this, FB_ATTRNAME_ALLOWADDING,
                  tr("Allow own values"), FBNoRole, false);
    attrs.insert(attrAllowVals);
}

void FBElemCombobox::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);

    // ANDROID STYLE
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
        lvMain->setContentsMargins(0,0,0,0);
        lvMain->setSpacing(0);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        labText = new QLabel(this);
        labText->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;"
                               "border-radius: none;}");
        lvMain->addWidget(labText);
        lvMain->setContentsMargins(0,0,0,0);
        lvMain->setSpacing(0);
    }

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemCombobox::changeAttrValue ()
{

}

void FBElemCombobox::updateAppearance ()
{
    QString text = attrListvalsPtr->getDefDispValue();
    if (text == FB_DEFVALUE_NOTSELECTED)
        text = "";
    labText->setText(" " + text);
}


/******************************************************************************/
/*                                                                            */
/*                           FBElemDoublecombobox                             */
/*                                                                            */
/******************************************************************************/

FBElemDoublecombobox::FBElemDoublecombobox (FBFactory *fctPtr, QWidget *appWidget):
    FBElemInputVariate(fctPtr)
{
    // REMOVE ATTRIBUTE
    attrs.remove(attrFieldPtr);
    delete attrFieldPtr;

    // ATTRIBUTE
    FBAttrField *attrField1Ptr = new FBAttrField(this, FB_ATTRNAME_FIELD_dcmb_1,
                  tr("Layer field for 1"), FBImportant);
    attrs.insert(attrField1Ptr);

    // ATTRIBUTE
    FBAttrField *attrField2Ptr = new FBAttrField(this, FB_ATTRNAME_FIELD_dcmb_2,
                  tr("Layer field for 2"), FBImportant);
    attrs.insert(attrField2Ptr);

    // ATTRIBUTE
    attrListvalsDoublePtr = new FBAttrListvaluesDouble(this, FB_ATTRNAME_VALUE_mult,
                  tr("Values"), FBNoRole, appWidget);
    QObject::connect(attrListvalsDoublePtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrListvalsDoublePtr);
}

void FBElemDoublecombobox::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    this->setMaximumHeight(60);

    // ANDROID STYLE
    if (styleName == FB_STYLENAME_ANDROID)
    {
        labText1 = new QLabel(this);
        labText1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText1->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                                FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText1->setStyleSheet("QLabel"
                               "{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
        labText2 = new QLabel(this);
        labText2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText2->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                                FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText2->setStyleSheet("QLabel"
                               "{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
        QWidget *widDecor1 = new QWidget(this);
        widDecor1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        widDecor1->setMinimumHeight(7);
        widDecor1->setStyleSheet("QWidget"
                                "{border-top: none;"
                                "border-left: none;"
                                "border-right: none;"
                                "border-bottom: 3px solid "
                                 +QString(FB_COLOR_MEDIUMGREY)+";}");
        QWidget *widDecor2 = new QWidget(this);
        widDecor2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        widDecor2->setMinimumHeight(7);
        widDecor2->setStyleSheet("QWidget"
                                "{border-top: none;"
                                "border-left: none;"
                                "border-right: none;"
                                "border-bottom: 3px solid "
                                 +QString(FB_COLOR_MEDIUMGREY)+";}");
        QPixmap pixmap = QPixmap(":/img/for_combo.png");
        QLabel *labImg1 = new QLabel(this);
        labImg1->setStyleSheet("QLabel{border: none;}");
        labImg1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg1->setFixedWidth(pixmap.width());
        labImg1->setAlignment(Qt::AlignBottom);
        labImg1->setPixmap(pixmap);
        labImg1->setContentsMargins(0,0,0,0);
        QLabel *labImg2 = new QLabel(this);
        labImg2->setStyleSheet("QLabel{border: none;}");
        labImg2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg2->setFixedWidth(pixmap.width());
        labImg2->setAlignment(Qt::AlignBottom);
        labImg2->setPixmap(pixmap);
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
        lvMain->addLayout(hlay1);
        lvMain->addLayout(hlay2);
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(4);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        labText1 = new QLabel(this);
        labText1->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;"
                               "border-radius: none;}");
        labText2 = new QLabel(this);
        labText2->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;"
                               "border-radius: none;}");
        lvMain->addWidget(labText1);
        lvMain->addWidget(labText2);
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(4);
    }

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemDoublecombobox::changeAttrValue ()
{

}

void FBElemDoublecombobox::updateAppearance ()
{
    QString str1;
    QString str2;
    attrListvalsDoublePtr->getDefDispValues(str1,str2);
    if (str1 == FB_DEFVALUE_NOTSELECTED)
        str1 = "";
    if (str2 == FB_DEFVALUE_NOTSELECTED)
        str2 = "";
    labText1->setText(" " + str1);
    labText2->setText(" " + str2);
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemCheckbox                                */
/*                                                                            */
/******************************************************************************/

FBElemCheckbox::FBElemCheckbox (FBFactory *fctPtr):
    FBElemInputVariate(fctPtr)
{
    pixOnAndroid = QPixmap(":/img/for_check_on.png");
    pixOffAndroid = QPixmap(":/img/for_check_off.png");

    // ATTRIBUTE
    attrTextPtr = new FBAttrText(this, FB_ATTRNAME_TEXT,
                  tr("Text"), FBNoRole, tr("Check"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrTextPtr);

    // ATTRIBUTE
    attrCheckPtr = new FBAttrBoolean(this, FB_ATTRNAME_INITVALUE,
                  tr("Initial value"), FBNoRole, false);
    QObject::connect(attrCheckPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrCheckPtr);
}

void FBElemCheckbox::changeStyle (QString styleName)
{
    curStyleName = styleName;

    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);

    // ANDROID STYLE
    if (styleName == FB_STYLENAME_ANDROID)
    {
        QHBoxLayout *hlayAll = new QHBoxLayout();
        hlayAll->setContentsMargins(2,2,2,2);
        hlayAll->setSpacing(0);
        labImg = new QLabel(this);
        labImg->setStyleSheet("QLabel{border: none;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg->setFixedWidth(pixOffAndroid.width()); // sizes of pixmaps are equal
        labImg->setAlignment(Qt::AlignCenter);
        labImg->setPixmap(pixOffAndroid);
        labImg->setContentsMargins(0,0,0,0);
        labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                               FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText->setStyleSheet("QLabel{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                                   "border-top: none;"
                                   "border-left: none;"
                                   "border-right: none;"
                                   "border-bottom: none;}");
        hlayAll->addWidget(labImg);
        hlayAll->addWidget(labText);
        lvMain->addLayout(hlayAll);
        lvMain->setContentsMargins(0,0,0,0);
        lvMain->setSpacing(0);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        QHBoxLayout *hlayAll = new QHBoxLayout();
        hlayAll->setContentsMargins(0,0,0,0);
        hlayAll->setSpacing(0);
        labImg = new QLabel(this);
        labImg->setStyleSheet("QLabel{"
                              "border: 1px solid black;"
                              "border-radius: none;}");
        labImg->setText("  ");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labText = new QLabel(this);
        labText->setStyleSheet("QLabel{border: none;}");
        hlayAll->addWidget(labImg);
        hlayAll->addWidget(labText);
        lvMain->addLayout(hlayAll);
        lvMain->setContentsMargins(0,0,0,0);
        lvMain->setSpacing(0);
    }

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemCheckbox::changeAttrValue ()
{

}

void FBElemCheckbox::updateAppearance ()
{
    labText->setText(" " + attrTextPtr->getValue());

    // ANDROID STYLE
    if (curStyleName == FB_STYLENAME_ANDROID)
    {
        if (attrCheckPtr->getValue() == true)
            labImg->setPixmap(pixOnAndroid);
        else
            labImg->setPixmap(pixOffAndroid);
    }
}


/******************************************************************************/
/*                                                                            */
/*                             FBElemRadiogroup                               */
/*                                                                            */
/******************************************************************************/

FBElemRadiogroup::FBElemRadiogroup (FBFactory *fctPtr, QWidget *appWidget):
    FBElemInputVariate(fctPtr)
{
    pixOnAndroid = QPixmap(":/img/for_radio_on.png");
    pixOffAndroid = QPixmap(":/img/for_radio_off.png");

    // ATTRIBUTE
    attrListvalsStrictPtr = new FBAttrListvaluesStrict(this, FB_ATTRNAME_VALUE_mult,
                  tr("Values"), FBNoRole, appWidget);
    QObject::connect(attrListvalsStrictPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrListvalsStrictPtr);

}

void FBElemRadiogroup::changeStyle (QString styleName)
{
    curStyleName = styleName;

    // ALL STYLES
    // Size policy fixed for all styles: radio-group must be extended for
    // its content: items.
    // Maximum height can be any.
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);

    // NOTE: style settings moved to updateAppearance().
    // Unlike in many other Elems, we change style there, because
    // the whole look of the Radio-group depends on its attribute values.

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemRadiogroup::changeAttrValue ()
{

}

void FBElemRadiogroup::updateAppearance ()
{
    QStringList list = attrListvalsStrictPtr->getDispValues();
    int selectedIndex = attrListvalsStrictPtr->getDefIndex();

    // CLEAR STYLE
    this->clearContents();

    // ANDROID STYLE
    if (curStyleName == FB_STYLENAME_ANDROID)
    {
        for (int i=0; i<list.size(); i++)
        {
            QLabel *labText = new QLabel(this);
            labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                                   FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
            labText->setStyleSheet("QLabel{color: "+QString(FB_COLOR_VERYDARKGREY)+";"
                                       "border-top: none;"
                                       "border-left: none;"
                                       "border-right: none;"
                                       "border-bottom: none;}");
            labText->setText(list[i]);
            QLabel *labImg = new QLabel(this);
            labImg->setStyleSheet("QLabel{border: none;}");
            labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
            labImg->setFixedWidth(pixOffAndroid.width()); // sizes are equal
            labImg->setAlignment(Qt::AlignCenter);
            labImg->setContentsMargins(0,0,0,0);
            if (i == selectedIndex) // will be found anyway, because radio-group must
            {                       // have selected value
                labImg->setPixmap(pixOnAndroid);
            }
            else
            {
                labImg->setPixmap(pixOffAndroid);
            }
            QHBoxLayout *hlay = new QHBoxLayout();
            hlay->addWidget(labImg);
            hlay->addWidget(labText);
            lvMain->addLayout(hlay);
        }
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(6);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        for (int i=0; i<list.size(); i++)
        {
            QHBoxLayout *hlay = new QHBoxLayout();
            hlay->setContentsMargins(0,0,0,0);
            hlay->setSpacing(5);
            QLabel *labImg = new QLabel(this);
            labImg->setStyleSheet("QLabel{"
                                  "border: 1px solid black;"
                                  "border-radius: 7px;}");
            labImg->setText("  ");
            labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            QLabel *labText = new QLabel(this);
            labText->setStyleSheet("QLabel{border: none;}");
            labText->setText(list[i]);
            hlay->addWidget(labImg);
            hlay->addWidget(labText);
            lvMain->addLayout(hlay);
        }
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(6);
    }
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemDatetime                                */
/*                                                                            */
/******************************************************************************/

FBElemDatetime::FBElemDatetime (FBFactory *fctPtr, QWidget *appWidget):
    FBElemInputVariate(fctPtr)
{
    // ATTRIBUTE
    QStringList strs;
    for (int i=0; i<FBForm::DATETIME_FORMATS.size(); i++)
    {
        strs.append(FBForm::DATETIME_FORMATS[i]->name);
    }
    attrTypePtr = new FBAttrSelect(this, FB_ATTRNAME_DATETYPE,
                  tr("Type"), FBNoRole, strs, 0);
    QObject::connect(attrTypePtr, SIGNAL(changeOtherAttr()),
                     this, SLOT(changeAttrValue()));          // will have influence
    QObject::connect(attrTypePtr, SIGNAL(changeAppearance()), // on the attrDatetimePtr
                     this, SLOT(updateAppearance()));
    attrs.insert(attrTypePtr);

    // ATTRIBUTE
    attrDatetimePtr = new FBAttrDatetime(this, FB_ATTRNAME_INITVALUE_datetime,
                  tr("Initial value"), FBNoRole, appWidget);
    QObject::connect(attrDatetimePtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrDatetimePtr);
}

void FBElemDatetime::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);

    // ANDROID STYLE
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
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        QHBoxLayout *hlayAll = new QHBoxLayout();
        hlayAll->setContentsMargins(0,0,0,0);
        hlayAll->setSpacing(5);
        QLabel *labImg = new QLabel(this);
        labImg->setStyleSheet("QLabel{"
                              "border: 1px solid black;"
                              "border-radius: none;}");
        labImg->setText("          ");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labText = new QLabel(this);
        labText->setStyleSheet("QLabel{border: none;}");
        hlayAll->addWidget(labText);
        hlayAll->addWidget(labImg);
        lvMain->addLayout(hlayAll);
        lvMain->setContentsMargins(0,0,0,0);
        lvMain->setSpacing(0);
    }

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemDatetime::changeAttrValue ()
{
    attrDatetimePtr->changeFormat(
                FBForm::DATETIME_FORMATS[attrTypePtr->getValue()]);
}

void FBElemDatetime::updateAppearance ()
{
    labText->setText(" " + attrDatetimePtr->getValueString());
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemButton                                   */
/*                                                                           */
/*****************************************************************************/

FBElemButton::FBElemButton (FBFactory *fctPtr):
    FBElemInput(fctPtr)
{
    // ATTRIBUTE
    attrTextPtr = new FBAttrText(this, FB_ATTRNAME_TEXT,
                  tr("Text"), FBNoRole, tr("Press"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrTextPtr);

    // ATTRIBUTE
    FBAttrText *attrValuePtr = new FBAttrText(this, FB_ATTRNAME_VALUE,
                  tr("Value"), FBNoRole, false);
    attrs.insert(attrValuePtr);
}

void FBElemButton::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    // ANDROID STYLE
    if (styleName == FB_STYLENAME_ANDROID)
    {
        this->setMaximumHeight(36);
        this->setMinimumHeight(36);
        labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        labText->setFont(QFont(FB_ANDROIDSCREEN_FONTTYPE,
                               FB_ANDROIDSCREEN_FONTSIZE_NORMAL));
        labText->setAlignment(Qt::AlignCenter);
        labText->setStyleSheet("QLabel{color: "+QString(FB_COLOR_DARKGREY)+";"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: 1px solid "+FB_COLOR_DARKGREY+";"
                               "background-color: "+FB_COLOR_LIGHTMEDIUMGREY+";"
                               "border-top-left-radius: 1px;"
                               "border-top-right-radius: 1px;"
                               "border-bottom-left-radius: 1px;"
                               "border-bottom-right-radius: 1px;}");
        lvMain->addWidget(labText);
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        this->setMinimumHeight(25);
        this->setMinimumHeight(25);
        labText = new QLabel(this);
        labText->setAlignment(Qt::AlignCenter);
        labText->setStyleSheet("QLabel{"
                               "border: 1px solid black;"
                               "border-radius: none}");
        lvMain->addWidget(labText);
        lvMain->setContentsMargins(2,2,2,2);
        lvMain->setSpacing(0);
    }

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemButton::changeAttrValue ()
{

}

void FBElemButton::updateAppearance ()
{
    labText->setText(" " + attrTextPtr->getValue());
}



/*****************************************************************************/
/*                                                                           */
/*                            FBElemPhoto                                    */
/*                                                                           */
/*****************************************************************************/

FBElemPhoto::FBElemPhoto (FBFactory *fctPtr):
    FBElem(fctPtr)
{
    // ATTRIBUTE
    attrMaxCountPtr = new FBAttrNumber(this, FB_ATTRNAME_GALLERYSIZE,
                  tr("Max. photo count"), FBNoRole, 1, 1, 5);
    QObject::connect(attrMaxCountPtr, SIGNAL(changeAppearance()),
                     this, SLOT(updateAppearance()));
    attrs.insert(attrMaxCountPtr);
}

void FBElemPhoto::changeStyle (QString styleName)
{
    curStyleName = styleName; // so to use it in update appearance method

    // CLEAR STYLE
    this->clearContents();
    labsImg.clear(); // do not forget to clear this as it contains invalid pointers now!

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(60);
    lhItems = new QHBoxLayout();
    lhItems->setContentsMargins(2,2,2,2);
    lhItems->setSpacing(10);
    lhItems->addStretch();
    lvMain->setContentsMargins(0,0,0,0);
    lvMain->setSpacing(0);
    lvMain->addLayout(lhItems);

    // ANDROID STYLE
    if (styleName == FB_STYLENAME_ANDROID)
    {
        QLabel *labImg = new QLabel(this);
        QPixmap pixmap = QPixmap(":/img/for_photo.png");
        labImg->setStyleSheet("QLabel"
                              "{border: none;"
                              "background-color: "+QString(FB_COLOR_MEDIUMGREY)+";"
                              "border-top-left-radius: 2px;"
                              "border-top-right-radius: 2px;"
                              "border-bottom-left-radius: 2px;"
                              "border-bottom-right-radius: 2px;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labImg->setFixedWidth(56);
        labImg->setFixedHeight(56);
        labImg->setAlignment(Qt::AlignCenter);
        labImg->setPixmap(pixmap);
        labImg->setContentsMargins(0,0,0,0);
        lhItems->insertWidget(0,labImg);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        QLabel *labImg = new QLabel(this);
        labImg->setStyleSheet("QLabel"
                              "{border: 2px solid black;"
                              "border-radius: none;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labImg->setFixedWidth(40);
        labImg->setFixedHeight(40);
        lhItems->insertWidget(0,labImg);
    }

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemPhoto::changeAttrValue ()
{

}

void FBElemPhoto::updateAppearance ()
{
    int photoCount = attrMaxCountPtr->getValue();

    // Delete old items.
    QLayoutItem *childIem;
    for (int i=0; i<labsImg.size(); i++)
    {
        childIem = lhItems->takeAt(lhItems->indexOf(labsImg[i]));
        delete childIem->widget();
    }
    labsImg.clear();

    // Add new items.
    for (int i=0; i<photoCount; i++)
    {
        QLabel *labImg = new QLabel(this);
        labImg->setObjectName("lab_photo_"+QString::number(i));

        // ANDROID STYLE
        if (curStyleName == FB_STYLENAME_ANDROID)
        {
            labImg->setStyleSheet("QLabel"
                                   "{border: 2px solid "+QString(FB_COLOR_MEDIUMGREY)+";"
                                   "background-color: rgba(0,0,0,0);"
                                   "border-top-left-radius: 2px;"
                                   "border-top-right-radius: 2px;"
                                   "border-bottom-left-radius: 2px;"
                                   "border-bottom-right-radius: 2px;}");
            labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            labImg->setFixedWidth(56);
            labImg->setFixedHeight(56);
        }

        // DEVELOPERS: add style here.
        //...

        // DEFAULT STYLE
        else
        {
            labImg->setStyleSheet("QLabel"
                                  "{border: 1px solid black;"
                                  "border-radius: none;}");
            labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            labImg->setFixedWidth(40);
            labImg->setFixedHeight(40);
        }

        lhItems->insertWidget(1,labImg);
        labsImg.append(labImg); // so to delete later
    }
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemSignature                                */
/*                                                                           */
/*****************************************************************************/

FBElemSignature::FBElemSignature (FBFactory *fctPtr):
    FBElem(fctPtr)
{
    // No attributes.
}

void FBElemSignature::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(60);

    // ANDROID STYLE
    if (styleName == FB_STYLENAME_ANDROID)
    {
        QLabel *labImg = new QLabel(this);
        QPixmap pixmap = QPixmap(":/img/for_signature.png");
        labImg->setStyleSheet("QLabel"
                              "{border: 2px solid "+QString(FB_COLOR_MEDIUMGREY)+";"
                              "background-color: white;"
                              "border-top-left-radius: 1px;"
                              "border-top-right-radius: 1px;"
                              "border-bottom-left-radius: 1px;"
                              "border-bottom-right-radius: 1px;}");
        labImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labImg->setFixedHeight(56);
        labImg->setAlignment(Qt::AlignCenter);
        labImg->setPixmap(pixmap);
        labImg->setContentsMargins(0,0,0,0);
        lvMain->addWidget(labImg);
        lvMain->setContentsMargins(2,2,2,2);
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        QLabel *labImg = new QLabel(this);
        labImg->setStyleSheet("QLabel"
                              "{border: 1px solid black;"
                              "border-radius: none;}");
        labImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labImg->setFixedHeight(40);
        lvMain->addWidget(labImg);
        lvMain->setContentsMargins(0,0,0,0);
    }
}







