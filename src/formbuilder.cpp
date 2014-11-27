#include "formbuilder.h"
#include "ui_formbuilder.h"

#include "json/json.h"


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

    //FBParentLabel::currentAction = FBActionMoving;

    emit elemPressed();
}

/*
void FBElem::mouseMoveEvent (QMouseEvent *event)
{
    // Событие должно передаваться родителю - родительскому лейблу.
    event->ignore();
}

void FBElem::mouseReleaseEvent (QMouseEvent *event)
{
    // Событие должно передаваться родителю - родительскому лейблу.
    event->ignore();
}
*/

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

    //newElem->caption = this->caption;
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
                           QString::number(this->type) +
                           QString("_") +
                           QString::number(FBElem::GetNextId()));

    // Формируем внешний вид элемента интерфейса. Это необходимо делать после
    // задания имени объекта, т.к. стиль будет задаваться по имени.
    newElem->deselect(); // Задаём стиль контура - элемент не выделен.

    // Копируем названия и поля свойств объекта.
    for (int i=0; i<vParams.size(); i++)
    {
        newElem->vParamValues.append(QPair<QPair<QString,QString>,QString>
                                     (vParams[i],""));
    }

    // TODO: проверки, и если не прошло - delete newElem, return NULL

    return newElem;
}


void FBElemType::mousePressEvent (QMouseEvent *event)
{
    CURRENT = this;

    //FBParentLabel::currentAction = FBActionCreating;

    emit elemTypePressed();

    //grabMouse();
    //releaseMouse();
}


/*
void FBElemType::mouseMoveEvent (QMouseEvent *event)
{
    //releaseMouse();
    event->ignore();
}
*/

// ************************************************************************** //
// ******************************* FBParentLabel **************************** //
// ************************************************************************** //

//FBParentLabelAction FBParentLabel::currentAction = FBActionNone;


FBParentLabel::FBParentLabel(QScrollArea *parent): QLabel (parent)
{
    QVBoxLayout *parentLayout = new QVBoxLayout(this);
    parentLayout->addStretch();

    /*
    selector = new QLabel(this);
    selector->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    //selector->resize(500,4);
    selector->setMaximumSize(200,4);
    selector->setMinimumSize(200,4);
    selector->setAutoFillBackground(false);
    selector->setStyleSheet("QLabel { border: 2px solid red}");

    parentLayout->insertWidget(0,selector);
    selector->hide();

    posToInsertWidget = 0;

    setMouseTracking(true);
    */
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

    // Виджет был изъят из массива. Добавляем обратно, но уже на новую позицию.
    //elements.insert(posToInsertWidget,QPair<FBElem*,int>(FBElem::CURRENT,
                                                //FBElem::CURRENT->height()/2));
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


/*
void FBParentLabel::mouseMoveEvent (QMouseEvent *event)
{
    //if (currentAction != FBActionNone)
    if (currentAction == FBActionMoving)
    {
        //if (currentAction == FBActionMoving)
        //{
            FBElem::CURRENT->hide();
        //}

        int curPos = -1;
        if (elements.isEmpty())
        {
            curPos = 0;
        }
        else
        {
            for (int i=1; i<elements.size(); i++)
            {
                if (event->pos().y() > (elements[i-1].first->y() + elements[i-1].second)
                && event->pos().y() <= (elements[i].first->y() + elements[i].second))
                {
                    curPos = i;
                    break;
                }
            }
            if (curPos == -1)
            {
                if (event->pos().y() > (elements[elements.size()-1].first->y()
                                        + elements[elements.size()-1].second))
                    curPos = elements.size();
                else
                    curPos = 0;
            }
        }

        // Меняем позицию только если было изменение в процессе движения курсора.
        //if (curPos != posToInsertWidget)
        //{
            // Отображаем в текущей позиции некий селектор, чтобы показать, куда можно
            // переместить элемент.
            posToInsertWidget = curPos;
            QVBoxLayout *parentLayout = (QVBoxLayout*)this->layout();
            parentLayout->removeWidget(selector);
            parentLayout->insertWidget(posToInsertWidget,selector);
        //}

        // В любом случае отображаем селектор.
        selector->show();
    }
}
*/


/*
void FBParentLabel::mouseReleaseEvent (QMouseEvent *event)
{
    selector->hide();

    if (currentAction == FBActionCreating)
    {
        if (elements.size() == FBMaxElemInPageCount)
        {
            // TODO: сигнал-слот с сообщением.
        }
        else
        {
            FBElem *newElem = FBElemType::CURRENT->CreateElem();
            connect(newElem, SIGNAL(elemPressed()), this, SLOT(OnElemPressed()));
            //InsertElem(newElem);
            InsertElem(newElem,0);

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
            QVBoxLayout *layout = (QVBoxLayout*)this->layout();
            int totalChildrenHeight =
                    layout->contentsMargins().top()
                    + layout->contentsMargins().bottom();
            for (int k=0; k < elements.size(); k++)
            {
                // Проверяем, является ли ребёнок - виджетом, т.к. родительский
                // лейбл будет иметь по крайней мере один такой объект - раскладку,
                // не являющуюся виджетом.
                totalChildrenHeight += elements[k].first->height() + layout->spacing();
            }
            if (totalChildrenHeight > startHeight)
            {
                resize(width(),totalChildrenHeight);
            }
        }
    }

    else if (currentAction == FBActionMoving)
    {
        QVBoxLayout *parentLayout = (QVBoxLayout*)this->layout();
        parentLayout->removeWidget(FBElem::CURRENT);
        parentLayout->insertWidget(posToInsertWidget,FBElem::CURRENT);

        // Виджет был изъят из массива. Добавляем обратно, но уже на новую позицию.
        elements.insert(posToInsertWidget,QPair<FBElem*,int>(FBElem::CURRENT,
                                                FBElem::CURRENT->height()/2));

        FBElem::CURRENT->show();
    }

    currentAction = FBActionNone;
    FBElemType::CURRENT = NULL;
}
*/

/*
void FBParentLabel::OnElemPressed ()
{

    // Запоминаем изначальную позицию элемента, чтобы вернуть его, если
    // курсор мыши вышел за пределы родительского лейбла (или вообще
    // что-то пошло не так).
    for (int i=0; i<elements.size(); i++)
    {
        if (elements[i].first == FBElem::CURRENT)
        {
            posToInsertWidget = i;

            // Изымаем виджет из массива, т.к. при перемещении он будет спрятан.
            // Ищем старую позицию виджета в сортированном массиве и удаляем его.
            // Обратное добавление произойдёт когда мышь отпустится.
            elements.removeAt(i);

            break;
        }
    }
}


void FBParentLabel::OnElemTypePressed ()
{
    // Изначальная позиция для добавления нового элемента - 0.
    posToInsertWidget = 0;
}
*/


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
    if (poDS != NULL)
        GDALClose(poDS);

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
    menuFile = new QMenu(QString::fromUtf8("Файл"),menuBar);
    menuBar->addMenu(menuFile);
      menuNew = new QMenu(QString::fromUtf8("Новый"),menuBar);
      menuFile->addMenu(menuNew);
        actionNewVoid = new QAction(QString::fromUtf8("Пустая форма"),menuBar);
        menuNew->addAction(actionNewVoid);
        actionNewDataset = new QAction(QString::fromUtf8("Источник данных"),menuBar);
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
    ui->tableWidget->clear();
    ui->tableWidget->horizontalHeader()->hide();
    //ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("test"));
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //ui->tableWidget->verticalHeader()->setCascadingSectionResizes(true);
    //ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    //ui->tableWidget->horizontalHeader()->setMinimumWidth(minwidth);
    ui->toolButton->setIcon(QIcon(":/img/up"));
    ui->toolButton_2->setIcon(QIcon(":/img/down"));
    ui->toolButton_3->setIcon(QIcon(":/img/delete"));

    // Настройки для диалогов.
    lastSavePath = QDir::currentPath();

    // Настройки GDAL.
    GDALAllRegister();
    poDS = NULL;
}


//-----------------------------------------------------------------------//
//   Создание типов элементов и их лейблов в левой панели инструментов   //
//-----------------------------------------------------------------------//
void FormBuilder::CreateElemTypes ()
{
    FBElemType* elemType;

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Статический текст");
    elemType->name = QString::fromUtf8("text_label");
    elemType->alias_ru = QString::fromUtf8("Статический текст");
    elemType->alias_eng = QString::fromUtf8("Text label");
    elemType->type = FBText;
    elemType->imgPaths.append(":/img/text");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_caption,
                               QString::fromUtf8("Текст")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Текстовое поле");
    elemType->name = QString::fromUtf8("text_edit");
    elemType->alias_ru = QString::fromUtf8("Текстовое поле");
    elemType->alias_eng = QString::fromUtf8("Text edit");
    elemType->type = FBTextedit;
    elemType->imgPaths.append(":/img/textfield");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                             QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_default,
                             QString::fromUtf8("Начальный текст")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_only_figures,
                             QString::fromUtf8("Только цифры? [ДА/НЕТ]")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_max_char_count,
                             QString::fromUtf8("Максимальное кол-во символов")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Кнопка");
    elemType->name = QString::fromUtf8("button");
    elemType->alias_ru = QString::fromUtf8("Кнопка");
    elemType->alias_eng = QString::fromUtf8("Button");
    elemType->type = FBButton;
    elemType->imgPaths.append(":/img/button");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_caption,
                             QString::fromUtf8("Заголовок")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Выпадающий список");
    elemType->name = QString::fromUtf8("combobox");
    elemType->alias_ru = QString::fromUtf8("Выпадающий список");
    elemType->alias_eng = QString::fromUtf8("Combobox");
    elemType->type = FBCombobox;
    elemType->imgPaths.append(":/img/combobox");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                             QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_values,
                             QString::fromUtf8("Значения через \';\'")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_default,
                             QString::fromUtf8("Значение по умолчанию")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Флажок");
    elemType->name = QString::fromUtf8("checkbox");
    elemType->alias_ru = QString::fromUtf8("Флажок");
    elemType->alias_eng = QString::fromUtf8("Checkbox");
    elemType->type = FBCheckbox;
    elemType->imgPaths.append(":/img/checkbox");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                                 QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_caption,
                                 QString::fromUtf8("Заголовок")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Радио-группа");
    elemType->name = QString::fromUtf8("radio_group");
    elemType->alias_ru = QString::fromUtf8("Радио-группа");
    elemType->alias_eng = QString::fromUtf8("Radio-group");
    elemType->type = FBRadiogroup;
    elemType->imgPaths.append(":/img/radiobutton");
    elemType->imgPaths.append(":/img/radiobutton_void");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                               QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_values,
                             QString::fromUtf8("Значения через \';\'")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_default,
                             QString::fromUtf8("Значение по умолчанию")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Компас");
    elemType->name = QString::fromUtf8("compass");
    elemType->alias_ru = QString::fromUtf8("Компас");
    elemType->alias_eng = QString::fromUtf8("Compass");
    elemType->type = FBCompass;
    elemType->imgPaths.append(":/img/compass");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                               QString::fromUtf8("Целевое поле")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Дата-Время");
    elemType->name = QString::fromUtf8("date_time");
    elemType->alias_ru = QString::fromUtf8("Дата-Время");
    elemType->alias_eng = QString::fromUtf8("Date-time");
    elemType->type = FBDateTime;
    elemType->imgPaths.append(":/img/date_date");
    elemType->imgPaths.append(":/img/date_time");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                                 QString::fromUtf8("Целевое поле")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Двухуровневый список");
    elemType->name = QString::fromUtf8("double_combobox");
    elemType->alias_ru = QString::fromUtf8("Двухуровневый список");
    elemType->alias_eng = QString::fromUtf8("Double combobox");
    elemType->type = FBDoubleList;
    elemType->imgPaths.append(":/img/doublelist_1");
    elemType->imgPaths.append(":/img/doublelist_2");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field,
                               QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_values_sets,
                             QString::fromUtf8("Значения через \':\' и \';\'")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_default,
                             QString::fromUtf8("Значение по умолчанию для списка 1")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_default,
                             QString::fromUtf8("Значения по умолчанию для списков 2")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Фото");
    elemType->name = QString::fromUtf8("photo");
    elemType->alias_ru = QString::fromUtf8("Фото");
    elemType->alias_eng = QString::fromUtf8("Photo");
    elemType->type = FBPhoto;
    elemType->imgPaths.append(":/img/photo_targ");
    elemType->imgPaths.append(":/img/photo_but");
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field_name,
                               QString::fromUtf8("Целевое поле названия снимка")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field_azimuth,
                               QString::fromUtf8("Целевое поле азимута снимка")));
    elemType->vParams.append(QPair<QString,QString>(FBPARAM_field_datetime,
                               QString::fromUtf8("Целевое поле даты снимка")));
    vElemTypes.append(elemType);

    elemType = new FBElemType(ui->groupBox);
    //elemType->caption = QString::fromUtf8("Пробел");
    elemType->name = QString::fromUtf8("space");
    elemType->alias_ru = QString::fromUtf8("Пробел");
    elemType->alias_eng = QString::fromUtf8("Space");
    elemType->type = FBSpace;
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
        vElemTypes[i]->setToolTip(vElemTypes[i]->alias_ru);
        connect(vElemTypes[i], SIGNAL(elemTypePressed()),
                this, SLOT(OnElemTypePressed()));
    }

    QVBoxLayout *twoButtonLayout = new QVBoxLayout();

    pElemTypeGroupStart = new FBElemType(ui->groupBox_5);
    //ui->horizontalLayout_5->addWidget(pElemTypeGroupStart);
    //pElemTypeGroupStart->caption = QString::fromUtf8("Начало группы");
    pElemTypeGroupStart->name = QString::fromUtf8("group_start");
    pElemTypeGroupStart->alias_ru = QString::fromUtf8("Начало группы");
    pElemTypeGroupStart->alias_eng = QString::fromUtf8("Group start");
    pElemTypeGroupStart->type = FBGroupEnd;
    pElemTypeGroupStart->imgPaths.append(":/img/void");
    pElemTypeGroupStart->imgPaths.append(":/img/group_start");
    pElemTypeGroupStart->vParams.append(QPair<QString,QString>(FBPARAM_caption,
                               QString::fromUtf8("Заголовок")));
    pElemTypeGroupStart->setToolTip(QString::fromUtf8("Поместите в начало будущей группы"));
    pElemTypeGroupStart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    QIcon icon1(pElemTypeGroupStart->imgPaths[1]);
    pElemTypeGroupStart->setIcon(icon1);
    pElemTypeGroupStart->setIconSize(QSize(80,31));
    connect(pElemTypeGroupStart, SIGNAL(elemTypePressed()),
            this, SLOT(OnElemTypePressed()));

    pElemTypeGroupEnd = new FBElemType(ui->groupBox_5);
    //ui->horizontalLayout_5->addWidget(pElemTypeGroupEnd);
    //pElemTypeGroupEnd->caption = QString::fromUtf8("Конец группы");
    pElemTypeGroupEnd->name = QString::fromUtf8("group_end");
    pElemTypeGroupEnd->alias_ru = QString::fromUtf8("Конец группы");
    pElemTypeGroupEnd->alias_eng = QString::fromUtf8("Group end");
    pElemTypeGroupEnd->type = FBGroupEnd;
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
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
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


/*
// Отжатие мышки в любом месте экрана.
// Нужен для отлова нажатий во вне всех важных виджетов - если пользователь
// недовёл указатель мыши до нужного места, когда пытался создать / переместить
// элемент интерфейса.
void FormBuilder::mouseReleaseEvent (QMouseEvent *event)
{
    // Отменяем все предыдущие нажатия.
    FBElemType::CURRENT = NULL;
    FBParentLabel::currentAction = FBActionNone;
}
*/

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

    //correspondence[widget].second->mouseReleaseEvent(NULL);

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

    ui->tableWidget->clearContents(); // TODO: Это удаляет виджеты-комбобоксы?
    ui->tableWidget->setRowCount(0); // TODO: Правильно таким образом очищать список строк?
    //ui->tableWidget->horizontalHeader()->show();
    //ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("7777"));
    for (int i=0; i<FBElem::CURRENT->vParamValues.size(); i++)
    {
        int r = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(r + 1);
        ui->tableWidget->setVerticalHeaderItem(i,
          new QTableWidgetItem(FBElem::CURRENT->vParamValues[i].first.second));
        // Если есть поля из каких выбирать - выводим.
        if (!availableFields.isEmpty()
                && (FBElem::CURRENT->vParamValues[i].first.first == FBPARAM_field
                    || FBElem::CURRENT->vParamValues[i].first.first == FBPARAM_field_name
                    || FBElem::CURRENT->vParamValues[i].first.first == FBPARAM_field_datetime
                    || FBElem::CURRENT->vParamValues[i].first.first == FBPARAM_field_azimuth
                    ))
        {
            // Создаём комбобокс.
            QComboBox *combo = new QComboBox(ui->tableWidget);
            combo->setEditable(true);
            // Заполняем комбобокс.
            int index = -1;
            for (int j=0; j<availableFields.size(); j++)
            {
                if (availableFields[j] == FBElem::CURRENT->vParamValues[i].second)
                {
                    index = j;
                }
                combo->addItem(availableFields[j]);
            }
            ui->tableWidget->setCellWidget(i,0,combo);

            // Ищем старое сохранённое значение.
            combo->setCurrentIndex(index);
            if (index == -1)
            {
                combo->setEditText(FBElem::CURRENT->vParamValues[i].second);
            }
        }
        else
        {
            ui->tableWidget->setItem(i,0,
              new QTableWidgetItem(FBElem::CURRENT->vParamValues[i].second));
        }
    }
    ui->groupBox_4->setTitle(QString::fromUtf8("Редактирование элемента: ") +
                         FBElem::CURRENT->elemType->alias_ru);
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
void FormBuilder::on_pushButton_5_clicked()
{
    if (FBElem::CURRENT == NULL)
        return;

    for (int i=0; i<FBElem::CURRENT->vParamValues.size(); i++)
    {
        // Мы должны понять что находится в ячейке и как достать оттуда текст.
        // Используем item(i,0). 0 - т.к. самая левая колонка - это не ячейка
        // а заголовок.
        QWidget *widget = ui->tableWidget->cellWidget(i,0);
        if (widget == NULL) // Определяем комбобокс или нет.
        {
            FBElem::CURRENT->vParamValues[i].second
                    = ui->tableWidget->item(i,0)->text();
        }
        else
        {
            // TODO: определять что за ячейка:
            //QMetaObject *objInfo = widget->metaObject();
            //if ()
            FBElem::CURRENT->vParamValues[i].second
                    = static_cast<QComboBox*>(widget)->currentText();
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


// Новый пустой.
void FormBuilder::OnActionNewVoid ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Это сотрёт текущую форму. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    if (poDS != NULL)
    {
        GDALClose(poDS);
    }
    availableFields.clear();

    ui->label_2->setText(QString::fromUtf8("Информация об источнике данных: источник данных не выбран"));

    // Стираем и удаляем всё старое - создаём новый проект.
    ClearAll();
}


// Новый с ИД.
void FormBuilder::OnActionNewDataset ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Это сотрёт текущую форму. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    QFileDialog dialogNewDataset (this);// = new QFileDialog (this);
    dialogNewDataset.setAcceptMode(QFileDialog::AcceptOpen);
    // Выбрать можно только один существующий файл:
    dialogNewDataset.setFileMode(QFileDialog::ExistingFile);
    dialogNewDataset.setViewMode(QFileDialog::List);
    //dialogNewDataset->setDefaultSuffix("shp");
    //dialogNewDataset->setNameFilter("*.shp");
    dialogNewDataset.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogNewDataset.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogNewDataset.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogNewDataset.setLabelText(QFileDialog::Accept,QString::fromUtf8("Выбрать"));
    dialogNewDataset.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogNewDataset.setWindowTitle(QString::fromUtf8("Новая форма: выберите источник данных ..."));

    dialogNewDataset.setDirectory(QDir()); //ставим текущую директорию
    //dialogNewDataset->selectFile("form.xml"); //имя файла по умолчанию
    if (dialogNewDataset.exec())
    {
        QStringList path = dialogNewDataset.selectedFiles();

        if (poDS != NULL)
        {
            GDALClose(poDS);
        }
        availableFields.clear();

        QByteArray arr = path[0].toUtf8(); // всегда будет выбран только один файл
        poDS = (GDALDataset*) GDALOpenEx(arr.data(), GDAL_OF_VECTOR, NULL, NULL, NULL);
        if(poDS == NULL)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка! Невозможно открыть выбранный источник данных."));
            return;
        }

        // Проверяем кол-во слоёв.
        if (poDS->GetLayerCount() == 0 || poDS->GetLayerCount() > 1)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка: источник данных ")
                           + QString::fromUtf8(arr.data())
                           + QString::fromUtf8(" содержит 0 или больше чем 1 слой"));
            return;
        }

        OGRLayer *poLayer =  poDS->GetLayer(0);
        if (poLayer == NULL)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка: невозможно считать слой в источнике данных ")
                           + QString::fromUtf8(arr.data()));
            return;
        }

        // Проверяем правильность системы координат.
        OGRSpatialReference SpaRef1(SRS_WKT_WGS84);
        OGRSpatialReference SpaRef2;
        SpaRef2.SetFromUserInput("EPSG:3857");
        if ((poLayer->GetSpatialRef()->IsSame(&SpaRef1) == FALSE)
                && (poLayer->GetSpatialRef()->IsSame(&SpaRef2) == FALSE))
        {
            ShowMsgBox(QString::fromUtf8("Ошибка: слой источник данных ")
                           + QString::fromUtf8(arr.data())
                           + QString::fromUtf8(" имеет неподдерживаемую систему координат (поддерживаются только EPSG:4326 и EPSG:3857)"));
            return;
        }

        // Считываем набор доступных целевых полей.
        OGRFeatureDefn *poLayerDefn = poLayer->GetLayerDefn();
        for(int i=0; i<poLayerDefn->GetFieldCount(); i++)
        {
            OGRFieldDefn *poFieldDefn = poLayerDefn->GetFieldDefn(i);
            availableFields.append(QString(poFieldDefn->GetNameRef()));
        }

        ui->label_2->setText(QString::fromUtf8("Информация об источнике данных: используется ")
                           + arr.data());

        // Стираем и удаляем всё старое - создаём новый проект.
        ClearAll();
    }
}


// Новый с NGW.
void FormBuilder::OnActionNewNGW ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Это сотрёт текущую форму. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    QDialog dialogNgw(this);
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialogNgw);
    dialogNgw.setWindowTitle(QString::fromUtf8("Установите доступ к NextGIS Web"));

    QLabel* label;
    QHBoxLayout *hLayout;

    hLayout = new QHBoxLayout();
    label = new QLabel(&dialogNgw);
    label->setText(QString::fromUtf8("URL: "));
    hLayout->addWidget(label);
    QLineEdit* lineEdit1 = new QLineEdit(&dialogNgw);
    lineEdit1->setText(QString::fromUtf8(FB_DEFAULT_NGW_URL));
    hLayout->addWidget(lineEdit1);
    dialogLayout->addLayout(hLayout);

     hLayout = new QHBoxLayout();
     label = new QLabel(&dialogNgw);
     label->setText(QString::fromUtf8("Логин: "));
     hLayout->addWidget(label);
     QLineEdit* lineEdit2 = new QLineEdit(&dialogNgw);
     lineEdit2->setText(QString::fromUtf8(FB_DEFAULT_NGW_LOGIN));
     hLayout->addWidget(lineEdit2);
     dialogLayout->addLayout(hLayout);

      hLayout = new QHBoxLayout();
      label = new QLabel(&dialogNgw);
      label->setText(QString::fromUtf8("Пароль: "));
      hLayout->addWidget(label);
      QLineEdit* lineEdit3 = new QLineEdit(&dialogNgw);
      lineEdit3->setText(QString::fromUtf8(FB_DEFAULT_NGW_PASSWORD));
      hLayout->addWidget(lineEdit3);
      dialogLayout->addLayout(hLayout);

    QHBoxLayout *cancelLayout = new QHBoxLayout();
    QLabel *statusLabel = new QLabel(&dialogNgw);
    statusLabel->setText("                                        ");
    QProgressBar *progBar = new QProgressBar(&dialogNgw);
    progBar->setValue(0);
    progBar->setMaximum(100);
    progBar->setTextVisible(false);
    QPushButton *button3 = new QPushButton(&dialogNgw);
    button3->setText(QString::fromUtf8("Отменить"));
    button3->setEnabled(false);
    cancelLayout->addWidget(statusLabel);
    cancelLayout->addWidget(progBar);
    cancelLayout->addWidget(button3);

    QTreeWidget *treeWidget = new QTreeWidget(&dialogNgw);
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderLabel(QString::fromUtf8("Доступные ресурсы:"));
    QPushButton *button2 = new QPushButton(&dialogNgw);
    button2->setText(QString::fromUtf8("Выбрать"));
    connect(button2, SIGNAL(clicked()), &dialogNgw, SLOT(accept()));
    button2->setEnabled(false);
    FBConnectButton *button1 = new FBConnectButton(&dialogNgw,lineEdit1,lineEdit2,
                                                   lineEdit3,treeWidget,button3,
                                                   progBar,statusLabel,button2);
    button1->setText(QString::fromUtf8("Соединить"));
    connect(button1, SIGNAL(clicked()), button1, SLOT(OnClicked()));

    dialogLayout->addWidget(button1);
    dialogLayout->addLayout(cancelLayout);
    dialogLayout->addWidget(treeWidget);
    dialogLayout->addWidget(button2);

    if (dialogNgw.exec())
    {
        // Диалог завершился (по нажатию кнопки "Выбрать"), но не удалился,
        // поэтому мы можем считать данные из его полей.
/*
        QString logPas = lineEdit2->text() + ":" + lineEdit3->text();
        QByteArray logPasBa = logPas.toUtf8();

        CPLSetConfigOption("GDAL_HTTP_USERPWD", logPasBa.data());

        QString finalUrl = lineEdit1->text() + "/resource/"
                + treeWidget->currentItem()->data(0,Qt::UserRole).toString()
                + "/geojson/";
        QByteArray finalUrlBa = finalUrl.toUtf8();

        poDS = (GDALDataset*) GDALOpenEx(finalUrlBa.data(), GDAL_OF_VECTOR, NULL, NULL, NULL);
        if(poDS == NULL)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка! Невозможно открыть выбранный источник данных."));
            return;
        }

        _openGdalDataset(); */

    }
}


void FormBuilder::_openGdalDataset (char *datasetName)
{

}


// Открыть.
void FormBuilder::OnActionOpen ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Это сотрёт текущую форму. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    // Настраиваем диалог открытия проекта.
    QFileDialog dialogOpen(this);// = new QFileDialog (this);
    dialogOpen.setAcceptMode(QFileDialog::AcceptOpen);
    dialogOpen.setFileMode(QFileDialog::ExistingFile);
    dialogOpen.setViewMode(QFileDialog::List);
    dialogOpen.setDefaultSuffix("xml");
    dialogOpen.setNameFilter("*.xml");
    dialogOpen.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogOpen.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogOpen.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogOpen.setLabelText(QFileDialog::Accept,QString::fromUtf8("Открыть"));
    dialogOpen.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogOpen.setWindowTitle(QString::fromUtf8("Открыть форму ..."));

    QString path;// = "form.xml";
    dialogOpen.setDirectory(lastSavePath);
    if (dialogOpen.exec())
    {
        path = dialogOpen.selectedFiles().first();
        if (!path.endsWith(".xml",Qt::CaseInsensitive))
        {
            ShowMsgBox(QString::fromUtf8("Ошибка! Выбран неверный XML файл"));
            return;
        }

        QFile file (path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось открыть выбранный XML файл"));
            return;
        }

        QTextStream in(&file);
        in.setCodec("UTF-8");
        // TODO: при желании можно сделать чтобы сразу формировал QDomDocument
        // при помощи xml reader.
        QString xmlStr;
        QString onexmlstr;
        do
        {
            onexmlstr = in.readLine();
            if (onexmlstr.isNull())
                break;
            xmlStr += onexmlstr;
        }
        while (true);
        QDomDocument xmlDoc;
        QString errMsg; int errLine = 0, errColumn = 0;
        bool res = xmlDoc.setContent(xmlStr, &errMsg, &errLine, &errColumn);
        if (res)
        {
            // Считываем, есть ли у данной форму сопутствующий источник данных.
            QDomElement root = xmlDoc.documentElement();
            QDomNamedNodeMap attrsRoot = root.attributes();
            QDomNode atDataset = attrsRoot.namedItem(QString(FBXML_Attr_Dataset));

            // Если нет, то всё равно обнуляем всё, что связано с ИД.
            if (atDataset.nodeValue() == FBXML_Value_DatasetNo)
            {
                if (poDS != NULL)
                {
                    GDALClose(poDS);
                }
                availableFields.clear();
                ui->label_2->setText(QString::fromUtf8("Информация об источнике данных: источник данных не используется"));
            }

            // Иначе пытаемся загрузить поля из ИД.
            else if (atDataset.nodeValue() == FBXML_Value_DatasetYes)
            {
                GDALAllRegister();
                path.chop(4);
                path += "_data.json";
                QByteArray arr1 = QString(path).toUtf8();
                GDALDataset *poJsonDS = (GDALDataset*) GDALOpenEx(arr1.data(),
                                               GDAL_OF_VECTOR, NULL, NULL, NULL );
                if(poJsonDS == NULL)
                {
                    ShowMsgBox(QString::fromUtf8("Ошибка! XML файл найден, но сопутствующий ему JSON источник данных не удаётся открыть"));
                    return;
                }
                OGRLayer *poJsonLayer = poJsonDS->GetLayer(0);
                if(poJsonLayer == NULL)
                {
                    ShowMsgBox(QString::fromUtf8("Ошибка! XML файл найден, но сопутствующий ему JSON источник данных не удаётся открыть: некорректный слой данных"));
                    return;
                }

                if (poDS != NULL)
                {
                    GDALClose(poDS);
                }
                availableFields.clear();
                ui->label_2->setText(QString::fromUtf8("Информация об источнике данных: используется ")
                                   + arr1.data());

                // Загрузка полей целевого слоя.
                OGRFeatureDefn *poJsonDefn = poJsonLayer->GetLayerDefn();
                for(int y=0; y<poJsonDefn->GetFieldCount(); y++)
                {
                    OGRFieldDefn *poFieldDefn = poJsonDefn->GetFieldDefn(y);
                    availableFields.append(QString(poFieldDefn->GetNameRef()));
                }

                GDALClose(poJsonDS);
            }

            else
            {
                ShowMsgBox(QString::fromUtf8("Ошибка! XML файл сформирован неверно: отсутствует указание наличия связанного JSON источника данных"));
                return;
            }

            // Удаляем даже самую первую вкладку в обоих табах, т.к.
            // она будет загружена из файла.
            ClearAll();
            QWidget *tabToDel = tabWidget->widget(0);
            tabWidget->clear();
            delete tabToDel;
            tabToDel = tabHorWidget->widget(0);
            tabHorWidget->clear();
            delete tabToDel;
            // Не забываем полностью очистить массив соответствий:
            correspondence.clear();

            // Загружаем вкладки и их элементы:

            // Просматриваем вкладки в XML-файле.
            QDomNodeList tabs = root.childNodes();
            for (int i=0; i<tabs.length(); i++)
            {
                // Создаём вкладку: горизонтальную и вертикальную.
                CreatePage();
                // По сути тут не важна какая ориентация активная, но задать надо.
                isCurrentVertical = true;
                // Считываем имя вкладки.
                QDomNamedNodeMap attrs = tabs.item(i).attributes();//.at(i).attributes();
                QDomNode atType = attrs.namedItem(QString(FBXML_Attr_TabName));
                // Делаем текущую вкладку для обоих табов активной. Это важно,
                // т.к. именно на неё сейчас будут добавляться элементы.
                tabWidget->setCurrentIndex(tabWidget->count()-1);
                // Переименовываем созданную вкладку.
                ChangeTabName(atType.nodeValue());

                // Просматриваем ориентации для текущей вкладки в XML-файле.
                QDomNodeList orientations = tabs.at(i).childNodes();
                for (int j=0; j<orientations.length(); j++)
                {
                    QWidget *curWidget;

                    if (orientations.at(j).nodeName() == QString(FBXML_Node_Port))
                    {
                        isCurrentVertical = true;
                        curWidget = tabWidget->currentWidget();
                    }
                    else if (orientations.at(j).nodeName() == QString(FBXML_Node_Alb))
                    {
                        isCurrentVertical = false;
                        curWidget = tabHorWidget->currentWidget();
                    }
                    else break; // На всякий случай, если в файл будет некорректным.

                    // Получаем родительский лейбл для текущего
                    // виджета - понадобится в дальнейшем.
                    // curParentLabel не должен быть нулём, т.к. родительский лейбл
                    // был только что добавлен.
                    FBParentLabel *parentLabel = correspondence[curWidget].second;
                    /*
                    for (int ii=0; ii<screenParentLabels.size(); ii++)//.count(); ii++)
                    {
                        // Ищем родительский лейбл куда добавлять, сравнивая его виджет с
                        // текущим открытым.
                        if (screenParentLabels[ii].first == curWidget)
                        {
                            curParentLabel = screenParentLabels[ii].second.second;
                            break;
                        }
                    }*/

                    // Просматриваем элементы текущей вкладки для текущей ориентации
                    // в XML-файле.
                    QDomNodeList elements = orientations.at(j).childNodes();
                    for (int k=elements.length()-1; k>=0; k--) // просматриваем с конца, т.к. добавление будет происходить с верху
                    {
                        QDomNamedNodeMap attrs = elements.at(k).attributes();
                        QDomNode atType = attrs.namedItem(QString(FBXML_Attr_Type));
                        // Задаём текщий тип элемента, как будто выбираем из
                        // правой колонки.
                        FBElemType::CURRENT = NULL;
                        if (pElemTypeGroupStart->name == atType.nodeValue())
                        {
                            FBElemType::CURRENT = pElemTypeGroupStart;
                        }
                        else if (pElemTypeGroupEnd->name == atType.nodeValue())
                        {
                            FBElemType::CURRENT = pElemTypeGroupEnd;
                        }
                        else
                            for (int kk=0; kk<vElemTypes.size(); kk++)
                            {
                                if (vElemTypes[kk]->name == atType.nodeValue())
                                {
                                    FBElemType::CURRENT = vElemTypes[kk];
                                    break;
                                }
                            }
                        // На вскяий случай, если файл был изменён вручную: если
                        // не нашли тип элемента - просто не добавляем его на сцену:
                        if (FBElemType::CURRENT == NULL)
                            continue;

                        FBElem *newElem = FBElemType::CURRENT->CreateElem();
                        if (parentLabel->InsertElem(newElem,0))
                        {
                            connect(newElem, SIGNAL(elemPressed()), this, SLOT(OnElemPressed()));
                        }
                        else
                        {
                            delete newElem;
                            continue;
                        }

                        // Очищаем параметры элемента, т.к. они сейчас будут загружены
                        // из файла.
                        newElem->vParamValues.clear();

                        // Загружаем атрибуты элемента.
                        QDomNodeList props = elements.at(k).childNodes();
                        for (int l=0; l<props.length(); l++)
                        {
                            QDomNamedNodeMap attrs = props.at(l).attributes();

                            QDomNode attrName = attrs.namedItem(QString(FBXML_Attr_Name));
                            QDomNode attrAlias = attrs.namedItem(QString(FBXML_Attr_Alias));
                            QDomNode attrValue = attrs.namedItem(QString(FBXML_Attr_Value));
                            newElem->vParamValues.append(
                                        QPair<QPair<QString,QString>,QString>(
                                        QPair<QString,QString>(
                                                attrName.nodeValue(),attrAlias.nodeValue()),
                                            attrValue.nodeValue()));
                        }
                    }
                }
            }

            // Пользователь начинает с 1-ой открытой вертикальной вкладки:
            isCurrentVertical = false; // чтоб с гарантией сменилось при смене ориентации + т.к. последняя была всё=таки горизонтальной
            OnActionOrtnPrt(); // вызываем смену ориентации
            actionOrtnPrt->setChecked(true); // Добавляем галку которая либо есть либо нету
            tabWidget->setCurrentIndex(0);
            ui->lineEdit->setText(tabWidget->tabText(0));
            ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                                     ui->lineEdit->text());
        }
        else
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при попытке прочесть XML файл формы: ")
                           + errMsg);
        }
    }
}


// Сохранить.
void FormBuilder::OnActionSave ()
{
    // Настраиваем диалог сохранения проекта.
    QFileDialog dialogSave(this);// = new QFileDialog (this);
    dialogSave.setAcceptMode(QFileDialog::AcceptSave);
    dialogSave.setFileMode(QFileDialog::AnyFile);
    dialogSave.setViewMode(QFileDialog::List);
    dialogSave.setDefaultSuffix("xml");
    dialogSave.setNameFilter("*.xml");
    dialogSave.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogSave.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogSave.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogSave.setLabelText(QFileDialog::Accept,QString::fromUtf8("Сохранить"));
    dialogSave.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogSave.setWindowTitle(QString::fromUtf8("Сохранить форму ..."));

    QString path = "form.xml";
    dialogSave.setDirectory(lastSavePath);//QDir());
    // TODO: сделать выбор последнего имени.
    dialogSave.selectFile("form.xml"); //имя файла по умолчанию
    if (dialogSave.exec())
    {
        // TODO: (возможно через сигнал accepted) -
        // не смотря на то, что задан суффикс по дефолту - при сохранении файла с
        // другим суффиксом (не .xml) диалог не выдаст предупреждение.
        path = dialogSave.selectedFiles().first();
        if (!path.endsWith(".xml",Qt::CaseInsensitive))
            path = path + ".xml";

        // TODO: Проверка на недопустимое имя файла и выдачу сообщения.
        QFile file (path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось открыть выбранный файл"));
            return;
        }

        QTextStream out(&file);
        // Важно поставить кодек, иначе будет неправильная кодировка.
        out.setCodec("UTF-8");

        lastSavePath = path;

        // Для каждой вкладки:
        // Для каждой ориентации (если имеется):
        // Для каждого элемента:
        // Сохраняем его состояние и параметры в файл.

        QDomDocument doc;
        //QDomImplementation::setInvalidDataPolicy(QDomImplementation::ReturnNullNode);
        QDomProcessingInstruction instr
                = doc.createProcessingInstruction("xml", "version=\"1.0\"");
        doc.appendChild(instr);
        // TODO: сделать енкодинг путём добавления нового child, т.к. для
        // свойств childNodes так же актуален.
        //QDomProcessingInstruction instr2
                //= doc.createProcessingInstruction("xml", "encoding=\"UTF-8\"");
        //doc.appendChild(instr2);

        QDomElement root = doc.createElement(QString(FBXML_Root_Form));
        doc.appendChild(root);

        root.setAttribute(FBXML_Attr_Version,FBXML_Value_CURRENTVERSION);

        if (poDS != NULL)
            root.setAttribute(FBXML_Attr_Dataset,FBXML_Value_DatasetYes);
        else
            root.setAttribute(FBXML_Attr_Dataset,FBXML_Value_DatasetNo);

        for (int i=0; i<tabWidget->count(); i++)
        {
            QDomElement tabElem = doc.createElement(QString(FBXML_Node_Tab));
            tabElem.setAttribute(FBXML_Attr_TabName,
                                 tabWidget->tabText(i));
            root.appendChild(tabElem);

            QDomElement verLayElem = doc.createElement(QString(FBXML_Node_Port));
            QDomElement horLayElem = doc.createElement(QString(FBXML_Node_Alb));
            tabElem.appendChild(verLayElem);
            tabElem.appendChild(horLayElem);

            // по очереди для обоих ориентаций:
            for (int ornt=0; ornt<=1; ornt++)
            {
                FBTabWidget *curTabWidget;
                QDomElement *curOrntElem;
                if (ornt == 0)
                {
                    curTabWidget = tabWidget;
                    curOrntElem = &verLayElem;
                }
                else
                {
                    curTabWidget = tabHorWidget;
                    curOrntElem = &horLayElem;
                }

                // Получаем родительский лейбл для данной вкладки, для того чтобы взять
                // все его виджеты - элементы интерфейса.
                FBParentLabel *parentLabel = correspondence[curTabWidget->widget(i)].second;

                for (int k=0; k<parentLabel->elements.size(); k++)
                {
                    QDomElement elemElem = doc.createElement(QString(FBXML_Node_Elem));
                    curOrntElem->appendChild(elemElem);

                    elemElem.setAttribute(QString(FBXML_Attr_Type),
                       parentLabel->elements[k]->elemType->name);

                    QList<QPair<QPair<QString,QString>, QString> > params;
                    params = parentLabel->elements[k]->vParamValues;
                    for (int l=0; l<params.size(); l++)
                    {
                        QDomElement propElem = doc.createElement(QString(FBXML_Node_Prop));
                        elemElem.appendChild(propElem);

                        propElem.setAttribute(FBXML_Attr_Name,
                                              params[l].first.first);

                        propElem.setAttribute(FBXML_Attr_Alias,
                                              params[l].first.second);

                        propElem.setAttribute(FBXML_Attr_Value,
                                              params[l].second);
                    }
                }
            }
        }

        // Записываем сформированный XML в файл.
        QString result = doc.toString();
        out<<result;

        // Сохраняем данные в GeoJSON.
        GDALAllRegister();
        GDALDriver *poJsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
        if(poJsonDriver == NULL)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при создании источника данных формата GeoJSON. Не удалось инициализировать драйвер формата GeoJSON"));
            // TODO: удалить XML файл.
            return;
        }

        path.chop(4);
        QByteArray arr = QString(path + "_data.json").toUtf8();
        GDALDataset *poJsonDS = poJsonDriver->Create(arr.data(), 0, 0, 0, GDT_Unknown, NULL);
        if(poJsonDS == NULL)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при создании источника "
          "данных формата GeoJSON. Не удалось создать источник данных"));
            // TODO: удалить XML файл.
            return;
        }

        if (poDS != NULL)
        {
            OGRLayer *poSourceLayer = poDS->GetLayer(0);
            OGRLayer *poJsonLayer = poJsonDS->CopyLayer(poSourceLayer,
                                                        poSourceLayer->GetName());
            if(poJsonLayer == NULL)
            {
                ShowMsgBox(QString::fromUtf8("Ошибка при создании источника данных"
                "формата GeoJSON. Не удалось создать слой данных"));
                // TODO: удалить XML файл.
                return;
            }
        }

        GDALClose(poJsonDS);
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


/*
void FBConnectButton::HttpCancel()
{

}

void FBConnectButton::HttpSslErrors(QNetworkReply*,const QList<QSslError> &errors)
{

}
*/

