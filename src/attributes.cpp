/******************************************************************************
 *
 * Name:     attributes.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Attributes classes implementation.
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


FBAttr::FBAttr(FBElem *elem, QString strJsonName)
{
    this->elem = elem;
    this->strJsonName = strJsonName;
}
FBAttr::~FBAttr()
{
}


// ================================================================== //
//                         FBFieldAttr                                //
// ================================================================== //

FBFieldAttr::FBFieldAttr(FBElem *elem, QString strJsonName):
    FBAttr (elem,strJsonName)
{
    value = FB_STR_NOT_SELECTED;
}

Json::Value FBFieldAttr::toJson ()
{
    Json::Value jsonFinal; // по умолчанию будет = null.
    if (value != FB_STR_NOT_SELECTED)
    {
        QByteArray baFinal = value.toUtf8();
        jsonFinal = baFinal.data();
    }
    return jsonFinal;
}

void FBFieldAttr::fromJson (Json::Value jsonVal)
{
    if (jsonVal.isNull())
    {
        value = FB_STR_NOT_SELECTED;
    }
    else
    {
        QByteArray ba = jsonVal.asString().data();
        value = QString::fromUtf8(ba);
    }

}

QWidget* FBFieldAttr::getWidget ()
{
    // Заполняем возвращаемый комбобокс списком доступных полей слоя и
    // выбираем выбранное ранее значение (если было).
    QComboBox *widget = new QComboBox();
    widget->addItem(FB_STR_NOT_SELECTED);
    Json::Value jsonMeta = FB::CUR_PROJECT->getJsonMeta();
    Json::Value jsonArFields = jsonMeta[FB_JSON_META_FIELDS];
    int curIndex = 0;
    for (int i=0; i<jsonArFields.size(); ++i)
    {
        Json::Value jsonField = jsonArFields[i];
        QString strField = QString::fromUtf8(jsonField[FB_JSON_META_KEYNAME].asString().data());
        widget->addItem(strField);
        if (strField == value)
            curIndex = i+1;
    }
    widget->setCurrentIndex(curIndex);
    connect(widget,SIGNAL(currentIndexChanged(QString)),this,SLOT(onEditEnd(QString)));
    return widget;
}

void FBFieldAttr::onEditEnd (QString fieldName)
{
    value = fieldName;
}


// ================================================================== //
//                          FBTextAttr                                //
// ================================================================== //

FBTextAttr::FBTextAttr(FBElem *elem, QString strJsonName, QString strInitValue):
    FBAttr(elem,strJsonName)
{
    value = strInitValue;
}

Json::Value FBTextAttr::toJson()
{
    QByteArray ba = value.toUtf8();
    Json::Value jsonFinal = ba.data();
    return jsonFinal;
}

void FBTextAttr::fromJson(Json::Value jsonVal)
{
    QByteArray ba = jsonVal.asString().data();
    value = QString::fromUtf8(ba);
}

QWidget* FBTextAttr::getWidget()
{
    QLineEdit *widget = new QLineEdit();
    widget->setText(value);
    connect(widget,SIGNAL(textChanged(QString)),this,SLOT(onEditEnd(QString)));
    return widget;
}

void FBTextAttr::onEditEnd(QString lineEditText)
{
    value = lineEditText;
    //emit preview(value);
    emit changeAppearance();
}


// ================================================================== //
//                          FBNumberAttr                              //
// ================================================================== //

FBNumberAttr::FBNumberAttr(FBElem *elem, QString strJsonName, int nInitValue):
    FBAttr(elem,strJsonName)
{
    value = nInitValue;
}

Json::Value FBNumberAttr::toJson()
{
    Json::Value jsonFinal = value;
    return jsonFinal;
}

void FBNumberAttr::fromJson(Json::Value jsonVal)
{
    value = jsonVal.asInt();
}

QWidget* FBNumberAttr::getWidget()
{
    QSpinBox *widget = new QSpinBox();
    widget->setMaximum(FB_LIMIT_ATTR_MAXSTRINGS);
    widget->setValue(value);
    connect(widget,SIGNAL(valueChanged(int)),this,SLOT(onEditEnd(int)));
    return widget;
}

void FBNumberAttr::onEditEnd(int newValue)
{
    value = newValue;
    emit changeAppearance();
}


// ================================================================== //
//                          FBYesNoAttr                               //
// ================================================================== //

FBYesNoAttr::FBYesNoAttr(FBElem *elem, QString strJsonName, bool bInitValue):
    FBAttr(elem,strJsonName)
{
    value = bInitValue;
}

Json::Value FBYesNoAttr::toJson()
{
    Json::Value jsonFinal = value;
    return jsonFinal;
}

void FBYesNoAttr::fromJson(Json::Value jsonVal)
{
    value = jsonVal.asBool();
}

QWidget* FBYesNoAttr::getWidget()
{
    QCheckBox *widget = new QCheckBox();
    widget->setChecked(value);
    connect(widget,SIGNAL(stateChanged(int)),this,SLOT(onEditEnd(int)));
    return widget;
}

void FBYesNoAttr::onEditEnd(int checkboxState)
{
    if (checkboxState == Qt::Unchecked)
        value = false;
    else
        value = true;
    //emit preview(value);
    emit changeAppearance();
}


// ===================================================================================== //
//                                FBListValuesAttr                                       //
// ===================================================================================== //

FBListValuesAttr::FBListValuesAttr(FBElem *elem, QString strJsonName):
    FBAttr(elem,strJsonName)
{
    defIndex = -1; // элемент по умолчанию не выбран
}

Json::Value FBListValuesAttr::toJson ()
{
    Json::Value jsonFinal;
    QByteArray ba;
    for (int i=0; i<values.size(); i++)
    {
        Json::Value jsonElem;
        ba = values[i].first.toUtf8();
        jsonElem[FB_JSON_NAME] = ba.data();
        ba = values[i].second.toUtf8();
        jsonElem[FB_JSON_ALIAS] = ba.data();
        if (i == defIndex) // если defIndex == -1, то сюда просто не зайдёт
        {
            jsonElem[FB_JSON_DEFAULT] = true;
        }
        jsonFinal.append(jsonElem);
    }
    return jsonFinal;
}

void FBListValuesAttr::fromJson (Json::Value jsonVal)
{
    values.clear();
    defIndex = -1;
    for (int i=0; i<jsonVal.size(); ++i)
    {
        QPair<QString,QString> pair(
                    QString::fromUtf8(jsonVal[i][FB_JSON_NAME].asString().data()),
                    QString::fromUtf8(jsonVal[i][FB_JSON_ALIAS].asString().data()));
        values.append(pair);
        Json::Value jsonDef;
        jsonDef = jsonVal[i][FB_JSON_DEFAULT];
        if (!jsonDef.isNull())// && defValue == true)
        {
            defIndex = i; // для радио-группы сюда зайдёт в любом случае.
        }
    }
}

QWidget* FBListValuesAttr::getWidget ()
{
    QPushButton *widget = new QPushButton();
    widget->setText(tr("Задать"));
    connect(widget, SIGNAL(clicked()), this, SLOT(onEditStart()));
    return widget;
}

void FBListValuesAttr::onEditStart ()
{
    // От имени элемента, владеющего этим атрибутом зависит поведение
    // и структура диалога. См. классы-наследники.
    FBListValuesDialog dialog(elem->getJsonName());

    // Загружаем в диалог уже имеющийся список значений.
    for (int i=0; i<values.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(values[i].second);
        item->setData(Qt::UserRole,values[i].first);
        dialog.listL->addItem(item);
        dialog.comboL->addItem(values[i].second);
    }
    int dialogDefIndex;
    if (elem->getJsonName() == FB_JSON_RADIOGROUP)
        dialogDefIndex = defIndex;
    else
        dialogDefIndex = defIndex + 1; // надо сместить все индексы, т.к. в комбобоксе уже добавлен фиктивный элемент
    dialog.comboL->setCurrentIndex(dialogDefIndex);

    // Сохраняем из диалога получившийся список значений.
    if (dialog.exec())
    {
        values.clear();
        for (int i=0; i<dialog.listL->count(); i++)
        {
            QPair<QString,QString> pair(dialog.listL->item(i)->data(Qt::UserRole).toString(),
                                        dialog.listL->item(i)->text());
            values.append(pair);
        }
        int realIndex;
        if (elem->getJsonName() == FB_JSON_RADIOGROUP)
            realIndex = dialog.comboL->currentIndex();
        else
            realIndex = dialog.comboL->currentIndex() - 1; // аналогично смещаем индексы.
        defIndex = realIndex;

        emit changeAppearance();
    }
}

// Возвращаем строку для вывода на экран в элемент-список. Эта строка - алиас выбранного
// по умолчанию элемента спсика.
QString FBListValuesAttr::getDefValueText ()
{
    if (defIndex == -1)
        return QString(FB_STR_NOT_SELECTED);
    return values[defIndex].second;
}


// ===================================================================================== //
//                               FBRadioValuesAttr                                       //
// ===================================================================================== //

FBRadioValuesAttr::FBRadioValuesAttr(FBElem *elem, QString strJsonName):
    FBListValuesAttr(elem,strJsonName)
{
    values.append(QPair<QString,QString>("1",tr("Радио-кнопка 1")));
    values.append(QPair<QString,QString>("2",tr("Радио-кнопка 2")));
    defIndex = 0; // элемент по умолчанию не может быть не выбран - выбираем первый.
}

// Формируем и возвращаем список значений, от которых зависит внешний вид элемента.
QList<QPair<bool,QString> > FBRadioValuesAttr::getValues ()
{
    QList<QPair<bool,QString> > ret;
    for (int i=0; i<values.size(); i++)
    {
        if (i == defIndex)
        {
            ret.append(QPair<bool,QString>(true,values[i].second));
        }
        else
        {
            ret.append(QPair<bool,QString>(false,values[i].second));
        }
    }
    return ret;
}


// ===================================================================================== //
//                             FBDoubleListValuesAttr                                    //
// ===================================================================================== //
/*
FBDoubleListValuesAttr::FBDoubleListValuesAttr(FBElem *elem, QString strJsonName):
    FBAttr(elem,strJsonName)
{
    defIndex = -1;
}

Json::Value FBDoubleListValuesAttr::toJson ()
{
    Json::Value jsonFinal;

    return jsonFinal;
}

void FBDoubleListValuesAttr::fromJson (Json::Value jsonVal)
{

}

QWidget *FBDoubleListValuesAttr::getWidget ()
{
    QPushButton *widget = new QPushButton();
    widget->setText(tr("Задать"));
    connect(widget, SIGNAL(clicked()), this, SLOT(onEditStart()));
    return widget;
}

void FBDoubleListValuesAttr::onEditStart ()
{

}

void FBDoubleListValuesAttr::getDefValueTexts (QString &str1, QString &str2)
{
    if (defIndex == -1)
    {
        str1 = QString(FB_STR_NOT_SELECTED);
        str2 = QString(FB_STR_NOT_SELECTED);
        return;
    }
    str1 = values[defIndex].second;
    if (defIndexes2[defIndex] == -1)
    {
        str2 = QString(FB_STR_NOT_SELECTED);
        return;
    }
    str2 = values2[defIndex][defIndexes2[defIndex]].second;
}
*/


