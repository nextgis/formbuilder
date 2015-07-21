/******************************************************************************
 *
 * Name:     elements.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Elements classes implementation.
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


FBElem* FBElem::SELECTED = NULL;

bool FBElem::IS_SELECTED_MOVING = false;

FBElem::FBElem(FBFactory *factoryPtr): QWidget()
{
    this->factoryPtr = factoryPtr;
}

void FBElem::setSelectStyle()
{
    this->setStyleSheet("QWidget"//#" + this->objectName() +
                            "{"
                                "background-color: rgba(0,0,0,0); "
                                "border: 2px dashed red; "
                            "}");
}

void FBElem::setDeselectStyle()
{
    this->setStyleSheet("QWidget"//#" + this->objectName() +
                            "{"
                                "background-color: rgba(0,0,0,0); "
                                "border: 2px dashed rgba(0,0,0,0); "
                            "}");
}


QString FBElem::getJsonName ()
{
    return factoryPtr->getJsonName();
}


QString FBElem::getDisplayName ()
{
    return factoryPtr->getDisplayName();
}


void FBElem::mousePressEvent (QMouseEvent *event)
{
    // Выделяем элемент.
    if (SELECTED != NULL)
        SELECTED->setDeselectStyle();
    this->setSelectStyle();
    SELECTED = this;

    // Выводим его артибуты в правую колонку.
    emit pressElem ();

    IS_SELECTED_MOVING = true;
}


void FBElem::mouseMoveEvent (QMouseEvent *event)
{
    // Единственный способ, при котором мне удалось сделать передвижение
    // элементов по экрану (при условии что сами элементы и виджеты для вставки -
    // это виджеты с одним общим родителем - экраном) - это код далее. Все остальные
    // способы - не рабочие, из-за одной причины: Qt захватывает (grab) мышь при нажатии
    // на виджет элемента интерфейса и все остальные виджеты не получают никаких событий
    // пока не произойдёт отпускание мыши в любом месте экрана. Убрать это не удаётся.
    // 1. [не работает] - метод releaseMouse() в событии mousePress виджета элемента
    // интерфейса.
    // 2. [не работает] - игнорирование события через event->ignore() во всех событиях
    // мыши.
    // 3. [не работает] - установка eventFilter на виджеты с непропусканием событий
    // нажатия или движения.
    // 4. [не работает] - setAttribute(Qt::WA_TransparentForMouseEvents).
    // 5. [не работает] - QCoreApplication::postEvent() для соседнего виджета для вставки.
    // В данном подходе мы используем простой поиск виджетов по координатам. Здесь есть и
    // плюс - из-за того что grab мыши будет всегда на виджете, на который мы нажали до его
    // отпускания - мы можем сделать всю работу по отрисовке внутри трёх событий FBElem:
    // press, move и release.
    if (IS_SELECTED_MOVING)
    {
        // Используя координаты родителя (виджет экрана) мы ищем, над каким из
        // виджетов для вставки находится сейчас курсор и выделяем его.
        QWidget* screenWidget = this->parentWidget();
        QPoint coords = screenWidget->mapFromGlobal(QPoint(event->globalX(),
                                                           event->globalY()));
        QWidget* widget = screenWidget->childAt(coords);
        if (widget == 0)
            return;
        QString className = widget->metaObject()->className();
        if (className == "FBInsertWidget")
        {
            if (static_cast<FBInsertWidget*>(widget) == FBInsertWidget::SHOWED)
                return;
            if (FBInsertWidget::SHOWED != NULL)
                FBInsertWidget::SHOWED->setHideStyle();
            static_cast<FBInsertWidget*>(widget)->setShowStyle();
            FBInsertWidget::SHOWED = static_cast<FBInsertWidget*>(widget);
        }
    }
}


void FBElem::mouseReleaseEvent (QMouseEvent *event)
{
    IS_SELECTED_MOVING = false;

    if (FBInsertWidget::SHOWED == NULL)
        return;

    // Снимаем выделение с виджета для вставки.
    // См. FBElem::mouseMoveEvent за комментами.
    FBInsertWidget::SHOWED->setHideStyle();
    FBInsertWidget* insWidget = FBInsertWidget::SHOWED;
    FBInsertWidget::SHOWED = NULL;

    // Вставляем на место виджета для вставки перемещаемый виджет
    // элемента интерфейса.
    emit moveElem(this,insWidget);
}


// См. главный исходник для чего это:
void FBElem::paintEvent(QPaintEvent *pe)
{
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}


// ================================================================================= //
//                                      Текст                                        //
// ================================================================================= //

FBTextLabelElem::FBTextLabelElem(FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    QVBoxLayout *vlayAll = new QVBoxLayout(this);
    vlayAll->setContentsMargins(2,2,2,2);
    vlayAll->setSpacing(0);

    labText = new QLabel(this);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont("Candara",14)); // TODO: шрифт Roboto.
    labText->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(100,100,100);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");

    vlayAll->addWidget(labText);

    // Атрибуты:

    FBTextAttr *attr = new FBTextAttr(this,FB_JSON_TEXT,tr("Заголовок"));
    mapAttrs.insert(tr("Текст"),attr);
    connect(attr,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    // Обновляем внешний вид в зависимости от изначальных значений атрибутов.
    updateAppearance();
}

void FBTextLabelElem::updateAppearance()
{
    labText->setText(" " + static_cast<FBTextAttr*>
                     (mapAttrs.value(tr("Текст")))->getValue());
}


// ================================================================================= //
//                                     Пробел                                        //
// ================================================================================= //

FBSpaceElem::FBSpaceElem(FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setMinimumHeight(30);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();
}


// ================================================================================= //
//                                 Текстовое поле                                    //
// ================================================================================= //

FBTextEditElem::FBTextEditElem(FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    QVBoxLayout *vlayAll = new QVBoxLayout(this);
    vlayAll->setContentsMargins(2,2,2,2);
    vlayAll->setSpacing(0);

    labText = new QLabel(this);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont("Candara",14));
    labText->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(46,46,46);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");

    QWidget *widDecor = new QWidget(this);
    widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor->setMinimumHeight(7);
    widDecor->setStyleSheet("QWidget"//#"+widDecor->objectName()+
                            "{"
                                "border-top: none;"
                                "border-left: 3px solid rgb(170,170,170);"
                                "border-right: 3px solid rgb(170,170,170);"
                                "border-bottom: 3px solid rgb(170,170,170);"
                            "}");

    vlayAll->addWidget(labText);
    vlayAll->addWidget(widDecor);

    // Атрибуты:

    FBFieldAttr *attr1 = new FBFieldAttr(this,FB_JSON_FIELD);
    mapAttrs.insert(tr("Поле слоя"),attr1);

    FBTextAttr *attr2 = new FBTextAttr(this,FB_JSON_TEXT,tr("Текст"));
    mapAttrs.insert(tr("Начальный текст"),attr2);
    connect(attr2,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    FBYesNoAttr *attr3 = new FBYesNoAttr(this,FB_JSON_ONLY_FIGURES,false);
    mapAttrs.insert(tr("Только цифры"),attr3);

    FBNumberAttr *attr4 = new FBNumberAttr(this,FB_JSON_MAX_STRING_COUNT,FB_INIT_ATTR_MAXSTRINGS);
    mapAttrs.insert(tr("Макс. число строк"),attr4);

    FBYesNoAttr *attr5 = new FBYesNoAttr(this,FB_JSON_LAST,false);
    mapAttrs.insert(tr("Запоминать последнее значение"),attr5);

    updateAppearance();
}


void FBTextEditElem::updateAppearance()
{
    QString newText = static_cast<FBTextAttr*>(mapAttrs.value(tr("Начальный текст")))->getValue();
    labText->setText(" " + newText);
}


// ================================================================================= //
//                               Выпадающий список                                   //
// ================================================================================= //

FBComboBoxElem::FBComboBoxElem(FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet("QWidget" //#" + this->objectName() +
                        "{"
                            "background-color: rgba(0,0,0,0); "
                            "border: 2px dashed rgba(0,0,0,0); "
                        "}");

    QHBoxLayout *hlayAll = new QHBoxLayout(this);
    hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);

    QVBoxLayout *vlayText = new QVBoxLayout();
    vlayText->setContentsMargins(0,0,0,0);
    vlayText->setSpacing(0);

    labText = new QLabel(this);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont("Candara",14));
    labText->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(46,46,46);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");

    QWidget *widDecor = new QWidget(this);
    widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor->setMinimumHeight(7);
    widDecor->setStyleSheet("QWidget"//#"+widDecor->objectName()+
                            "{"
                                "border-top: none;"
                                "border-left: none;"
                                "border-right: none;"
                                "border-bottom: 3px solid rgb(170,170,170);"
                            "}");

    vlayText->addWidget(labText);
    vlayText->addWidget(widDecor);

    QLabel *labImg = new QLabel(this);
    QPixmap pixmap = QPixmap(":/img/for_combo.png");
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pixmap.width());
    labImg->setAlignment(Qt::AlignBottom);
    labImg->setPixmap(pixmap);
    labImg->setContentsMargins(0,0,0,0);

    hlayAll->addLayout(vlayText);
    hlayAll->addWidget(labImg);

    // Атрибуты:

    FBFieldAttr *attr1 = new FBFieldAttr(this,FB_JSON_FIELD);
    mapAttrs.insert(tr("Поле слоя"),attr1);

    FBListValuesAttr *attr2 = new FBListValuesAttr(this,FB_JSON_VALUES);
    mapAttrs.insert(tr("Значения"),attr2);
    connect(attr2,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    FBYesNoAttr *attr5 = new FBYesNoAttr(this,FB_JSON_LAST,false);
    mapAttrs.insert(tr("Запоминать последнее значение"),attr5);

    updateAppearance();
}

void FBComboBoxElem::updateAppearance ()
{
    labText->setText(" " + static_cast<FBListValuesAttr*>
                     (mapAttrs.value(tr("Значения")))->getDefValueText());
}


// ================================================================================= //
//                     Двухуровневый выпадающий список                               //
// ================================================================================= //

FBDoubleComboBoxElem::FBDoubleComboBoxElem(FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    this->setMaximumHeight(60);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    labText1 = new QLabel(this);
    labText1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText1->setFont(QFont("Candara",14));
    labText1->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(46,46,46);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");
    labText2 = new QLabel(this);
    labText2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText2->setFont(QFont("Candara",14));
    labText2->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(46,46,46);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");
    QWidget *widDecor1 = new QWidget(this);
    widDecor1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor1->setMinimumHeight(7);
    widDecor1->setStyleSheet("QWidget"//#"+widDecor->objectName()+
                            "{"
                                "border-top: none;"
                                "border-left: none;"
                                "border-right: none;"
                                "border-bottom: 3px solid rgb(170,170,170);"
                            "}");
    QWidget *widDecor2 = new QWidget(this);
    widDecor2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor2->setMinimumHeight(7);
    widDecor2->setStyleSheet("QWidget"//#"+widDecor->objectName()+
                            "{"
                                "border-top: none;"
                                "border-left: none;"
                                "border-right: none;"
                                "border-bottom: 3px solid rgb(170,170,170);"
                            "}");
    QPixmap pixmap = QPixmap(":/img/for_combo.png");
    QLabel *labImg1 = new QLabel(this);
    labImg1->setStyleSheet("QLabel{border: none;}");
    labImg1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg1->setFixedWidth(pixmap.width());
    labImg1->setAlignment(Qt::AlignBottom);
    labImg1->setPixmap(pixmap);
    labImg1->setContentsMargins(0,0,0,0);
    QLabel *labImg2 = new QLabel(this);
    labImg2->setStyleSheet("QLabel{border: none;}");
    labImg2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg2->setFixedWidth(pixmap.width());
    labImg2->setAlignment(Qt::AlignBottom);
    labImg2->setPixmap(pixmap);
    labImg2->setContentsMargins(0,0,0,0);

    QVBoxLayout *vlayAll = new QVBoxLayout(this);
    vlayAll->setContentsMargins(2,2,2,2);
    vlayAll->setSpacing(4);
    QHBoxLayout *hlay1 = new QHBoxLayout();
    hlay1->setContentsMargins(0,0,0,0);
    hlay1->setSpacing(0);
    QHBoxLayout *hlay2 = new QHBoxLayout();
    hlay2->setContentsMargins(0,0,0,0);
    hlay2->setSpacing(0);
    QVBoxLayout *vlayText1 = new QVBoxLayout();
    vlayText1->setContentsMargins(0,0,0,0);
    vlayText1->setSpacing(0);
    QVBoxLayout *vlayText2 = new QVBoxLayout();
    vlayText2->setContentsMargins(0,0,0,0);
    vlayText2->setSpacing(0);

    vlayText1->addWidget(labText1);
    vlayText1->addWidget(widDecor1);
    vlayText2->addWidget(labText2);
    vlayText2->addWidget(widDecor2);
    hlay1->addLayout(vlayText1);
    hlay1->addWidget(labImg1);
    hlay2->addLayout(vlayText2);
    hlay2->addWidget(labImg2);
    vlayAll->addLayout(hlay1);
    vlayAll->addLayout(hlay2);

    // Атрибуты:
/*
    FBFieldAttr *attr1 = new FBFieldAttr(this,FB_JSON_FIELD_LEVEL1);
    mapAttrs.insert(tr("Поле слоя уровня 1"),attr1);

    FBFieldAttr *attr2 = new FBFieldAttr(this,FB_JSON_FIELD_LEVEL2);
    mapAttrs.insert(tr("Поле слоя уровня 2"),attr2);

//    FBDoubleListValuesAttr *attr3 = new FBDoubleListValuesAttr(this,FB_JSON_VALUES);
//    mapAttrs.insert(tr("Значения"),attr3);
//    connect(attr3,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    FBYesNoAttr *attr4 = new FBYesNoAttr(this,FB_JSON_LAST,false);
    mapAttrs.insert(tr("Запоминать последнее значение"),attr4);
*/
    updateAppearance();
}

void FBDoubleComboBoxElem::updateAppearance ()
{
//    labText1->setText(" " + static_cast<FBListValuesAttr*>
//                     (mapAttrs.value(tr("Значения")))->getDefValueText1());
//    labText2->setText(" " + static_cast<FBListValuesAttr*>
//                     (mapAttrs.value(tr("Значения")))->getDefValueText2());
}


// ================================================================================= //
//                                    Флажок                                         //
// ================================================================================= //

FBCheckBoxElem::FBCheckBoxElem(FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    // TODO: сделать это единым (статически) для всех экземпляров FBCheckBoxElem!
    pixmapOn = QPixmap(":/img/for_check_on.png");
    pixmapOff = QPixmap(":/img/for_check_off.png");

    QHBoxLayout *hlayAll = new QHBoxLayout(this);
    hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);

    labImg = new QLabel(this);
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pixmapOff.width()); // размеры пиксмапов одинаковые
    labImg->setAlignment(Qt::AlignCenter);
    labImg->setPixmap(pixmapOff);
    labImg->setContentsMargins(0,0,0,0);

    labText = new QLabel(this);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont("Candara",14));
    labText->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(46,46,46);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");

    hlayAll->addWidget(labImg);
    hlayAll->addWidget(labText);

    // Атрибуты:

    FBFieldAttr *attr1 = new FBFieldAttr(this,FB_JSON_FIELD);
    mapAttrs.insert(tr("Поле слоя"),attr1);

    FBYesNoAttr *attr2 = new FBYesNoAttr(this,FB_JSON_INIT_VALUE,false);
    mapAttrs.insert(tr("Начальное значение"),attr2);
    connect(attr2,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    FBTextAttr *attr3 = new FBTextAttr(this,FB_JSON_TEXT,tr("Флажок"));
    mapAttrs.insert(tr("Текст"),attr3);
    connect(attr3,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    FBYesNoAttr *attr5 = new FBYesNoAttr(this,FB_JSON_LAST,false);
    mapAttrs.insert(tr("Запоминать последнее значение"),attr5);

    updateAppearance();
}

void FBCheckBoxElem::updateAppearance ()
{
    labText->setText(" " + static_cast<FBTextAttr*>
                     (mapAttrs.value(tr("Текст")))->getValue());

    bool newValue = static_cast<FBYesNoAttr*>
            (mapAttrs.value(tr("Начальное значение")))->getValue();
    if (newValue)
        labImg->setPixmap(pixmapOn); // из док-ии: Setting the pixmap clears any previous content.
    else
        labImg->setPixmap(pixmapOff);
}


// ================================================================================= //
//                                 Радио-группа                                      //
// ================================================================================= //

FBRadioGroupElem::FBRadioGroupElem (FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    // TODO: сделать это единым (статически) для всех экземпляров FBRadioGroupElem!
    pixmapOn = QPixmap(":/img/for_radio_on.png");
    pixmapOff = QPixmap(":/img/for_radio_off.png");

    vlayAll = new QVBoxLayout(this);
    vlayAll->setContentsMargins(2,2,2,2);
    vlayAll->setSpacing(8);

    // Атрибуты:

    FBFieldAttr *attr1 = new FBFieldAttr(this,FB_JSON_FIELD);
    mapAttrs.insert(tr("Поле слоя"),attr1);

    FBRadioValuesAttr *attr2 = new FBRadioValuesAttr(this,FB_JSON_VALUES);
    mapAttrs.insert(tr("Значения"),attr2);
    connect(attr2,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    FBYesNoAttr *attr5 = new FBYesNoAttr(this,FB_JSON_LAST,false);
    mapAttrs.insert(tr("Запоминать последнее значение"),attr5);

    updateAppearance();
}

void FBRadioGroupElem::updateAppearance()
{
    QList<QPair<bool,QString> > ret =
        static_cast<FBRadioValuesAttr*>(mapAttrs.value(tr("Значения")))->getValues();

    // Удаляем все раскладки с виджетами, которые находились в раскладке до этого.
    // Мы точно знаем, что встретится в раскладке - проверять не надо.
    QLayoutItem *childIem;
    while ((childIem = vlayAll->takeAt(0)) != 0)
    {
        QLayout *childLayout = childIem->layout();
        QLayoutItem *childIem2;
        while ((childIem2 = childLayout->takeAt(0)) != 0)
        {
            QWidget *childWidget = childIem2->widget();
            delete childWidget;
        }
        delete childLayout;
    }

    // Добавляем всё с нуля.
    for (int i=0; i<ret.size(); i++)
    {
        QLabel *labText = new QLabel(this);
        labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        labText->setFont(QFont("Candara",14));
        labText->setStyleSheet("QLabel"
                               "{"
                                   "color: rgb(46,46,46);"
                                   "border-top: none;"
                                   "border-left: none;"
                                   "border-right: none;"
                                   "border-bottom: none;"
                               "}");
        labText->setText(ret[i].second);

        QLabel *labImg = new QLabel(this);
        labImg->setStyleSheet("QLabel{border: none;}");
        labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        labImg->setFixedWidth(pixmapOff.width());
        labImg->setAlignment(Qt::AlignCenter);
        labImg->setContentsMargins(0,0,0,0);
        if (ret[i].first)
        {
            labImg->setPixmap(pixmapOn);
        }
        else
        {
            labImg->setPixmap(pixmapOff);
        }

        QHBoxLayout *hlay = new QHBoxLayout();
        hlay->addWidget(labImg);
        hlay->addWidget(labText);
        vlayAll->addLayout(hlay);
    }
}


// ================================================================================= //
//                                    Кнопка                                         //
// ================================================================================= //

FBButtonElem::FBButtonElem (FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMinimumHeight(36);
    this->setMaximumHeight(36);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    QHBoxLayout *hlayAll = new QHBoxLayout(this);
    hlayAll->setContentsMargins(2,2,2,2);
    hlayAll->setSpacing(0);

    labText = new QLabel(this);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    labText->setFont(QFont("Candara",14));
    labText->setAlignment(Qt::AlignCenter);
    labText->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(100,100,100);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: 1px solid rgb(170,170,170);"
                               "background-color: rgb(220,220,220);"
                               "border-top-left-radius: 1px;"
                               "border-top-right-radius: 1px;"
                               "border-bottom-left-radius: 1px;"
                               "border-bottom-right-radius: 1px;"
                           "}");
    hlayAll->addWidget(labText);

    // Атрибуты:

    FBFieldAttr *attr1 = new FBFieldAttr(this,FB_JSON_FIELD);
    mapAttrs.insert(tr("Поле слоя"),attr1);

    FBTextAttr *attr2 = new FBTextAttr(this,FB_JSON_VALUE,"");
    mapAttrs.insert(tr("Значение"),attr2);

    FBTextAttr *attr3 = new FBTextAttr(this,FB_JSON_TEXT,tr("Кнопка"));
    mapAttrs.insert(tr("Текст"),attr3);
    connect(attr3,SIGNAL(changeAppearance()),this,SLOT(updateAppearance()));

    updateAppearance();
}

void FBButtonElem::updateAppearance ()
{
    labText->setText(" " + static_cast<FBTextAttr*>
                     (mapAttrs.value(tr("Текст")))->getValue());
}


// ================================================================================= //
//                                 Дата и время                                      //
// ================================================================================= //

FBDateTimeElem::FBDateTimeElem (FBFactory *factoryPtr): FBElem(factoryPtr)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMaximumHeight(30);
    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    labText = new QLabel(this);
    labText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labText->setFont(QFont("Candara",14));
    labText->setStyleSheet("QLabel"
                           "{"
                               "color: rgb(46,46,46);"
                               "border-top: none;"
                               "border-left: none;"
                               "border-right: none;"
                               "border-bottom: none;"
                           "}");

    QLabel *labImg = new QLabel(this);
    QPixmap pixmap = QPixmap(":/img/for_date.png");
    labImg->setStyleSheet("QLabel{border: none;}");
    labImg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    labImg->setFixedWidth(pixmap.width());
    labImg->setAlignment(Qt::AlignTop);
    labImg->setPixmap(pixmap);
    labImg->setContentsMargins(0,0,0,0);

    QWidget *widDecor = new QWidget(this);
    widDecor->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    widDecor->setFixedHeight(3);
    widDecor->setStyleSheet("QWidget"//#"+widDecor->objectName()+
                            "{"
                                "border-top: none;"
                                "border-left: none;"
                                "border-right: none;"
                                "border-bottom: 2px solid rgb(170,170,170);"
                            "}");

    QHBoxLayout *hlayTop = new QHBoxLayout();
    hlayTop->setContentsMargins(0,0,0,0);
    hlayTop->setSpacing(0);
    QVBoxLayout *vlayAll = new QVBoxLayout(this);
    vlayAll->setContentsMargins(2,2,2,2);
    vlayAll->setSpacing(0);
    hlayTop->addWidget(labText);
    hlayTop->addWidget(labImg);
    vlayAll->addLayout(hlayTop);
    vlayAll->addWidget(widDecor);

    updateAppearance();
}

void FBDateTimeElem::updateAppearance ()
{
    labText->setText(" 01.01.2016");
}






















