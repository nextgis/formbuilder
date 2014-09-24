#include "formbuilder.h"
#include "ui_formbuilder.h"


// Добавлять новые типы элементов сюда.
void FormBuilder::CreateElemTypes ()
{
    FBElemType* elemType;

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Пробел");
    elemType->type = FBEType::FBSpace;
    elemType->imgPaths.append(":/img/space");
    vElemTypes.append(*elemType);

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Статический текст");
    elemType->type = FBEType::FBText;
    elemType->imgPaths.append(":/img/text");
    elemType->vParams.append(QPair<QString,QString>("text",
                               QString::fromUtf8("Текст")));
    vElemTypes.append(*elemType);

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Текстовое поле");
    elemType->type = FBEType::FBTextedit;
    elemType->imgPaths.append(":/img/textfield");
    elemType->vParams.append(QPair<QString,QString>("field",
                             QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>("initial_value",
                             QString::fromUtf8("Начальный текст")));
    vElemTypes.append(*elemType);

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Выпадающий список");
    elemType->type = FBEType::FBCombobox;
    elemType->imgPaths.append(":/img/combobox");
    elemType->vParams.append(QPair<QString,QString>("field",
                             QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>("values",
                             QString::fromUtf8("Значения через ;")));
    vElemTypes.append(*elemType);

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Чекбокс");
    elemType->type = FBEType::FBCheckbox;
    elemType->imgPaths.append(":/img/checkbox");
    elemType->vParams.append(QPair<QString,QString>("field",
                                 QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>("text",
                                 QString::fromUtf8("Текст")));
    vElemTypes.append(*elemType);

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Радио-кнопка");
    elemType->type = FBEType::FBRadiobutton;
    elemType->imgPaths.append(":/img/radiobutton");
    elemType->vParams.append(QPair<QString,QString>("group",
                              QString::fromUtf8("ID группы")));
    elemType->vParams.append(QPair<QString,QString>("field",
                               QString::fromUtf8("Целевое поле")));
    elemType->vParams.append(QPair<QString,QString>("text",
                                 QString::fromUtf8("Текст")));
    vElemTypes.append(*elemType);

    elemType = new FBElemType();
    elemType->caption = QString::fromUtf8("Дата-Время");
    elemType->type = FBEType::FBDateTime;
    elemType->imgPaths.append(":/img/date");
    elemType->imgPaths.append(":/img/time");
    elemType->vParams.append(QPair<QString,QString>("date_field",
                                 QString::fromUtf8("Целевое поле даты")));
    elemType->vParams.append(QPair<QString,QString>("time_field",
                                QString::fromUtf8("Целевое поле времени")));
    vElemTypes.append(*elemType);
}


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
    QLabel *parentLabel = new QLabel(scrollArea);
    parentLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    parentLabel->setStyleSheet("QLabel { background-color: rgb(31,33,35) }");
    parentLabel->show();
    QVBoxLayout *parentLayout = new QVBoxLayout(parentLabel);
    parentLayout->addStretch();

    // Важный момент - если не сделать ресайз, то лейбл не растянется!
    // scrollArea->setWidgetResizable(true) не помогает.
    // Может быть есть и другой способ сочетать изменение размеров scrollArea
    // и поведение раскладки внутри подконтрольного ей виджета.
    // Следующие цифры подобраны вручную и имеют место быть, поскольку размер
    // "экрана" фиксированный.
    if (isVertical)
        parentLabel->resize(FBParentLableVertWidth,
                            FBParentLableVertStartHeight);
    else
        parentLabel->resize(FBParentLableHorWidth,
                            FBParentLableHorStartHeight);

    scrollArea->setWidget(parentLabel);
    screenLayout2->addWidget(scrollArea);

    // Сохраняем соответствие страницы и виджета:
    // В данном случае screenLabel->parent() - это сам виджет страницы.
    screenParentLabels.append(QPair<QWidget*,QPair<QLabel*,QLabel*> >(pageWidget,
                         QPair<QLabel*,QLabel*> (screenLabel2,parentLabel)));

    return screenLabel2;
}


// Добавление новой вкладки.
void FormBuilder::OnActionAddPage ()
{
    if (tabWidget->count() == FBMaxPageCount)
        return;

    QString newPageName = QString::fromUtf8("Вкладка ") +
            QString::number(tabWidget->count() + 1);

//-----------------------------------------------------------------------//
//                     Добавление вертикальной вкладки                   //
//-----------------------------------------------------------------------//

    QWidget *newPage = new QWidget();
    tabWidget->addTab(newPage, newPageName);

        QVBoxLayout *pageLayout = new QVBoxLayout(newPage);
        pageLayout->setSpacing(0);
        pageLayout->setContentsMargins(0,0,0,0);
        pageLayout->addStretch();

        // Верхняя часть экрана: для красоты.
           QLabel *screenLabel = new QLabel(newPage);
           screenLabel->setPixmap(QString(":/img/top"));
           screenLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pageLayout->addWidget(screenLabel);

        // Средняя часть экрана:
        // Создаём экранный лейбл, который будет иметь только две вещи: фоновую
        // картинку и виджет прокручиваемой области. Тем самым мы фиксируем
        // саму прокручиваемую область, но то, что будет внутри неё соответственно
        // будет изменять свои размеры.
           QLabel *screenLabel2 = CreateScreenLabel(newPage,true);
        pageLayout->addWidget(screenLabel2);

        // Нижняя часть экрана: для красоты.
           QLabel *screenLabel3 = new QLabel(newPage);
           screenLabel3->setPixmap(QString(":/img/bottom"));
           screenLabel3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pageLayout->addWidget(screenLabel3);

        pageLayout->addStretch();

        pageLayout->setAlignment(screenLabel,Qt::AlignCenter);
        pageLayout->setAlignment(screenLabel2,Qt::AlignCenter);
        pageLayout->setAlignment(screenLabel3,Qt::AlignCenter);

//-----------------------------------------------------------------------//
//                   Добавление горизонтальной вкладки                   //
//-----------------------------------------------------------------------//

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
           QLabel *screenHorLabel = new QLabel(newHorPage);
           screenHorLabel->setPixmap(QString(":/img/top_hor"));
           screenHorLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        subPageHorLayout->addWidget(screenHorLabel);

           QLabel *screenHorLabel2 = CreateScreenLabel(newHorPage,false);
        subPageHorLayout->addWidget(screenHorLabel2);
        subPageHorLayout->addStretch();

           QLabel *screenHorLabel3 = new QLabel(newHorPage);
           screenHorLabel3->setPixmap(QString(":/img/bottom_hor"));
           screenHorLabel3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pageHorLayout->addWidget(screenHorLabel3);

        pageHorLayout->addStretch();

        pageHorLayout->setAlignment(screenHorLabel,Qt::AlignCenter);
        pageHorLayout->setAlignment(screenHorLabel2,Qt::AlignCenter);
        pageHorLayout->setAlignment(screenHorLabel3,Qt::AlignCenter);
}


FormBuilder::FormBuilder(QWidget *parent) :QWidget(parent),ui(new Ui::FormBuilder)
{
    ui->setupUi(this);

    move(0,0);

    // Создаём верхнее меню: "Файл", ...
    menuBar = new QMenuBar(this);
    this->layout()->setMenuBar(menuBar);
    menuFile = new QMenu(QString::fromUtf8("Файл"),menuBar);
    menuBar->addMenu(menuFile);
      //actionNew = new QAction(QString::fromUtf8("Новый"),menuBar);
      //menuFile->addAction(actionNew);
      menuNew = new QMenu(QString::fromUtf8("Новый"),menuBar);
      menuFile->addMenu(menuNew);
        actionNewVoid = new QAction(QString::fromUtf8("Пустая форма"),menuBar);
        menuNew->addAction(actionNewVoid);
        actionNewDataset = new QAction(QString::fromUtf8("Выбрать источник данных"),menuBar);
        menuNew->addAction(actionNewDataset);
      actionOpen = new QAction(QString::fromUtf8("Открыть"),menuBar);
      menuFile->addAction(actionOpen);
      actionSave = new QAction(QString::fromUtf8("Сохранить"),menuBar);
      menuFile->addAction(actionSave);
    menuScreen = new QMenu(QString::fromUtf8("Экран"),menuBar);
    menuBar->addMenu(menuScreen);
      //new QAction(QString::fromUtf8("Ориентация"),menuBar);
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

    //connect(actionNew, SIGNAL(triggered()), this, SLOT(OnActionNew()));
    connect(actionNewVoid, SIGNAL(triggered()), this, SLOT(OnActionNewVoid()));
    connect(actionNewDataset, SIGNAL(triggered()), this, SLOT(OnActionNewDataset()));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(OnActionOpen()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(OnActionSave()));
    connect(actionOrtnPrt, SIGNAL(triggered(bool)), this, SLOT(OnActionOrtnPrt(bool)));
    connect(actionOrtnAlb, SIGNAL(triggered(bool)), this, SLOT(OnActionOrtnAlb(bool)));
    connect(actionAddPage, SIGNAL(triggered()), this, SLOT(OnActionAddPage()));

    // Создаём экран телефона с одной страницей - вертикальной и горизонтальной.
    QVBoxLayout *groupLayout = new QVBoxLayout(ui->groupBox_2);
    tabWidget = new QTabWidget(ui->groupBox_2);
    tabHorWidget = new QTabWidget(ui->groupBox_2);
    groupLayout->addWidget(tabWidget);
    groupLayout->addWidget(tabHorWidget);
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(OnTabIndexChanged(int)));
    connect(tabHorWidget,SIGNAL(currentChanged(int)),this,SLOT(OnTabIndexChanged(int)));

    // Начинаем с портретной ориентации.
    ui->groupBox_2->setTitle(QString::fromUtf8("Экран приложения: портретная ориентация"));
    tabHorWidget->hide();
    isCurrentVertical = true;

    // Добавляем первую страницу (вкладку) для обоих ориентаций.
    OnActionAddPage();

    // Создаём типы элементов.
    this->CreateElemTypes();

    // Выводим их в правую колонку.
    QVBoxLayout* pVLayout = (QVBoxLayout*)ui->groupBox->layout();
    for (int i=0; i<vElemTypes.count(); i++)
    {
        QVBoxLayout *pVLayout2;
        FBLabelToDrag* labelImg;
        QLabel* labelText;
        labelText = new QLabel(ui->groupBox);
        labelText->setText(vElemTypes[i].caption);
        labelText->setStyleSheet("QLabel {text-decoration: underline}");
        labelImg = new FBLabelToDrag(ui->groupBox);
        labelImg->setPixmap(QPixmap(vElemTypes[i].imgPaths[0]));
        //labelImg->setAcceptDrops(true);
        pVLayout2 = new QVBoxLayout();
        pVLayout2->addWidget(labelText);
        pVLayout2->addWidget(labelImg);
        pVLayout->addLayout(pVLayout2);
        pVLayout->addSpacing(20);

        /*
        QFrame *frame = new QFrame(ui->groupBox);
        pVLayout->addWidget(frame);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        frame->show();
        */

        // Чтобы потом перехватить событие перетаскивания:
        //labelImg->type = vElemTypes[i].type;
        //vElemTypes[i].labelToDrag = labelImg;
        labelImg->elemArrayIndex = i;
    }

    pVLayout->addStretch();

    // Настраиваем правую панель.
    ui->tableWidget->clear();
    //ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->setColumnCount(1);
    //ui->tableWidget->setHorizontalHeaderItem
            //(0,new QTableWidgetItem(QString::fromUtf8("Атрибут")));
    //ui->tableWidget->setHorizontalHeaderItem
            //(0,new QTableWidgetItem(QString::fromUtf8("Значение")));

    // Настраиваем диалог сохранения файла.
    dialogSave = new QFileDialog (this);
    dialogSave->setAcceptMode(QFileDialog::AcceptSave);
    dialogSave->setFileMode(QFileDialog::AnyFile);
    dialogSave->setViewMode(QFileDialog::List);
    dialogSave->setDefaultSuffix("xml");
    dialogSave->setNameFilter("*.xml");
    dialogSave->setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogSave->setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogSave->setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogSave->setLabelText(QFileDialog::Accept,QString::fromUtf8("Сохранить"));
    dialogSave->setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    //dlg_path->setLabelText(QFileDialog::,QString::fromUtf8("Открыть"));
    dialogSave->setWindowTitle(QString::fromUtf8("Сохранить ..."));

    // Настраиваем диалог выбора датасета для новой формы.
    dialogNewDataset = new QFileDialog (this);
    dialogNewDataset->setAcceptMode(QFileDialog::AcceptOpen);
    // Выбрать можно только один существующий файл:
    dialogNewDataset->setFileMode(QFileDialog::ExistingFile);
    dialogNewDataset->setViewMode(QFileDialog::List);
    //dialogNewDataset->setDefaultSuffix("xml");
    //dialogNewDataset->setNameFilter("*.xml");
    dialogNewDataset->setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogNewDataset->setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogNewDataset->setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogNewDataset->setLabelText(QFileDialog::Accept,QString::fromUtf8("Выбрать"));
    dialogNewDataset->setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogNewDataset->setWindowTitle(QString::fromUtf8("Выбирете источник данных ..."));

    // Настройки GDAL.
    GDALAllRegister();
    poDS = NULL;
}


FormBuilder::~FormBuilder()
{
    if (poDS != NULL)
        GDALClose(poDS);

    delete ui;
}


// Действует на весь виджет, даже если отжатие было в дочернем виджете.
// Добавить элемент.
void FormBuilder::mouseReleaseEvent (QMouseEvent *event)
{ 
    if (FBLabelToDrag::eCurElemArrayIndex != -1)
    {
        /*
        int mx = event->x();
        int my = event->y();
        int gbx = ui->groupBox_2->x();
        int gby = ui->groupBox_2->y();
        int gbw = ui->groupBox_2->width();
        int gbh = ui->groupBox_2->height();
        if (mx < gbx || my < gby || mx > (gbx + gbw) || my > (gby + gbh))
            return;
        */

        // Определяем текущую ориентацию.
        QTabWidget *curTabWidget;
        int startHeight;
        if (isCurrentVertical)
        {
            startHeight = FBParentLableVertStartHeight;
            curTabWidget = tabWidget;
        }
        else
        {
            startHeight = FBParentLableHorStartHeight;
            curTabWidget = tabHorWidget;
        }

        QLabel *currentParentLabel = NULL;
        for (int i=0; i<screenParentLabels.count(); i++)
        {
            // Ищем родительский лейбл куда добавлять, сравнивая его виджет с
            // текущим открытым.
            if (screenParentLabels[i].first == curTabWidget->currentWidget())
            {
                currentParentLabel = screenParentLabels[i].second.second;
            }
        }
        if (currentParentLabel == NULL)
        {
            FBLabelToDrag::eCurElemArrayIndex = -1;
            return;
        }

        // Проверяем на максимум возможное кол-во добавленных элементов.
        // FBMaxElemInPageCount + 1, т.к. внутренняя раскладка - не виджет.
        if (currentParentLabel->children().count() == FBMaxElemInPageCount + 1)
        {
            FBLabelToDrag::eCurElemArrayIndex = -1;
            return;
        }

        // Создаём элемент в родительском лейбле.
        FBElem *elem =
        vElemTypes[FBLabelToDrag::eCurElemArrayIndex].CreateElem(currentParentLabel,
                         vElemTypes[FBLabelToDrag::eCurElemArrayIndex].type);

        // TEMP ---------------------------------------------------------------
        // Помещаем элемент в самый верх - чтобы при просмотре порядка виджетов
        // не сортировать каждый раз массив.
        elem->lower();
        // --------------------------------------------------------------------

        connect(elem,SIGNAL(elemSelected(FBElem*)),
                                  this,SLOT(OnElemSelected(FBElem*)));

        // Обязательно изменяем высоту родительского лейбла, иначе не будет
        // прокрутки.
        // Высоту изменяем только если есть на какую величину изменять.
        // При перерасчёте надо учесть отступы сверху и снизу иначе элементы
        // будут залезать друг на друга.
        int totalChildrenHeight =
                currentParentLabel->layout()->contentsMargins().top()
                + currentParentLabel->layout()->contentsMargins().bottom();
        for (int k=0; k < currentParentLabel->children().count(); k++)
        {
            // Проверяем, является ли ребёнок - виджетом, т.к. родительский
            // лейбл будет иметь по крайней мере один такой объект - раскладку,
            // не являющуюся виджетом.
            QObject *child = currentParentLabel->children().at(k);
            if (child->isWidgetType())
            {
                totalChildrenHeight +=
                    static_cast<QWidget*>(child)->height()
                        + currentParentLabel->layout()->spacing();
            }
        }
        if (totalChildrenHeight > startHeight)
        {
            currentParentLabel->resize(currentParentLabel->width(),
                                       totalChildrenHeight);
        }

        FBLabelToDrag::eCurElemArrayIndex = -1;
    }
}


// Выбрать элемент.
void FormBuilder::OnElemSelected (FBElem *pElem)
{
    ui->tableWidget->clearContents(); // TODO: Это удаляет виджеты-комбобоксы?
    ui->tableWidget->setRowCount(0); // TODO: Правильно таким образом очищать список строк?
    for (int i=0; i<pElem->vParamValues.count(); i++)
    {
        int r = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(r + 1);
        //ui->tableWidget->setItem(r,0,
              //new QTableWidgetItem(pElem->vParamValues[i].first.second));
        ui->tableWidget->setVerticalHeaderItem
                    (i,new QTableWidgetItem(pElem->vParamValues[i].first.second));
        // Если есть поля из каких выбирать - выводим.
        if (!availableFields.isEmpty()
                && (pElem->vParamValues[i].first.first == "field"
                    || pElem->vParamValues[i].first.first == "date_field"
                    || pElem->vParamValues[i].first.first == "time_field"))
        {
            // Создаём комбобокс.
            QComboBox *combo = new QComboBox(ui->tableWidget);
            combo->setEditable(true);
            // Заполняем комбобокс.
            int index = -1;
            for (int j=0; j<availableFields.count(); j++)
            {
                if (availableFields[j] == pElem->vParamValues[i].second)
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
                combo->setEditText(pElem->vParamValues[i].second);
            }
        }
        else
        {
            ui->tableWidget->setItem(i,0,
              new QTableWidgetItem(pElem->vParamValues[i].second));
        }
    }
    ui->groupBox_4->setTitle(QString::fromUtf8("Редактирование элемента: ") +
                         pElem->caption);
}


// Удалить элемент.
void FormBuilder::on_pushButton_clicked()
{
    if (FBElem::pLastSelected == NULL)
        return;

    // TODO: Уменьшаем высоту родительского лейбла, если требуется.

    delete FBElem::pLastSelected;

    FBElem::pLastSelected = NULL;

    OnDeselectElement();
}


// Очистить вкладку.
void FormBuilder::on_pushButton_2_clicked()
{
    // Определяем текущую ориентацию.
    QTabWidget *curTabWidget;
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

    // TODO: массив соответствий screenParentLabels можно было бы и сделать
    // через словарь с ключом по widget.
    for (int i=0; i<screenParentLabels.count(); i++)
    {
        if (screenParentLabels[i].first == widget)
        {
            // Сначала снимаем выделение т.к. если выделенный элемент находится
            // на текущей вкладке - он будет удалён и мы не будем знать этого
            // в конце.
            if (FBElem::pLastSelected != NULL)
            {
                FBElem::pLastSelected->deselect();
                FBElem::pLastSelected = NULL;
                OnDeselectElement();
            }

            // Удаляем а затем добавляем экранный лейбл. Родительский виджет
            // тоже удалится (внутри удаления экранного).
            delete screenParentLabels[i].second.first;
            // Надо ли удалять указатель из массива детей у родителя?
            // Ответ: нет. После delete его уже нет в родитель->children(),
            // проверено в дебаге.

            // Удаляем из массива соответствий всю запись, т.к. она создастся снова
            // при вызове CreateScreenLabel().
            screenParentLabels.removeAt(i);

            QLabel *screenLabel2 = CreateScreenLabel(widget,isCurrentVertical);
            if (isCurrentVertical)
            {
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
                for (int j=0; j<children.count(); j++)
                {
                    // Ищем единственную горизонтальную раскладку этого виджета. Должна
                    // встретится первой.
                    // TODO: найти другой, более безопасный способ поиска этой раскладки.
                    if (!children[j]->isWidgetType())
                    {
                        static_cast<QHBoxLayout*>(children[j])
                                ->insertWidget(2,screenLabel2);
                        static_cast<QHBoxLayout*>(children[j])
                                ->setAlignment(screenLabel2,Qt::AlignCenter);
                    }
                }
            }

            break;
        }
    }
}


// Удалить вкладку.
void FormBuilder::on_pushButton_3_clicked()
{
    // Определяем текущую ориентацию. Нужно чтобы определить открытую вкладку.
    QTabWidget *curTabWidget;
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
    bool foundVer = false;
    bool foundHor = false;
    for (int i=0; i<screenParentLabels.count(); i++)
    {
        if (screenParentLabels[i].first == widgetHor)
        {
            screenParentLabels.removeAt(i);
            foundHor = true;
        }
        else if (screenParentLabels[i].first == widgetVer)
        {
            screenParentLabels.removeAt(i);
            foundVer = true;
        }
        if (foundVer && foundHor)
            break;
    }

    // Удаляем одну и ту же вкладку в обоих ориентациях.
    tabWidget->removeTab(tabToDeleteIndex);
    tabHorWidget->removeTab(tabToDeleteIndex);

    // Сначала снимаем выделение т.к. если выделенный элемент находится
    // на текущей вкладке - он будет удалён и мы не будем знать этого
    // в конце.
    if (FBElem::pLastSelected != NULL)
    {
        FBElem::pLastSelected->deselect();
        FBElem::pLastSelected = NULL;
        OnDeselectElement();
    }

    // Затем удаляем сами виджеты.
    delete widgetVer;
    delete widgetHor;

    ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                             curTabWidget->tabText(curTabWidget->currentIndex()));
}


void FormBuilder::OnDeselectElement ()
{
    // TODO: Удостовериться, что виджеты комбобоксов удаляются.
    // Если что в справке сказано, что после добавления виджета -
    // passing the ownership of the widget to the table.

    ui->groupBox_4->setTitle(QString::fromUtf8("Редактирование элемента: "));
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}


// Смена ориентации.
void FormBuilder::OnActionOrtnPrt (bool is)
{
    if (!isCurrentVertical)
    {
        actionOrtnAlb->setChecked(false);
        isCurrentVertical = true;
        tabWidget->show();
        tabHorWidget->hide();
        //tabWidget->adjustSize();
        //ui->groupBox_2->adjustSize();
    }
    else
    {
        // Не даём снять галку.
        actionOrtnPrt->setChecked(true);
    }
}
void FormBuilder::OnActionOrtnAlb (bool is)
{
    if (isCurrentVertical)
    {
        actionOrtnPrt->setChecked(false);
        isCurrentVertical = false;
        tabWidget->hide();
        tabHorWidget->show();
        //tabHorWidget->adjustSize();
        //ui->groupBox_2->adjustSize();
    }
    else
    {
        // Не даём снять галку.
        actionOrtnAlb->setChecked(true);
    }
}


// Смена активной вкладки.
void FormBuilder::OnTabIndexChanged (int index)
{
    // Определяем текущую ориентацию, поскольку слот вызовется после сигналов
    // от обоих таб вджетов.
    QTabWidget *curTabWidget;
    if (isCurrentVertical)
        curTabWidget = tabWidget;
    else
        curTabWidget = tabHorWidget;

    if (index == -1)
        return;

    ui->lineEdit->setText(curTabWidget->tabText(index));

    ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                             curTabWidget->tabText(index));
}


// Изменить название вкладки.
void FormBuilder::on_pushButton_4_clicked()
{
    QTabWidget *curTabWidget;
    if (isCurrentVertical)
        curTabWidget = tabWidget;
    else
        curTabWidget = tabHorWidget;

    if (ui->lineEdit->text().size() > FBMaxTabNameCharCount)
        return;

    curTabWidget->setTabText(curTabWidget->currentIndex(),ui->lineEdit->text());

    ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                             ui->lineEdit->text());
}


// Зафиксировать изменения.
void FormBuilder::on_pushButton_5_clicked()
{
    for (int i=0; i<FBElem::pLastSelected->vParamValues.count(); i++)
    //for (int i=0; i<ui->tableWidget->columnCount(); i++)
    {
        // Мы должны понять что находится в ячейке и как достать оттуда текст.
        // Не забываем item(i,0), 0 - т.к. самая левая колонка - это не ячейка
        // а заголовок.
        QWidget *widget = ui->tableWidget->cellWidget(i,0);
        if (widget == NULL)
        {
            FBElem::pLastSelected->vParamValues[i].second
                    = ui->tableWidget->item(i,0)->text();
        }
        else
        {
            //QMetaObject *objInfo = widget->metaObject();
            //if ()

            FBElem::pLastSelected->vParamValues[i].second
                    = static_cast<QComboBox*>(widget)->currentText();
        }
    }

}


// Очистить всё для создания нового проекта.
void FormBuilder::ClearAll ()
{
    // Удаляем все страницы кроме последней.
    // Легче это сделать через существующий слот, т.к. там уже есть вся работа по
    // удалению дочерних виджетов.
    while (tabWidget->count() > 1)
    {
        on_pushButton_3_clicked();
    }
    // Последнюю вкладку очищаем.
    on_pushButton_2_clicked();
    // А так же очищаем её пару другой ориентации.
    isCurrentVertical = !isCurrentVertical;
    on_pushButton_2_clicked();
    isCurrentVertical = !isCurrentVertical;
    // Переименовываем все поля.
    // TODO: !!!
}


//void FormBuilder::OnActionNew ()
//{
//}


void FormBuilder::OnActionNewVoid ()
{

}


void FormBuilder::OnActionNewDataset ()
{
    dialogNewDataset->setDirectory(QDir()); //ставим текущую директорию
    //dialogNewDataset->selectFile("form.xml"); //имя файла по умолчанию
    if (dialogNewDataset->exec())
    {
        QStringList path = dialogNewDataset->selectedFiles();

        if (poDS != NULL)
        {
            GDALClose(poDS);
        }
        availableFields.clear();

        QByteArray arr = path[0].toUtf8();
        poDS = (GDALDataset*) GDALOpenEx(arr.data(), GDAL_OF_VECTOR, NULL, NULL, NULL);
        if(poDS == NULL)
        {
            ui->label_2->setText(QString::fromUtf8("Ошибка: невозможно открыть источник данных ")
                                 + QString::fromUtf8(arr.data()));
            return;
        }

        // Проверяем кол-во слоёв.
        if (poDS->GetLayerCount() == 0 || poDS->GetLayerCount() > 1)
        {
            ui->label_2->setText(QString::fromUtf8("Ошибка: источник данных ")
                                 + QString::fromUtf8(arr.data())
                                 + QString::fromUtf8(" содержит больше чем 1 слой")
                                 );
            return;
        }

        // Проверяем правильность системы координат.
        OGRLayer *poLayer =  poDS->GetLayer(0);
        if (poLayer == NULL)
        {
            ui->label_2->setText(QString::fromUtf8("Ошибка: невозможно считать слой в источнике данных ")
                                 + QString::fromUtf8(arr.data()));
            return;
        }
        /*
        if (poLayer->GetSpatialRef()->GetAuthorityName())
        {

        }
        */

        // Считываем набор доступных целевых полей.
        OGRFeatureDefn *poLayerDefn = poLayer->GetLayerDefn();
        for(int i=0; i<poLayerDefn->GetFieldCount(); i++)
        {
            OGRFieldDefn *poFieldDefn = poLayerDefn->GetFieldDefn(i);
            availableFields.append(QString(poFieldDefn->GetNameRef()));
        }

        ui->label_2->setText(QString::fromUtf8("Используется источник данных: ")
                           + arr.data());

        // Стираем и удаляем всё старое - создаём новый проект.
        ClearAll();
    }
}


void FormBuilder::OnActionOpen ()
{

}


void FormBuilder::OnActionSave ()
{
    QString path = "form.xml";
    dialogSave->setDirectory(QDir()); //ставим текущую директорию
    dialogSave->selectFile("form.xml"); //имя файла по умолчанию
    if (dialogSave->exec())
    {
        // TODO: (возможно через сигнал accepted) -
        // не смотря на то, что задан суффикс по дефолту - при сохранении файла с
        // другим суффиксом (не .xml) диалог не выдаст предупреждение.
        path = dialogSave->selectedFiles().first();
        if (!path.endsWith(".xml",Qt::CaseInsensitive))
            path = path + ".xml";

        // TODO: Проверка на недопустимое имя файла и выдачу сообщения.
        QFile file (path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        // Важно поставить кодек, иначе будет неправильная кодировка.
        out.setCodec("UTF-8");

        // Для каждой вкладки:
        // Для каждой ориентации (если имеется):
        // Для каждого элемента:
        // Сохраняем его состояние и параметры в файл.

        QDomDocument doc;//(FBXML_Root_Form);
        //QDomImplementation::setInvalidDataPolicy(QDomImplementation::ReturnNullNode);
        QDomProcessingInstruction instr
                //= doc.createProcessingInstruction("xml", "version=\"1.0\"");
                = doc.createProcessingInstruction("xml", "encoding=\"UTF-8\"");
        doc.appendChild(instr);

        for (int i=0; i<tabWidget->count(); i++)
        {
            QDomElement tabElem = doc.createElement(QString(FBXML_Node_Tab));
            tabElem.setAttribute(FBXML_Attr_TabName,
                                 tabWidget->tabText(i));
            doc.appendChild(tabElem);

            QDomElement verLayElem = doc.createElement(QString(FBXML_Node_Port));
            QDomElement horLayElem = doc.createElement(QString(FBXML_Node_Alb));
            tabElem.appendChild(verLayElem);
            tabElem.appendChild(horLayElem);

            // Получаем родительский лейбл для данной вкладки, для того чтобы взять
            // все его виджеты - элементы интерфейса.
            QObjectList verList;
            for (int j=0; j<screenParentLabels.count(); j++)
            {
                if (screenParentLabels[j].first == tabWidget->widget(i))
                {
                    // Копируем массив чтобы при его сортировке не запортить оргинальный.
                    // TODO: возможно копирование не нужно.
                    verList = QList<QObject*>(screenParentLabels[j].second.second->children());
                    break;
                }
            }

            // Сортируем массив в порядке возрастания Y координаты элементов
            // (т.к. дочерние виджеты находятся в массиве в порядке их добавления, а
            // не геометрическом порядке) и записываем его в xml.
            // TODO: если сделать поддержку раскладок, то такой способ не подайдёт,
            // т.к. элементы уже не будут расположены друг за другом по вертикали.
            // TODO: сделать сортировку с досрочным выходом из цикла.
            // IMPORTANT TODO: убрать сортировку отсюда, а при добавлении элементов
            // на форму добавлять их сразу в сортированный массив через raise() и lower().
            if (!verList.isEmpty()) // наверное не требуется эта проверка
            {
                /*
                for (int kk=verList.count()-1; kk>=1; kk--)
                {
                    for (int jj=0; jj<kk-1; jj++)
                    {
                        if (verList[jj]->isWidgetType()
                                && verList[jj+1]->isWidgetType())
                        {
                            if (static_cast<QLabel*>(verList[jj])->y()
                                > static_cast<QLabel*>(verList[jj+1])->y())
                            {
                                QObject *buf = verList[jj];
                                verList[jj] = verList[jj+1];
                                verList[jj+1] = buf;
                            }
                        }
                    }
                }
                */

                for (int k=0; k<verList.count(); k++)
                {
                    if (verList[k]->isWidgetType())
                    {
                        QDomElement elemElem = doc.createElement(QString(FBXML_Node_Elem));
                        verLayElem.appendChild(elemElem);

                        elemElem.setAttribute(QString(FBXML_Attr_Type),
                                              static_cast<FBElem*>(verList[k])->caption);

                        QList<QPair<QPair<QString,QString>, QString> > params;
                        params = static_cast<FBElem*>(verList[k])->vParamValues;
                        for (int l=0; l<params.count(); l++)
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
        }

        // Записываем сформированный XML в файл.
        QString result = doc.toString();
        out<<result;
    }
}


