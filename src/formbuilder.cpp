/******************************************************************************
 *
 * Name:     formbuilder.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  General classes implementation
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, NextGIS
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

#include "formbuilder.h"
#include "ui_formbuilder.h"


// ************************************************************************** //
// ******************************* FBProject ******************************** //
// ************************************************************************** //

FBProject::FBProject()
{
    path = QDir::currentPath();
    name_base = "form";
    poDS = NULL;
    NGWConnection = "";
    //availableFields
}

FBProject::~FBProject()
{
    if (poDS != NULL)
        OGRDataSource::DestroyDataSource(poDS);
}

// ************************************************************************** //
// ******************************* FBElem *********************************** //
// ************************************************************************** //

FBElem* FBElem::CURRENT = NULL;

long FBElem::idCounter = 0;

long FBElem::GetNextId ()
{
    idCounter++;
    return idCounter - 1;
}


FBElem::~FBElem()
{
    for (int i=0; i<images.size(); i++)
    {
        if (images[i] != NULL)
            delete images[i];
    }
    images.clear();
}

void FBElem::mousePressEvent (QMouseEvent *event)
{
    if (CURRENT != NULL && CURRENT != this)
    {
        CURRENT->deselect();
    }

    this->select();
    CURRENT = this;

    emit elemPressed();
}

void FBElem::select ()
{
    setStyleSheet("QLabel#" + this->objectName() + " { border: 2px solid red}");
}

void FBElem::deselect ()
{
    setStyleSheet("QLabel#" + this->objectName() + " { border: none}");
}


// ************************************************************************** //
// ******************************* FBElemType ******************************* //
// ************************************************************************** //

FBElemType* FBElemType::CURRENT = NULL;


FBElem *FBElemType::CreateElem ()
{
    FBElem *newElem = new FBElem();
    QVBoxLayout *newElemLayout = new QVBoxLayout(newElem);
    newElemLayout->setSpacing(0);
    newElemLayout->setContentsMargins(0,0,0,0);

    int totalHeight = 0;

    newElem->elemType = this;

    // Создаём компоненты элемента.
    for (int i=0; i<imgPaths.size(); i++)
    {
        QLabel *image = new QLabel(newElem);
        newElem->images.append(image);
        image->setPixmap(imgPaths[i]);
        image->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        image->setScaledContents(true);
        newElemLayout->addWidget(image);
        totalHeight+=image->height();
    }

    // Если не просуммировать общий минимальный размер дочерних виджетов,
    // то раскладка не будет знать, что нельзя уменьшать до бесконечности
    // добавляемый элемент.
    newElem->setMinimumHeight(totalHeight);
    newElem->setMaximumHeight(totalHeight);
    newElem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    //newElem->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    // Задаём уникальное имя. Используется как минимум для того, чтобы
    // можно было задавать стили только для конкретных виджетов, не трогая
    // их дочерние виджеты.
    newElem->setObjectName(QString("e_") +
                           //QString::number(this->type) +
                           QString("_") +
                           QString::number(FBElem::GetNextId()));

    // Формируем внешний вид элемента интерфейса. Это необходимо делать после
    // задания имени объекта, т.к. стиль будет задаваться по имени.
    newElem->deselect(); // Задаём стиль контура - элемент не выделен.

    // Копируем названия и поля свойств объекта.
    for (int i=0; i<attributeNames.size(); i++)
    {
        Json::Value value; // будет = null
        newElem->attributes.append(QPair<QString,Json::Value>(attributeNames[i].first,value));
    }

    // TODO: проверки, и если не прошло - delete newElem, return NULL

    return newElem;
}


void FBElemType::mousePressEvent (QMouseEvent *event)
{
    CURRENT = this;

    emit elemTypePressed();
}


// ************************************************************************** //
// ******************************* FBParentLabel **************************** //
// ************************************************************************** //


FBParentLabel::FBParentLabel(QScrollArea *parent): QLabel (parent)
{
    QVBoxLayout *parentLayout = new QVBoxLayout(this);
    parentLayout->addStretch();
}


bool FBParentLabel::InsertElem (FBElem *elem, int index)
{
    if (elem == NULL)
        return false;

    QVBoxLayout *parentLayout = (QVBoxLayout*)this->layout();
    if (elements.size() == FBMaxElemInPageCount)
    //if (parentLayout->children().count() == FBMaxElemInPageCount)
    //if (children().count()-1 == FBMaxElemInPageCount) // -1, т.к. там ещё раскладка
    {
        // TODO: сигнал-слот с сообщением.
        return false;
    }

    elem->setParent(this);

    int indexToInsert;
    if (index < 0)
        //indexToInsert = posToInsertWidget;
        indexToInsert = 0;
    else
        indexToInsert = index;

    // TODO: if (parentLayout != NULL)
    parentLayout->insertWidget(indexToInsert,elem);

    //elements.insert(indexToInsert, QPair<FBElem*,int>(elem,elem->height()/2));
    //elements.insert(indexToInsert, elem);
    elements.insert(indexToInsert, elem);

    // Обязательно изменяем высоту родительского лейбла, иначе не будет
    // прокрутки.
    // Высоту изменяем только если есть на какую величину изменять.
    // При перерасчёте надо учесть отступы сверху и снизу иначе элементы
    // будут залезать друг на друга.
    int startHeight;
    if (isVertical)
        startHeight = FBParentLabelVertStartHeight;
    else
        startHeight = FBParentLabelHorStartHeight;
    int totalChildrenHeight =
            parentLayout->contentsMargins().top()
            + parentLayout->contentsMargins().bottom();
    for (int k=0; k < elements.size(); k++)
    //for (int k=0; k < parentLayout->children().size(); k++)
    //for (int k=0; k < children().size(); k++)
    {
        // Проверяем, является ли ребёнок - виджетом, т.к. родительский
        // лейбл будет иметь по крайней мере один такой объект - раскладку,
        // не являющуюся виджетом.
        //totalChildrenHeight += parentLabel->elements[k].first->height() + parentLayout->spacing();
        //if ((children()[k])->isWidgetType())
        //{
            totalChildrenHeight += elements[k]->height()
                + parentLayout->spacing();
        //}
    }
    if (totalChildrenHeight > startHeight)
    {
        resize(width(),totalChildrenHeight);
    }

    FBElemType::CURRENT = NULL;

    return true;
}


void FBParentLabel::MoveElem (FBElem *elem, bool isUp)
{
    if (elem == NULL)
        return;

    QVBoxLayout *parentLayout = (QVBoxLayout*)this->layout();

    //int index = parentLayout->indexOf(FBElem::CURRENT);
    int index = -1;
    for (int i=0; i<elements.size(); i++)
    {
        if (elements[i] == elem)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
        return;

    if (isUp)
    {
        if (index != 0)
        {
            parentLayout->removeWidget(FBElem::CURRENT);
            elements.removeAt(index);
            parentLayout->insertWidget(index-1,FBElem::CURRENT);
            elements.insert(index-1,FBElem::CURRENT);
        }
    }
    else
    {
        if (index != elements.size()-1)
        {
            parentLayout->removeWidget(FBElem::CURRENT);
            elements.removeAt(index);
            parentLayout->insertWidget(index+1,FBElem::CURRENT);
            elements.insert(index+1,FBElem::CURRENT);
        }
    }
}


void FBParentLabel::DeleteElem (FBElem *elem)
{
    if (elem == NULL)
        return;

    for (int i=0; i<elements.size(); i++)
    {
        if (elements[i] == elem)
        {
            elements.removeAt(i);
            break;
        }
    }

    delete elem;
    elem = NULL;
}


// ************************************************************************** //
// ******************************* FBTabWidget ****************************** //
// ************************************************************************** //

void FBTabWidget::mousePressEvent(QMouseEvent *event)
{
    emit tabPressed();
}


// ************************************************************************** //
// ******************************* FormBuilder ****************************** //
// ************************************************************************** //

FormBuilder::~FormBuilder()
{
    delete CUR_PRJ;
    delete ui;
}

FormBuilder::FormBuilder(QWidget *parent) :QWidget(parent),ui(new Ui::FormBuilder)
{
    ui->setupUi(this);

    //move(0,0);
    this->showMaximized();

    // Создаём верхнее меню: "Файл", ...
    menuBar = new QMenuBar(this);
    this->layout()->setMenuBar(menuBar);
    menuFile = new QMenu(QString::fromUtf8("Проект"),menuBar);
    menuBar->addMenu(menuFile);
      menuNew = new QMenu(QString::fromUtf8("Новый"),menuBar);
      menuFile->addMenu(menuNew);
        actionNewVoid = new QAction(QString::fromUtf8("Пустой"),menuBar);
        menuNew->addAction(actionNewVoid);
        actionNewDataset = new QAction(QString::fromUtf8("Файловый ИД GDAL"),menuBar);
        menuNew->addAction(actionNewDataset);
        actionNewNGW = new QAction(QString::fromUtf8("NextGIS Web"),menuBar);
        menuNew->addAction(actionNewNGW);
      actionOpen = new QAction(QString::fromUtf8("Открыть"),menuBar);
      menuFile->addAction(actionOpen);
      actionSave = new QAction(QString::fromUtf8("Сохранить"),menuBar);
      menuFile->addAction(actionSave);
    menuScreen = new QMenu(QString::fromUtf8("Экран"),menuBar);
    menuBar->addMenu(menuScreen);
      menuOrtn = new QMenu(QString::fromUtf8("Ориентация"),menuBar);
      menuScreen->addMenu(menuOrtn); //->addAction(actionOrtn);
        actionOrtnPrt = new QAction(QString::fromUtf8("Портретная"),menuBar);
        actionOrtnPrt->setCheckable(true);
        actionOrtnPrt->setChecked(true);
        menuOrtn->addAction(actionOrtnPrt);
        actionOrtnAlb = new QAction(QString::fromUtf8("Альбомная"),menuBar);
        actionOrtnAlb->setCheckable(true);
        menuOrtn->addAction(actionOrtnAlb);
      actionAddPage = new QAction(QString::fromUtf8("Добавить вкладку"),menuBar);
      menuScreen->addAction(actionAddPage);

    connect(actionNewVoid, SIGNAL(triggered()), this, SLOT(OnActionNewVoid()));
    connect(actionNewDataset, SIGNAL(triggered()), this, SLOT(OnActionNewDataset()));
    connect(actionNewNGW, SIGNAL(triggered()), this, SLOT(OnActionNewNGW()));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(OnActionOpen()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(OnActionSave()));
    connect(actionOrtnPrt, SIGNAL(triggered()), this, SLOT(OnActionOrtnPrt()));
    connect(actionOrtnAlb, SIGNAL(triggered()), this, SLOT(OnActionOrtnAlb()));
    connect(actionAddPage, SIGNAL(triggered()), this, SLOT(OnActionAddPage()));

    // Создаём типы элементов.
    CreateElemTypes();

    // Выводим их в левую колонку.
    QVBoxLayout* pVLayout = (QVBoxLayout*)ui->groupBox->layout();
    for (int i=0; i<vElemTypes.size(); i++)
    {
        QVBoxLayout *pVLayout2;
        //QLabel* labelText;
        //labelText = new QLabel(ui->groupBox);
        //labelText->setText(vElemTypes[i]->caption);
        //labelText->setStyleSheet("QLabel {text-decoration: underline}");
        pVLayout2 = new QVBoxLayout();
        //pVLayout2->addWidget(labelText);
        pVLayout2->addWidget(vElemTypes[i]);
        pVLayout->addLayout(pVLayout2);
        pVLayout->addSpacing(5);

        //QFrame *frame = new QFrame(ui->groupBox);
        //pVLayout->addWidget(frame);
        //frame->setFrameShape(QFrame::StyledPanel);
        //frame->setFrameShadow(QFrame::Sunken);
        //frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        //frame->show();
    }

    // Добавляем в левую колонку вниз растягиватель.
    pVLayout->addStretch();

    // Создаём экран телефона с одной страницей - вертикальной и горизонтальной.
    QVBoxLayout *groupLayout = new QVBoxLayout(ui->groupBox_2);
    tabWidget = new FBTabWidget(ui->groupBox_2);
    tabHorWidget = new FBTabWidget(ui->groupBox_2);
    groupLayout->addWidget(tabWidget);
    groupLayout->addWidget(tabHorWidget);
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(OnTabIndexChanged(int)));
    connect(tabHorWidget,SIGNAL(currentChanged(int)),this,SLOT(OnTabIndexChanged(int)));
    connect(tabWidget,SIGNAL(tabPressed()),this,SLOT(DeselectElement()));
    connect(tabHorWidget,SIGNAL(tabPressed()),this,SLOT(DeselectElement()));

    // Начинаем с портретной ориентации.
    ui->groupBox_2->setTitle(QString::fromUtf8("Экран приложения: портретная ориентация"));
    tabHorWidget->hide();
    isCurrentVertical = true;

    // Добавляем первую страницу (вкладку) для обоих ориентаций.
    //OnActionAddPage();
    CreatePage();

    // Настраиваем правую панель.
    // Таблица без вертикальной шапки, поскольку это не даёт
    // её нормально скроллить. Шапку имитирует жирный шрифт левой колонки.
    ui->tableWidget1->clear();
    ui->tableWidget1->verticalHeader()->hide();
    ui->tableWidget1->setColumnCount(2);
    ui->tableWidget1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tableWidget1->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget1->horizontalHeader()->setClickable(false);
    QStringList theaders;
    theaders.append(QString::fromUtf8("Атрибут"));
    theaders.append(QString::fromUtf8("Значение"));
    ui->tableWidget1->setHorizontalHeaderLabels(theaders);

    ui->toolButton->setIcon(QIcon(":/img/up"));
    ui->toolButton_2->setIcon(QIcon(":/img/down"));
    ui->toolButton_3->setIcon(QIcon(":/img/delete"));
    ui->toolButton_4->setIcon(QIcon(":/img/save"));

    // Настройки GDAL.
    OGRRegisterAll();

    CUR_PRJ = new FBProject();
    NEW_PRJ = NULL;

// TEST ----------------------------------------------------------------
    CPLSetConfigOption("CPL_DEBUG","ON");
    CPLSetConfigOption("CPL_CURL_VERBOSE","YES");
    CPLSetConfigOption("CPL_LOG","D:/Projects/FormBuilder/last_log.txt");
    CPLSetConfigOption("CPL_LOG_ERRORS","ON");
// ---------------------------------------------------------------------
}


//-----------------------------------------------------------------------//
//   Создание типов элементов и их лейблов в левой панели инструментов   //
//-----------------------------------------------------------------------//
void FormBuilder::CreateElemTypes ()
{
    FBElemType* elemType;

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_text_label);
    elemType->alias.ru = QString::fromUtf8("Статический текст");
    elemType->alias.en = QString::fromUtf8("Text label");
    elemType->imgPaths.append(":/img/text");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_text,FBAlias(
                               QString::fromUtf8("Текст"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_text_edit);
    elemType->alias.ru = QString::fromUtf8("Текстовое поле");
    elemType->alias.en = QString::fromUtf8("Text edit");
    elemType->imgPaths.append(":/img/textfield");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                             QString::fromUtf8("Целевое поле"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_text,FBAlias(
                             QString::fromUtf8("Начальный текст"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_only_figures,FBAlias(
                             QString::fromUtf8("Только цифры?"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_max_string_count,FBAlias(
                             QString::fromUtf8("Максимальное кол-во строк"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_button);
    elemType->alias.ru = QString::fromUtf8("Кнопка");
    elemType->alias.en = QString::fromUtf8("Button");
    elemType->imgPaths.append(":/img/button");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_text,FBAlias(
                             QString::fromUtf8("Заголовок"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                             QString::fromUtf8("Целевое поле"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_value,FBAlias(
                             QString::fromUtf8("Значение"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_combobox);
    elemType->alias.ru = QString::fromUtf8("Выпадающий список");
    elemType->alias.en = QString::fromUtf8("Combobox");
    elemType->imgPaths.append(":/img/combobox");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                             QString::fromUtf8("Целевое поле"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_values,FBAlias(
                             QString::fromUtf8("Значения"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_checkbox);
    elemType->alias.ru = QString::fromUtf8("Флажок");
    elemType->alias.en = QString::fromUtf8("Checkbox");
    elemType->imgPaths.append(":/img/checkbox");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                                 QString::fromUtf8("Целевое поле"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_text,FBAlias(
                                 QString::fromUtf8("Заголовок"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_radio_group);
    elemType->alias.ru = QString::fromUtf8("Радио-группа");
    elemType->alias.en = QString::fromUtf8("Radio-group");
    elemType->imgPaths.append(":/img/radiobutton");
    elemType->imgPaths.append(":/img/radiobutton_void");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                               QString::fromUtf8("Целевое поле"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_values,FBAlias(
                             QString::fromUtf8("Значения"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_compass);
    elemType->alias.ru = QString::fromUtf8("Компас");
    elemType->alias.en = QString::fromUtf8("Compass");
    elemType->imgPaths.append(":/img/compass");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                               QString::fromUtf8("Целевое поле"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_date_time);
    elemType->alias.ru = QString::fromUtf8("Дата-Время");
    elemType->alias.en = QString::fromUtf8("Date-time");
    elemType->imgPaths.append(":/img/date_date");
    elemType->imgPaths.append(":/img/date_time");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field,FBAlias(
                                 QString::fromUtf8("Целевое поле"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_double_combobox);
    elemType->alias.ru = QString::fromUtf8("Двухуровневый список");
    elemType->alias.en = QString::fromUtf8("Double combobox");
    elemType->imgPaths.append(":/img/doublelist_1");
    elemType->imgPaths.append(":/img/doublelist_2");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field_level1,FBAlias(
                               QString::fromUtf8("Целевое поле уровня 1"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field_level2,FBAlias(
                               QString::fromUtf8("Целевое поле уровня 2"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_values,FBAlias(
                             QString::fromUtf8("Значения"))));
    // В дальнейшем по ходу программы этот список может быть расширен!
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_photo);
    elemType->alias.ru = QString::fromUtf8("Фото");
    elemType->alias.en = QString::fromUtf8("Photo");
    elemType->imgPaths.append(":/img/photo_targ");
    elemType->imgPaths.append(":/img/photo_but");
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field_name,FBAlias(
                               QString::fromUtf8("Целевое поле названия снимка"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field_azimuth,FBAlias(
                               QString::fromUtf8("Целевое поле азимута снимка"))));
    elemType->attributeNames.append(QPair<QString,FBAlias>(FBATTR_field_datetime,FBAlias(
                               QString::fromUtf8("Целевое поле даты снимка"))));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    elemType->name = QString::fromUtf8(FBELEMTYPE_space);
    elemType->alias.ru = QString::fromUtf8("Пробел");
    elemType->alias.en = QString::fromUtf8("Space");
    elemType->imgPaths.append(":/img/void");
    vElemTypes.append(elemType);

    // Для всех:
    for (int i=0; i<vElemTypes.size(); i++)
    {
        //vElemTypes[i]->setPixmap(QPixmap(vElemTypes[i]->imgPaths[0]));
        QIcon icon(vElemTypes[i]->imgPaths[0]);
        vElemTypes[i]->setIcon(icon);
        //vElemTypes[i]->setIconSize(icon.actualSize(vElemTypes[i]->iconSize()));
        // TODO: разобраться как взять настоящие размеры иконки.
        vElemTypes[i]->setIconSize(QSize(100,21));
        vElemTypes[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        vElemTypes[i]->setMinimumHeight(50);
        vElemTypes[i]->setToolTip(vElemTypes[i]->alias.ru);
        connect(vElemTypes[i], SIGNAL(elemTypePressed()),
                this, SLOT(OnElemTypePressed()));
    }

    QVBoxLayout *twoButtonLayout = new QVBoxLayout();

    pElemTypeGroupStart = new FBElemType(ui->groupBox_5);
    pElemTypeGroupStart->name = QString::fromUtf8(FBELEMTYPE_group_start);
    pElemTypeGroupStart->alias.ru = QString::fromUtf8("Начало группы");
    pElemTypeGroupStart->alias.en = QString::fromUtf8("Group start");
    pElemTypeGroupStart->imgPaths.append(":/img/void");
    pElemTypeGroupStart->imgPaths.append(":/img/group_start");
    pElemTypeGroupStart->attributeNames.append(QPair<QString,FBAlias>(FBATTR_text,FBAlias(
                               QString::fromUtf8("Заголовок"))));
    pElemTypeGroupStart->setToolTip(QString::fromUtf8("Поместите в начало будущей группы"));
    pElemTypeGroupStart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    QIcon icon1(pElemTypeGroupStart->imgPaths[1]);
    pElemTypeGroupStart->setIcon(icon1);
    pElemTypeGroupStart->setIconSize(QSize(80,31));
    connect(pElemTypeGroupStart, SIGNAL(elemTypePressed()),
            this, SLOT(OnElemTypePressed()));

    pElemTypeGroupEnd = new FBElemType(ui->groupBox_5);
    pElemTypeGroupEnd->name = QString::fromUtf8(FBELEMTYPE_group_end);
    pElemTypeGroupEnd->alias.ru = QString::fromUtf8("Конец группы");
    pElemTypeGroupEnd->alias.en = QString::fromUtf8("Group end");
    pElemTypeGroupEnd->imgPaths.append(":/img/group_end");
    pElemTypeGroupEnd->imgPaths.append(":/img/void");
    pElemTypeGroupEnd->setToolTip(QString::fromUtf8("Поместите в конец будущей группы"));
    pElemTypeGroupEnd->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    QIcon icon2(pElemTypeGroupEnd->imgPaths[0]);
    pElemTypeGroupEnd->setIcon(icon2);
    pElemTypeGroupEnd->setIconSize(QSize(80,31));
    connect(pElemTypeGroupEnd, SIGNAL(elemTypePressed()),
            this, SLOT(OnElemTypePressed()));

    twoButtonLayout->addWidget(pElemTypeGroupEnd);
    twoButtonLayout->addWidget(pElemTypeGroupStart);
    ui->horizontalLayout_5->addLayout(twoButtonLayout);
}


//-----------------------------------------------------------------------//
//                          Создание вкладки                             //
//-----------------------------------------------------------------------//
void FormBuilder::CreatePage()
{
    QString newPageName = QString::fromUtf8("Вкладка ") +
            QString::number(tabWidget->count() + 1);

// Добавление вертикальной вкладки:

    QWidget *newPage = new QWidget();
    tabWidget->addTab(newPage, newPageName);

        QVBoxLayout *pageLayout = new QVBoxLayout(newPage);
        pageLayout->setSpacing(0);
        pageLayout->setContentsMargins(0,0,0,0);
        pageLayout->addStretch();

        // Верхняя часть экрана: для красоты.
           QLabel *label = new QLabel(newPage);
           label->setPixmap(QString(":/img/top"));
           label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pageLayout->addWidget(label);

        // Средняя часть экрана:
        // Создаём экранный лейбл, который будет иметь только две вещи: фоновую
        // картинку и виджет прокручиваемой области. Тем самым мы фиксируем
        // саму прокручиваемую область, но то, что будет внутри неё соответственно
        // будет изменять свои размеры.
           QLabel *screenLabel = CreateScreenLabel(newPage,true);
        pageLayout->addWidget(screenLabel);

        // Нижняя часть экрана: для красоты.
           QLabel *label3 = new QLabel(newPage);
           label3->setPixmap(QString(":/img/bottom"));
           label3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pageLayout->addWidget(label3);

        pageLayout->addStretch();

        pageLayout->setAlignment(label,Qt::AlignCenter);
        pageLayout->setAlignment(screenLabel,Qt::AlignCenter);
        pageLayout->setAlignment(label3,Qt::AlignCenter);

// Добавление горизонтальной вкладки:

        QWidget *newHorPage = new QWidget();
        tabHorWidget->addTab(newHorPage, newPageName);

        QHBoxLayout *pageHorLayout = new QHBoxLayout(newHorPage);
        pageHorLayout->setSpacing(0);
        pageHorLayout->setContentsMargins(0,0,0,0);
        pageHorLayout->addStretch();
        QVBoxLayout *subPageHorLayout = new QVBoxLayout();
        subPageHorLayout->setSpacing(0);
        subPageHorLayout->setContentsMargins(0,0,0,0);
        pageHorLayout->addLayout(subPageHorLayout);

        subPageHorLayout->addStretch();
           QLabel *horLabel = new QLabel(newHorPage);
           horLabel->setPixmap(QString(":/img/top_hor"));
           horLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        subPageHorLayout->addWidget(horLabel);

           QLabel *screenHorLabel = CreateScreenLabel(newHorPage,false);
        subPageHorLayout->addWidget(screenHorLabel);
        subPageHorLayout->addStretch();

           QLabel *horLabel3 = new QLabel(newHorPage);
           horLabel3->setPixmap(QString(":/img/bottom_hor"));
           horLabel3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pageHorLayout->addWidget(horLabel3);

        pageHorLayout->addStretch();

        pageHorLayout->setAlignment(horLabel,Qt::AlignCenter);
        pageHorLayout->setAlignment(screenHorLabel,Qt::AlignCenter);
        pageHorLayout->setAlignment(horLabel3,Qt::AlignCenter);
}


//-----------------------------------------------------------------------//
//            Создание экранного лейбла вместе с родительским            //
//-----------------------------------------------------------------------//
QLabel *FormBuilder::CreateScreenLabel (QWidget *pageWidget, bool isVertical)
{
    QLabel *screenLabel2 = new QLabel(pageWidget);

    if (isVertical)
        screenLabel2->setPixmap(QString(":/img/back"));
    else
        screenLabel2->setPixmap(QString(":/img/back_hor"));

    screenLabel2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    // Создаём раскладку, прокручиваему область и её подконтрольный виджет -
    // родительский виджет всем элементам интерфейса.
    QVBoxLayout *screenLayout2 = new QVBoxLayout(screenLabel2);
    screenLayout2->setContentsMargins(0,0,0,0);
    screenLayout2->setSpacing(0);
    QScrollArea *scrollArea = new QScrollArea(screenLabel2);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //scrollArea->setStyleSheet("QScrollArea { border: 0px; }");
    scrollArea->setStyleSheet("QScrollArea { border: 0px; }"
                              "QScrollBar:vertical {"
                              "border: 0px;"
                              "background: rgb(31,33,35);"
                              "}"
                              "QScrollBar::handle:vertical {"
                              "background: rgb(81,83,85);"
                              "}"
                              );
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Создаём родительский лейбл и добавляем его под контроль
    // прокручиваемой области.
    // NOTE: scrollArea указывается в родителях для parentLabel потому что
    // иначе при создании страницы быдет мелькать лейбл как отдельное окно.
    // По идее это можно убрать или выбрать другого родителя.
    FBParentLabel *parentLabel = new FBParentLabel(scrollArea);
    parentLabel->isVertical = isVertical;
    parentLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    parentLabel->setStyleSheet("QLabel { background-color: rgb(31,33,35) }");
    parentLabel->show();
    //connect(parentLabel, SLOT(OnElemPressed()), this, SLOT(OnElemPressed()));
    //for (int l=0; l<vElemTypes.size(); l++)
    //{
        //connect(vElemTypes[l], SIGNAL(elemTypePressed()),
                //parentLabel, SLOT(OnElemTypePressed()));
    //}
    //QVBoxLayout *parentLayout = new QVBoxLayout(parentLabel);
    //parentLayout->addStretch();

    // Важный момент - если не сделать ресайз, то лейбл не растянется!
    // scrollArea->setWidgetResizable(true) не помогает.
    // Может быть есть и другой способ сочетать изменение размеров scrollArea
    // и поведение раскладки внутри подконтрольного ей виджета.
    // Следующие цифры подобраны вручную и имеют место быть, поскольку размер
    // "экрана" фиксированный.
    if (isVertical)
        parentLabel->resize(FBParentLabelVertWidth,
                            FBParentLabelVertStartHeight);
    else
        parentLabel->resize(FBParentLabelHorWidth,
                            FBParentLabelHorStartHeight);

    scrollArea->setWidget(parentLabel);
    screenLayout2->addWidget(scrollArea);

    // Сохраняем соответствие страницы и виджет:
    // Добавляем уникальное значение:
    correspondence.insert(pageWidget,
                          QPair<QLabel*,FBParentLabel*> (screenLabel2,parentLabel));

    return screenLabel2;
}


//-----------------------------------------------------------------------//
//                           Очистка вкладки                             //
//-----------------------------------------------------------------------//
// Удаляем а затем добавляем экранный лейбл. Родительский виджет
// тоже удалится (внутри удаления экранного).
void FormBuilder::ClearPage ()
{
    // Определяем текущую ориентацию.
    FBTabWidget *curTabWidget;
    if (isCurrentVertical)
    {
        curTabWidget = tabWidget;
    }
    else
    {
        curTabWidget = tabHorWidget;
    }

    QWidget *widget = curTabWidget->currentWidget();
    if (widget == NULL)
        return;

    // Сначала снимаем выделение т.к. если выделенный элемент находится
    // на текущей вкладке - он будет удалён и мы не будем знать этого
    // в конце.
    DeselectElement();

    // Здесь мы не делаем FBParentLabel::DeleteElem(), т.к. всё равно весь
    // родительский лейбл будет удалён.

    delete correspondence[widget].first;

    // Надо ли удалять указатель из массива детей у родителя?
    // Ответ: нет. После delete его уже нет в родитель->children(),
    // проверено в дебаге.

    // Удаляем из массива соответствий всю запись, т.к. она создастся снова
    // при вызове CreateScreenLabel().
    correspondence.remove(widget);

    QLabel *screenLabel2 = CreateScreenLabel(widget,isCurrentVertical);
    if (isCurrentVertical)
    {
        // Мы заранее знаем, куда вставлять виджет:
        static_cast<QVBoxLayout*>(widget->layout())
                ->insertWidget(2, screenLabel2);
        static_cast<QVBoxLayout*>(widget->layout())
                ->setAlignment(screenLabel2,Qt::AlignCenter);
    }
    else
    {
        // В случае с альбомной ориентацией мы должны использовать не собственную
        // раскладку экранного лейбла, а её дочернюю раскладку.
        QObjectList children = widget->layout()->children();
        for (int j=0; j<children.size(); j++)
        {
            // Ищем единственную горизонтальную раскладку этого виджета. Должна
            // встретится первой.
            // TODO: удостовериться, что единственный ребёнок не виджет - это
            // эта самая раскладка.
            if (!children[j]->isWidgetType())
            {
                static_cast<QHBoxLayout*>(children[j])
                        ->insertWidget(2,screenLabel2);
                static_cast<QHBoxLayout*>(children[j])
                        ->setAlignment(screenLabel2,Qt::AlignCenter);
            }
        }
    }
}


//-----------------------------------------------------------------------//
//                           Удаление вкладки                            //
//-----------------------------------------------------------------------//
// Удаляем пару вкладок с обоих виджетов.
void FormBuilder::DeletePage ()
{
    // Определяем текущую ориентацию. Нужно чтобы определить открытую вкладку.
    FBTabWidget *curTabWidget;
    int tabToDeleteIndex;
    if (isCurrentVertical)
    {
        curTabWidget = tabWidget;
        tabToDeleteIndex = tabWidget->currentIndex();
    }
    else
    {
        curTabWidget = tabHorWidget;
        tabToDeleteIndex = tabHorWidget->currentIndex();
    }

    if (tabWidget->count() == 1)
        return;

    QWidget *widgetVer = tabWidget->widget(tabToDeleteIndex);
    if (widgetVer == NULL)
        return;

    QWidget *widgetHor = tabHorWidget->widget(tabToDeleteIndex);
    if (widgetHor == NULL)
        return;

    // Не забываем удалить виджеты из массива соответствий.
    // Не забываем, что соответствие 1 к 1: один виджет -> один экранный ->
    // один родительский лейбл.
    correspondence.remove(widgetVer);
    correspondence.remove(widgetHor);

    // Удаляем одну и ту же вкладку в обоих ориентациях.
    // Экранный и родительский лейбл удалятся автоматически.
    tabWidget->removeTab(tabToDeleteIndex);
    tabHorWidget->removeTab(tabToDeleteIndex);

    // Сначала снимаем выделение т.к. если выделенный элемент находится
    // на текущей вкладке - он будет удалён и мы не будем знать этого
    // в конце.
    DeselectElement();

    // Затем удаляем сами виджеты.
    // Здесь мы не делаем FBParentLabel::DeleteElem(), т.к. всё равно весь
    // родительский лейбл будет удалён.
    delete widgetVer;
    delete widgetHor;

    ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                             curTabWidget->tabText(curTabWidget->currentIndex()));
}


//-----------------------------------------------------------------------//
//                      Изменить имя вкладки                             //
//-----------------------------------------------------------------------//
// Меняем название для обоих ориентаций.
void FormBuilder::ChangeTabName (QString newName)
{
    FBTabWidget *curTabWidget;
    //QTabWidget *otherTabWidget;
    if (isCurrentVertical)
    {
        curTabWidget = tabWidget;
        //otherTabWidget = tabHorWidget;
    }
    else
    {
        curTabWidget = tabHorWidget;
        //otherTabWidget = tabWidget;
    }

    //curTabWidget->setTabText(curTabWidget->currentIndex(),newName);
    //otherTabWidget->setTabText(curTabWidget->currentIndex(),newName);
    tabWidget->setTabText(curTabWidget->currentIndex(),newName);
    tabHorWidget->setTabText(curTabWidget->currentIndex(),newName);
}


//-----------------------------------------------------------------------//
//                           Очистить всё                                //
//-----------------------------------------------------------------------//
void FormBuilder::ClearAll ()
{
    // Удаляем все страницы кроме последней.
    while (tabWidget->count() > 1)
    {
        DeletePage();
    }

    // Последнюю вкладку и её пару очищаем.
    ClearPage();
    isCurrentVertical = !isCurrentVertical;
    ClearPage();
    isCurrentVertical = !isCurrentVertical;

    // Обнуляем счётчик элементов, т.к. будет создание новой формы.
    FBElem::idCounter = 0;

    // Переименовываем все поля.
    // TODO: !!!
}


// Некоторые общие действия после снятия выделения с элемента интерфейса.
void FormBuilder::AfterDeselectElement ()
{
    // TODO: Удостовериться, что виджеты комбобоксов удаляются.
    // Если что в справке сказано, что после добавления виджета -
    // passing the ownership of the widget to the table.
    ui->groupBox_4->setTitle(QString::fromUtf8("Выберите элемент ..."));
    ui->tableWidget1->clearContents();
    ui->tableWidget1->setRowCount(0);
}


void FormBuilder::DeselectElement ()
{
    if (FBElem::CURRENT != NULL)
    {
        FBElem::CURRENT->deselect();
        FBElem::CURRENT = NULL;
        AfterDeselectElement();
    }
}


void FormBuilder::ShowMsgBox (QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


//-----------------------------------------------------------------------//
//                        СЛОТЫ МЫШИ                                     //
//-----------------------------------------------------------------------//


// Создать элемент.
void FormBuilder::OnElemTypePressed ()
{
    if (FBElemType::CURRENT == NULL)
        return;

    QWidget *widget;
    if (isCurrentVertical)
        widget = tabWidget->currentWidget();
    else
        widget = tabHorWidget->currentWidget();

    // Определяем родителький лейбл в который добавляем элемент.
    FBParentLabel *parentLabel = correspondence[widget].second;

    // Определяем, если некий элемент выделен, вставляем новый рядом с ним.
    int indexToInsert = 0;
    if (FBElem::CURRENT != NULL)
    {
        for (int i=0; i<parentLabel->elements.size(); i++)
        {
            if (parentLabel->elements[i] == FBElem::CURRENT)
            {
                indexToInsert = i + 1; // Будем вставлять после выделенного.
                break;
            }
        }
    }

    FBElem *newElem = FBElemType::CURRENT->CreateElem();
    if (parentLabel->InsertElem(newElem,indexToInsert))
    {
        connect(newElem, SIGNAL(elemPressed()), this, SLOT(OnElemPressed()));
    }
    else
    {
        delete newElem;
    }
}


// Выбрать элемент.
void FormBuilder::OnElemPressed ()
{
    if (FBElem::CURRENT == NULL)
        return;

    // Подготавливаем таблицу для вывода атрибутов.
    // TODO: Это удаляет виджеты-комбобоксы/кнопки?
    ui->tableWidget1->clearContents();
    // TODO: Правильно таким образом очищать список строк?
    ui->tableWidget1->setRowCount(0);

    // Просматриваем все атрибуты элемента и выводим в таблицу:
    // 1. Название атрибута (для всех);
    // 2. Способ его редактирования (для тех, где этот способ
    // отличается от простого едита ячейки таблицы);
    // 3. Имеющееся значение (если его можно вывести).
    for (int i=0; i<FBElem::CURRENT->attributes.size(); i++)
    {
        int r = ui->tableWidget1->rowCount();
        ui->tableWidget1->setRowCount(r + 1);

        // Ищем алиас атрибута.
        QString attrAlias;
        for (int x=0; x<FBElem::CURRENT->elemType->attributeNames.size(); x++)
        {
            // Ищем алиас атрибута по его имени.
            if (FBElem::CURRENT->elemType->attributeNames[x].first
                == FBElem::CURRENT->attributes[i].first)
            {
                // Сюда зайдёт обязательно, т.к. элемент FBElem создавался через
                // FBElemType путём копирования имён атрибутов.
                // Исторически сложилось что тут мы не использем Map, а ищем в цикле.
                attrAlias = FBElem::CURRENT->elemType->attributeNames[x].second.ru;
                break;
            }
        }

        // Выводим алиас атрибута как название строки таблицы.
        QTableWidgetItem *itemToAdd;
        itemToAdd = new QTableWidgetItem(attrAlias);
        itemToAdd->setForeground(QBrush(QColor(80,80,80)));
        itemToAdd->setFlags(Qt::ItemIsEditable);
        ui->tableWidget1->setItem(i,0,itemToAdd);

        // В зависимости от типа элемента и данного его атрибута требуется разная логика
        // для задания способоа редактирования атрибута и его значения:

        // 1. Элемент - любой; Атрибут - все виды "Целевого поля".
        if (!CUR_PRJ->availableFields.isEmpty()
                && (FBElem::CURRENT->attributes[i].first == FBATTR_field
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_name
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_datetime
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_azimuth
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_level1
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_level2
                    ))
        {
            // Создаём комбобокс.
            QComboBox *combo = new QComboBox(ui->tableWidget1);
            combo->setEditable(true);

            // Получаем строковое значение атрибута.
            QString attrValue = QString::fromUtf8(
                        FBElem::CURRENT->attributes[i].second.asString().data());

            // Заполняем комбобокс.
            int index = -1;
            for (int j=0; j<CUR_PRJ->availableFields.size(); j++)
            {
                if (CUR_PRJ->availableFields[j]
                        == attrValue)
                {
                    index = j;
                }
                combo->addItem(CUR_PRJ->availableFields[j]);
            }
            ui->tableWidget1->setCellWidget(i,1,combo);

            // Ищем старое сохранённое значение.
            combo->setCurrentIndex(index);
            if (index == -1)
            {
                // TODO: всегда присваивать "<не задано>".

                combo->setEditText(attrValue);
            }
        }

        // 2. Элемент - текстовый едит; Атрибут - "Только цифры".
        else if (FBElem::CURRENT->attributes[i].first == FBATTR_only_figures)
        {
            QComboBox *combo = new QComboBox(ui->tableWidget1);
            combo->setEditable(false);
            combo->addItem(QString::fromUtf8("да"));
            combo->addItem(QString::fromUtf8("нет"));
            ui->tableWidget1->setCellWidget(i,1,combo);

            // Ставим значение атрибута.
            if (FBElem::CURRENT->attributes[i].second.asBool() == true)
            {
                combo->setCurrentIndex(0);
            }
            else
            {
                combo->setCurrentIndex(1);
            }
        }

        // 3. Элемент - комбобокс или радиогруппа; Атрибут - "Значения".
        else if ((FBElem::CURRENT->elemType->name == FBELEMTYPE_combobox
                  || FBElem::CURRENT->elemType->name == FBELEMTYPE_radio_group)
                 && FBElem::CURRENT->attributes[i].first == FBATTR_values)
        {
            // Выводим кнопку. Текущее значение выводить строкой не нужно.
            QPushButton *but = new QPushButton(ui->tableWidget1);
            but->setText(QString::fromUtf8("Задать ..."));
            ui->tableWidget1->setCellWidget(i,1,but);
            connect(but,SIGNAL(clicked()),this,SLOT(ShowComboOrRadioDialog()));
            // NOTE: Считывание JSON-значения пройдёт в методе,
            // подсоединённому к сигналу.
        }

        // 4. Элемент - двухуровневый список; Атрибут - "Значения".
        else if (FBElem::CURRENT->elemType->name == FBELEMTYPE_double_combobox
                 && FBElem::CURRENT->attributes[i].first == FBATTR_values)
        {
            // Выводим кнопку. Текущее значение выводить строкой не нужно.
            QPushButton *but = new QPushButton(ui->tableWidget1);
            but->setText(QString::fromUtf8("Задать ..."));
            ui->tableWidget1->setCellWidget(i,1,but);
            connect(but,SIGNAL(clicked()),this,SLOT(ShowDoubleComboDialog()));
            // NOTE: Считывание JSON-значения пройдёт в методе,
            // подсоединённому к сигналу.
        }

        // 5. Элемент - все остальные; Атрибут - все остальные.
        else
        {
            // Считаем это обычной строкой и выводим в обычную ячейку таблицы.
            QTableWidgetItem *itemToAdd;
            itemToAdd = new QTableWidgetItem(QString::fromUtf8(
                        FBElem::CURRENT->attributes[i].second.asString().data()));
            ui->tableWidget1->setItem(i,1,itemToAdd);
        }
    }

    ui->groupBox_4->setTitle(QString::fromUtf8("Редактирование элемента: ") +
                         FBElem::CURRENT->elemType->alias.ru);
}


//-----------------------------------------------------------------------//
//                        СЛОТЫ КНОПОК                                   //
//-----------------------------------------------------------------------//


// Очистить вкладку.
void FormBuilder::on_pushButton_2_clicked()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Очистить вкладку?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;
    ClearPage();
}


// Удалить вкладку.
void FormBuilder::on_pushButton_3_clicked()
{
    if (tabWidget->count() == 1)
        return;
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Удалить вкладку?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;
    DeletePage();
}


// Изменить название вкладки.
void FormBuilder::on_pushButton_4_clicked()
{
    if (ui->lineEdit->text().size() > FBMaxTabNameCharCount)
        return;
    ChangeTabName(ui->lineEdit->text());
    ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                             ui->lineEdit->text());
}


// Переместить элемент выше.
void FormBuilder::on_toolButton_clicked()
{
    if (FBElem::CURRENT == NULL)
        return;

    QWidget *widget;
    if (isCurrentVertical)
        widget = tabWidget->currentWidget();
    else
        widget = tabHorWidget->currentWidget();

    correspondence[widget].second->MoveElem(FBElem::CURRENT,true);
}


// Переместить элемент ниже.
void FormBuilder::on_toolButton_2_clicked()
{
    if (FBElem::CURRENT == NULL)
        return;

    QWidget *widget;
    if (isCurrentVertical)
        widget = tabWidget->currentWidget();
    else
        widget = tabHorWidget->currentWidget();

    correspondence[widget].second->MoveElem(FBElem::CURRENT,false);
}


// Удалить элемент.
void FormBuilder::on_toolButton_3_clicked()
{
    if (FBElem::CURRENT == NULL)
        return;

    QWidget *widget;
    if (isCurrentVertical)
        widget = tabWidget->currentWidget();
    else
        widget = tabHorWidget->currentWidget();
    //if (widget == NULL)
        //return;
    // TODO: Уменьшаем высоту родительского лейбла!

    correspondence[widget].second->DeleteElem(FBElem::CURRENT);
    FBElem::CURRENT = NULL; // обязательно добавить здесь! (TODO: проверить в дебаге, почему не срабатывает обнуление в функции удаления)
    AfterDeselectElement();
}


// Зафиксировать изменения.
void FormBuilder::on_toolButton_4_clicked()
{
    if (FBElem::CURRENT == NULL)
        return;

    // Надо снять выделение с ячеек в таблице, т.к. в момент редактирования
    // ячейки в ней находится особый виджет.
    ui->tableWidget1->setCurrentCell(-1,-1);

    // Просматриваем все атрибуты выделенного элемента (строки таблицы
    // полностью повторяют порядок следования) и сохраняем значения
    // из таблицы в список атрибутов элемента.
    for (int i=0; i<FBElem::CURRENT->attributes.size(); i++)
    {
        // В зависимости от типа элемента и данного его атрибута требуется
        // разная логика доставания значения атрибута из элементов его редактировнаия
        // и сохранения в массив атрибутов:

        // 1. Элемент - любой; Атрибут - все виды "Целевого поля".
        if (!CUR_PRJ->availableFields.isEmpty()
                && (FBElem::CURRENT->attributes[i].first == FBATTR_field
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_name
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_datetime
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_azimuth
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_level1
                    || FBElem::CURRENT->attributes[i].first == FBATTR_field_level2
                    ))
        {
            // Используем item(i,0). 0 - т.к. самая левая колонка - это не ячейка
            // а заголовок.
            QWidget *widget = ui->tableWidget1->cellWidget(i,1);
            // Проверяем, комбобокс ли это.
            //if (widget != NULL)
            //{
                //const QMetaObject *objInfo = widget->metaObject();
                //QString className = objInfo->className();
                //if (className == "QComboBox")
                //{
                    Json::Value attrValue;
                    QByteArray baValue = static_cast<QComboBox*>(widget)
                            ->currentText().toUtf8();
                    attrValue = baValue.data();
                    FBElem::CURRENT->attributes[i].second = attrValue;
                //}
            //}
        }

        // 2. Элемент - текстовый едит; Атрибут - "Только цифры".
        else if (FBElem::CURRENT->attributes[i].first == FBATTR_only_figures)
        {
            // Используем item(i,0). 0 - т.к. самая левая колонка - это не ячейка
            // а заголовок.
            QWidget *widget = ui->tableWidget1->cellWidget(i,1);
            // Проверяем, комбобокс ли это.
            //if (widget != NULL)
            //{
                //const QMetaObject *objInfo = widget->metaObject();
                //QString className = objInfo->className();
                //if (className == "QComboBox")
                //{
                    Json::Value attrValue;
                    if (static_cast<QComboBox*>(widget)->currentText()
                            == QString::fromUtf8("да"))
                        attrValue = true;
                    else
                        attrValue = false;
                    FBElem::CURRENT->attributes[i].second = attrValue;
                //}
            //}
        }

        // 3. Элемент - комбобокс или радиогруппа; Атрибут - "Значения".
        else if ((FBElem::CURRENT->elemType->name == FBELEMTYPE_combobox
                  || FBElem::CURRENT->elemType->name == FBELEMTYPE_radio_group)
                 && FBElem::CURRENT->attributes[i].first == FBATTR_values)
        {
            // NOTE: атрибут будет считан и сохранён в специальном диалоге
            // при нажатии кнопки в этой ячейке. Это ветвление нужно чтобы не
            // выводилась простая строка.
        }

        // 4. Элемент - двухуровневый список; Атрибут - "Значения".
        else if (FBElem::CURRENT->elemType->name == FBELEMTYPE_double_combobox
                 && FBElem::CURRENT->attributes[i].first == FBATTR_values)
        {
            // NOTE: атрибут будет считан и сохранён в специальном диалоге
            // при нажатии кнопки в этой ячейке. Это ветвление нужно чтобы не
            // выводилась простая строка.
        }

        // 5. Элемент - все остальные; Атрибут - все остальные.
        else
        {
            Json::Value attrValue;
            QByteArray baValue = ui->tableWidget1->item(i,1)->text().toUtf8();
            attrValue = baValue.data();
            FBElem::CURRENT->attributes[i].second = attrValue;
        }
    }
}


// Изменить текущую вкладку.
void FormBuilder::OnTabIndexChanged (int index)
{
    if (index == -1)
        return;

    // Определяем текущую ориентацию, поскольку слот вызовется после сигналов
    // от обоих таб вджетов.
    // Меняем текущую вкладку у обоих табов.
    FBTabWidget *curTabWidget;
    FBTabWidget *otherTabWidget;
    if (isCurrentVertical)
    {
        curTabWidget = tabWidget;
        otherTabWidget = tabHorWidget;
    }
    else
    {
        curTabWidget = tabHorWidget;
        otherTabWidget = tabWidget;
    }

    // Не зациклится, т.к. при заходе сюда со второго таб-виджета, у первого уже
    // будет стоять именно этот индекс.
    otherTabWidget->setCurrentIndex(index);

    // Имена у обоих вкладок одинаковые.
    ui->lineEdit->setText(curTabWidget->tabText(index));
    ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                             curTabWidget->tabText(index));

    // Снимаем выделение, если было.
    DeselectElement();
}



//-----------------------------------------------------------------------//
//                         СЛОТЫ МЕНЮ                                    //
//-----------------------------------------------------------------------//

// Добавить вкладку.
void FormBuilder::OnActionAddPage ()
{
    // Проверяем любой из таб-виджетов. Кол-во вкладок в обоих одинаковое.
    if (tabWidget->count() == FBMaxPageCount)
        return;
    CreatePage();

    // Снимаем выделение, если было.
    DeselectElement();
}

// Вертикальная ориентация.
void FormBuilder::OnActionOrtnPrt ()
{
    if (!isCurrentVertical)
    {
        actionOrtnAlb->setChecked(false);
        isCurrentVertical = true;
        tabWidget->show();
        tabHorWidget->hide();
        //tabWidget->adjustSize();
        //ui->groupBox_2->adjustSize();
        ui->groupBox_2->setTitle(
                    QString::fromUtf8("Экран приложения: портретная ориентация"));

        // Снимаем выделение, если было.
        DeselectElement();
    }
    else
    {
        // Не даём снять галку.
        actionOrtnPrt->setChecked(true);
    }
}

// Горизонтальная ориентация.
void FormBuilder::OnActionOrtnAlb ()
{
    if (isCurrentVertical)
    {
        actionOrtnPrt->setChecked(false);
        isCurrentVertical = false;
        tabWidget->hide();
        tabHorWidget->show();
        //tabHorWidget->adjustSize();
        //ui->groupBox_2->adjustSize();
        ui->groupBox_2->setTitle(
                    QString::fromUtf8("Экран приложения: альбомная ориентация"));

        // Снимаем выделение, если было.
        DeselectElement();
    }
    else
    {
        // Не даём снять галку.
        actionOrtnAlb->setChecked(true);
    }
}


// Редактирование элементов комбобокса или радиогруппы.
void FormBuilder::ShowComboOrRadioDialog()
{
    FBListDialog dialog (FBElem::CURRENT->elemType->name,this);

    //Достаём данные из JSON-значения атрибута и выводим их в созданный диалог.

    Json::Value arrayValue;
    for (int i=0; i<FBElem::CURRENT->attributes.size(); i++)
    {
        if (FBElem::CURRENT->attributes[i].first == FBATTR_values)
        {
            arrayValue = FBElem::CURRENT->attributes[i].second;
            break;
        }
    }

    int defaultIndex = 0;
    for (int i=0; i<arrayValue.size(); ++i)
    {
        Json::Value elemValue;
        elemValue = arrayValue[i];
        QListWidgetItem *item;
        item = new QListWidgetItem();

        item->setText(QString::fromUtf8(elemValue[FBJSONKEY_alias].asString().data()));
        item->setData(Qt::UserRole,QString::fromUtf8(elemValue[FBJSONKEY_name].asString().data()));
        dialog.listLeft->addItem(item);

        Json::Value defaultValue;
        defaultValue = elemValue[FBJSONKEY_default];
        if (!defaultValue.isNull() && defaultValue == true)
        {
            defaultIndex = i+1; // почему +1 см. ниже
        }
    }
    dialog.refreshLeftCombo(0,"nothing"); // чтобы наполнить комбобокс элементами
    dialog.comboLeft->setCurrentIndex(defaultIndex);

    //Достаём данные из диалога и формируем JSON-значение атрибута.

    if (dialog.exec())
    {
        Json::Value arrayValue;

        for (int i=0; i<dialog.listLeft->count(); i++)
        {
            Json::Value elemValue;
            Json::Value value;
            QListWidgetItem *item;
            QByteArray ba;

            item = dialog.listLeft->item(i);

            ba = item->text().toUtf8();
            value = ba.data();
            elemValue[FBJSONKEY_alias] = value;

            ba = item->data(Qt::UserRole).toString().toUtf8();
            value = ba.data();
            elemValue[FBJSONKEY_name] = value;

            if (dialog.comboLeft->currentIndex() != 0
                   && dialog.comboLeft->currentIndex() == i+1) // почему +1 см. ниже
            {
                elemValue[FBJSONKEY_default] = true;
            }

            arrayValue.append(elemValue);
        }

        // Ищем атрибут "Значения" и присваеваем ему заданное значение.
        for (int i=0; i<FBElem::CURRENT->attributes.size(); i++)
        {
            if (FBElem::CURRENT->attributes[i].first == FBATTR_values)
            {
                FBElem::CURRENT->attributes[i].second = arrayValue;
                break;
            }
        }
    }
}


// Редактирование элементов сдвоенного комбобокса.
void FormBuilder::ShowDoubleComboDialog()
{
    FBListDialog dialog (FBElem::CURRENT->elemType->name,this);

    //Достаём данные из JSON-значения атрибута и выводим их в созданный диалог.

    Json::Value arrayValue;
    for (int i=0; i<FBElem::CURRENT->attributes.size(); i++)
    {
        if (FBElem::CURRENT->attributes[i].first == FBATTR_values)
        {
            arrayValue = FBElem::CURRENT->attributes[i].second;
            break;
        }
    }

    int defaultIndex = 0;
    for (int i=0; i<arrayValue.size(); ++i)
    {
        Json::Value elemValue;
        elemValue = arrayValue[i];
        QListWidgetItem *item;
        item = new QListWidgetItem();

        item->setText(QString::fromUtf8(elemValue[FBJSONKEY_alias].asString().data()));
        item->setData(Qt::UserRole,QString::fromUtf8(elemValue[FBJSONKEY_name].asString().data()));
        dialog.listLeft->addItem(item);

        Json::Value defaultValue;
        defaultValue = elemValue[FBJSONKEY_default];
        if (!defaultValue.isNull() && defaultValue == true)
        {
            defaultIndex = i+1; // почему +1 см. ниже
        }

        dialog.listsRight.append(new QListWidget(this));
        dialog.listsLayoutRight->addWidget(dialog.listsRight.last());
        connect(dialog.listsRight.last(), SIGNAL(itemClicked(QListWidgetItem*)),
                &dialog, SLOT(OnRightItemSelected(QListWidgetItem*)));
        dialog.listsRight.last()->hide();
        dialog.combosRight.append(new QComboBox(this));
        dialog.combosLayoutRight->addWidget(dialog.combosRight.last());
        dialog.combosRight.last()->hide(); // наполнение будет потом
        Json::Value array2Value;
        array2Value = elemValue[FBJSONKEY_values];
        int default2Index = 0;
        for (int j=0; j<array2Value.size(); ++j)
        {
            Json::Value elem2Value;
            elem2Value = array2Value[j];
            item = new QListWidgetItem();
            item->setText(QString::fromUtf8(elem2Value[FBJSONKEY_alias].asString().data()));
            item->setData(Qt::UserRole,QString::fromUtf8(elem2Value[FBJSONKEY_name].asString().data()));
            dialog.listsRight.last()->addItem(item);

            Json::Value default2Value;
            default2Value = elem2Value[FBJSONKEY_default];
            if (!default2Value.isNull() && default2Value == true)
            {
                default2Index = j+1; // почему +1 см. ниже
            }
        }
        dialog.pListRight = dialog.listsRight.last(); // чтобы сработало обновление комбобокса
        dialog.pComboRight = dialog.combosRight.last();
        dialog.refreshRightCombo(0,"nothing"); // чтобы наполнить комбобокс элементами
        dialog.pComboRight->setCurrentIndex(default2Index);
        dialog.pListRight = dialog.listsRight.first(); // возвращаем на место
        dialog.pComboRight = dialog.combosRight.first();
    }
    dialog.refreshLeftCombo(0,"nothing"); // чтобы наполнить комбобокс элементами
    dialog.comboLeft->setCurrentIndex(defaultIndex);

    //Достаём данные из диалога и формируем JSON-значение атрибута.

    if (dialog.exec())
    {
        Json::Value arrayValue;

        for (int i=0; i<dialog.listLeft->count(); i++)
        {
            Json::Value elemValue;
            Json::Value value;
            QListWidgetItem *item;
            QByteArray ba;

            item = dialog.listLeft->item(i);

            ba = item->text().toUtf8();
            value = ba.data();
            elemValue[FBJSONKEY_alias] = value;

            ba = item->data(Qt::UserRole).toString().toUtf8();
            value = ba.data();
            elemValue[FBJSONKEY_name] = value;

            if (dialog.comboLeft->currentIndex() != 0
                   && dialog.comboLeft->currentIndex() == i+1) // почему +1 см. ниже
            {
                elemValue[FBJSONKEY_default] = true;
            }

            Json::Value array2Value;
            for (int j=0; j<dialog.listsRight[i+1]->count(); j++) // почему +1 см. ниже
            {
                Json::Value elem2Value;
                item = dialog.listsRight[i+1]->item(j);
                ba = item->text().toUtf8();
                value = ba.data();
                elem2Value[FBJSONKEY_alias] = value;
                ba = item->data(Qt::UserRole).toString().toUtf8();
                value = ba.data();
                elem2Value[FBJSONKEY_name] = value;
                if (dialog.combosRight[i+1]->currentIndex() != 0
                      && dialog.combosRight[i+1]->currentIndex() == j+1) // почему +1 см. ниже
                {
                    elem2Value[FBJSONKEY_default] = true;
                }
                array2Value.append(elem2Value);
            }
            elemValue[FBJSONKEY_values] = array2Value;

            arrayValue.append(elemValue);
        }

        // Ищем атрибут "Значения" и присваеваем ему заданное значение.
        for (int i=0; i<FBElem::CURRENT->attributes.size(); i++)
        {
            if (FBElem::CURRENT->attributes[i].first == FBATTR_values)
            {
                FBElem::CURRENT->attributes[i].second = arrayValue;
                break;
            }
        }
    }
}


// ************************************************************************** //
// *************************** FBConnectButton ****************************** //
// ************************************************************************** //

FBConnectButton::FBConnectButton(QWidget *Parent,
                 QLineEdit* inUrl, QLineEdit* inLog,
                 QLineEdit* inPas, QTreeWidget *outTree,
                 QPushButton *cancelButton, QProgressBar *progBar, QLabel* statusLabel,
                 QPushButton *selectButton):
    QPushButton (Parent)
{
    _inUrl = inUrl;
    _inLog = inLog;
    _inPas = inPas;
    _outTree = outTree;
    _cancelButton = cancelButton;
    _progBar = progBar;
    _statusLabel = statusLabel;
    _selectButton = selectButton;

    connect(_outTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(HttpOnItemExpended(QTreeWidgetItem*)));
    connect(_outTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(HttpOnItemCollapsed(QTreeWidgetItem*)));
    connect(_outTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(HttpOnItemClicked(QTreeWidgetItem*,int)));

    _itemToExpand = NULL;

// TEST -------------------------------------
//    _testEdit = new QTextEdit(Parent);
//    QLayout *l = Parent->layout();
//    l->addWidget(_testEdit);
// ------------------------------------------
}


// Выполнить соединение с NGW.
void FBConnectButton::OnClicked ()
{
    _outTree->clear();
    _statusLabel->setText(QString::fromUtf8("Идёт соединение..."));
    _progBar->setValue(25);
    _cancelButton->setEnabled(true);

    // Обнуляем строку, т.к. дальше будет не единственное считывание, а несколько
    // считываний с конкатенацией (по мере прихода ответов с сервера).
    receivedJson = "";

    QNetworkRequest logPasRequest(QUrl(_inUrl->text()+"/login"));
    QByteArray barr = QString("login=%1&password=%2")
       .arg(_inLog->text()).arg(_inPas->text()).toUtf8();
    logPasRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                            //QVariant(QString("text/plain;charset=utf-8")));
                            QVariant(QString("application/x-www-form-urlencoded")));
                            // Обязательно поставить именно этот хедер!
    httpAuthReply = httpManager.post(logPasRequest,barr);
    connect(httpAuthReply, SIGNAL(finished()),
            this, SLOT(HttpAuthFinished()));
    connect(httpAuthReply, SIGNAL(readyRead()),
            this, SLOT(HttpReadyAuthRead()));
}


// Раскрыть элемент дерева для просмотра дочерних ресурсов данного ресурса.
void FBConnectButton::HttpOnItemExpended (QTreeWidgetItem *treeItem)
{
    // Очищаем весь элемент дерева, чтобы затем наполнить его заного.
    QList<QTreeWidgetItem*> childrenToDel = treeItem->takeChildren();
    for (int i=0; i<childrenToDel.size(); i++)
    {
        // Удаляем из словаря соотвествующий итем по ключу - его айдишнику.
        _itemTypes.erase(childrenToDel[i]->data(0,Qt::UserRole).toInt());
        delete childrenToDel[i];
    }

    _itemToExpand = treeItem;

    _statusLabel->setText(QString::fromUtf8("Идёт соединение..."));
    _progBar->setValue(50);
    _cancelButton->setEnabled(true);

    //_selectButton->setEnabled(false);

    // Обнуляем строку, т.к. дальше будет не единственное считывание, а несколько
    // считываний с конкатенацией (по мере прихода ответов с сервера).
    receivedJson = "";

    // Отправляем запрос на выдачу дочерних ресурсов для данного ресурса.
    QNetworkRequest request(QUrl(_inUrl->text()+"/resource/"
                         + treeItem->data(0,Qt::UserRole).toString() + "/child/"));
    httpResourceReply = httpManager.get(request);
    connect(httpResourceReply, SIGNAL(finished()),
            this, SLOT(HttpResourceFinished()));
    connect(httpResourceReply, SIGNAL(readyRead()),
            this, SLOT(HttpReadyResourceRead()));
}


void FBConnectButton::HttpOnItemCollapsed (QTreeWidgetItem *treeItem)
{
    _selectButton->setEnabled(false);
}


void FBConnectButton::HttpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn)
{
    int itemId = treeItem->data(0,Qt::UserRole).toInt();

    if (_itemTypes[itemId] == FB_NGW_ITEM_TYPE_VECTORLAYER
        || _itemTypes[itemId] == FB_NGW_ITEM_TYPE_POSTGISLAYER)
    {
        _selectButton->setEnabled(true);
    }
    else
    {
        _selectButton->setEnabled(false);
    }
}


void FBConnectButton::HttpReadyAuthRead()
{
    QByteArray barr;
    barr = httpAuthReply->readAll();
}


void FBConnectButton::HttpReadyRead()
{
    QByteArray barr;
    barr = httpReply->readAll();
    receivedJson += QString(barr).toStdString();
}


void FBConnectButton::HttpReadyResourceRead()
{
    QByteArray barr;
    barr = httpResourceReply->readAll();
    receivedJson += QString(barr).toStdString();
}


void FBConnectButton::HttpAuthFinished()
{
    if (httpAuthReply->error() == QNetworkReply::NoError)
    {
        httpAuthReply->deleteLater();

        _progBar->setValue(50);

        QNetworkRequest request(QUrl(_inUrl->text()+"/resource/0/child/"));
        //QNetworkRequest request(QUrl(_inUrl->text()+"/resource/store/"));
        httpReply = httpManager.get(request);
        connect(httpReply, SIGNAL(finished()),
                this, SLOT(HttpFinished()));
        connect(httpReply, SIGNAL(readyRead()),
                this, SLOT(HttpReadyRead()));
    }
    else
    {
        _statusLabel->setText(QString::fromUtf8("Ошибка соединения"));
        _progBar->setValue(0);
    }
}


void FBConnectButton::HttpFinished()
{
    QList<QTreeWidgetItem*> newItems = _ParseJsonReply(httpReply);
    if (!newItems.isEmpty())
    {
        _outTree->insertTopLevelItems(0,newItems);
        _statusLabel->setText(QString::fromUtf8("Соединение успешно"));
        _progBar->setValue(100);
    }
    httpReply->deleteLater();
    _cancelButton->setEnabled(false);
}


void FBConnectButton::HttpResourceFinished()
{
    QList<QTreeWidgetItem*> newItems = _ParseJsonReply(httpResourceReply);
    if (!newItems.isEmpty())
    {
        _itemToExpand->insertChildren(0,newItems);
        _statusLabel->setText(QString::fromUtf8("Соединение успешно"));
        _progBar->setValue(100);
    }
    _itemToExpand = NULL;
    httpResourceReply->deleteLater();
    _cancelButton->setEnabled(false);
}


QList<QTreeWidgetItem*> FBConnectButton::_ParseJsonReply(QNetworkReply *reply)
{
    QList<QTreeWidgetItem*> newItems;

    if (reply->error() == QNetworkReply::NoError)
    {
        if (!receivedJson.empty())
        {
            Json::Value jResources; // will contains the root value after parsing.
            Json::Reader jReader;

// TEST -------------------------------------
//            _testEdit->setText(QString::fromStdString(receivedJson));
// ------------------------------------------

            if (jReader.parse(receivedJson, jResources, false))
            {
                //  return a 'null' value if
                // there is no such member.
                for (int index = 0; index < jResources.size(); ++index)
                {
                    Json::Value jResource = jResources[index];
                    if (jResource.isNull())
                        continue;

                    Json::Value jRes = jResource["resource"];
                    if (jRes.isNull())
                        continue;

                    Json::Value jCls = jRes["cls"];
                    if (jCls.isNull())
                        continue;

                    if (jCls.asString() == "resource_group" ||
                            jCls.asString() == "postgis_layer" ||
                            jCls.asString() == "vector_layer")
                    {
                        Json::Value jId = jRes["id"];
                        if (jId.isNull())
                            continue;

                        QString sDispName;
                        Json::Value jDispName = jRes["display_name"];
                        if (jDispName.isNull())
                        {
                            sDispName = QString("<resource-no-name>");
                        }
                        else
                        {
                            sDispName = QString::fromUtf8(jDispName.asString().data());
                            // Делаем именно так, т.к. fromStdString использует ASCII
                            // преобразование.
                        }

                        int itemType = FB_NGW_ITEM_TYPE_UNDEFINED;
                        QString res_type = "";
                        if (jCls.asString() == "postgis_layer")
                        {
                            res_type = " [PostGIS]";
                            itemType = FB_NGW_ITEM_TYPE_POSTGISLAYER;
                        }
                        else if (jCls.asString() == "vector_layer")
                        {
                            res_type = " [Vector]";
                            itemType = FB_NGW_ITEM_TYPE_VECTORLAYER;
                        }
                        else if (jCls.asString() == "resource_group")
                        {
                            itemType = FB_NGW_ITEM_TYPE_RESOURCEGROUP;
                        }

                        QTreeWidgetItem *treeItem;
                        //treeItem = new QTreeWidgetItem(_outTree);
                        treeItem = new QTreeWidgetItem();
                        treeItem->setText(0,sDispName + res_type);
                        treeItem->setData(0,Qt::UserRole,jId.asInt());
                        newItems.append(treeItem);

                        // Добавляем в словарь айдишник и тип итема, чтобы потом
                        // можно была разрешить/запретить выбор этого итема в качестве
                        // источника данных при работе с NGW-диалогом.
                        _itemTypes.insert(std::make_pair(jId.asInt(),itemType));

                        Json::Value jChildren = jRes["children"];
                        if (!jChildren.isNull()
                                && jChildren.asString() == "true"
                                && jCls.asString() == "resource_group")
                        {
                            // Для того, чтобы в дальнейшем можно было раскрыть этот
                            // список.
                            treeItem->addChild(new QTreeWidgetItem(QStringList("to_delete")));
                        }
                    }
                }

                //_outTree->insertTopLevelItems(0, treeItems);
            }
            else
            {
                _statusLabel->setText(QString::fromUtf8("Ошибка соединения"));
                _progBar->setValue(0);
            }

        }
        else
        {
            _statusLabel->setText(QString::fromUtf8("Ошибка соединения"));
            _progBar->setValue(0);
        }

        //reply->deleteLater();
    }
    else
    {
        _statusLabel->setText(QString::fromUtf8("Ошибка соединения"));
        _progBar->setValue(0);
    }

    return newItems;
}


//void FBConnectButton::HttpCancel()
//{
//
//}

//void FBConnectButton::HttpSslErrors(QNetworkReply*,const QList<QSslError> &errors)
//{
//
//}


// ************************************************************************** //
// ************************ FBDoubleListEditDialog ************************** //
// ************************************************************************** //

FBListDialog::FBListDialog(QString listType, QWidget *parent)
{
    //this->setParent(parent);

    this->listType = listType;

// Левая часть:

    labelLeft = new QLabel(this);
    labelLeft->setText(QString::fromUtf8("Главный список:"));

    listLeft = new QListWidget(this);

    buttonAddLeft = new QPushButton(this);
    buttonAddLeft->setText(QString::fromUtf8("Добавить"));
    buttonRemoveLeft = new QPushButton(this);
    buttonRemoveLeft->setText(QString::fromUtf8("Удалить"));
    buttonRemoveLeft->setEnabled(false);
    QHBoxLayout *hll = new QHBoxLayout();
    hll->addWidget(buttonAddLeft);
    hll->addWidget(buttonRemoveLeft);

    groupLeft = new QGroupBox(this);
    groupLeft->setTitle(QString::fromUtf8("Редактирование:"));
    groupLeft->setEnabled(false);
    editNameLeft = new QLineEdit(groupLeft);
    labelNameLeft = new QLabel(groupLeft);
    labelNameLeft->setText(QString::fromUtf8("Название: "));
    QHBoxLayout *hlll1 = new QHBoxLayout();
    hlll1->addWidget(labelNameLeft);
    hlll1->addWidget(editNameLeft);
    editAliasLeft = new QLineEdit(groupLeft);
    labelAliasLeft = new QLabel(groupLeft);
    labelAliasLeft->setText(QString::fromUtf8("Отображаемое название: "));
    QHBoxLayout *hlll2 = new QHBoxLayout();
    hlll2->addWidget(labelAliasLeft);
    hlll2->addWidget(editAliasLeft);
    buttonModifyLeft = new QPushButton(groupLeft);
    buttonModifyLeft->setText(QString::fromUtf8("Изменить"));
    QVBoxLayout *vll = new QVBoxLayout(groupLeft);
    vll->addLayout(hlll1);
    vll->addLayout(hlll2);
    vll->addWidget(buttonModifyLeft);

    labelComboLeft = new QLabel(this);
    labelComboLeft->setText(QString::fromUtf8("По умолчанию:"));
    comboLeft = new QComboBox(this);
    comboLeft->addItem(QString::fromUtf8("<не задано>"));
    QHBoxLayout *hll2 = new QHBoxLayout();
    hll2->addWidget(labelComboLeft);
    hll2->addWidget(comboLeft);

    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(labelLeft);
    vl->addWidget(listLeft);
    vl->addLayout(hll);
    vl->addWidget(groupLeft);
    vl->addLayout(hll2);

// Правая часть:

    QVBoxLayout *vr;

    // Выводим её только, если диалог для двухуровневого списка.
    if (listType == FBELEMTYPE_double_combobox)
    {
        labelRight = new QLabel(this);
        labelRight->setText(QString::fromUtf8("Зависимый список:"));

        // Добавляем пустой список. После загрузки остальных списков (во внешнем
        // методе), этот обязательно остаётся для показа пустого списка, если
        // остальные будут в процессе удалены.
        listsLayoutRight = new QHBoxLayout();
        listsRight.append(new QListWidget(this));
        // Текущий список - тот что добавили.
        pListRight = listsRight[0];
        // Мы создали специальную раскладку, что бы в неё добавлять все виджеты
        // для списков второго уровня.
        listsLayoutRight->addWidget(pListRight);
        pListRight->setEnabled(false);

        buttonAddRight = new QPushButton(this);
        buttonAddRight->setText(QString::fromUtf8("Добавить"));
        buttonAddRight->setEnabled(false);
        buttonRemoveRight = new QPushButton(this);
        buttonRemoveRight->setText(QString::fromUtf8("Удалить"));
        buttonRemoveRight->setEnabled(false);
        QHBoxLayout *hrr = new QHBoxLayout();
        hrr->addWidget(buttonAddRight);
        hrr->addWidget(buttonRemoveRight);

        groupRight = new QGroupBox(this);
        groupRight->setTitle(QString::fromUtf8("Редактирование:"));
        groupRight->setEnabled(false);
        editNameRight = new QLineEdit(groupRight);
        labelNameRight = new QLabel(groupRight);
        labelNameRight->setText(QString::fromUtf8("Название: "));
        QHBoxLayout *hrrr1 = new QHBoxLayout();
        hrrr1->addWidget(labelNameRight);
        hrrr1->addWidget(editNameRight);
        editAliasRight = new QLineEdit(groupRight);
        labelAliasRight = new QLabel(groupRight);
        labelAliasRight->setText(QString::fromUtf8("Отображаемое название: "));
        QHBoxLayout *hrrr2 = new QHBoxLayout();
        hrrr2->addWidget(labelAliasRight);
        hrrr2->addWidget(editAliasRight);
        buttonModifyRight = new QPushButton(groupRight);
        buttonModifyRight->setText(QString::fromUtf8("Изменить"));
        QVBoxLayout *vrr = new QVBoxLayout(groupRight);
        vrr->addLayout(hrrr1);
        vrr->addLayout(hrrr2);
        vrr->addWidget(buttonModifyRight);

        labelComboRight = new QLabel(this);
        labelComboRight->setText(QString::fromUtf8("По умолчанию:"));
        // У нас будет несколько комбобоксов, по аналогии со списками.
        // Сейчас мы добавляем комбо-заглушку так же по аналогии: что бы
        // просто отображать что-то, пока никаких элементов ещё нет (или
        // все уже удалены).
        combosRight.append(new QComboBox(this));
        pComboRight = combosRight[0];
        pComboRight->setEnabled(false);
        combosLayoutRight = new QHBoxLayout();
        combosLayoutRight->addWidget(labelComboRight);
        combosLayoutRight->addWidget(pComboRight);

        vr = new QVBoxLayout();
        vr->addWidget(labelRight);
        vr->addLayout(listsLayoutRight);
        vr->addLayout(hrr);
        vr->addWidget(groupRight);
        vr->addLayout(combosLayoutRight);
    }

    // Если это одноуровневый список то переименовываем поля левой части.
    else if (listType == FBELEMTYPE_combobox)
    {
        labelLeft->setText(QString::fromUtf8("Элементы списка:"));
    }

    // Если это радиогруппа то переименовываем поля левой части.
    else if (listType == FBELEMTYPE_radio_group)
    {
        labelLeft->setText(QString::fromUtf8("Элементы группы:"));
    }

// Общая часть:

    QHBoxLayout *h = new QHBoxLayout();
    h->addLayout(vl);
    if (listType == FBELEMTYPE_double_combobox)
    {
        h->addSpacing(10);
        h->addLayout(vr);
    }

    buttonOk = new QPushButton(this);
    buttonOk->setText(QString::fromUtf8("ОК"));

    QVBoxLayout *v = new QVBoxLayout();
    v->addLayout(h);
    v->addWidget(buttonOk);
    setLayout(v);

    connect(buttonOk, SIGNAL(clicked()), this, SLOT(OnOkClicked()));

    connect(listLeft, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(OnLeftItemSelected(QListWidgetItem*)));
    connect(buttonAddLeft, SIGNAL(clicked()), this, SLOT(OnLeftAddClicked()));
    connect(buttonRemoveLeft, SIGNAL(clicked()), this, SLOT(OnLeftDeleteClicked()));
    connect(buttonModifyLeft, SIGNAL(clicked()), this, SLOT(OnLeftFixClicked()));

    if (listType == FBELEMTYPE_double_combobox)
    {
        setWindowTitle(QString::fromUtf8("Ввод элементов двухуровневого списка ..."));

        connect(pListRight, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(OnRightItemSelected(QListWidgetItem*)));
        connect(buttonAddRight, SIGNAL(clicked()), this, SLOT(OnRightAddClicked()));
        connect(buttonRemoveRight, SIGNAL(clicked()), this, SLOT(OnRightDeleteClicked()));
        connect(buttonModifyRight, SIGNAL(clicked()), this, SLOT(OnRightFixClicked()));
    }
    else if (listType == FBELEMTYPE_combobox)
    {
        setWindowTitle(QString::fromUtf8("Ввод элементов выпадающего списка ..."));
    }
    else if (listType == FBELEMTYPE_radio_group)
    {
        setWindowTitle(QString::fromUtf8("Ввод элементов радио-группы ..."));
    }
}


void FBListDialog::OnOkClicked()
{
    this->accept();
}

void FBListDialog::OnCancelClicked()
{
    this->reject();
}


void FBListDialog::OnLeftItemSelected(QListWidgetItem* item)
{
    editAliasLeft->setText(item->text());
    editNameLeft->setText(item->data(Qt::UserRole).toString());
    groupLeft->setEnabled(true);
    buttonRemoveLeft->setEnabled(true);

    // Выводим содержимое правого списка + комбокса для умолчания, если мы
    // редактируем двухуровневый.
    if (listType == FBELEMTYPE_double_combobox)
    {
        // +1, т.к. в массиве списков их всегда на один больше - нулевой всегда
        // дополнительный и пустой.
        pListRight->hide();
        pListRight = listsRight[listLeft->currentRow() + 1];
        pListRight->show();
         pComboRight->hide();
         pComboRight = combosRight[listLeft->currentRow() + 1];
         pComboRight->show();
        pListRight->setCurrentRow(-1);
        editAliasRight->setText("");
        editNameRight->setText("");
        refreshRightCombo(0,"modified"); // чтобы просто отобразить когда-то выбранное значение
        pListRight->setEnabled(true);
        buttonAddRight->setEnabled(true);
        pComboRight->setEnabled(true);
        buttonRemoveRight->setEnabled(false);
        groupRight->setEnabled(false);
        // Укорачиваем имя элемента, если оно слишком большое.
        QString level1name = item->text();
        if (level1name.size() > 15)
        {
            level1name.truncate(15);
            level1name += QString::fromUtf8("...");
        }
        labelRight->setText(QString::fromUtf8("Зависимый список для [")
                            + level1name + QString::fromUtf8("]:"));
    }
}


void FBListDialog::OnLeftAddClicked()
{
    if (listLeft->count() == FBMaxComboElemsCount)
        return;
    QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(FBLIST_new_alias));
    item->setData(Qt::UserRole,QString::fromUtf8(FBLIST_new_name));
    listLeft->addItem(item);
    refreshLeftCombo(0,"added"); // вместо 0 можно любое число, здесь это не играет роли
    listLeft->setCurrentRow(listLeft->count()-1);
    editAliasLeft->setText(QString::fromUtf8(FBLIST_new_alias));
    editNameLeft->setText(QString::fromUtf8(FBLIST_new_name));
    groupLeft->setEnabled(true);
    buttonRemoveLeft->setEnabled(true);

    // Добавляем новый пустой список и комбобокс для умолчаний в правую часть.
    if (listType == FBELEMTYPE_double_combobox)
    {
        pListRight->hide();
        listsRight.append(new QListWidget(this));
        pListRight = listsRight.last();
        connect(pListRight, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(OnRightItemSelected(QListWidgetItem*)));
        listsLayoutRight->addWidget(pListRight);
        pListRight->show();
         pComboRight->hide();
         combosRight.append(new QComboBox(this));
         pComboRight = combosRight.last();
         pComboRight->addItem(QString::fromUtf8("<не задано>"));
         combosLayoutRight->addWidget(pComboRight);
        pListRight->setCurrentRow(-1);
        editAliasRight->setText("");
        editNameRight->setText("");
        refreshRightCombo(0,"nothing"); // чтобы отобразить, что ещё ничего не выбрано в качестве умолчаний
        pListRight->setEnabled(true);
        buttonAddRight->setEnabled(true);
        pComboRight->setEnabled(true);
        buttonRemoveRight->setEnabled(false);
        groupRight->setEnabled(false);
        QString level1name = item->text();
        if (level1name.size() > 15)
        {
            level1name.truncate(15);
            level1name += QString::fromUtf8("...");
        }
        labelRight->setText(QString::fromUtf8("Зависимый список для [")
                            + level1name + QString::fromUtf8("]:"));
    }
}

void FBListDialog::OnLeftDeleteClicked()
{
    if (listLeft->currentRow() < 0)
        return;

    // Удаляем соответствующий список для элементов второго уровня из правой части.
    if (listType == FBELEMTYPE_double_combobox)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("Это так же удалит все соответствующие"
                                         " элементы уровня 2. Продолжить?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret != QMessageBox::Ok)
            return;

        // Показываем пустой список.
        pListRight->hide();
        pListRight = listsRight[0];
        pListRight->show();
        // Удаляем старый. Почему +1 см выше.
        QListWidget *widget = listsRight.takeAt(listLeft->currentRow() + 1);
        delete widget;
         pComboRight->hide();
         pComboRight = combosRight[0]; // по аналогии показываем пустой комбо для умолчаний
         pComboRight->show();
         QComboBox *combo = combosRight.takeAt(listLeft->currentRow() + 1);
         delete combo;
        pListRight->setCurrentRow(-1);
        editAliasRight->setText("");
        editNameRight->setText("");
        refreshRightCombo(0,"nothing"); // здесь не должно ничего показаться, т.к. текущий комбо - пустой
        pListRight->setEnabled(false);
        buttonAddRight->setEnabled(false);
        buttonRemoveRight->setEnabled(false);
        groupRight->setEnabled(false);
        pComboRight->setEnabled(false);
        labelRight->setText(QString::fromUtf8("Зависимый список:"));
    }

    int curRow = listLeft->currentRow();
    QListWidgetItem *item = listLeft->takeItem(curRow);
    delete item;
    listLeft->setCurrentRow(-1);
    editAliasLeft->setText("");
    editNameLeft->setText("");
    refreshLeftCombo(curRow,"deleted");
    buttonRemoveLeft->setEnabled(false);
    groupLeft->setEnabled(false);
}

void FBListDialog::OnLeftFixClicked()
{
    if (listLeft->currentRow() < 0)
        return;
    QListWidgetItem *item = listLeft->currentItem();
    item->setText(editAliasLeft->text());
    item->setData(Qt::UserRole,editNameLeft->text());
    refreshLeftCombo(0,"modified"); // вместо 0 можно любое число, здесь это не играет роли

    if (listType == FBELEMTYPE_double_combobox)
    {
        QString level1name = item->text();
        if (level1name.size() > 15)
        {
            level1name.truncate(15);
            level1name += QString::fromUtf8("...");
        }
        labelRight->setText(QString::fromUtf8("Зависимый список для [")
                            + level1name + QString::fromUtf8("]:"));
    }
}


void FBListDialog::refreshLeftCombo(int elemIndex, QString actionWithElem)
{
    // Определяем изменится ли элемент по-умолчанию и как.
    int indexToSet;

    // Т.к. элементов в списке всегда на 1 меньше чем элементов в комбобоксе
    // (там есть элемент "не задано") надо инкрементировать индекс элемента,
    // с которым было произведено дейсвтие. На этом этапе ВСЕ индексы в списке
    // соответсвуют индексам в комбобоксе.
    elemIndex++;

    // Если уже стояло "не задано", то ничего измениться не может.
    if (comboLeft->currentIndex() == 0)
    {
        indexToSet = 0;
    }

    // Добавление происходит всегда в конец и индекс не может измениться.
    else if (actionWithElem == "added")
    {
        indexToSet = comboLeft->currentIndex();
    }

    // Если удалили элемент, расположенный в списке до элемента по умолчанию,
    // то инжекс надо декрементировать. Если удалили именно элемент по умолчанию
    // то индекс надо поставить на "не определено".
    else if (actionWithElem == "deleted")
    {
        if (elemIndex == comboLeft->currentIndex())
        {
            indexToSet = 0;
        }
        else if (elemIndex < comboLeft->currentIndex())
        {
            indexToSet = comboLeft->currentIndex() - 1;
        }
        else
        {
            indexToSet = comboLeft->currentIndex();
        }
    }

    // При простом изменении ничего не происходит.
    else if (actionWithElem == "modified")
    {
        indexToSet = comboLeft->currentIndex();
    }

    // Если не так заданы параметры.
    else
    {
        indexToSet = 0;
    }

    comboLeft->clear();
    comboLeft->addItem(QString::fromUtf8("<не задано>"));
    for (int i=0; i<listLeft->count(); i++)
    {
        comboLeft->addItem(listLeft->item(i)->text());
    }

    comboLeft->setCurrentIndex(indexToSet);
}





void FBListDialog::OnRightItemSelected(QListWidgetItem* item)
{
    editAliasRight->setText(item->text());
    editNameRight->setText(item->data(Qt::UserRole).toString());
    buttonRemoveRight->setEnabled(true);
    groupRight->setEnabled(true);
}

void FBListDialog::OnRightAddClicked()
{
    if (pListRight->count() == FBMaxComboElemsCount)
        return;
    QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(FBLIST_new_alias));
    item->setData(Qt::UserRole,QString::fromUtf8(FBLIST_new_name));
    pListRight->addItem(item);
    refreshRightCombo(0,"added"); // 0 тут не важен, см. аналогичное для левого комбо
    pListRight->setCurrentRow(pListRight->count()-1);
    editAliasRight->setText(QString::fromUtf8(FBLIST_new_alias));
    editNameRight->setText(QString::fromUtf8(FBLIST_new_name));
    buttonRemoveRight->setEnabled(true);
    groupRight->setEnabled(true);
}

void FBListDialog::OnRightDeleteClicked()
{
    if (pListRight->currentRow() < 0)
        return;
    int curRow = pListRight->currentRow();
    QListWidgetItem *item = pListRight->takeItem(curRow);
    delete item;
    refreshRightCombo(curRow,"deleted");
    pListRight->setCurrentRow(-1);
    editAliasRight->setText("");
    editNameRight->setText("");
    buttonRemoveRight->setEnabled(false);
    groupRight->setEnabled(false);
}

void FBListDialog::OnRightFixClicked()
{
    if (pListRight->currentRow() < 0)
        return;
    QListWidgetItem *item = pListRight->currentItem();
    item->setText(editAliasRight->text());
    item->setData(Qt::UserRole,editNameRight->text());
    refreshRightCombo(0,"modified"); // 0 тут не важен, см. аналогичное для левого комбо
}


void FBListDialog::refreshRightCombo(int elemIndex, QString actionWithElem)
{
    // Все комментарии см. аналогичный метод для левого комбобокса.
    int indexToSet;

    elemIndex++;

    if (pComboRight->currentIndex() == 0)
    {
        indexToSet = 0;
    }

    else if (actionWithElem == "added")
    {
        indexToSet = pComboRight->currentIndex();
    }

    else if (actionWithElem == "deleted")
    {
        if (elemIndex == pComboRight->currentIndex())
        {
            indexToSet = 0;
        }
        else if (elemIndex < pComboRight->currentIndex())
        {
            indexToSet = pComboRight->currentIndex() - 1;
        }
        else
        {
            indexToSet = pComboRight->currentIndex();
        }
    }

    else if (actionWithElem == "modified")
    {
        indexToSet = pComboRight->currentIndex();
    }

    else
    {
        indexToSet = 0;
    }

    pComboRight->clear();
    pComboRight->addItem(QString::fromUtf8("<не задано>"));
    for (int i=0; i<pListRight->count(); i++)
    {
        pComboRight->addItem(pListRight->item(i)->text());
    }

    pComboRight->setCurrentIndex(indexToSet);
}
