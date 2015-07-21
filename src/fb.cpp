/******************************************************************************
 *
 * Name:     fb.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Main widget (application window) class implementation.
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
#include "ui_fb.h"


FB::~FB()
{
    // TODO: удаляем всякие диалоги и другие виджеты? Или они удалятся как виджеты-дети?
    //delete dlgProgress;
    delete ui;
}


FBProject* FB::CUR_PROJECT = NULL;

FB::FB(QWidget *parent): QWidget(parent), ui(new Ui::FB)
{
    #ifdef FB_GDAL_DEBUG
    CPLSetConfigOption("CPL_DEBUG","ON");
    CPLSetConfigOption("CPL_CURL_VERBOSE","YES");
    CPLSetConfigOption("CPL_LOG",FB_GDAL_DEBUG);
    CPLSetConfigOption("CPL_LOG_ERRORS","ON");
    #endif

    // TODO: проверить это в разных версия Джидала.
    CPLSetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS",FB_PROJECT_EXTENSION);

    #ifndef FB_GDAL_2_X
    OGRRegisterAll();
    #else
    GDALAllRegister();
    #endif

    ui->setupUi(this);

    this->setStyleSheet("QWidget { background: white }"); // для всех виджетов в приложении.
    this->layout()->setContentsMargins(0,5,0,5);
    this->layout()->setSpacing(10);

    canScrollToEnd = false;

//---------------------------------------------------------------------------
//                              Главное меню
//---------------------------------------------------------------------------

    QWidget *tabProject = new QWidget();
    ui->tabWidget->addTab(tabProject, tr(" Проект "));
    QHBoxLayout *layProject = new QHBoxLayout(tabProject);
    layProject->setContentsMargins(0,0,0,0);
    QWidget *tabDevice = new QWidget();
    ui->tabWidget->addTab(tabDevice, tr("   Вид   "));//tr(" Устройство "));
    QHBoxLayout *layDevice = new QHBoxLayout(tabDevice);
    QWidget *tabTools = new QWidget();
    ui->tabWidget->addTab(tabTools, tr(" Инструменты "));
    QHBoxLayout *layTools = new QHBoxLayout(tabTools);
//    QWidget *tabSettings = new QWidget();
//    ui->tabWidget->addTab(tabSettings, tr(" Настройки "));
    QWidget *tabAbout = new QWidget();
    ui->tabWidget->addTab(tabAbout, tr(" О программе "));

    ui->tabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    ui->tabWidget->setMaximumHeight(120);
    ui->tabWidget->setMinimumHeight(100);
    ui->tabWidget->setFont(QFont("Candara",11));
    ui->tabWidget->setStyleSheet("QTabWidget::pane {"
                                   "border-top: 1px solid rgb(170,170,170);"
                                   "border-bottom: 1px solid rgb(170,170,170);"
                                   //"border-left: 1px solid rgb(170,170,170);"
                                   //"border-right: 1px solid rgb(170,170,170);"
                                   "}"
                                 "QTabWidget::tab-bar {"
                                   "left: 5px; "
                                   "top: 1px; }"
                                 "QTabBar::tab {"
                                    //"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                      //"stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                                      //"stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                                    "border: 1px solid rgb(170,170,170);"
                                    "border-bottom-color: rgb(255,255,255); "
                                    "border-top-left-radius: 4px;"
                                    "border-top-right-radius: 4px;"
                                    "min-width: 8ex;"
                                    "padding: 2px;}"
                                 //"QTabBar::tab:selected, QTabBar::tab:hover {"
                                    //"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                       //"stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
                                       //"stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);}"
                                 //"QTabBar::tab:selected {"
                                     //"border-color: #9B9B9B;"
                                     //"border-bottom-color: rgb(255,255,255); }"
                                 "QTabBar::tab:!selected {"
                                     "border: 1px solid rgb(255,255,255);"
                                     "border-bottom-color: rgb(170,170,170);"
                                     "margin-top: 2px; "
                                 "}");
    butNewShp = createMainMenuButton(tabProject,layProject,
                        ":/img/new_shp.png",tr("Новый из\nShapefile"));
    butNewNgw = createMainMenuButton(tabProject,layProject,
                        ":/img/new_ngw.png",tr("Новый из\nNextGIS Web"));
    butOpen = createMainMenuButton(tabProject,layProject,
                        ":/img/open.png",tr("Открыть"));
    butSave = createMainMenuButton(tabProject,layProject,
                        ":/img/save.png",tr("Сохранить"));
    butSave->setEnabled(false);
    butSaveAs = createMainMenuButton(tabProject,layProject,
                        ":/img/save_as.png",tr("Сохранить\nкак ..."));
    butSaveAs->setEnabled(false);
    butAndrAll = createMainMenuButton(tabDevice,layDevice,
                                      ":/img/dev_andrall.png",tr("Экран Android\nМаксимально"));
    butPhoneAndrVer = createMainMenuButton(tabDevice,layDevice,
                        ":/img/dev_andrphone_ver.png",tr("Смартфон Android\nВертикально"));
    butPhoneAndrHor = createMainMenuButton(tabDevice,layDevice,
                        ":/img/dev_andrphone_hor.png",tr("Смартфон Android\nГоризонтально"));
    butPhoneAndrHor->setEnabled(false);
    butTabletAndrVer = createMainMenuButton(tabDevice,layDevice,
                        ":/img/dev_andrtablet_ver.png",tr("Планшет Android\nВертикально"));
    butTabletAndrVer->setEnabled(false);
    butTabletAndrHor = createMainMenuButton(tabDevice,layDevice,
                        ":/img/dev_andrtablet_hor.png",tr("Планшет Android\nГоризонтально"));
    butTabletAndrHor->setEnabled(false);
    butWeb = createMainMenuButton(tabDevice,layDevice,
                        ":/img/dev_web.png",tr("Формы для\nWeb"));
    butWeb->setEnabled(false);
    butQgis = createMainMenuButton(tabDevice,layDevice,
                        ":/img/dev_qgis.png",tr("Формы для\nQGIS"));
    butQgis->setEnabled(false);
    butClearScreen = createMainMenuButton(tabTools,layTools,
                  ":/img/clear_screen.png",tr("Очистить экран"));
    butClearScreen->setEnabled(false);
    butDeleteElem = createMainMenuButton(tabTools,layTools,
                  ":/img/delete_elem.png",tr("Удалить элемент"));
    butDeleteElem->setEnabled(false);
    layProject->addStretch();
    layDevice->addStretch();
    layTools->addStretch();
    connect(butNewShp,SIGNAL(clicked()),this,SLOT(onNewShpClick()));
    connect(butNewNgw,SIGNAL(clicked()),this,SLOT(onNewNgwClick()));
    connect(butOpen,SIGNAL(clicked()),this,SLOT(onOpenClick()));
    connect(butSave,SIGNAL(clicked()),this,SLOT(onSaveClick()));
    connect(butSaveAs,SIGNAL(clicked()),this,SLOT(onSaveAsClick()));
    connect(butAndrAll,SIGNAL(clicked()),this,SLOT(onAndrAllClick()));
    connect(butPhoneAndrVer,SIGNAL(clicked()),this,SLOT(onAndrPhoneVerClick()));
    connect(butPhoneAndrHor,SIGNAL(clicked()),this,SLOT(onAndrPhoneHorClick()));
    connect(butTabletAndrVer,SIGNAL(clicked()),this,SLOT(onAndrTabletVerClick()));
    connect(butTabletAndrHor,SIGNAL(clicked()),this,SLOT(onAndrTabletHorClick()));
    connect(butClearScreen,SIGNAL(clicked()),this,SLOT(onClearScreenClick()));
    connect(butDeleteElem,SIGNAL(clicked()),this,SLOT(onDeleteElemClick()));

//---------------------------------------------------------------------------
//                              Левый виджет
//---------------------------------------------------------------------------

    ui->leftWidget->setStyleSheet("QWidget#"+ui->leftWidget->objectName()+"{"
                                    "border-top: 1px solid rgb(170,170,170);"
                                    "border-bottom: 1px solid rgb(170,170,170);"
                                    "border-right: 1px solid rgb(170,170,170);"
                                    "border-top-right-radius: 4px;"
                                    "border-bottom-right-radius: 4px;"
                                  "}");
    ui->leftWidget->setMaximumWidth(240);
    ui->leftWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    QHBoxLayout *leftLayout = new QHBoxLayout(ui->leftWidget);
    // Для того чтобы список и стрелка не растягивались по-вертикали
    // делаем для них отдельные раскладки:
    QVBoxLayout *leftVLayout1 = new QVBoxLayout();
    QVBoxLayout *leftVLayout2 = new QVBoxLayout();

    leftArrow = new QPushButton(ui->leftWidget);
    leftVLayout1->addWidget(leftArrow);
    leftVLayout1->addStretch();
    leftArrow->setIcon(QIcon(":/img/arrow_right.png")); // т.к. изначально будет показан упрощённый список.
    leftArrow->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    //leftArrow->setAlignment(Qt::AlignTop);
    leftArrow->setFlat(true);
    leftArrow->setStyleSheet("QPushButton:default{border:none;}"
                             "QPushButton:flat{border:none;}"
                             "QPushButton:checked{border:none;}");
    leftArrow->setCursor(Qt::PointingHandCursor);
    connect(leftArrow,SIGNAL(clicked()),this,SLOT(onLeftArrowClicked()));
    leftLayout->addLayout(leftVLayout1);

    treeWidget = new QTreeWidget(ui->leftWidget);
    treeWidget2 = new QTreeWidget(ui->leftWidget);
    treeWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    treeWidget2->setMaximumWidth(35);
    treeWidget2->setMinimumWidth(35);
    treeWidget2->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    leftVLayout2->addWidget(treeWidget);
    leftVLayout2->addWidget(treeWidget2);
    //leftVLayout2->addStretch();
    leftLayout->insertLayout(0,leftVLayout2);
    treeWidget->setStyleSheet("QTreeWidget"
                              "{"
                                  "border: none;"
                                  "icon-size: 20px"
                              "}"
                              "QTreeView::branch:has-children:opened"
                              "{"
                                   "image: url(:/img/arrow_open.png);"
                              "}"
                              "QTreeView::branch:has-children:closed"
                              "{"
                                   "image: url(:/img/arrow_close.png);"
                              "}"
                              "QTreeView::branch:!has-children:opened"
                              "{"
                                   "image: none;"
                              "}"
                              "QTreeView::branch:!has-children:closed"
                              "{"
                                   "image: none;"
                              "}"
                              "QTreeWidget::item"
                              "{"
                                  "border: none;"
                                  "padding-top: 5;"
                                  "padding-bottom: 5;"
                              "}"
                              "QTreeWidget::item:has-children"
                              "{"
                                  "text-decoration: underline;"
                                  "color: rgb(100, 100, 100);"
                              "}"
                              "QTreeWidget::item:has-children:hover"
                              "{"
                                  "background-color: rgb(255, 255, 255);"
                              "}"
                              "QTreeWidget::item:!has-children:hover"
                              "{"
                                  "background-color: rgb(23, 111, 193);"
                              "}"
                              "QTreeWidget::item:has-children:selected"
                              "{"
                                  "background-color: rgb(255, 255, 255);"
                                  "color: rgb(100,100,100);"
                              "}"
                              "QTreeWidget::item::!has-children:selected"
                              "{"
                                  "background-color: rgb(139, 183, 224);"
                                  "color: rgb(0,0,0);"
                              "}");
    // Копируем стиль из основного списка в дополнительный:
    treeWidget2->setStyleSheet(treeWidget->styleSheet());
    treeWidget2->setIndentation(0);
    treeWidget2->setItemsExpandable(false);
    treeWidget->setFont(QFont("Candara",11));
    treeWidget->setColumnCount(1);
    treeWidget2->setColumnCount(1);
    treeWidget->setHeaderHidden(true);
    treeWidget2->setHeaderHidden(true);
    treeWidget->setCursor(Qt::PointingHandCursor);
    treeWidget2->setCursor(Qt::PointingHandCursor);
    // Чтобы не отображалась пунктирная рамка:
    treeWidget->setFocusPolicy(Qt::NoFocus);
    treeWidget2->setFocusPolicy(Qt::NoFocus);
    treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    treeWidget2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Изначально показан упрощённый список и оба списка не доступны.
    treeWidget->hide();
    treeWidget->setEnabled(false);
    treeWidget2->setEnabled(false);

    recreateFactories();

    // Для перетаскивания элементов с панели слева надо ловить касание
    // мышью.
    //connect(treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            //this,SLOT(onTreeItemPressed(QTreeWidgetItem*,int)));
    //connect(treeWidget2,SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            //this,SLOT(onTreeItemPressed(QTreeWidgetItem*,int)));
    // Для перетаскивания внутри экрана надо ловить полный клик, а появляется
    // виджет всегда вконце.
    connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onTreeItemClicked(QTreeWidgetItem*,int)));
    connect(treeWidget2,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onTreeItemClicked(QTreeWidgetItem*,int)));

//---------------------------------------------------------------------------
//                              Правый виджет
//---------------------------------------------------------------------------

    ui->rightWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->rightWidget->setStyleSheet("QWidget#"+ui->rightWidget->objectName()+"{"
                                   "border-top: 1px solid rgb(170,170,170);"
                                   "border-bottom: 1px solid rgb(170,170,170);"
                                   "border-left: 1px solid rgb(170,170,170);"
                                   "border-top-left-radius: 4px;"
                                   "border-bottom-left-radius: 4px;"
                                  "}");
    ui->rightWidget->setMaximumWidth(300);
    ui->rightWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    QHBoxLayout *rightLayout = new QHBoxLayout(ui->rightWidget);
    QVBoxLayout *rightVLayout1 = new QVBoxLayout();
    QVBoxLayout *rightVLayout2 = new QVBoxLayout();

    rightArrow = new QPushButton(ui->rightWidget);
    rightVLayout1->addWidget(rightArrow);
    rightVLayout1->addStretch();
    rightArrow->setIcon(QIcon(":/img/arrow_left.png")); // т.к. изначально виджет убран.
    rightArrow->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    rightArrow->setFlat(true);
    rightArrow->setStyleSheet("QPushButton:default{border:none;}"
                             "QPushButton:flat{border:none;}"
                             "QPushButton:checked{border:none;}");
    rightArrow->setCursor(Qt::PointingHandCursor);
    connect(rightArrow,SIGNAL(clicked()),this,SLOT(onRightArrowClicked()));
    rightLayout->addLayout(rightVLayout1);

    labTableCaption = new QLabel(ui->rightWidget);
    labTableCaption->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    rightVLayout2->addWidget(labTableCaption);
    labTableCaption->setAlignment(Qt::AlignCenter);
    labTableCaption->setFont(QFont("Candara",11));
    setRightWidgetCaption(false);

    tableWidget = new QTableWidget(ui->rightWidget);
    tableWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    rightVLayout2->addWidget(tableWidget);
    rightLayout->addLayout(rightVLayout2);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->setStyleSheet("QTableWidget"
                               "{"
                                   "border: none;"
                                   "selection-background-color: rgb(139, 183, 224);"
                                   "gridline-color: white;"
                               "}"
                               "QTableWidget::item"
                               "{"
                                   //"background-color:"
                                      //"qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5,"
                                      //"stop: 0 rgb(240,240,240), stop: 1 rgb(255,255,255));"
                                   "border: 1px solid rgb(220,220,220);"
                               "}"
                               "QTableWidget::item:selected"
                               "{"
                                   "border: 1px solid white;"
                                   "background-color: rgb(139, 183, 224);"
                                "}"
                                "QTableWidget::item:focus"
                                "{"
                                      //"border: 2px solid red;"
                                 "}"
                               );
    tableWidget->setFont(QFont("Candara",11));
    tableWidget->verticalHeader()->hide();
    tableWidget->horizontalHeader()->hide();
    tableWidget->setRowCount(0);
    //tableWidget->setRowCount(8);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);

    // Изначально правый виджет убран.
    tableWidget->hide();
    labTableCaption->hide();

//---------------------------------------------------------------------------
//                              Центральный виджет
//---------------------------------------------------------------------------

    // Эти виджеты нужны для того, чтобы разносить по углам левое и правое меню
    // в тот момент, когда центральный виджет станет фиксированной ширины и
    // особенно когда левое и правое меню при этом свернутся пользователем.
    // Но если мы хотим, чтобы центральный виджет был на всю ширину свободного
    // пространства - эти виджеты надо спрятать.
    ui->leftStretch->hide();
    ui->leftStretch->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
    ui->leftStretch->setMinimumWidth(0);
    ui->rightStretch->hide();
    ui->rightStretch->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
    ui->rightStretch->setMinimumWidth(0);

    ui->centerWidget->setStyleSheet("QWidget {"
                                        "background-color: rgb(238,238,238);"
                                    "}");
    ui->centerWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->centerWidget->setMinimumWidth(100);
    ui->centerWidget->setMinimumHeight(200);

    layCenter = new QVBoxLayout(ui->centerWidget);
    layCenter->setContentsMargins(0,0,0,0);
    layCenter->setSpacing(0);

    // TODO: ...
    // Делаем чтобы центральный виджет перекрывался с боков левым и
    // правым меню.
    // Сделать это так: боковые виджеты сделать фиксированной длины (для обычного
    // и свёрнутого состояния) и привязать их к краям окна. А центральный виджет
    // поместить в самый низ иерархии и сделать его expanding.

//---------------------------------------------------------------------------
//                              Нижняя строка
//---------------------------------------------------------------------------

    ui->label->setFont(QFont("Candara",11));
    ui->label->setStyleSheet("QLabel {color: rgb(100,100,100)}");
    ui->label->setText(tr("  Создайте новый или откройте существующий проект ..."));

//---------------------------------------------------------------------------
//                              Диалоги
//---------------------------------------------------------------------------

    dlgProgress = new FBProgressDialog (this);

//---------------------------------------------------------------------------
//                              О программе
//---------------------------------------------------------------------------

    QHBoxLayout *layAbout = new QHBoxLayout(tabAbout);
    layAbout->setContentsMargins(10,5,10,5);
    layAbout->setSpacing(30);

    QLabel *labAboutText1 = new QLabel(tabAbout);
    labAboutText1->setText(tr("Версия\nпрограммы"));
    labAboutText1->setAlignment(Qt::AlignCenter);
    labAboutText1->setFont(QFont("Candara",9));
    labAboutText1->setStyleSheet("QLabel {color: rgb(170,170,170)}");
    QLabel *labAboutText2 = new QLabel(tabAbout);
    labAboutText2->setText(QString::number(FB_VERSION,'f',1));
    labAboutText2->setAlignment(Qt::AlignCenter);
    labAboutText2->setFont(QFont("Candara",18));
    labAboutText2->setStyleSheet("QLabel {color: rgb(100,100,100)}");
    QVBoxLayout *vAbout1 = new QVBoxLayout();
    vAbout1->setContentsMargins(0,0,0,0);
    vAbout1->setSpacing(5);
    vAbout1->addWidget(labAboutText2);
    vAbout1->addWidget(labAboutText1);
    layAbout->addLayout(vAbout1);

    QLabel *labAboutImg = new QLabel(tabAbout);
    QPixmap pixAbout = QPixmap(":/img/nextgis.png");
    labAboutImg->setPixmap(pixAbout);
    labAboutImg->setAlignment(Qt::AlignCenter);
    QLabel *labAboutUrl = new QLabel(tabAbout);
    labAboutUrl->setAlignment(Qt::AlignCenter);
    labAboutUrl->setText(QString("<a href=\"") + FB_NEXTGIS_URL + QString("\">")
                         + tr("Скачать последнюю версию програмы") + "</a>");
    labAboutUrl->setTextFormat(Qt::RichText);
    labAboutUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labAboutUrl->setOpenExternalLinks(true);
    QVBoxLayout *layAbout3 = new QVBoxLayout();
    layAbout3->setContentsMargins(0,0,0,0);
    layAbout3->setSpacing(5);
    layAbout3->addWidget(labAboutImg);
    layAbout3->addWidget(labAboutUrl);
    layAbout->addLayout(layAbout3);

    layAbout->addStretch();

    QVBoxLayout *vAbout2 = new QVBoxLayout();
    vAbout2->setContentsMargins(0,0,0,0);
    vAbout2->setSpacing(5);
    butAboutGraphics = createMainMenuButton(tabAbout,vAbout2,
       ":/img/cc.png",tr("Показать авторов"));
    connect(butAboutGraphics,SIGNAL(clicked()),this,SLOT(onAboutGraphicsClick()));
    QLabel *labAboutUrlL = new QLabel(tabAbout);
    labAboutUrlL->setAlignment(Qt::AlignCenter);
    labAboutUrlL->setFont(QFont("Candara",9));
    labAboutUrlL->setText(tr("Изображения\nв программе"));
    labAboutUrlL->setStyleSheet("QLabel {color: rgb(170,170,170)}");
    vAbout2->addWidget(labAboutUrlL);
    layAbout->addLayout(vAbout2);
}


void FB::recreateFactories ()
{
    treeWidget->clear();

        QTreeWidgetItem *decorationItem = new QTreeWidgetItem();
        decorationItem->setText(0,tr("Оформление"));
        treeWidget->addTopLevelItem(decorationItem);
        QTreeWidgetItem *inputItem = new QTreeWidgetItem();
        inputItem->setText(0,tr("Ввод данных"));
        treeWidget->addTopLevelItem(inputItem);
    //    QTreeWidgetItem *groupItem = new QTreeWidgetItem();
    //    groupItem->setText(0,tr("Группировка"));
    //    treeWidget->addTopLevelItem(groupItem);
    //    QTreeWidgetItem *positionItem = new QTreeWidgetItem();
    //    positionItem->setText(0,tr("Расположение"));
    //    treeWidget->addTopLevelItem(positionItem);

        FBFactory *item;
        FBFactory *item2;

        //(decorationItems,":/img/text.png",tr("Текст"));
        item = new FBTextLabelFactory(true);
        decorationItem->addChild(item);
        item2 = new FBTextLabelFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(decorationItems,":/img/image.png",tr("Изображение"));

        //(decorationItems,":/img/space.png",tr("Пробел"));
        item = new FBSpaceFactory(true);
        decorationItem->addChild(item);
        item2 = new FBSpaceFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/textedit.png",tr("Текстовое поле"));
        item = new FBTextEditFactory(true);
        inputItem->addChild(item);
        item2 = new FBTextEditFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/combo.png",tr("Список"));
        item = new FBComboBoxFactory(true);
        inputItem->addChild(item);
        item2 = new FBComboBoxFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/double_combo.png",tr("Двойной список"));
        item = new FBDoubleComboBoxFactory(true);
        inputItem->addChild(item);
        item2 = new FBDoubleComboBoxFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/check.png",tr("Флажок"));
        item = new FBCheckBoxFactory(true);
        inputItem->addChild(item);
        item2 = new FBCheckBoxFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/radio.png",tr("Радио-группа"));
        item = new FBRadioGroupFactory(true);
        inputItem->addChild(item);
        item2 = new FBRadioGroupFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/compass.png",tr("Компас"));

        //(inputItems,":/img/button.png",tr("Кнопка"));
        item = new FBButtonFactory(true);
        inputItem->addChild(item);
        item2 = new FBButtonFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(inputItems,":/img/date.png",tr("Дата и время"));
        item = new FBDateTimeFactory(true);
        inputItem->addChild(item);
        item2 = new FBDateTimeFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //(groupItems,":/img/tab.png",tr("Вкладки"));

        //(groupItems,":/img/group_start.png",tr("Начало группы"));

        //(groupItems,":/img/group_end.png",tr("Конец группы"));

        //(positionItems,":/img/ver_layout.png",tr("Раскладка Вер"));

        //(positionItems,":/img/hor_layout.png",tr("Раскладка Гор"));
}


// Очистка экрана -> преобразование к пустому общему экрану.
// После этого надо вызвать метод создания конкретного экрана.
void FB::clearScreen ()
{
    // Удаляем все составляющие "экрана".
    // Их внутренние виджеты (в т.ч. элементы интерфейса) будут
    // удалены автоматически.
    // Не забываем, что сам проект удаляется в другом месте!
    QLayoutItem *child;
    while ((child = layCenter->takeAt(0)) != 0)
    {
        delete child;
    }
    treeWidget->collapseAll();
    treeWidget->setEnabled(false);
    treeWidget2->setEnabled(false);
}


// Создание Андроид-экрана с удалением предыдущего экрана.
void FB::recreateAndroidScreen ()
{
    clearScreen();

    // IMPORTANT TODO: удостовериться, что удаление элементов раскладки при очистке
    // экрана прошло упешно и что тут можно выделять память под новые элементы.

    // Верхняя первая декоративная линия:
    QLabel *labDecorTop = new QLabel(ui->centerWidget);
    labDecorTop->setStyleSheet("QLabel"
                               "{"
                                   "background-color: black;"
                                   "border-top-left-radius: 4px;"
                                   "border-top-right-radius: 4px;"
                               "}");
    labDecorTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labDecorTop->setFixedHeight(15);
    layCenter->addWidget(labDecorTop);
    QHBoxLayout *layDecorTop = new QHBoxLayout(labDecorTop);
    layDecorTop->setContentsMargins(0,0,0,0);
    layDecorTop->setSpacing(0);
    QLabel *labDecorTop10 = new QLabel(labDecorTop);
    labDecorTop10->setPixmap(QPixmap(":/img/decor_andr_5.png"));
    labDecorTop10->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop11 = new QLabel(labDecorTop);
    labDecorTop11->setPixmap(QPixmap(":/img/decor_andr_6.png"));
    labDecorTop11->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layDecorTop->addSpacing(5);
    layDecorTop->addWidget(labDecorTop10);
    layDecorTop->addStretch();
    layDecorTop->addWidget(labDecorTop11);
    layDecorTop->addSpacing(5);

    // Верхняя вторая декоративная линия:
    QLabel *labDecorTop2 = new QLabel(ui->centerWidget);
    labDecorTop2->setStyleSheet("QLabel{background-color: rgb(3,169,244)}");
    labDecorTop2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labDecorTop2->setFixedHeight(35);
    layCenter->addWidget(labDecorTop2);
    QHBoxLayout *layDecorTop2 = new QHBoxLayout(labDecorTop2);
    layDecorTop2->setContentsMargins(0,0,0,0);
    layDecorTop2->setSpacing(0);
    QLabel *labDecorTop20 = new QLabel(labDecorTop2);
    labDecorTop20->setPixmap(QPixmap(":/img/decor_andr_4.png"));
    labDecorTop20->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop21 = new QLabel(labDecorTop2);
    labDecorTop21->setPixmap(QPixmap(":/img/decor_andr_3.png"));
    labDecorTop21->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop22 = new QLabel(labDecorTop2);
    labDecorTop22->setPixmap(QPixmap(":/img/decor_andr_2.png"));
    labDecorTop22->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorTop23 = new QLabel(labDecorTop2);
    labDecorTop23->setPixmap(QPixmap(":/img/decor_andr_1.png"));
    labDecorTop23->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layDecorTop2->addSpacing(5);
    layDecorTop2->addWidget(labDecorTop20);
    layDecorTop2->addStretch();
    layDecorTop2->addWidget(labDecorTop21);
    layDecorTop2->addSpacing(9);
    layDecorTop2->addWidget(labDecorTop22);
    layDecorTop2->addSpacing(5);
    layDecorTop2->addWidget(labDecorTop23);
    layDecorTop2->addSpacing(5);

    // Экран:
    wScrollArea = new QScrollArea(ui->centerWidget);
    layCenter->addWidget(wScrollArea);
    wScrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(wScrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(moveScrollBarToBottom(int, int)));
    wScrollArea->setStyleSheet("QScrollArea{border: 0px;}");
    widScreen = new QWidget();
    widScreen->setStyleSheet("QWidget{background-color:rgba(0,0,0,0);}");
    wScrollArea->setWidget(widScreen);
    wScrollArea->setWidgetResizable(true);
    layScreen = new QVBoxLayout(widScreen);
    layScreen->setContentsMargins(0,0,0,0);
    layScreen->setSpacing(0);

    // В самом начале мы добавляем всего один виджет для вставки элементов.
    FBInsertWidget *insWidget = new FBInsertWidget(widScreen);
    layScreen->addWidget(insWidget);

    // Добавляем в конец раскладки растянивающий не-виджет элемент. Не забываем
    // об этом далее, т.к. будет просмотр в цикле всех элементов в раскладке
    // и преобразование к QWidget, тогда как этот элемент - не виджет!
    // Все последующие элементы-виджеты добавлять ПЕРЕД этим элементом.
    layScreen->addStretch();

    // Нижняя декоративная линия:
    QLabel *labDecorBottom = new QLabel(ui->centerWidget);
    labDecorBottom->setStyleSheet("QLabel"
                                  "{"
                                      "background-color: black;"
                                      "border-bottom-left-radius: 4px;"
                                      "border-bottom-right-radius: 4px;"
                                  "}");
    labDecorBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labDecorBottom->setFixedHeight(30);
    layCenter->addWidget(labDecorBottom);
    QHBoxLayout *layDecorBottom = new QHBoxLayout(labDecorBottom);
    layDecorBottom->setContentsMargins(0,0,0,0);
    layDecorBottom->setSpacing(0);
    QLabel *labDecorBottom1 = new QLabel(labDecorBottom);
    labDecorBottom1->setPixmap(QPixmap(":/img/decor_andr_7.png"));
    labDecorBottom1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorBottom2 = new QLabel(labDecorBottom);
    labDecorBottom2->setPixmap(QPixmap(":/img/decor_andr_8.png"));
    labDecorBottom2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QLabel *labDecorBottom3 = new QLabel(labDecorBottom);
    labDecorBottom3->setPixmap(QPixmap(":/img/decor_andr_9.png"));
    labDecorBottom3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layDecorBottom->addStretch();
    layDecorBottom->addWidget(labDecorBottom1);
    layDecorBottom->addStretch();
    layDecorBottom->addWidget(labDecorBottom2);
    layDecorBottom->addStretch();
    layDecorBottom->addWidget(labDecorBottom3);
    layDecorBottom->addStretch();

    // Устройство по умолчанию - на весь экран.
    onAndrAllClick();

    treeWidget->expandAll();
    treeWidget->setEnabled(true);
    treeWidget2->setEnabled(true);
}


// Очистка формы Андроид-экрана.
void FB::clearAndroidScreen ()
{
    // Удаляем всё, что находится в главной экранной раскладке - а именно
    // элементы интерфейса и вспомогательные виджеты для вставки.
    QLayoutItem *child;
    while ((child = layScreen->takeAt(0)) != 0)
    {
        // TODO: понять, почему здесь delete может удалить только преобразовав
        // к виджету, а в recreateVoidScreen() спокойно удаляет без этого.
        // М.б. потому что там все дети являются раскладками, которые сами удаляют
        // свои виджеты, а здесь все дети - виджеты непосредственно (кроме последнего
        // растягивающего элемента).
        // TODO: понять так же, почему не выдаёт ошибки и спокойной удаляет последний
        // растягивающий элемент - если тут идёт преобразование к виджету, а он - не
        // виджет?
        delete child->widget();
    }

    // Не забываем добавить самый первый виджет для вставки.
    FBInsertWidget *insWidget = new FBInsertWidget(widScreen);
    layScreen->addWidget(insWidget);

    // Так же не забываем добавить растягивающий элемент-невиджет, т.к. он был тоже удалён.
    layScreen->addStretch();
}


void FB::clearElemSelection ()
{
    if (FBElem::SELECTED != NULL)
    {
        FBElem::SELECTED->setDeselectStyle();
    }
    FBElem::SELECTED = NULL;
    FBElem::IS_SELECTED_MOVING = false; // TODO: проверить, обязательно ли это
    clearAttrTable();
    setRightWidgetCaption(false);
    butDeleteElem->setEnabled(false);
}


void FB::clearAttrTable ()
{
    for (int i=0; i<tableWidget->rowCount(); i++)
    {
        tableWidget->removeCellWidget(i,1);
    }
    tableWidget->clearContents();
    // TODO: Правильно таким образом очищать список строк?
    tableWidget->setRowCount(0);
}


QToolButton *FB::createMainMenuButton (QWidget *parent, QLayout *layout,
                             QString imgPath, QString text)
{
    QToolButton *but = new QToolButton(parent);
    but->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    but->setAutoRaise(true);
    but->setIcon(QIcon(imgPath));
    but->setText(text);
    but->setFont(QFont("Candara",8));
    but->setIconSize(QSize(60,60));
    but->setMaximumWidth(90);
    //but->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    but->setCursor(Qt::PointingHandCursor);
    layout->addWidget(but);
    but->setStyleSheet("QToolButton"
                       "{"
                           "border: none;"
                       "}"
                       "QToolButton:hover"
                       "{"
                           "background-color: rgb(23, 111, 193);"
                       "}"
                       "QToolButton:pressed"
                       "{"
                           "background-color: rgb(139, 183, 224);"
                       "}"
                       "QToolButton:disabled"
                       "{"

                       // Делавем полупрозрачный стиль (~20%).
                       // TODO ...

                       "}");
    but->setToolTip(text);
    return but;
}


void FB::setRightWidgetCaption (bool isElemSelected)
{
    if (isElemSelected)
    {
        labTableCaption->setStyleSheet("QLabel{border: none;color: rgb(23,111,193);}");
        if (FBElem::SELECTED != NULL)
            //labTableCaption->setText(tr("Элемент: ") + FBElem::SELECTED->getDisplayName());
            labTableCaption->setText(FBElem::SELECTED->getDisplayName());
        else
            labTableCaption->setText("...");
    }
    else
    {
        labTableCaption->setStyleSheet("QLabel{border: none;color: rgb(139,183,224);}");
        labTableCaption->setText(tr("Выберите элемент ..."));
    }
}


void FB::setLeftWidgetVisible (bool isVisible)
{
    treeWidget->setVisible(isVisible);
    treeWidget2->setVisible(!isVisible);
    if (!isVisible)
        leftArrow->setIcon(QIcon(":/img/arrow_right.png"));
    else
        leftArrow->setIcon(QIcon(":/img/arrow_left.png"));

    // Открываем и выделяем в показанном списке то, что было выделено и открыто
    // в предыдущем.
    // TODO ...
}

void FB::setRightWidgetVisible (bool isVisible)
{
    tableWidget->setVisible(isVisible);
    labTableCaption->setVisible(isVisible);
    if (!isVisible)
        rightArrow->setIcon(QIcon(":/img/arrow_left.png"));
    else
        rightArrow->setIcon(QIcon(":/img/arrow_right.png"));
}

void FB::onLeftArrowClicked ()
{
    setLeftWidgetVisible(!treeWidget->isVisible());
}

void FB::onRightArrowClicked ()
{
    setRightWidgetVisible(!tableWidget->isVisible());
}


void FB::onAndrAllClick ()
{
    ui->leftStretch->hide();
    ui->rightStretch->hide();
    ui->centerWidget->setMaximumWidth(16777215);
    ui->centerWidget->setMaximumHeight(16777215);
    butAndrAll->setDown(true);
    butPhoneAndrVer->setDown(false);
    butPhoneAndrHor->setDown(false);
    butTabletAndrVer->setDown(false);
    butTabletAndrHor->setDown(false);
}


void FB::onAndrPhoneVerClick ()
{
    ui->leftStretch->show();
    ui->rightStretch->show();
    ui->centerWidget->setMaximumWidth(350);
    ui->centerWidget->setMaximumHeight(16777215);
    butAndrAll->setDown(false);
    butPhoneAndrVer->setDown(true);
    butPhoneAndrHor->setDown(false);
    butTabletAndrVer->setDown(false);
    butTabletAndrHor->setDown(false);
}


void FB::onAndrPhoneHorClick ()
{
}


void FB::onAndrTabletVerClick ()
{
}


void FB::onAndrTabletHorClick ()
{
}


FBElem * FB::onTreeItemClicked (QTreeWidgetItem* item, int column)
{
    // Проверяем, не заголовок группы ли это (если клик по основному дереву).
    if (item->childCount() != 0)
        return NULL;

    // Создаём новый элемент и добавляем его в конец.
    FBElem *newElem = static_cast<FBFactory*>(item)->Create();
    connect(newElem,SIGNAL(moveElem(FBElem*,FBInsertWidget*)),
            this,SLOT(onMoveElem(FBElem*,FBInsertWidget*)));
    connect(newElem,SIGNAL(pressElem()),
            this,SLOT(onPressElem()));
    canScrollToEnd = true; // чтобы экран проскроллился на добавляемый элемент. См. соотв. слот.
    onAddElem(newElem);
    return newElem;
}


void FB::onAddElem (FBElem *elem)
{
    // Добавляем элемент в конец, если была нажата кнопка в левом меню и
    // был создан новый элемент. В этом случае верхний виджет для вставки
    // гарантированно имеется.
    // На самом деле вызываем не addWidget(), а insertWidget() и вставляем
    // элемент на предпоследнее место, т.к. последним всегда идёт растягивающий
    // не-виджет элемент.
    layScreen->insertWidget(layScreen->count()-1,elem);
    FBInsertWidget *insWidget2 = new FBInsertWidget(widScreen);
    layScreen->insertWidget(layScreen->count()-1,insWidget2);

    // Выделяем элемент сразу после добавления.
    elem->mousePressEvent(NULL);
    elem->mouseReleaseEvent(NULL);
}


void FB::onMoveElem (FBElem *elem, FBInsertWidget *insWidget)
{
    int indexOfElemWidget = layScreen->indexOf(elem);
    int indexOfInsWidget = layScreen->indexOf(insWidget);

    // Проверяем индексы: технически нельзя переносить элемент ниже/выше
    // себя на 1 шаг.
    if ((indexOfElemWidget - indexOfInsWidget == -1)
      || (indexOfElemWidget - indexOfInsWidget == 1))
        return;

    // Сначала удаляем элемент и виджет для вставки, идущий за ним, а потом
    // вставляем ПОСЛЕ выбранного мышью виджета для вставки эту самую пару.
    FBInsertWidget* insWid
            = (FBInsertWidget*)(layScreen->itemAt(indexOfElemWidget+1)->widget());
    layScreen->removeWidget(elem); // ownership виджета не сменится, поэтому можно будет его заного добавить
    layScreen->removeWidget(insWid);
    // Индексы в раскладке автоматически изменились.
    indexOfInsWidget = layScreen->indexOf(insWidget);
    layScreen->insertWidget(indexOfInsWidget+1,elem);
    layScreen->insertWidget(indexOfInsWidget+2,insWid);
}


void FB::onPressElem ()
{
    // Подготавливаем таблицу для вывода атрибутов.
    clearAttrTable();

    // Выводим артибуты элемента в правую колонку.
    QMap<QString,FBAttr*>::const_iterator it = FBElem::SELECTED->mapAttrs.constBegin();
    int i=0;
    while (it != FBElem::SELECTED->mapAttrs.constEnd())
    {
        int rowCount = tableWidget->rowCount();
        tableWidget->setRowCount(rowCount + 1);

        // Выводим алиас.
        QString alias = it.key();
        QTableWidgetItem *itemAlias;
        itemAlias = new QTableWidgetItem(alias);
        itemAlias->setFlags(Qt::ItemIsEditable);
        // TODO: выводить посказку:
        //itemAlias->setToolTip();
        tableWidget->setItem(i,0,itemAlias);

        // Выводим виджет для редактирования со значением атрибута, если оно есть.
        QWidget *widget = it.value()->getWidget();
        tableWidget->setCellWidget(i,1,widget);

        i++;
        ++it;
    }

    setRightWidgetCaption(true);

    butDeleteElem->setEnabled(true);
}


void FB::onClearScreenClick ()
{
    butClearScreen->setDown(true);
    int ret = showAlertBox(tr("Очистить экран от всех элементов?"));
    if (ret == QMessageBox::Ok)
    {
        clearElemSelection();
        clearAndroidScreen();
    }
    butClearScreen->setDown(false);
}


void FB::onDeleteElemClick ()
{
    if (FBElem::SELECTED == NULL)
        return;
    int indexOfElemWidget = layScreen->indexOf(FBElem::SELECTED);
    clearElemSelection();
    // Удаляем пару - сам виджет элемента и виджет для вставки, идущий за ним,
    // который как раз создавался при создании элемента.
    QWidget *w;
    w = layScreen->takeAt(indexOfElemWidget)->widget();
    delete w;
    w = layScreen->takeAt(indexOfElemWidget)->widget(); // индекс виджета для вставки стал таким же
    delete w;
}


// Нужно, чтобы после каждого добавления элемента, скроллилось в конец.
// Простым setValue() этого не сделать.
// http://www.qtcentre.org/threads/32852-How-can-I-always-keep-the-scroll-bar-at-the-bottom-of-a-QScrollArea
void FB::moveScrollBarToBottom (int min, int max)
{
    //Q_UNUSED(min);
    if (canScrollToEnd)
    {
        wScrollArea->verticalScrollBar()->setValue(max);
        canScrollToEnd = false;
    }
}


void FB::showMsgBox (QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Сообщение"));
    msgBox.exec();
}


int FB::showAlertBox (QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setWindowTitle(tr("Предупреждение"));
    return msgBox.exec();
}


void FB::onAboutGraphicsClick ()
{
    FBAboutDialog dialog(this);
    dialog.exec();
}


void FB::onNewShpClick ()
{
    butNewShp->setDown(true);
    onNewAnyClick(false);
    butNewShp->setDown(false);
}
void FB::onNewNgwClick ()
{
    butNewNgw->setDown(true);
    onNewAnyClick(true);
    butNewNgw->setDown(false);
}
void FB::onNewAnyClick (bool isNgw)
{
    // Выдаём предупреждение, что уже есть открытый проект.
    if (FB::CUR_PROJECT != NULL)
    {
        int ret = showAlertBox(tr("При создании нового проекта все"
                            " несохранённые изменения в текущем проекте будут"
                            " потеряны. Продолжить?"));
        if (ret != QMessageBox::Ok)
        {
            butNewNgw->setDown(false);
            return;
        }
    }

    // Создаём диалог для выбора источника данных.
    QDialog *dialog;
    if (isNgw)
    {
        dialog = new FBNgwDialog(this);
    }
    else
    {
        dialog = new QFileDialog(this);
        static_cast<QFileDialog*>(dialog)->setAcceptMode(QFileDialog::AcceptOpen);
        static_cast<QFileDialog*>(dialog)->setFileMode(QFileDialog::ExistingFile);
        static_cast<QFileDialog*>(dialog)->setViewMode(QFileDialog::List);
        static_cast<QFileDialog*>(dialog)->setDefaultSuffix("shp");
        static_cast<QFileDialog*>(dialog)->setNameFilter("*.shp");
        static_cast<QFileDialog*>(dialog)->setLabelText(QFileDialog::LookIn,tr("Путь:"));
        static_cast<QFileDialog*>(dialog)->setLabelText(QFileDialog::FileName,tr("Имя файла"));
        static_cast<QFileDialog*>(dialog)->setLabelText(QFileDialog::FileType,tr("Тип файла"));
        static_cast<QFileDialog*>(dialog)->setLabelText(QFileDialog::Accept,tr("Выбрать"));
        static_cast<QFileDialog*>(dialog)->setLabelText(QFileDialog::Reject,tr("Отмена"));
        static_cast<QFileDialog*>(dialog)->setWindowTitle(tr("Создание нового проекта: выберите Shapefile ..."));
        static_cast<QFileDialog*>(dialog)->setDirectory(QDir()); //ставим текущую директорию
        #ifdef FB_TEST
        static_cast<QFileDialog*>(dialog)->setDirectory(QDir("D:\\nextgis\\formbuilder-2.0\\test_data"));
        #endif
    }

    // Запускаем диалог и пользователь делает выбор.
    if (dialog->exec())
    {
        QString strPath;
        QString strUrl, strLogin, strPass, strId;
        Json::Value jsonMeta;
        if (isNgw)
        {
            strPath = static_cast<FBNgwDialog*>
                    (dialog)->selectedNgwResource(strUrl,strLogin,strPass,strId,jsonMeta);
        }
        else
        {
            QStringList sPaths = static_cast<QFileDialog*>(dialog)->selectedFiles();
            strPath = sPaths[0];
        }

        // Создаём новый проект, пока не делая его текущим, т.к. в процессе инициализации
        // (например чтения ИД через GDAL) могут быть ошибки.
        FBProject* newProject = new FBProject(this);
        connect(newProject,SIGNAL(sendMsg(QString)),
                this,SLOT(showMsgBox(QString)));
        connect(newProject,SIGNAL(changeProgress(int)),
                dlgProgress,SLOT(onChangeProgress(int)));

        // Инициализируем проект. В процессе могут быть ошибки, как логические (не
        // поддерживаемые свойства проекта), так и физические (не удалось считать
        // выбранный слой через GDAL).
        QByteArray baPath = strPath.toUtf8();
        bool ok = false;
        if (isNgw)
        {
            ok = newProject->initFromNgw(baPath.data(),strUrl,strLogin,strPass,strId,jsonMeta);
        }
        else
        {
            ok = newProject->init(baPath.data());
        }

        if (!ok)
        {
            // Сообщения уже были выведены, можно просто удалить несоздавшийся проект.
            delete newProject;
        }
        else
        {
            // Ставим текущим проектом только что созданный, удаляя старый.
            if (FB::CUR_PROJECT != NULL)
                delete FB::CUR_PROJECT;
            FB::CUR_PROJECT = newProject;

            // Выводим информацию об открытом проекте.
            setBottomProjectString(baPath.data());

            // Очищаем старый экран и изменяем интерфейс.
            clearElemSelection();
            recreateAndroidScreen();
            setLeftWidgetVisible(true);
            setRightWidgetVisible(true);
            butSave->setEnabled(false);
            butSaveAs->setEnabled(true);
            butClearScreen->setEnabled(true);
        }
    }

    delete dialog;
}


void FB::onOpenClick ()
{
    butOpen->setDown(true);

    if (FB::CUR_PROJECT != NULL)
    {
        int ret = showAlertBox(tr("При открытии существующего проекта все"
                            " несохранённые изменения в текущем проекте будут"
                            " потеряны. Продолжить?"));
        if (ret != QMessageBox::Ok)
        {
            butOpen->setDown(false);
            return;
        }
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*."+QString(FB_PROJECT_EXTENSION));
    dialog.setLabelText(QFileDialog::LookIn,tr("Путь:"));
    dialog.setLabelText(QFileDialog::FileName,tr("Имя файла"));
    dialog.setLabelText(QFileDialog::FileType,tr("Тип файла"));
    dialog.setLabelText(QFileDialog::Accept,tr("Открыть"));
    dialog.setLabelText(QFileDialog::Reject,tr("Отмена"));
    dialog.setWindowTitle(tr("Открыть проект. Выберите файл"
                  " с расширением .") + QString(FB_PROJECT_EXTENSION) + " ...");
    dialog.setDirectory(QDir());
    #ifdef FB_TEST
    dialog.setDirectory(QDir("D:\\nextgis\\formbuilder-2.0\\test_forms"));
    #endif

    if (dialog.exec())
    {
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("." + QString(FB_PROJECT_EXTENSION),Qt::CaseInsensitive))
        {
            showMsgBox(tr("Неверное расширение. Выберите файл с расширением .")
                          + QString(FB_PROJECT_EXTENSION));
        }
        else
        {
            // Далее всё аналогично созданию нового проекта.
            // TODO: возможно объединить эти два метода в один.

            FBProject* openProject = new FBProject(this);
            connect(openProject,SIGNAL(sendMsg(QString)),
                    this,SLOT(showMsgBox(QString)));
            connect(openProject,SIGNAL(changeProgress(int)),
                    dlgProgress,SLOT(onChangeProgress(int)));

            Json::Value jsonForm;

            if (!openProject->open(strFullPath,jsonForm))
            {
                delete openProject;
            }
            else
            {
                if (FB::CUR_PROJECT != NULL)
                    delete FB::CUR_PROJECT;
                FB::CUR_PROJECT = openProject;

                clearElemSelection();
                recreateAndroidScreen();

                fillForm(jsonForm); // все проверки уже были сделаны. Выводим то что есть.

                setBottomProjectString("");
                setLeftWidgetVisible(true);
                setRightWidgetVisible(true);
                butSave->setEnabled(true);
                butSaveAs->setEnabled(true);
                butClearScreen->setEnabled(true);
            }
        }
    }

    butOpen->setDown(false);
}


// Сохранить.
void FB::onSaveClick ()
{
    butSave->setDown(true);

    // Формируем полное имя для сохранения файла каждый раз тут, т.к. в классе хранятся только
    // базовые компоненты имени.
    QString strFullPath;
    strFullPath = CUR_PROJECT->getFullPath();

    // Сохранение аналогично Сохранить Как, т.к. в зипе нельзя перезаписывать отдельный файл
    // и приходится переписывать всё.
    dlgProgress->setWindowTitle(tr("Сохранение ..."));
    FBSaveAsThread *thread = new FBSaveAsThread(this,strFullPath);
    connect(thread, &FBSaveAsThread::resultReady, this, &FB::onSaveAsEnded);
    connect(thread, &FBSaveAsThread::finished, thread, &QObject::deleteLater);
    thread->start();
    dlgProgress->exec();
}


// Сохранить как.
void FB::onSaveAsClick ()
{
    butSaveAs->setDown(true);

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*." + QString(FB_PROJECT_EXTENSION));
    dialog.setLabelText(QFileDialog::LookIn,tr("Путь:"));
    dialog.setLabelText(QFileDialog::FileName,tr("Имя файла"));
    dialog.setLabelText(QFileDialog::FileType,tr("Тип файла"));
    dialog.setLabelText(QFileDialog::Accept,tr("Сохранить"));
    dialog.setLabelText(QFileDialog::Reject,tr("Отмена"));
    dialog.setWindowTitle(tr("Сохранить проект как ..."));
    dialog.setDirectory(QDir(CUR_PROJECT->getStrPath())); // для удобства
#ifdef FB_TEST
    dialog.setDirectory(QDir("D:\\nextgis\\formbuilder-2.0\\test_forms"));
#endif
    dialog.selectFile(QString(FB_INIT_PROJECT_NAMEBASE)
                      + "." + QString(FB_PROJECT_EXTENSION));

    if (dialog.exec())
    {
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("."+QString(FB_PROJECT_EXTENSION),Qt::CaseInsensitive))
            strFullPath = strFullPath+"."+QString(FB_PROJECT_EXTENSION);

        // Начинаем новый поток, т.к. этот метод в перспективе - долгий и создаём
        // для этого диалог, который заблокирует интерфейс приложения и будет показывать
        // прогресс.
        // Почему надо блокировать:
        // Пользователь может нажимать на разные виджеты в приложении и все они
        // ОБЯЗАТЕЛЬНО получат события мыши и в них произойдут изменнения как только диалог
        // завершится. Через потоки - единственный способ при котором мне удалось этого не
        // допустить.
        // Что не помогло:
        // 1. Как написано в документации Qt: QProgressDialog pr(...); pr->show();
        // pr.setWindowModality(Qt::WindowModal).
        // 2. QApplication::processEvents(QEventLoop::ExcludeUserInputEvents) в дополнении к 1.
        // 3. Через блокировку всего окна приложения this->setEnabled(false) или всех виджетов
        // по отдельности. Виджеты всё равно получают события мыши.
        dlgProgress->setWindowTitle(tr("Сохранение ..."));
        FBSaveAsThread *thread = new FBSaveAsThread(this,strFullPath);
        connect(thread, &FBSaveAsThread::resultReady, this, &FB::onSaveAsEnded);
        connect(thread, &FBSaveAsThread::finished, thread, &QObject::deleteLater);
        thread->start();
        // TODO: проверить ещё (вроде ok) что можно вызывать exec() у диалога здесь, тогда
        // как с ним внутри потока уже может что-то происходить сейчас.
        dlgProgress->exec();

        // Дальше работа в потоке, а обработка результатов в специальном методе.
    }
    else // else нужен, потому что до сюда не должно доходить, когда начался поток с долгим методом
    {
        butSaveAs->setDown(false);
    }
}

// Для обработки результатов долгих действий в потоках:
void FB::onSaveAsEnded (bool result)
{
    // Закрываем диалог и делаем нужные действия.
    dlgProgress->accept();
    if (result)
    {
        butSave->setEnabled(true);
        showMsgBox(tr("Проект успешно сохранён"));
        setBottomProjectString("");
    }
    else
    {

    }
    butSave->setDown(false);
    butSaveAs->setDown(false);
}


void FB::setBottomProjectString (QString datasetPath)
{
    QString pathString;
    int limit;
    if (datasetPath == "")
    {
        pathString = CUR_PROJECT->getFullPath();
        limit = FB_LIMIT_BOTTOM_STRING_LEN_LONG;
    }
    else
    {
        pathString = datasetPath;
        limit = FB_LIMIT_BOTTOM_STRING_LEN_SHORT;
    }

    if (pathString.size() > limit)
    {
        pathString.remove(0,pathString.size()-limit);
        pathString.prepend("...");
    }

    if (datasetPath == "")
        ui->label->setText(tr("Текущий проект: ") + pathString);
    else
        ui->label->setText(tr("Используется источник данных: ") + pathString
                           + tr(". Сохраните проект."));
}


Json::Value FB::formToJson()
{
    // Считываем форму в json-объект.
    Json::Value jsonRootArray;

    // Используем метод QLayout::itemAt(int index) для просмотра всех виджетов
    // в раскладке с последующим доставанием координаты У и сортировке по ней. После
    // чего формируем итоговый json формы для сохранения.

    // Формируем словарь, который будет автоматически упорядочен по ключу, где ключ -
    // Y координата элемента в расклаке.
    QMap<int,FBElem*> mapElems;
    QVBoxLayout *layScreenPtr = getLayScreenPtr();
    for (int i = 0; i < layScreenPtr->count(); ++i)
    {
        QWidget *w = layScreenPtr->itemAt(i)->widget();
        FBElem *e = qobject_cast<FBElem*>(w);
        if (e != NULL)
        {
            // Y координата должна быть уникальной.
            // TODO: подумать как быть с вложенными элементами - ведь их Y м.б.
            // может совпадать.
            mapElems.insert(e->y(),e);
        }
    }

    // Просматриваем словарь и записываем элементы в итоговый json-массив.
    QByteArray ba;
    QMap<int,FBElem*>::const_iterator it = mapElems.constBegin();
    while (it != mapElems.constEnd())
    {
        FBElem* elem = it.value();
        Json::Value jsonElem;

        ba = elem->getJsonName().toUtf8();
        jsonElem[FB_JSON_TYPE] = ba.data();

        Json::Value jsonAttrs;
        QMap<QString,FBAttr*>::const_iterator it2 = elem->mapAttrs.constBegin();
        //for (int i = 0; i < elem->listAttrs.size(); i++)
        while (it2 != elem->mapAttrs.constEnd())
        {
            //FBAttr *attr = elem->listAttrs[i].second;
            FBAttr *attr = it2.value();
            ba = attr->strJsonName.toUtf8();
            jsonAttrs[ba.data()] = attr->toJson();
            ++it2;
        }
        jsonElem[FB_JSON_ATTRIBUTES] = jsonAttrs;

        // TODO: добавить тут запись тега Элементы.
        // jsonElem[FB_JSON_ELEMENTS] = ;

        jsonRootArray.append(jsonElem);
        ++it;
    }

    return jsonRootArray;
}


// Вызывать этот метод только при пустом экране!!!
// Элементы для создания (фабрики) в левом tree-widget-е должны быть уже созданы!!!
void FB::fillForm (Json::Value jsonForm)
{
    if (jsonForm.isNull())
        return;

    // Считывая элементы формы, добавляем по парам - элемент + виджет для вставки.
    // Первый виджет для вставки уже имеется.
    QTreeWidgetItem* item;
    for (int i=0; i<jsonForm.size(); ++i)
    {
        item = NULL;
        QString strJsonName = QString::fromUtf8(jsonForm[i][FB_JSON_TYPE].asString().data());

        for (int k=0; k<treeWidget2->topLevelItemCount(); k++)
        {
            item = treeWidget2->topLevelItem(k);
            if (static_cast<FBFactory*>(item)->getJsonName() == strJsonName)
                break; // ищем фабрику по имени элемента
        }

        if (item == NULL) // какое-то неизвестное имя элемента.
            continue;

        // Добавляем элемент на форму.
        FBElem *elem = onTreeItemClicked(item,0); // column не важен.

        // Заполняем значения атрибутов этого элемента.
        Json::Value jsonAttrs = jsonForm[i][FB_JSON_ATTRIBUTES];
        QMap<QString,FBAttr*>::const_iterator it2 = elem->mapAttrs.constBegin();
        while(it2 != elem->mapAttrs.constEnd())
        {
            QString attrName = it2.value()->strJsonName;
            QByteArray ba = attrName.toUtf8();
            Json::Value jsonAttr = jsonAttrs[ba.data()];

            if (!jsonAttr.isNull()) // какое-то неизвестное имя атрибута.
            {
                it2.value()->fromJson(jsonAttr);
            }

            ++it2;
        }

        // Обязательно изменям внешний вид элемента, т.к. сейчас он имеет стандартный,
        // а его атрибуты изменились.
        elem->updateAppearance();
    }
}


// ================================================================== //
//                         FBSaveAsThread                             //
// ================================================================== //

FBSaveAsThread::FBSaveAsThread(QObject *parent,
     QString strFullPath): QThread (parent)
{
    // Сохраняем эти параметры просто чтобы передать в метод класса FBProject.
    this->strFullPath = strFullPath;
}

void FBSaveAsThread::run()
{
    bool progress;
    progress = FB::CUR_PROJECT->saveAs(strFullPath);
    emit resultReady(progress);
}
