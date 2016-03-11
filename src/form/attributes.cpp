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

QStringList FBAttrField::keyNames;

FBAttrField::FBAttrField (FBElem *parentElem, QString keyName, QString displayName,
            FBAttrrole role):
    FBAttr (parentElem ,keyName, displayName, role)
{
    keyNameSelected = FB_DEFVALUE_NOTSELECTED;
}

// Update fields list.
void FBAttrField::updateValues (QStringList newKeyNames) // STATIC
{
    keyNames.clear();
    keyNames.append(FB_DEFVALUE_NOTSELECTED);
    for (int i=0; i<newKeyNames.size(); i++)
    {
        if (newKeyNames[i] == FB_DEFVALUE_NOTSELECTED) // skip reserved values if
            continue;                                  // was some
        keyNames.append(newKeyNames[i]);
    }
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

bool FBAttrField::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull())
    {
        keyNameSelected = FB_DEFVALUE_NOTSELECTED;
    }
    else
    {
        if (!jsonVal.isString())
            return false; // TODO: other checks for json correctness?
        QByteArray ba = jsonVal.asString().data();
        keyNameSelected = QString::fromUtf8(ba);
    }
    return true;
}

QWidget *FBAttrField::getWidget ()
{
    QComboBox *widget = new QComboBox();
    for (int i=0; i<FBAttrField::keyNames.size(); i++)
    {
        widget->addItem(FBAttrField::keyNames[i]);
    }
    // Set default value:
    // From docs of QComboBox: " ... Otherwise, if there is a matching text in the
    // list, currentIndex is set to the corresponding index."
    widget->setCurrentText(keyNameSelected);
    QObject::connect(widget, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(onEditEnd(QString)));
    return widget;

}

void FBAttrField::resetValue ()
{
    keyNameSelected = FB_DEFVALUE_NOTSELECTED;
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

bool FBAttrText::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull() || !jsonVal.isString())
        return false; // TODO: how else to check for correct string conversion?
    QByteArray ba = jsonVal.asString().data();
    value = QString::fromUtf8(ba);
    return true;
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

bool FBAttrNumber::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull() || !(jsonVal.isInt()))// || jsonVal.isInt64()))
        return false; // TODO: how else to check for correct integer conversion?
    value = jsonVal.asInt();
    return true;
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
    QByteArray ba;
    for (int i=0; i<values.size(); i++)
    {
        Json::Value jsonOneVal;
        ba = values[i].first.toUtf8();
        jsonOneVal[FB_JSONKEY_ATTRVAL_KEYNAME] = ba.data();
        ba = values[i].second.toUtf8();
        jsonOneVal[FB_JSONKEY_ATTRVAL_DISPNAME] = ba.data();
        if (i == valueDefault)
        {
            jsonOneVal[FB_JSONKEY_ATTRVAL_DEFAULT] = true;
        }
        jsonRet.append(jsonOneVal);
    }
    return jsonRet;
}

bool FBAttrListvalues::fromJson (Json::Value jsonVal)
{
    QList<QPair<QString,QString> > newValues;
    int newValueDefault = -1;

    for (int k=0; k<jsonVal.size(); ++k)
    {
        Json::Value jsonKn = jsonVal[k][FB_JSONKEY_ATTRVAL_KEYNAME];
        Json::Value jsonDn = jsonVal[k][FB_JSONKEY_ATTRVAL_DISPNAME];
        if (jsonKn.isNull() || !jsonKn.isString()
                || jsonDn.isNull() || !jsonDn.isString())
        {
            return false;
        }
        QPair<QString,QString> pair(
           QString::fromUtf8(jsonKn.asString().data()),
           QString::fromUtf8(jsonDn.asString().data()));
        newValues.append(pair);
        Json::Value jsonDf;
        jsonDf = jsonVal[k][FB_JSONKEY_ATTRVAL_DEFAULT];
        if (!jsonDf.isNull())// && jsonDf.asBool() == true)
        {
            newValueDefault = k;
        }
    }

    values.clear();
    for (int i=0; i<newValues.size(); i++)
    {
        values.append(newValues[i]);
    }
    valueDefault = newValueDefault;

    return true;
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
    dialog = new FBDialogListvalues(parentForDialog); // if the parent is NULL it will
                                                // cause issues in dialog displaying for
                                                // some OSs
    dialog->putValues(values,valueDefault);
    if (dialog->exec())
    {
        dialog->getValues(values,valueDefault);
        emit changeAppearance();
    }
    delete dialog;
}
 


