/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  attributes implementation
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
 
#include "attributes.h"
 

/*****************************************************************************/
/*                                                                           */
/*                            FBAttrDialog                                   */
/*                                                                           */
/*****************************************************************************/

FBAttrDialog::FBAttrDialog (FBElem *parentElem, QString keyName, QString displayName,
       FBAttrrole role, QWidget *parentForDialog):
    FBAttr (parentElem ,keyName, displayName, role)
{
    this->parentForDialog = parentForDialog;
}

QPushButton *FBAttrDialog::getWidget ()
{
    QPushButton *widget = new QPushButton();
    widget->setText(tr("Set"));
    QObject::connect(widget, SIGNAL(clicked()),
                     this, SLOT(onEditStart()));
    return widget;
}


/******************************************************************************/
/*                                                                            */
/*                               FBAttrField                                  */
/*                                                                            */
/******************************************************************************/

FBAttrField::FBAttrField (FBElem *parentElem, QString keyName, QString displayName,
            FBAttrrole role):
    FBAttr (parentElem ,keyName, displayName, role)
{
    keyNames.append(FB_DEFVALUE_NOTSELECTED);
    keyNameSelected = FB_DEFVALUE_NOTSELECTED;
}

Json::Value FBAttrField::toJson ()
{
    Json::Value jsonRet;
    if (keyNameSelected != FB_DEFVALUE_NOTSELECTED)
    {
        QByteArray ba = keyNameSelected.toUtf8();
        jsonRet = ba.data();
    }
    return jsonRet; // will be NULL for default
}

FBErr FBAttrField::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull())
    {
        keyNameSelected = FB_DEFVALUE_NOTSELECTED;
    }
    else
    {
        // TODO: other checks for json correctness.

        QByteArray ba = jsonVal.asString().data();
        keyNameSelected = QString::fromUtf8(ba);
    }
    return FBErrNone;
}

QWidget *FBAttrField::getWidget ()
{
    QComboBox *widget = new QComboBox();
    for (int i=0; i<keyNames.size(); i++)
    {
        widget->addItem(keyNames[i]);
    }
    // From docs of QComboBox: " ... Otherwise, if there is a matching text in the
    // list, currentIndex is set to the corresponding index."
    widget->setCurrentText(keyNameSelected);
    QObject::connect(widget, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(onEditEnd(QString)));
    return widget;

}

void FBAttrField::updateValues (QStringList newKeyNames)
{
    // Get new values.
    keyNames.clear();
    keyNames.append(FB_DEFVALUE_NOTSELECTED);
    bool wasOldKeyName = false;
    for (int i=0; i<newKeyNames.size(); i++)
    {
        if (newKeyNames[i] == FB_DEFVALUE_NOTSELECTED) // skip reserved values if
            continue;
        keyNames.append(newKeyNames[i]);
        if (newKeyNames[i] == keyNameSelected)
        {
            wasOldKeyName = true;
        }
    }
    // Return old value if it is in the list of new values.
    if (!wasOldKeyName)
    {
        keyNameSelected = FB_DEFVALUE_NOTSELECTED;
    }
}

void FBAttrField::onEditEnd (QString keyNameSelected)
{
    this->keyNameSelected = keyNameSelected;
}


/******************************************************************************/
/*                                                                            */
/*                              FBAttrText                                    */
/*                                                                            */
/******************************************************************************/

FBAttrText::FBAttrText (FBElem *parentElem, QString keyName, QString displayName,
                         FBAttrrole role, QString initValue):
    FBAttr(parentElem ,keyName, displayName, role)
{
    value = initValue;
}

Json::Value FBAttrText::toJson ()
{
    Json::Value jsonRet;
    QByteArray ba = value.toUtf8();
    jsonRet = ba.data();
    return jsonRet;
}

FBErr FBAttrText::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull())
    {
        return FBErrIncorrectJson;
    }
    // TODO: how else to check for correct string conversion?
    QByteArray ba = jsonVal.asString().data();
    value = QString::fromUtf8(ba);
    return FBErrNone;
}

QWidget *FBAttrText::getWidget ()
{
    QLineEdit *widget = new QLineEdit();
    widget->setText(value);
    QObject::connect(widget, SIGNAL(textChanged(QString)),
                     this, SLOT(onEditEnd(QString)));
    return widget;
}

void FBAttrText::onEditEnd (QString lineEditText)
{
    value = lineEditText;
    emit changeAppearance();
}


/******************************************************************************/
/*                                                                            */
/*                               FBAttrNumber                                 */
/*                                                                            */
/******************************************************************************/

FBAttrNumber::FBAttrNumber (FBElem *parentElem, QString keyName, QString displayName,
            FBAttrrole role, int initValue, int min, int max):
    FBAttr (parentElem ,keyName, displayName, role)
{
    this->value = initValue;
    this->min = min;
    this->max = max;
}

Json::Value FBAttrNumber::toJson ()
{
    Json::Value jsonRet;
    jsonRet = value;
    return jsonRet;
}

FBErr FBAttrNumber::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull())
    {
        return FBErrIncorrectJson;
    }
    // TODO: how else to check for correct integer conversion?
    value = jsonVal.asInt();
    return FBErrNone;
}

QWidget *FBAttrNumber::getWidget ()
{
    QSpinBox *widget = new QSpinBox();
    widget->setMaximum(max);
    widget->setMinimum(min);
    widget->setValue(value);
    QObject::connect(widget, SIGNAL(valueChanged(int)),
                     this, SLOT(onEditEnd(int)));
    return widget;
}

void FBAttrNumber::onEditEnd (int spinBoxValue)
{
    value = spinBoxValue;
    //emit changeAppearance();
}


/******************************************************************************/
/*                                                                            */
/*                             FBAttrListvalues                               */
/*                                                                            */
/******************************************************************************/


FBAttrListvalues::FBAttrListvalues (FBElem *parentElem, QString keyName,
           QString displayName, FBAttrrole role, QWidget *parentForDialog):
    FBAttrDialog (parentElem, keyName, displayName, role, parentForDialog)
{
    valueDefault = -1; // no default item selected
}

Json::Value FBAttrListvalues::toJson ()
{
    Json::Value jsonRet;

    return jsonRet;
}

FBErr FBAttrListvalues::fromJson (Json::Value jsonVal)
{
    return FBErrNone;
}

QString FBAttrListvalues::getDefDispValue ()
{
    if (valueDefault == -1)
        return QString(FB_DEFVALUE_NOTSELECTED);
    return values[valueDefault].second;
}

void FBAttrListvalues::onEditStart ()
{
    FBDialogListvalues *dialog;
    if (parentForDialog == NULL)
    {
        dialog = new FBDialogListvalues(NULL); // will cause issues in displaying for
    }                                          // some OSs if init without parent
    else
    {
        dialog = new FBDialogListvalues(parentForDialog);
    }
    dialog->putValues(values,valueDefault);
    if (dialog->exec())
    {
        dialog->getValues(values,valueDefault);
        emit changeAppearance();
    }
    delete dialog;
}
 





