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


/*****************************************************************************/
/*                                                                           */
/*                            FBElemInput                                    */
/*                                                                           */
/*****************************************************************************/

void FBElemInput::updateFields (QStringList fieldsKeyNames) // STATIC
{
    FBAttrField::updateValues(fieldsKeyNames);
}

FBElemInput::FBElemInput ():
    FBElem ()
{
    // ATTRIBUTE
    attrFieldPtr = new FBAttrField(this, FB_ATTRNAME_FIELD,
        tr("Layer's field"),
        tr("The field of the underlying layer into which\nthe element will put its value"),
        FBImportant);
    attrs.insert(attrFieldPtr->getKeyName(),attrFieldPtr);
}

void FBElemInput::resetSelectedField (QString keyname) // keyname unused here
{
    attrFieldPtr->resetValue();
}

QStringList FBElemInput::getSelectedFields ()
{
    QStringList list;
    QString selectedField = attrFieldPtr->getValue().toString();
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

FBElemInputVariate::FBElemInputVariate ():
    FBElemInput ()
{
    // ATTRIBUTE
    attrKeepLastPtr = new FBAttrBoolean(this, FB_ATTRNAME_STORELAST,
        tr("Keep last value"),
        tr("Whether to keep last entered value from\nthe last input session"),
        FBNoRole,
        false);
    attrs.insert(attrKeepLastPtr->getKeyName(),attrKeepLastPtr);
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemContainer                                */
/*                                                                           */
/*****************************************************************************/

/*
FBElemContainer::FBElemContainer ():

{

}

FBElemContainer::~FBElemContainer ()
{

}

void FBElemContainer::clearContents()
{

}
*/


/******************************************************************************/
/*                                                                            */
/*                              FBElemText                                    */
/*                                                                            */
/******************************************************************************/

FBElemText::FBElemText ():
    FBElem()
{
    // ATTRIBUTE
    FBAttrString *attrStrPtr = new FBAttrString(this, FB_ATTRNAME_TEXT,
        tr("Text"),
        tr("Displayed text"),
        FBNoRole,
        tr("Caption"));
    QObject::connect(attrStrPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrStrPtr->getKeyName(),attrStrPtr);
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemSpace                                    */
/*                                                                            */
/******************************************************************************/

FBElemSpace::FBElemSpace ():
    FBElem()
{
    // No attributes.
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemTextedit                                */
/*                                                                            */
/******************************************************************************/

FBElemTextedit::FBElemTextedit ():
    FBElemInputVariate()
{
    // ATTRIBUTE
    attrTextPtr = new FBAttrString(this, FB_ATTRNAME_TEXT,
        tr("Initial text"),
        tr("Initial text"),
        FBNoRole,
        tr("Text"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrTextPtr->getKeyName(),attrTextPtr);

    // ATTRIBUTE
    FBAttrNumber *attrMaxCountPtr = new FBAttrNumber(this, FB_ATTRNAME_MAXSTRINGCOUNT,
        tr("Max. string count"),
        tr("Maximium number of lines"),
        FBNoRole,
        1, 1, 65535);
    attrs.insert(attrMaxCountPtr->getKeyName(),attrMaxCountPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrOnlyNumbers = new FBAttrBoolean(this, FB_ATTRNAME_ONLYNUMBERS,
        tr("Only numbers"),
        tr("User can type only whole/decimal numbers"),
        FBNoRole,
        false);
    attrs.insert(attrOnlyNumbers->getKeyName(),attrOnlyNumbers);

    // ATTRIBUTE
    attrNgwLogin = new FBAttrBoolean(this, FB_ATTRNAME_NGWLOGIN,
        tr("NextGIS Web login"),
        tr("The value in this element will be replaced with \n"
           "user's login if the connection with NextGIS Web \n"
           "exists for the form"),
        FBNoRole,
        false);
    QObject::connect(attrNgwLogin, SIGNAL(changeOtherAttr(FBAttr*)),
                     this, SLOT(onChangeAttrValue(FBAttr*)));
    QObject::connect(attrNgwLogin, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrNgwLogin->getKeyName(),attrNgwLogin);
}

void FBElemTextedit::onChangeAttrValue (FBAttr* attr)
{
    // This is logic for the 'NGW login' attribute: we do not allow to save string
    // value if the NGW login attr was set to true. Also we change elem's look
    // accordingly.
    attrTextPtr->setIgnoreValue(attrNgwLogin->getValue().toBool());
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemCombobox                                */
/*                                                                            */
/******************************************************************************/

FBElemCombobox::FBElemCombobox (QWidget *appWidget):
    FBElemInputVariate()
{
    // ATTRIBUTE
    attrListvalsPtr = new FBAttrListvalues(this, FB_ATTRNAME_VALUE_mult,
        tr("Values"),
        tr("List ov values and default selected value"),
        FBNoRole,
        appWidget);
    QObject::connect(attrListvalsPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrListvalsPtr->getKeyName(),attrListvalsPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrInputSearch = new FBAttrBoolean(this, FB_ATTRNAME_INPUTSEARCH,
        tr("Input with search"),
        tr("To show or not the corresponding items in the\nlist during the typing"),
        FBNoRole,
        false);
    attrs.insert(attrInputSearch->getKeyName(),attrInputSearch);

    // ATTRIBUTE
    FBAttrBoolean *attrAllowVals = new FBAttrBoolean(this, FB_ATTRNAME_ALLOWADDING,
        tr("Allow own values"),
        tr("User can add own values to the list"),
        FBNoRole,
        false);
    attrs.insert(attrAllowVals->getKeyName(),attrAllowVals);
}

void FBElemCombobox::modifyJsonOut (Json::Value &jsonValue)
{
    // TODO: move this to the according attribute! See comment
    // in FBAttrListvalues::toJson().
    jsonValue[FB_JSONKEY_ELEM_ATTRS][FB_ATTRNAME_NGW_ID]
            = attrListvalsPtr->getNgwLookupId();
}
void FBElemCombobox::modifyElemIn (Json::Value jsonValue)
{
    attrListvalsPtr->setNgwLookupId(jsonValue[FB_JSONKEY_ELEM_ATTRS]
                                    [FB_ATTRNAME_NGW_ID].asInt());
}


/******************************************************************************/
/*                                                                            */
/*                           FBElemDoublecombobox                             */
/*                                                                            */
/******************************************************************************/

FBElemDoublecombobox::FBElemDoublecombobox (QWidget *appWidget):
    FBElemInputVariate()
{
    // REMOVE ATTRIBUTE
    attrs.remove(attrFieldPtr->getKeyName()); // double combo has two field attrs
    delete attrFieldPtr;                       // instead
    attrFieldPtr = NULL;

    // ATTRIBUTE
    attrField1Ptr = new FBAttrField(this, FB_ATTRNAME_FIELD_dcmb_1,
        tr("Layer's field for 1"),
        tr("The field of the underlying layer into which\nthe first (main)"
        " combo will put its value"),
        FBImportant);
    attrs.insert(attrField1Ptr->getKeyName(),attrField1Ptr);

    // ATTRIBUTE
    attrField2Ptr = new FBAttrField(this, FB_ATTRNAME_FIELD_dcmb_2,
        tr("Layer's field for 2"),
        tr("The field of the underlying layer into which\nthe second (dependant)"
        " combo will put its value"),
        FBImportant);
    attrs.insert(attrField2Ptr->getKeyName(),attrField2Ptr);

    // ATTRIBUTE
    FBAttrListvaluesDouble *attrListvalsPtr = new FBAttrListvaluesDouble(this,
        FB_ATTRNAME_VALUE_mult,
        tr("Values"),
        tr("List of values for both combos and according\ndefault values"),
        FBNoRole,
        appWidget);
    QObject::connect(attrListvalsPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrListvalsPtr->getKeyName(),attrListvalsPtr);
}

void FBElemDoublecombobox::resetSelectedField (QString keyname)
{
    if (attrField1Ptr->getKeyName() == keyname)
        attrField1Ptr->resetValue();
    if (attrField2Ptr->getKeyName() == keyname)
        attrField2Ptr->resetValue();
}

QStringList FBElemDoublecombobox::getSelectedFields ()
{
    QStringList list;
    QString selectedField;
    selectedField = attrField1Ptr->getValue().toString();
    if (selectedField != FB_DEFVALUE_NOTSELECTED)
        list.append(selectedField);
    selectedField = attrField2Ptr->getValue().toString();
    if (selectedField != FB_DEFVALUE_NOTSELECTED)
        list.append(selectedField);
    return list;
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemCheckbox                                */
/*                                                                            */
/******************************************************************************/

FBElemCheckbox::FBElemCheckbox ():
    FBElemInputVariate()
{
    // ATTRIBUTE
    FBAttrString *attrTextPtr = new FBAttrString(this, FB_ATTRNAME_TEXT,
        tr("Text"),
        tr("Displayed text"),
        FBNoRole,
        tr("Check"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrTextPtr->getKeyName(),attrTextPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrCheckPtr = new FBAttrBoolean(this, FB_ATTRNAME_INITVALUE,
        tr("Initial value"),
        tr("Initial value"),
        FBNoRole,
        false);
    QObject::connect(attrCheckPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrCheckPtr->getKeyName(),attrCheckPtr);
}


/******************************************************************************/
/*                                                                            */
/*                             FBElemRadiogroup                               */
/*                                                                            */
/******************************************************************************/

FBElemRadiogroup::FBElemRadiogroup (QWidget *appWidget):
    FBElemInputVariate()
{
    // ATTRIBUTE
    FBAttrListvaluesStrict *attrListvalsPtr = new FBAttrListvaluesStrict(this,
      FB_ATTRNAME_VALUE_mult,
      tr("Values"),
      tr("List ov values and default selected value"),
      FBNoRole,
      appWidget);
    QObject::connect(attrListvalsPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrListvalsPtr->getKeyName(),attrListvalsPtr);
}


/******************************************************************************/
/*                                                                            */
/*                              FBElemDatetime                                */
/*                                                                            */
/******************************************************************************/

FBElemDatetime::FBElemDatetime (QWidget *appWidget):
    FBElemInputVariate()
{
    // ATTRIBUTE
    QStringList strs;
    for (int i=0; i<FBForm::DATETIME_FORMATS.size(); i++)
    {
        strs.append(FBForm::DATETIME_FORMATS[i]->name);
    }
    attrTypePtr = new FBAttrSelect(this, FB_ATTRNAME_DATETYPE,
       tr("Type"),
       tr("The type of value"),
       FBOtherAttrChange,
       strs, 0);
    QObject::connect(attrTypePtr, SIGNAL(changeOtherAttr(FBAttr*)),
                     this, SLOT(onChangeAttrValue(FBAttr*)));
    QObject::connect(attrTypePtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrTypePtr->getKeyName(),attrTypePtr);

    // ATTRIBUTE
    attrDatetimePtr = new FBAttrDatetime(this, FB_ATTRNAME_INITVALUE_datetime,
        tr("Initial value"),
        tr("Initial value"),
        FBNoRole,
        appWidget);
    QObject::connect(attrDatetimePtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrDatetimePtr->getKeyName(),attrDatetimePtr);
}

void FBElemDatetime::onChangeAttrValue (FBAttr *attr)
{
    attrDatetimePtr->changeFormat(
                FBForm::DATETIME_FORMATS[attrTypePtr->getValue().toInt()]);
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemButton                                   */
/*                                                                           */
/*****************************************************************************/

FBElemButton::FBElemButton ():
    FBElemInput()
{
    // ATTRIBUTE
    FBAttrString *attrTextPtr = new FBAttrString(this, FB_ATTRNAME_TEXT,
        tr("Text"),
        tr("Displayed text"),
        FBNoRole,
        tr("Press"));
    QObject::connect(attrTextPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrTextPtr->getKeyName(),attrTextPtr);

    // ATTRIBUTE
    FBAttrString *attrValuePtr = new FBAttrString(this, FB_ATTRNAME_VALUE,
        tr("Value"),
        tr("The single value which will be put\nto the field when pressing button"),
        FBNoRole,
        false);
    attrs.insert(attrValuePtr->getKeyName(),attrValuePtr);
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemPhoto                                    */
/*                                                                           */
/*****************************************************************************/

FBElemPhoto::FBElemPhoto ():
    FBElem()
{
    // ATTRIBUTE
    FBAttrNumber *attrMaxCountPtr = new FBAttrNumber(this, FB_ATTRNAME_GALLERYSIZE,
        tr("Max. photo count"),
        tr("Maximum number of photos can be made"),
        FBNoRole,
        1, 1, 5);
    QObject::connect(attrMaxCountPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrMaxCountPtr->getKeyName(),attrMaxCountPtr);
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemSignature                                */
/*                                                                           */
/*****************************************************************************/

FBElemSignature::FBElemSignature ():
    FBElem()
{
    // No attributes.
}


/*****************************************************************************/
/*                                                                           */
/*                            FBElemCounter                                  */
/*                                                                           */
/*****************************************************************************/

FBElemCounter::FBElemCounter ():
    FBElemInput()
{
    // ATTRIBUTE
    attrPrefixPtr = new FBAttrString(this, FB_ATTRNAME_PREFIX,
        tr("Prefix"),
        tr("Text to add before value (can be void)"),
        FBNoRole,
        tr(""));
    QObject::connect(attrPrefixPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrPrefixPtr->getKeyName(),attrPrefixPtr);

    // ATTRIBUTE
    attrSuffixPtr = new FBAttrString(this, FB_ATTRNAME_SUFFIX,
        tr("Suffix"),
        tr("Text to add after value (can be void)"),
        FBNoRole,
        tr(""));
    QObject::connect(attrSuffixPtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrSuffixPtr->getKeyName(),attrSuffixPtr);

    // ATTRIBUTE
    attrInitValuePtr = new FBAttrNumber(this, FB_ATTRNAME_INITVALUE,
        tr("Initial value"),
        tr("Initial value from which incrementing starts"),
        FBNoRole,
        0, 0, 65535);
    QObject::connect(attrInitValuePtr, SIGNAL(changeAppearance(FBAttr*)),
                     this, SLOT(onChangeAppearance(FBAttr*)));
    attrs.insert(attrInitValuePtr->getKeyName(),attrInitValuePtr);

    // ATTRIBUTE
    FBAttrNumber *attrIncrementPtr = new FBAttrNumber(this, FB_ATTRNAME_INCREMENT,
        tr("Increment"),
        tr("Which value is added to the current value\n"
           "each time user collects data"),
        FBNoRole,
        1, 1, 65535);
    attrs.insert(attrIncrementPtr->getKeyName(),attrIncrementPtr);
}


/*****************************************************************************/
/*                                                                           */
/*                          FBElemCoordinates                                */
/*                                                                           */
/*****************************************************************************/

FBElemCoordinates::FBElemCoordinates ():
    FBElemInput()
{
    // REMOVE ATTRIBUTE
    attrs.remove(attrFieldPtr->getKeyName()); // double combo has two field attrs
    delete attrFieldPtr;                      // instead
    attrFieldPtr = NULL;

    // ATTRIBUTE
    attrFieldLatPtr = new FBAttrField(this, FB_ATTRNAME_FIELD_lat,
        tr("Layer's field for lat"),
        tr("The field of the underlying layer into which\n"
           "latitude will be written"),
        FBImportant);
    attrs.insert(attrFieldLatPtr->getKeyName(),attrFieldLatPtr);

    // ATTRIBUTE
    attrFieldLongPtr = new FBAttrField(this, FB_ATTRNAME_FIELD_long,
        tr("Layer's field for long"),
        tr("The field of the underlying layer into which\n"
           "longitude will be written"),
        FBImportant);
    attrs.insert(attrFieldLongPtr->getKeyName(),attrFieldLongPtr);

    // ATTRIBUTE
    QStringList strs1;
    strs1.append("EPSG:4326");
    FBAttrSelect *attrCrsPtr = new FBAttrSelect(this, FB_ATTRNAME_CRS,
       tr("CRS"),
       tr("Coordinate Reference System"),
       FBNoRole,
       strs1, 0);
    attrs.insert(attrCrsPtr->getKeyName(),attrCrsPtr);

    // ATTRIBUTE
    QStringList strs2;
    strs2.append("dd.dddddd");
    FBAttrSelect *attrFormatPtr = new FBAttrSelect(this, FB_ATTRNAME_FORMAT,
       tr("Format"),
       tr("Coordinates will be written as a string in this format"),
       FBNoRole,
       strs2, 0);
    attrs.insert(attrFormatPtr->getKeyName(),attrFormatPtr);

    // ATTRIBUTE
    FBAttrBoolean *attrHiddenPtr = new FBAttrBoolean(this, FB_ATTRNAME_HIDDEN,
        tr("Hidden"),
        tr("Do not show this element on the form"),
        FBNoRole,
        false);
    attrs.insert(attrHiddenPtr->getKeyName(),attrHiddenPtr);
}

void FBElemCoordinates::resetSelectedField (QString keyname)
{
    if (attrFieldLatPtr->getKeyName() == keyname)
        attrFieldLatPtr->resetValue();
    if (attrFieldLongPtr->getKeyName() == keyname)
        attrFieldLongPtr->resetValue();
}

QStringList FBElemCoordinates::getSelectedFields ()
{
    QStringList list;
    QString selectedField;
    selectedField = attrFieldLatPtr->getValue().toString();
    if (selectedField != FB_DEFVALUE_NOTSELECTED)
        list.append(selectedField);
    selectedField = attrFieldLongPtr->getValue().toString();
    if (selectedField != FB_DEFVALUE_NOTSELECTED)
        list.append(selectedField);
    return list;
}


