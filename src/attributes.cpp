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
        QString text = QString::fromUtf8(jsonField[FB_JSON_META_KEYNAME].asString().data());
        widget->addItem(text);
        if (text == value)
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

void FBFieldAttr::setUndefinedValue ()
{
    value = FB_STR_NOT_SELECTED;
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
    // TODO: все проверки на некорректный json?
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
    emit changeAppearance();
}


// ================================================================== //
//                          FBNumberAttr                              //
// ================================================================== //

FBNumberAttr::FBNumberAttr(FBElem *elem, QString strJsonName, int nInitValue,
                           int nMin, int nMax): FBAttr(elem,strJsonName)
{
    value = nInitValue;
    min = nMin;
    max = nMax;
}

Json::Value FBNumberAttr::toJson()
{
    Json::Value jsonFinal = value;
    return jsonFinal;
}

void FBNumberAttr::fromJson(Json::Value jsonVal)
{
    // TODO: все проверки на некорректный json?
    value = jsonVal.asInt();
}

QWidget* FBNumberAttr::getWidget()
{
    QSpinBox *widget = new QSpinBox();
    widget->setMaximum(max);
    widget->setMinimum(min);
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
    // TODO: все проверки на некорректный json?
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
    // TODO: все проверки на некорректный json?
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
        dialog.comboL->addItem(FB::shortenStringForOutput(values[i].second));
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

FBDoubleListValuesAttr::FBDoubleListValuesAttr(FBElem *elem, QString strJsonName):
    FBAttr(elem,strJsonName)
{
    defIndex = -1; // три остальных массива просто пусты
}

Json::Value FBDoubleListValuesAttr::toJson ()
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

        if (i == defIndex) // если == -1, то сюда просто не зайдёт
        {
            jsonElem[FB_JSON_DEFAULT] = true;
        }

        Json::Value jsonFinal2;
        QByteArray ba2;
        int j = 0;
        for (j=0; j<values2[i].size(); j++)
        {
            Json::Value jsonElem2;

            ba2 = values2[i][j].first.toUtf8();
            jsonElem2[FB_JSON_NAME] = ba2.data();

            ba2 = values2[i][j].second.toUtf8();
            jsonElem2[FB_JSON_ALIAS] = ba2.data();

            if (j == defIndexes2[i])
            {
                jsonElem2[FB_JSON_DEFAULT] = true;
            }

            jsonFinal2.append(jsonElem2);
        }
        if (j == 0) // зависимый список не может быть пустой, добавляем фиктивный элемент
        {
            Json::Value jsonElem2;
            jsonElem2[FB_JSON_NAME] = FB_JSONVALUE_NONAME;
            jsonElem2[FB_JSON_ALIAS] = FB_JSONVALUE_NOALIAS;
            jsonFinal2.append(jsonElem2);
        }
        jsonElem[FB_JSON_VALUES] = jsonFinal2;

        jsonFinal.append(jsonElem);
    }

    return jsonFinal;
}

void FBDoubleListValuesAttr::fromJson (Json::Value jsonVal)
{
    // TODO: все проверки на некорректный json?
    values.clear();
    values2.clear();
    defIndex = -1;
    defIndexes2.clear();

    for (int i=0; i<jsonVal.size(); ++i)
    {
        QPair<QString,QString> pair(
                    QString::fromUtf8(jsonVal[i][FB_JSON_NAME].asString().data()),
                    QString::fromUtf8(jsonVal[i][FB_JSON_ALIAS].asString().data()));
        values.append(pair);

        Json::Value jsonDef;
        jsonDef = jsonVal[i][FB_JSON_DEFAULT];
        if (!jsonDef.isNull())
        {
            defIndex = i;
        }

        Json::Value jsonVal2 = jsonVal[i][FB_JSON_VALUES]; // пустого быть не может (в правильном файле)
        QList<QPair<QString,QString> > tempList;
        defIndexes2.append(-1);
        for (int j=0; j<jsonVal2.size(); ++j)
        {
            QPair<QString,QString> pair(
                        QString::fromUtf8(jsonVal2[j][FB_JSON_NAME].asString().data()),
                        QString::fromUtf8(jsonVal2[j][FB_JSON_ALIAS].asString().data()));

            // TODO: сделать ещё проверку на размер массива, потому что сейчас пользователь
            // может сам ввести такие значения в начале или середине списка, и всё что за ними
            // - не добавится в список.
            // Другой ворос что эти значения так подобраны, что пользователь их скорее всего
            // никогда не введёт.

            if (pair.first == FB_JSONVALUE_NONAME && pair.second == FB_JSONVALUE_NOALIAS)
            {
                break; // т.е. в values2 добавится пустой массив
            }

            tempList.append(pair);

            Json::Value jsonDef2;
            jsonDef2 = jsonVal2[j][FB_JSON_DEFAULT];
            if (!jsonDef2.isNull())
            {
                defIndexes2[i] = j;
            }
        }
        values2.append(tempList);
    }
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
    FBDoubleListValueDialog dialog;

    // Загружаем в диалог уже имеющийся список значений.
    for (int i=0; i<values.size(); i++)
    {
        dialog.table1->setCurrentCell(i,0); // это так же добавит пустую строку и таблицу, если надо
        dialog.table1->currentItem()->setText(values[i].first);
        dialog.table1->setCurrentCell(i,1); // это так же добавит элемент в комбобокс для умолчаний
        dialog.table1->currentItem()->setText(values[i].second);
        for (int j=0; j<values2[i].size(); j++)
        {
            dialog.tables2[i]->setCurrentCell(j,0);
            dialog.tables2[i]->currentItem()->setText(values2[i][j].first);
            dialog.tables2[i]->setCurrentCell(j,1);
            dialog.tables2[i]->currentItem()->setText(values2[i][j].second);
        }
        dialog.combos2[i]->setCurrentIndex(defIndexes2[i]+1);
    }
    dialog.combo1->setCurrentIndex(defIndex+1); // если индекс = -1, то он станет = 0, а т.е. с текстом не выбранного элемента

    // Сохраняем из диалога получившийся список значений.
    if (dialog.exec())
    {
        values.clear();
        values2.clear();
        defIndex = -1; // на случай, если элементом по умолчанию был выбран пустой элемент
        defIndexes2.clear();
        for (int i=0; i<dialog.table1->rowCount()-1; i++) // -1, т.к. не берём последний всегда пустой элемент
        {
            QString inStr1 = dialog.table1->item(i,0)->text();
            QString outStr1 = dialog.table1->item(i,1)->text();
            if (inStr1 == "" && outStr1 == "") // не сохраняем пустые строки вообще
            {
                continue;
            }
            else if (outStr1 == "") // не даём сохранить значения с пустой парой - дополняем её
            {
                outStr1 = inStr1;
            }
            else if (inStr1 == "")
            {
                inStr1 = outStr1;
            }
            values.append(QPair<QString,QString>(inStr1,outStr1));
            if (i == dialog.combo1->currentIndex()-1)
            {
                defIndex = i; // присвоится только если не было continue, иначе останется = -1
            }
            QList<QPair<QString,QString> > tempList;
            defIndexes2.append(-1); // будет заменён на заданный, если он есть. Создаём здесь = -1 в любом случае, т.к. элемент верхнего уровня имеется
            for (int j=0; j<dialog.tables2[i]->rowCount()-1; j++)
            {
                QString inStr2 = dialog.tables2[i]->item(j,0)->text();
                QString outStr2 = dialog.tables2[i]->item(j,1)->text();
                if (inStr2 == "" && outStr2 == "")
                {
                    continue;
                }
                else if (outStr2 == "")
                {
                    outStr2 = inStr2;
                }
                else if (inStr2 == "")
                {
                    inStr2 = outStr2;
                }
                tempList.append(QPair<QString,QString>(inStr2,outStr2));
                if (j == dialog.combos2[i]->currentIndex()-1)
                {
                    defIndexes2[i] = j;
                }
            }
            values2.append(tempList);
        }

        // Обновляем внешний вид элемента на форме.
        emit changeAppearance();
    }
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


// ===================================================================================== //
//                                FBSelectAttr                                           //
// ===================================================================================== //

FBSelectAttr::FBSelectAttr(FBElem *elem, QString strJsonName, QList<QString> strsValues,
                           int curIndex):
    FBAttr(elem,strJsonName)
{
    for (int i=0; i<strsValues.size(); i++)
    {
        this->strsValues.append(strsValues[i]);
    }
    this->curIndex = curIndex;
}

Json::Value FBSelectAttr::toJson()
{
    Json::Value jsonFinal = curIndex;
    return jsonFinal;
}

void FBSelectAttr::fromJson(Json::Value jsonVal)
{
    // TODO: все проверки на некорректный json?
    curIndex = jsonVal.asInt();
}

QWidget *FBSelectAttr::getWidget ()
{
    QComboBox *widget = new QComboBox();
    for (int i=0; i<strsValues.size(); i++)
    {
        widget->addItem(strsValues[i]);
    }
    widget->setCurrentIndex(curIndex);
    connect(widget,SIGNAL(currentIndexChanged(int)),this,SLOT(onEditEnd(int)));
    return widget;
}

void FBSelectAttr::onEditEnd (int indexSelected)
{
    curIndex = indexSelected;
    emit changeAppearance();
}



// ================================================================== //
//                         FBDateTimeAttr                             //
// ================================================================== //

FBDateTimeAttr::FBDateTimeAttr(FBElem *elem, QString strJsonName):
    FBAttr (elem,strJsonName)
{
    setDefault();
}

void FBDateTimeAttr::setDefault ()
{
    isDefaultCurrent = false;
    curDateTime.setDate(QDate(2016,1,1));
    curDateTime.setTime(QTime(0,0,0));
    curFormat = FB_DISPLAY_FORMAT_DATE;
}

Json::Value FBDateTimeAttr::toJson ()
{
    Json::Value jsonFinal;

    if (isDefaultCurrent)
    {
        // Пользователь ставил текущую дату/время - возвращаем null.
        return jsonFinal;
    }

    // Формат записи даты и времени должны соответствовать geojson формату (т.е. не тому,
    // что отображается на экране формы) согласно договорённости. Преобразовываем формат
    // для записи в .ngfp.
    QString strFormat;
    if (curFormat == FB_DISPLAY_FORMAT_DATE)
    {
        strFormat = "yyyy-MM-dd";
    }
    else if (curFormat == FB_DISPLAY_FORMAT_TIME)
    {
        strFormat = "HH:mm:ss";
    }
    else //if (curFormat == FB_DISPLAY_FORMAT_DATETIME)
    {
        strFormat = "yyyy-MM-dd HH:mm:ss";
    }

    QByteArray baFinal = curDateTime.toString(strFormat).toUtf8();
    jsonFinal = baFinal.data();
    return jsonFinal;
}

void FBDateTimeAttr::fromJson (Json::Value jsonVal)
{
    // TODO: все проверки на некорректный json?

    // Сразу проверяем, является ли дата/время текущими.
    if (jsonVal.isNull())
    {
        isDefaultCurrent = true;
        return;
    }
    isDefaultCurrent = false;

    QByteArray ba = jsonVal.asString().data();
    QString strDateTime = QString::fromUtf8(ba);

    // Поочерёдно пробуем считать дату-время и угадать формат (доступа к другим
    // атрибутам контрола тут нет, поэтому приходится угадывать).
    curDateTime = QDateTime::fromString(strDateTime,"yyyy-MM-dd HH:mm:ss");
    if (curDateTime.isValid())
    {
        curFormat = FB_DISPLAY_FORMAT_DATETIME;
        return;
    }

    curDateTime = QDateTime::fromString(strDateTime,"yyyy-MM-dd");
    if (curDateTime.isValid())
    {
        curDateTime.setTime(QTime(0,0,0)); // Задаём ненужное время, иначе поставится случайное.
        curFormat = FB_DISPLAY_FORMAT_DATE;
        return;
    }

    curDateTime = QDateTime::fromString(strDateTime,"HH:mm:ss");
    if (curDateTime.isValid())
    {
        curDateTime.setDate(QDate(2016,1,1)); // Задаём ненужную дату, иначе поставится случайная.
        curFormat = FB_DISPLAY_FORMAT_TIME;
        return;
    }

    // Значит дата-время каким-то образом сформированы внутри .ngfp некорректно. Создаём
    // дату-время по умолчанию.
    // TODO: тут нужно выдавать сообщения пользователю (что вероятно файл был изменён
    // вручную) иначе можно не заметить, сохранить и заменить все даты!
    setDefault();
}

QWidget* FBDateTimeAttr::getWidget ()
{
    QPushButton *widget = new QPushButton();
    widget->setText(tr("Задать"));
    connect(widget, SIGNAL(clicked()), this, SLOT(onEditStart()));
    return widget;
}

void FBDateTimeAttr::onEditStart()
{
    FBDateTimeDialog dialog;

    // Загружаем старое значение.
    if (isDefaultCurrent)
    {
        dialog.chbCurrent->setChecked(true);
        dialog.wDateTime->setEnabled(false);
    }
    else
    {
        dialog.chbCurrent->setChecked(false);
        dialog.wDateTime->setEnabled(true);
    }

    // Формат и дату для датапикера ставим в любом случае, на случай отмены даты/времени
    // по умолчанию.
    dialog.wDateTime->setDateTime(curDateTime);
    dialog.wDateTime->setDisplayFormat(curFormat);

    // Сохраняем новое значение.
    if (dialog.exec())
    {
        if (dialog.chbCurrent->isChecked())
        {
            isDefaultCurrent = true;
        }
        else
        {
            isDefaultCurrent = false;
            curDateTime = dialog.wDateTime->dateTime();
        }

        emit changeAppearance();
    }
}


QString FBDateTimeAttr::getDateTimeString ()
{
    if (isDefaultCurrent)
        return "...";
    return curDateTime.toString(curFormat);
}


void FBDateTimeAttr::updateFormat (int indexOfSelectAttr)
{
    switch (indexOfSelectAttr)
    {
        case 0: curFormat = FB_DISPLAY_FORMAT_DATE; break;
        case 1: curFormat = FB_DISPLAY_FORMAT_TIME; break;
        default: curFormat = FB_DISPLAY_FORMAT_DATETIME; break;
    }
}




