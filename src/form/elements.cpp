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
    FBAttrString *attrTextPtr = new FBAttrString(this, FB_ATTRNAME_TEXT,
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

    // ATTRIBUTE
    FBAttrField *attrField1Ptr = new FBAttrField(this, FB_ATTRNAME_FIELD_dcmb_1,
        tr("Layer's field for 1"),
        tr("The field of the underlying layer into which\nthe first (main)"
        " combo will put its value"),
        FBImportant);
    attrs.insert(attrField1Ptr->getKeyName(),attrField1Ptr);

    // ATTRIBUTE
    FBAttrField *attrField2Ptr = new FBAttrField(this, FB_ATTRNAME_FIELD_dcmb_2,
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
    QObject::connect(attrTypePtr, SIGNAL(changeOtherAttr()),
                     this, SLOT(onChangeAttrValue()));
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

void FBElemDatetime::onChangeAttrValue ()
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




