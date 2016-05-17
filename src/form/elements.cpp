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
                  tr("Initial text"), FBNoRole, tr("Caption"));
    attrs.insert(attrTextPtr);
}

void FBElemText::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(0);

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
    }

    // DEVELOPERS: add style here.
    // ...

    // DEFAULT STYLE
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
    FBElemInput(fctPtr)
{
    // ATTRIBUTE
    attrTextPtr = new FBAttrText(this, FB_ATTRNAME_TEXT,
                  tr("Initial text"), FBNoRole, tr("Text"));
    attrs.insert(attrTextPtr);

    // ATTRIBUTE
    FBAttrNumber *attrMaxCountPtr = new FBAttrNumber(this, FB_ATTRNAME_MAXSTRINGCOUNT,
                  tr("Max. string count"), FBNoRole, 1, 1, 65535);
    attrs.insert(attrMaxCountPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrOnlyNumbers = new FBAttrBoolean(this, FB_ATTRNAME_ONLYNUMBERS,
                  tr("Only numbers"), FBNoRole, false);
    attrs.insert(attrOnlyNumbers);

    // ATTRIBUTE
    FBAttrBoolean *attrRememberLastPtr = new FBAttrBoolean(this, FB_ATTRNAME_STORELAST,
                  tr("Store last value"), FBNoRole, false);
    attrs.insert(attrRememberLastPtr);
}

void FBElemTextedit::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(0);

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
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        labText = new QLabel(this);
        lvMain->addWidget(labText);
        labText->setStyleSheet("QLabel{"
                               "border-top: 1px solid black;"
                               "border-left: 1px solid black;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;}");
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
    FBElemInput(fctPtr)
{
    // ATTRIBUTE
    attrListvalsPtr = new FBAttrListvalues(this, FB_ATTRNAME_VALUE_mult,
                  tr("Values"), FBNoRole, appWidget);
    attrs.insert(attrListvalsPtr);
}

void FBElemCombobox::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(0,0,0,0);
    lvMain->setSpacing(0);

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
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
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
    labText->setText(text);
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemCheckbox                                */
/*                                                                            */
/******************************************************************************/

FBElemCheckbox::FBElemCheckbox (FBFactory *fctPtr):
    FBElemInput(fctPtr)
{
    pixOnAndroid = QPixmap(":/img/for_check_on.png");
    pixOffAndroid = QPixmap(":/img/for_check_off.png");

    // ATTRIBUTE
    attrTextPtr = new FBAttrText(this, FB_ATTRNAME_TEXT,
                  tr("Initial text"), FBNoRole, tr("Check"));
    attrs.insert(attrTextPtr);

    // ATTRIBUTE
    attrCheckPtr = new FBAttrBoolean(this, FB_ATTRNAME_INITVALUE,
                  tr("Initial value"), FBNoRole, false);
    attrs.insert(attrCheckPtr);
}

void FBElemCheckbox::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(0,0,0,0);
    lvMain->setSpacing(0);

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
                               "border-top: 1px solid black;"
                               "border-left: 1px solid black;"
                               "border-right: 1px solid black;"
                               "border-bottom: 1px solid black;}");
        labImg->setText(" ");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        labText = new QLabel(this);
        labText->setStyleSheet("QLabel{"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;}");
        hlayAll->addWidget(labImg);
        hlayAll->addWidget(labText);
        lvMain->addLayout(hlayAll);
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
    if (attrCheckPtr->getValue() == true)
        labImg->setPixmap(pixOnAndroid);
    else
        labImg->setPixmap(pixOffAndroid);
}


/******************************************************************************/
/*                                                                            */
/*                             FBElemRadiogroup                               */
/*                                                                            */
/******************************************************************************/

FBElemRadiogroup::FBElemRadiogroup (FBFactory *fctPtr, QWidget *appWidget):
    FBElemInput(fctPtr)
{
    pixOnAndroid = QPixmap(":/img/for_radio_on.png");
    pixOffAndroid = QPixmap(":/img/for_radio_off.png");

    // Size policy fixed for all styles: radio-group must be extended for
    // its content: items.
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);

    // ATTRIBUTE
    attrListvalsStrictPtr = new FBAttrListvaluesStrict(this, FB_ATTRNAME_VALUE_mult,
                  tr("Values"), FBNoRole, appWidget);
    attrs.insert(attrListvalsStrictPtr);

}

void FBElemRadiogroup::changeStyle (QString styleName)
{
    curStyleName = styleName;

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

    // ALL STYLES
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(6);
    // Note: no maximum height - can be any.

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
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
    else
    {
        for (int i=0; i<list.size(); i++)
        {
            QLabel *labText = new QLabel(this);
            labText->setText(list[i]);
            labText->setStyleSheet("QLabel{"
                                   "border-top: none;"
                                   "border-left: none;"
                                   "border-right: none;"
                                   "border-bottom: none;}");
            QHBoxLayout *hlay = new QHBoxLayout();
            hlay->addWidget(labText);
            lvMain->addLayout(hlay);
        }
    }
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemDatetime                                */
/*                                                                            */
/******************************************************************************/

FBElemDatetime::FBElemDatetime (FBFactory *fctPtr):
    FBElemInput(fctPtr)
{

}

void FBElemDatetime::changeStyle (QString styleName)
{
    // CLEAR STYLE
    this->clearContents();

    // ALL STYLES
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    lvMain->setContentsMargins(2,2,2,2);
    lvMain->setSpacing(0);

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
    }

    // DEVELOPERS: add style here.
    //...

    // DEFAULT STYLE
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

    // UPDATE STYLE
    this->updateAppearance();
}

void FBElemDatetime::changeAttrValue ()
{

}

void FBElemDatetime::updateAppearance ()
{
    //labText->setText("11.08.2016");
}





