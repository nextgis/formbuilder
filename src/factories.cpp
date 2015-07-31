/******************************************************************************
 *
 * Name:     factories.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Factories classes implementation.
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "fb.h"

FBFactory::FBFactory(bool isFull, QString strJsonName, QString imgPath, QString text):
    QTreeWidgetItem()
{
    this->strJsonName = strJsonName;
    this->strDisplayName = text;
    this->strIconPath = imgPath;
    this->bIsFull = isFull;

    this->setIcon(0,QIcon(imgPath));
    if (isFull)
        this->setText(0,text);
    else
        this->setText(0,"");
}


// ================================================================================== //
//                                   Decoration                                       //
// ===================================================================================//

FBTextLabelFactory::FBTextLabelFactory(bool isFull):
    FBFactory(isFull,FB_JSON_TEXT_LABEL,":/img/text.png",QObject::tr("Текст"))
{
}
FBElem *FBTextLabelFactory::Create()
{
    return new FBTextLabelElem(this);
}


FBSpaceFactory::FBSpaceFactory(bool isFull):
    FBFactory(isFull,FB_JSON_SPACE,":/img/space.png",QObject::tr("Пробел"))
{
}
FBElem *FBSpaceFactory::Create()
{
    return new FBSpaceElem(this);
}


// ================================================================================== //
//                                     Input                                          //
// ===================================================================================//

FBTextEditFactory::FBTextEditFactory(bool isFull):
    FBFactory(isFull,FB_JSON_TEXT_EDIT,":/img/textedit.png",QObject::tr("Текстовое поле"))
{
}
FBElem *FBTextEditFactory::Create()
{
    return new FBTextEditElem(this);
}


FBComboBoxFactory::FBComboBoxFactory(bool isFull):
    FBFactory(isFull,FB_JSON_COMBOBOX,":/img/combo.png",QObject::tr("Список"))
{
}
FBElem *FBComboBoxFactory::Create()
{
    return new FBComboBoxElem(this);
}


FBDoubleComboBoxFactory::FBDoubleComboBoxFactory(bool isFull):
    FBFactory(isFull,FB_JSON_DOUBLE_COMBOBOX,":/img/double_combo.png",QObject::tr("Сдвоенный список"))
{
}
FBElem *FBDoubleComboBoxFactory::Create()
{
    return new FBDoubleComboBoxElem(this);
}


FBCheckBoxFactory::FBCheckBoxFactory(bool isFull):
    FBFactory(isFull,FB_JSON_CHECKBOX,":/img/check.png",QObject::tr("Флажок"))
{
}
FBElem *FBCheckBoxFactory::Create()
{
    return new FBCheckBoxElem(this);
}

FBRadioGroupFactory::FBRadioGroupFactory(bool isFull):
    FBFactory(isFull,FB_JSON_RADIOGROUP,":/img/radio.png",QObject::tr("Радиогруппа"))
{
}
FBElem *FBRadioGroupFactory::Create()
{
    return new FBRadioGroupElem(this);
}

FBButtonFactory::FBButtonFactory(bool isFull):
    FBFactory(isFull,FB_JSON_BUTTON,":/img/button.png",QObject::tr("Кнопка"))
{
}
FBElem *FBButtonFactory::Create()
{
    return new FBButtonElem(this);
}

FBDateTimeFactory::FBDateTimeFactory(bool isFull):
    FBFactory(isFull,FB_JSON_DATE_TIME,":/img/date.png",QObject::tr("Дата и время"))
{
}
FBElem *FBDateTimeFactory::Create()
{
    return new FBDateTimeElem(this);
}

FBPhotoFactory::FBPhotoFactory(bool isFull):
    FBFactory(isFull,FB_JSON_PHOTO,":/img/photo.png",QObject::tr("Фото"))
{
}
FBElem *FBPhotoFactory::Create()
{
    return new FBPhotoElem(this);
}

FBSignatureFactory::FBSignatureFactory(bool isFull):
    FBFactory(isFull,FB_JSON_SIGNATURE,":/img/signature.png",QObject::tr("Подпись"))
{
}
FBElem *FBSignatureFactory::Create()
{
    return new FBSignatureElem(this);
}


