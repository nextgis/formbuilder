/******************************************************************************
 *
 * Name:     attributes.h
 * Project:  NextGIS Form Builder
 * Purpose:  Attributes classes declaration.
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

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>

#include "json/json.h"

class FBElem;

// Атрибут элемента интерфейса.
// Должен уметь:
// 1. Сохранять себя и загружать себя в/из json элемента интерфейса.
// 2. Выводить свой виджет для ввода в таблицу атрибутов, когда содержащий
// его элемент интерфейса выделен на экране, а затем сохранять в себя значения,
// введённые через этот виджет.
// 3. (Не все) Возвращать своё значение (возможн окомплексное), для того, чтобы
// элемент, владеющий этим атрибутом, обновил свой внешний вид на форме.
class FBAttr: public QObject
{
    Q_OBJECT
    public:
     FBAttr (FBElem *elem, QString strJsonName); // создание изначального атрибута.
     virtual ~FBAttr ();
     virtual Json::Value toJson () = 0;
     virtual void fromJson (Json::Value jsonVal) = 0; // замена изначального атрибута на взятое из json.
     virtual QWidget *getWidget () = 0;
    public: // следующие поля потребуется читать извне.
     FBElem *elem; // указатель на элемент интерфейса, владеющий этим атрибутом.
     QString strJsonName; // собственное имя-ключ атрибута для записи в json файл.
    signals:
     // После вызова этого сигнала, каждый элемент сам берёт значение атрибута и
     // обновляет свой внешний вид.
     void changeAppearance ();
};

class FBFieldAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBFieldAttr (FBElem *elem, QString strJsonName);
     //~FBFieldAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QString getValue () { return value; }
     void setUndefinedValue (); // требуется для метода "импорт контролов" и "менеджер полей"
    private slots:
     void onEditEnd (QString fieldName);
    private:
     QString value;
};

class FBTextAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBTextAttr(FBElem *elem, QString strJsonName, QString strInitValue);
     //~FBTextAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QString getValue () { return value; }
    private slots:
     void onEditEnd (QString lineEditText);
    private:
     QString value;
};

class FBNumberAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBNumberAttr(FBElem *elem, QString strJsonName, int nInitValue, int nMin, int nMax);
     //~FBNumberAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     int getValue () { return value; }
    private slots:
     void onEditEnd (int newValue);
    private:
     int value;
     int min;
     int max;
};

class FBYesNoAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBYesNoAttr(FBElem *elem, QString strJsonName, bool bInitValue);
     //~FBYesNoAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     bool getValue () { return value; }
    private slots:
     void onEditEnd (int checkboxState);
    private:
     // Мы не можем хранить тут сам Чекбокс и постоянно его выдавать, т.к.
     // таблица, в которую он будет выведен, будет его удалять при очистке.
     // Поэтому getWidget() всегда возвращает новый виджет, а значение атрибута
     // хранится в переменной.
     bool value;
};

class FBListValuesAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBListValuesAttr(FBElem *elem, QString strJsonName);
     //~FBListValuesAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     QString getDefValueText ();
    protected slots:
     void onEditStart ();
    protected:
     // Аналогично FBYesNoAttr: мы не можем хранить сами виджеты, т.к. они будут
     // всегда уничтожаться диалогом, в который они выводятся. Поэтому -
     // храним переменные и каждый раз выводим новые виджеты с загрузкой/
     // сохранением этих переменных.
     QList<QPair<QString,QString> > values; // inner string, outer string
     int defIndex; // первый фиктивный элемент, если он и был, здесь не учитывается
};

// Для значений Радио-группы нужен наследник, т.к.:
// - у радиогруппы свой набор исходных значений.
// - радиогруппа по-другому обновляет свой внешний вид.
class FBRadioValuesAttr: public FBListValuesAttr
{
    Q_OBJECT
    public:
     FBRadioValuesAttr (FBElem *elem, QString strJsonName);
     //void preview (QList<QPair<QString,QString> >, int);
     QList<QPair<bool,QString> > getValues ();
};


class FBDoubleListValuesAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBDoubleListValuesAttr(FBElem *elem, QString strJsonName);
     //~FBDoubleListValuesAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
     void getDefValueTexts (QString &str1, QString &str2);
    protected slots:
     void onEditStart ();
    protected:
     QList<QPair<QString,QString> > values; // inner string, outer string
     int defIndex;
     QList<QList<QPair<QString,QString> > > values2;
     QList<int> defIndexes2;
};


// Список с фиксированными (для самой программы) значениями.
class FBSelectAttr: public FBAttr
{
    Q_OBJECT
    public:
     FBSelectAttr(FBElem *elem, QString strJsonName, QList<QString> strsValues, int curIndex);
     //~FBSelectAttr();
     Json::Value toJson ();
     void fromJson (Json::Value jsonVal);
     QWidget *getWidget ();
    private slots:
     void onEditEnd (int indexSelected);
    private:
     QList<QString> strsValues;
     int curIndex;

};


#endif //ATTRIBUTES_H
