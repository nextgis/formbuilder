/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  various attributes implementation
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
 
#include <QStyledItemDelegate>

#include "attributes.h"

/*****************************************************************************/
/*                                                                           */
/*                            FBAttrDialog                                   */
/*                                                                           */
/*****************************************************************************/

FBAttrDialog::FBAttrDialog (FBElem *parentElem, QString keyName, QString displayName,
       QString descr, FBAttrrole role, QWidget *parentForDialog):
    FBAttr(parentElem ,keyName, displayName, descr, role)
{
    // IMPORTANT: If parentForDialog is NULL it will cause issues in dialog displaying
    // for  some OSs.
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

FBAttrField::FBAttrField (FBElem *parentElem, QString keyName, QString displayName,
            QString descr, FBAttrrole role):
    FBAttr (parentElem ,keyName, displayName, descr, role)
{
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
    QStyledItemDelegate *itDel = new QStyledItemDelegate(); // some magic in order style
    widget->setItemDelegate(itDel);                    // sheets work properly for Linux
    //widget->setStyleSheet("QComboBox QAbstractItemView {color: black}");
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
/*                              FBAttrString                                  */
/*                                                                            */
/******************************************************************************/

FBAttrString::FBAttrString (FBElem *parentElem, QString keyName, QString displayName,
                        QString descr, FBAttrrole role, QString initValue):
    FBAttr(parentElem ,keyName, displayName, descr, role)
{
    value = initValue;
    ignoreValue = false;
}

Json::Value FBAttrString::toJson ()
{
    Json::Value jsonRet;
    if (ignoreValue)
    {
        return jsonRet; // null will be returned
    }
    QByteArray ba = value.toUtf8();
    jsonRet = ba.data();
    return jsonRet;
}

bool FBAttrString::fromJson (Json::Value jsonVal)
{
    if (!jsonVal.isNull() && !jsonVal.isString())
        return false;
    if (jsonVal.isNull())
    {
        ignoreValue = true; // value will stay default (set in constructor)
        return true;
    }
    QByteArray ba = jsonVal.asString().data();
    value = QString::fromUtf8(ba);
    return true;
}

QWidget *FBAttrString::getWidget ()
{
    QLineEdit *widget = new QLineEdit();
    widget->setText(value);
    QObject::connect(widget, SIGNAL(textChanged(QString)),
                     this, SLOT(onEditEnd(QString)));
    return widget;
}

QVariant FBAttrString::getValue ()
{
    if (ignoreValue)
        return QString("...");
    return value;
}

void FBAttrString::onEditEnd (QString lineEditText)
{
    value = lineEditText;
    emit changeAppearance(this);
}


/******************************************************************************/
/*                                                                            */
/*                               FBAttrNumber                                 */
/*                                                                            */
/******************************************************************************/

FBAttrNumber::FBAttrNumber (FBElem *parentElem, QString keyName, QString displayName,
                            QString descr, FBAttrrole role, int initValue,
                            int min, int max):
    FBAttr(parentElem ,keyName, displayName, descr, role)
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
    emit changeAppearance(this);
}


/******************************************************************************/
/*                                                                            */
/*                              FBAttrBoolean                                 */
/*                                                                            */
/******************************************************************************/

FBAttrBoolean::FBAttrBoolean (FBElem *parentElem, QString keyName, QString displayName,
                              QString descr, FBAttrrole role, bool initValue):
    FBAttr(parentElem ,keyName, displayName, descr, role)
{
    value = initValue;
}

Json::Value FBAttrBoolean::toJson ()
{
    Json::Value jsonRet;
    jsonRet = value;
    return jsonRet;
}

bool FBAttrBoolean::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull() || !(jsonVal.isBool()))
        return false;
    value = jsonVal.asBool();
    emit changeOtherAttr(this);
    return true;
}

QWidget *FBAttrBoolean::getWidget ()
{
    QCheckBox *widget = new QCheckBox();
    widget->setChecked(value);
    QObject::connect(widget, SIGNAL(stateChanged(int)),
                     this, SLOT(onEditEnd(int)));
    return widget;
}

void FBAttrBoolean::onEditEnd (int checkBoxValue)
{
    if (checkBoxValue == Qt::Unchecked)
        value = false;
    else
        value = true;
    emit changeOtherAttr(this);
    emit changeAppearance(this);
}


/******************************************************************************/
/*                                                                            */
/*                             FBAttrListvalues                               */
/*                                                                            */
/******************************************************************************/

QString FBAttrListvalues::curNgwUrl = "";
QString FBAttrListvalues::curNgwLogin = "";
QString FBAttrListvalues::curNgwPass = "";

FBAttrListvalues::FBAttrListvalues (FBElem *parentElem, QString keyName,
           QString displayName, QString descr, FBAttrrole role, QWidget *parentForDialog):
    FBAttrDialog (parentElem, keyName, displayName, descr, role, parentForDialog)
{
    valueDefault = -1; // no default item selected
    ngwLookupId = -1;
}

void FBAttrListvalues::updateNgwParams (QString curNgwUrl, QString curNgwLogin,
                                        QString curNgwPass) // STATIC
{
    FBAttrListvalues::curNgwUrl = curNgwUrl;
    FBAttrListvalues::curNgwLogin = curNgwLogin;
    FBAttrListvalues::curNgwPass = curNgwPass;
}

Json::Value FBAttrListvalues::toJson ()
{
    Json::Value jsonRet;
    QByteArray ba;
    for (int i=0; i<values.size(); i++)
    {
        Json::Value jsonItem;
        ba = values[i].first.toUtf8();
        jsonItem[FB_JSONKEY_ATTRVAL_KEYNAME] = ba.data();
        ba = values[i].second.toUtf8();
        jsonItem[FB_JSONKEY_ATTRVAL_DISPNAME] = ba.data();
        if (i == valueDefault)
        {
            jsonItem[FB_JSONKEY_ATTRVAL_DEFAULT] = true;
        }
        jsonRet.append(jsonItem);
    }
    return jsonRet;

    // Note: here we do not save ngwLookupId because the value which is saved
    // - has an array type.
    // See according elem class how we write ngwLookupId into json file.
    // TODO: make the Json value which is written in this method not an array but
    // the collection of objects so it can be possible to store ngwLookupId in this
    // attribute.
}

bool FBAttrListvalues::fromJson (Json::Value jsonVal)
{
    // Note: jsonVal can be null and the list of values can be void.
    // TODO: check jsonVal for null anyway (so to skip this method)?
    // TODO: check that jsonVal is array?
    // TODO: check if the each item of the array may be null?
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
    // Only if all was correct we assign new values.
    values.clear();
    for (int i=0; i<newValues.size(); i++)
    {
        values.append(newValues[i]);
    }
    valueDefault = newValueDefault;
    return true;

    // Note: see comment in toJson() about ngwLookupId.
}

QVariant FBAttrListvalues::getValue ()
{
    FBListValue ret;
    ret.first = values;
    ret.second = valueDefault;
    QVariant var = QVariant::fromValue(ret);
    return var;
}

QString FBAttrListvalues::getDefDispValue ()
{
    if (valueDefault == -1)
        return QString(FB_DEFVALUE_NOTSELECTED);
    return values[valueDefault].second;
}

QStringList FBAttrListvalues::getDispValues ()
{
    QStringList ret;
    for (int i=0; i<values.size(); i++)
    {
        ret.append(values[i].second);
    }
    return ret;
}

void FBAttrListvalues::onEditStart ()
{
    FBDialogListvalues *dialog;
    dialog = new FBDialogListvalues(parentForDialog);
    dialog->putValues(values,valueDefault);
    dialog->putNgwParams(FBAttrListvalues::curNgwUrl,FBAttrListvalues::curNgwLogin,
                         FBAttrListvalues::curNgwPass,ngwLookupId);
    if (dialog->exec())
    {
        dialog->getValues(values,valueDefault);
        dialog->getNgwParams(ngwLookupId);
        emit changeAppearance(this);
    }
    delete dialog;
}
 

/******************************************************************************/
/*                                                                            */
/*                          FBAttrListvaluesStrict                            */
/*                                                                            */
/******************************************************************************/

FBAttrListvaluesStrict::FBAttrListvaluesStrict (FBElem *parentElem, QString keyName,
         QString displayName, QString descr, FBAttrrole role, QWidget *parentForDialog):
    FBAttrListvalues (parentElem, keyName, displayName, descr, role, parentForDialog)
{
    // Default index can not be undefined. Define initial values.
    values.append(QPair<QString,QString>("1",tr("One")));
    values.append(QPair<QString,QString>("2",tr("Two")));
    valueDefault = 0;
}

void FBAttrListvaluesStrict::onEditStart ()
{
    FBDialogListvalues *dialog;
    dialog = new FBDialogListvalues(parentForDialog,false);
    dialog->putValues(values,valueDefault);
    if (dialog->exec())
    {
        dialog->getValues(values,valueDefault);
        emit changeAppearance(this);
    }
    delete dialog;
}


/******************************************************************************/
/*                                                                            */
/*                         FBAttrDoublelistvalues                             */
/*                                                                            */
/******************************************************************************/

FBAttrListvaluesDouble::FBAttrListvaluesDouble (FBElem *parentElem, QString keyName,
       QString displayName, QString descr, FBAttrrole role, QWidget *parentForDialog):
    FBAttrListvalues(parentElem, keyName, displayName, descr, role, parentForDialog)
{
}

Json::Value FBAttrListvaluesDouble::toJson ()
{
    Json::Value jsonRet;
    // Get root json for main list.
    jsonRet = this->FBAttrListvalues::toJson();
    // Modify it for dependant lists.
    for (int i=0; i<values.size(); i++)
    {
        Json::Value jsonRet2;
        QByteArray ba2;
        int j=0;
        for (j=0; j<values2[i].size(); j++)
        {
            Json::Value jsonItem2;
            ba2 = values2[i][j].first.toUtf8();
            jsonItem2[FB_JSONKEY_ATTRVAL_KEYNAME] = ba2.data();
            ba2 = values2[i][j].second.toUtf8();
            jsonItem2[FB_JSONKEY_ATTRVAL_DISPNAME] = ba2.data();
            if (j == valuesDefault2[i])
            {
                jsonItem2[FB_JSONKEY_ATTRVAL_DEFAULT] = true;
            }
            jsonRet2.append(jsonItem2);
        }
        // WARNING. Here we do a restriction (specially for NextGIS Mobile): the
        // dependant list can not be void - we add a fictional item.
        // IMPORTANT TODO: think about the situation when user enters such values
        // manually. We need to check this in the dialog.
        if (j == 0)
        {
            Json::Value jsonItem2;
            jsonItem2[FB_JSONKEY_ATTRVAL_KEYNAME] = FB_JSONVALUE_ATTRVAL_NOKEYNAME;
            jsonItem2[FB_JSONKEY_ATTRVAL_DISPNAME] = FB_JSONVALUE_ATTRVAL_NODISPNAME;
            jsonRet2.append(jsonItem2);
        }
        jsonRet[i][FB_JSONKEY_ATTRVAL_VALUES] = jsonRet2; // modify root json
    }
    return jsonRet;
}

bool FBAttrListvaluesDouble::fromJson (Json::Value jsonVal)
{
    // Assign values for main list.
    // Note: all checks for jsonVal will be made inside. values array will fully
    // correspond to jsonVal after it.
    if (!this->FBAttrListvalues::fromJson(jsonVal))
        return false;
    // Assign values for dependant lists.
    QList<QList<QPair<QString,QString> > > newValues2;
    QList<int> newValuesDefault2;
    for (int i=0; i<values.size(); i++)
    {
        Json::Value jsonVal2 = jsonVal[i][FB_JSONKEY_ATTRVAL_VALUES];
        if (jsonVal2.isNull() || !jsonVal2.isArray() || jsonVal2.size()<1)
        {
            return false; // there MUST be at least one (can be fictional) value for
                          // the dependant array of values, otherwise the passed json
        }                 // is incorrect. See FBAttrListvaluesDouble::toJson()
        QList<QPair<QString,QString> > tempList;
        newValuesDefault2.append(-1); // will stay -1 if there will be no default found
        // TODO: check if the each item of the array may be null?
        for (int j=0; j<jsonVal2.size(); ++j)
        {
            Json::Value jsonKn = jsonVal2[j][FB_JSONKEY_ATTRVAL_KEYNAME];
            Json::Value jsonDn = jsonVal2[j][FB_JSONKEY_ATTRVAL_DISPNAME];
            if (jsonKn.isNull() || !jsonKn.isString()
                    || jsonDn.isNull() || !jsonDn.isString())
            {
                return false;
            }
            QPair<QString,QString> pair(
                        QString::fromUtf8(jsonKn.asString().data()),
                        QString::fromUtf8(jsonDn.asString().data()));
            // Note: if the passed json contains more than 1 items and one of them is
            // fictional (e.g. user enters these values self) - the false will be also
            // returned.
            // IMPORTANT TODO: see FBAttrListvaluesDouble::toJson() about fictional
            // item in the array.
            if (pair.first == FB_JSONVALUE_ATTRVAL_NOKEYNAME
                    && pair.second == FB_JSONVALUE_ATTRVAL_NODISPNAME)
            {
                break; // void tempList will be added to the values2
            }
            tempList.append(pair);
            Json::Value jsonDf2;
            jsonDf2 = jsonVal2[j][FB_JSONKEY_ATTRVAL_DEFAULT];
            if (!jsonDf2.isNull()) // && jsonDf2.asBool() == true)
            {
                newValuesDefault2[i] = j;
            }
        }
        newValues2.append(tempList);
    }
    // Only if all was correct we assign new values.
    values2.clear();
    valuesDefault2.clear();
    for (int i=0; i<newValues2.size(); i++)
    {
        QList<QPair<QString,QString> > tempList;
        for (int j=0; j<newValues2[i].size(); j++)
        {
            tempList.append(newValues2[i][j]);
        }
        values2.append(tempList);
        valuesDefault2.append(newValuesDefault2[i]);
    }
    return true;
}

void FBAttrListvaluesDouble::onEditStart ()
{
    FBDialogDlistvalues *dialog;
    dialog = new FBDialogDlistvalues(parentForDialog);
    dialog->putValues(values,valueDefault,values2,valuesDefault2);
    if (dialog->exec())
    {
        dialog->getValues(values,valueDefault,values2,valuesDefault2);
        emit changeAppearance(this);
    }
    delete dialog;
}

QVariant FBAttrListvaluesDouble::getValue ()
{
    FBDoublelistValue ret;
    ret.first.first = values;
    ret.first.second = valueDefault;
    ret.second.first = values2;
    ret.second.second = valuesDefault2;
    QVariant var = QVariant::fromValue(ret);
    return var;
}

void FBAttrListvaluesDouble::getDefDispValues (QString &str1, QString &str2)
{
    if (valueDefault == -1)
    {
        str1 = QString(FB_DEFVALUE_NOTSELECTED);
        str2 = QString(FB_DEFVALUE_NOTSELECTED);
        return;
    }
    str1 = values[valueDefault].second;
    if (valuesDefault2[valueDefault] == -1)
    {
        str2 = QString(FB_DEFVALUE_NOTSELECTED);
        return;
    }
    str2 = values2[valueDefault][valuesDefault2[valueDefault]].second;
}


/******************************************************************************/
/*                                                                            */
/*                              FBAttrSelect                                  */
/*                                                                            */
/******************************************************************************/

FBAttrSelect::FBAttrSelect (FBElem *parentElem, QString keyName, QString displayName,
           QString descr, FBAttrrole role, QStringList valuesRange, int initValue):
    FBAttr(parentElem ,keyName, displayName, descr, role)
{
    for (int i=0; i<valuesRange.size(); i++)
    {
        this->valuesRange.append(valuesRange[i]);
    }
    value = initValue;
}

Json::Value FBAttrSelect::toJson ()
{
    Json::Value jsonRet;
    jsonRet = value;
    return jsonRet;
}

bool FBAttrSelect::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull() || !jsonVal.isInt()) // TODO: .asInt64()?
        return false;
    value = jsonVal.asInt();
    emit changeOtherAttr(this);
    return true;
}

QWidget *FBAttrSelect::getWidget ()
{
    QComboBox *widget = new QComboBox();
    for (int i=0; i<valuesRange.size(); i++)
    {
        widget->addItem(valuesRange[i]);
    }
    widget->setCurrentIndex(value);
    QObject::connect(widget, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(onEditEnd(int)));
    return widget;
}

void FBAttrSelect::onEditEnd (int indexSelected)
{
    value = indexSelected;
    emit changeOtherAttr(this);
    emit changeAppearance(this);
}


/******************************************************************************/
/*                                                                            */
/*                             FBAttrDatetime                                 */
/*                                                                            */
/******************************************************************************/

FBAttrDatetime::FBAttrDatetime (FBElem *parentElem, QString keyName,
     QString displayName, QString descr, FBAttrrole role, QWidget *parentForDialog):
    FBAttrDialog (parentElem, keyName, displayName, descr, role, parentForDialog)
{
    value.setDate(QDate(2016,1,1));
    value.setTime(QTime(0,0,0));
    format = FBForm::DATETIME_FORMATS[0];
    ignoreValue = false;
}

Json::Value FBAttrDatetime::toJson ()
{
    Json::Value jsonRet;
    if (ignoreValue)
    {
        return jsonRet; // null will be returned
    }
    QByteArray ba = value.toString(format->strNgfp).toUtf8();
    jsonRet = ba.data();
    return jsonRet;
}

bool FBAttrDatetime::fromJson (Json::Value jsonVal)
{
    if (!jsonVal.isNull() && !jsonVal.isString())
        return false;
    if (jsonVal.isNull())
    {
        ignoreValue = true; // value will stay default (set in constructor)
        return true;
    }
    QByteArray ba = jsonVal.asString().data();
    QString str = QString::fromUtf8(ba);
    // Try to read value guessing date/time format. We do not know actual format in
    // this attribute and we will not write it even when we guess it. See TODO and
    // according issue in "Date&Time" element.
    // Note: the actual format will be read from the other attribute and set here via
    // the according attribute-connector signal/slot pair.
    for (int i=0; i<FBForm::DATETIME_FORMATS.size(); i++)
    {
        QDateTime d = QDateTime::fromString(str,FBForm::DATETIME_FORMATS[i]->strNgfp);
        if (d.isValid())
        {
            // Note: if here will be read "only date" or "only time" format - the
            // defaults for not set parts will be used - see Qt docs for QDateTime
            // class (1900-1-1 0:0:0).
            value.setDate(d.date());
            value.setTime(d.time());
            return true;
        }
    }
    return false;
}


QVariant FBAttrDatetime::getValue()
{
//    FBDatetimeValue ret;
//    ret.first = value;
//    QVariant var = QVariant::fromValue(ret);
//    return var;
    // Currently return string.
    if (ignoreValue)
        return "...";
    return value.toString(format->strDisplayEn);
}

void FBAttrDatetime::changeFormat (FBDatetimeFormat *newFormat)
{
    format = newFormat;
}

void FBAttrDatetime::onEditStart ()
{
    FBDialogDatetime *dialog;
    dialog = new FBDialogDatetime(parentForDialog);
    dialog->putValues(value,ignoreValue,format);
    if (dialog->exec())
    {
        dialog->getValues(value,ignoreValue);
        emit changeAppearance(this);
    }
    delete dialog;
}




