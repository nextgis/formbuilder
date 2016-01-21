/******************************************************************************
 *
 * Name:     elements.h
 * Project:  NextGIS Form Builder
 * Purpose:  Elements classes declaration.
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

#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>

#include <QMouseEvent>

//см. paintEvent() у виджетов с Q_OBJECT:
#include <QPainter>
#include <QStyleOption>

#include "json/json.h"

#include "attributes.h"

class FBInsertWidget;
class FBFactory;


// Базовый класс для всех элементов интерфейса (контролов), добавляемых
// на экран устройства для формирования формы.
// Любой элемент в FormBuilder содержит своё графическое представления
// (в рамках самой программы) и список атрибутов.
class FBElem: public QWidget
{
    Q_OBJECT // Q_OBJECT здесь требуется ещё и для определения типа виджета взятого из общей раскладки экрана

    public:
     static FBElem* SELECTED;
     static bool IS_SELECTED_MOVING;

     // ключ здесь - это не JSON-ключ, а уникальное название атрибута в элементе,
     // т.к. в элементе м.б. например два FB_JSON_TEXT. Ключи определяются в
     // конструкторе конкретного класса.
     QMap<QString,FBAttr*> mapAttrs;

    private:
     FBFactory *factoryPtr; // указатель на фабрику, которая создала этот элемент.

    public:
     // В конструкторах наследуемых классов - вся работа по заданию внешности и атрибутов.
     FBElem (FBFactory *factoryPtr);
     virtual ~FBElem () {}
     void setSelectStyle ();
     void setDeselectStyle ();
     QString getJsonName ();
     QString getDisplayName ();

    public slots:
     virtual void updateAppearance () = 0;

    public: // потребуется искусственно выделять элемент сразу после добавления на форму.
     virtual void mousePressEvent (QMouseEvent *event);
     virtual void mouseReleaseEvent (QMouseEvent *event);
    protected:
     virtual void mouseMoveEvent (QMouseEvent *event);
     virtual void paintEvent (QPaintEvent *pe);

    signals:
     void moveElem (FBElem*, FBInsertWidget*);
     void pressElem ();
};


// ================================================================================== //
//                                   Decoration                                       //
// ===================================================================================//

class FBTextLabelElem: public FBElem
{
    Q_OBJECT
    public:
     FBTextLabelElem (FBFactory *factoryPtr);
     ~FBTextLabelElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labText;
};

class FBSpaceElem: public FBElem
{
    Q_OBJECT
    public:
     FBSpaceElem (FBFactory *factoryPtr);
     ~FBSpaceElem () {}
    public slots:
     virtual void updateAppearance () {}
};


// ================================================================================== //
//                                     Input                                          //
// ===================================================================================//

class FBTextEditElem: public FBElem
{
    Q_OBJECT
    public:
     FBTextEditElem (FBFactory *factoryPtr);
     ~FBTextEditElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labText;
};

class FBComboBoxElem: public FBElem
{
    Q_OBJECT
    public:
     FBComboBoxElem (FBFactory *factoryPtr);
     ~FBComboBoxElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labText;
};

class FBDoubleComboBoxElem: public FBElem
{
    Q_OBJECT
    public:
     FBDoubleComboBoxElem (FBFactory *factoryPtr);
     ~FBDoubleComboBoxElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labText1;
     QLabel *labText2;
};

class FBCheckBoxElem: public FBElem
{
    Q_OBJECT
    public:
     FBCheckBoxElem (FBFactory *factoryPtr);
     ~FBCheckBoxElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labImg;
     QLabel *labText;
     QPixmap pixmapOn; // TODO: сделать static
     QPixmap pixmapOff;
};

class FBRadioGroupElem: public FBElem
{
    Q_OBJECT
    public:
     FBRadioGroupElem (FBFactory *factoryPtr);
     ~FBRadioGroupElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QVBoxLayout *vlayAll;
     QPixmap pixmapOn;
     QPixmap pixmapOff;
};

class FBButtonElem: public FBElem
{
    Q_OBJECT
    public:
     FBButtonElem (FBFactory *factoryPtr);
     ~FBButtonElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labText;
};

class FBDateTimeElem: public FBElem
{
    Q_OBJECT
    public:
     FBDateTimeElem (FBFactory *factoryPtr);
     ~FBDateTimeElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labText;
};

class FBPhotoElem: public FBElem
{
    Q_OBJECT
    public:
     FBPhotoElem (FBFactory *factoryPtr);
     ~FBPhotoElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QHBoxLayout *hAll;
     QList<QLabel*> labImgs2;
};

class FBSignatureElem: public FBElem
{
    Q_OBJECT
    public:
     FBSignatureElem (FBFactory *factoryPtr);
     ~FBSignatureElem () {}
    public slots:
     virtual void updateAppearance () {}
};

class FBCompassElem: public FBElem
{
    Q_OBJECT
    public:
     FBCompassElem (FBFactory *factoryPtr);
     ~FBCompassElem () {}
    public slots:
     virtual void updateAppearance () {}

};


// ================================================================================== //
//                                  Grouping                                          //
// ===================================================================================//

class FBGroupElem: public FBElem
{
    Q_OBJECT
    public:
     FBGroupElem (FBFactory *factoryPtr);
     ~FBGroupElem () {}
    public slots:
     virtual void updateAppearance ();
    private:
     QLabel *labHeader;
     QVBoxLayout *layContent;
};



#endif // ELEMENTS_H




