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

/*
static const char *file_dialog_strings[] = {
    QT_TRANSLATE_NOOP("QFileDialog", "Открыть"),
    QT_TRANSLATE_NOOP("QFileDialog", "Сохранить"),
    QT_TRANSLATE_NOOP("QFileDialog", "Отмена"),
    QT_TRANSLATE_NOOP("QFileDialog", "Тип файла"),
    QT_TRANSLATE_NOOP("QFileDialog", "Имя файла"),
    QT_TRANSLATE_NOOP("QFileDialog", "Путь:"),
    QT_TRANSLATE_NOOP("QFileDialog", "Выбрать")
};
QString global_translate(int type)
{
    return qApp->translate("QFileDialog",
           file_dialog_strings[type]);
}
*/


FB::~FB()
{
    writeSettings();

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

    CPLSetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS",FB_PROJECT_EXTENSION);

    GDALAllRegister();

    ui->setupUi(this);

    // Фиксируем стиль всего приложения через style sheets, поскольку из доков:
    // ... style authors are restricted by the different platforms' guidelines and
    // (on Windows XP and OS X) by the native theme engine.
    // При этом, несмотря на фразу из доков, работает утсановка цвета шрифта для всего
    // приложения:
    // When using Qt Style Sheets, a widget does not automatically inherit its font and
    // color setting from its parent widget.
    this->setStyleSheet("QWidget { background: white; color: black; }");

    this->layout()->setContentsMargins(0,5,0,5);
    this->layout()->setSpacing(10);

    canScrollToEnd = false;

    readSettings();

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
    QWidget *tabSettings = new QWidget();
    ui->tabWidget->addTab(tabSettings, tr(" Настройки "));
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
                                    "padding: 2px;"
                                    "color: black }"
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
    butNewVoid = createMainMenuButton(tabProject,layProject,
                        ":/img/new_void.png",tr("Новый"));
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
    butImportControls = createMainMenuButton(tabTools,layTools,
                  ":/img/import_controls.png",tr("Импортировать \nэлементы формы"));
    butImportControls->setEnabled(false);
    butUpdateData = createMainMenuButton(tabTools,layTools,
                  ":/img/update_data.png",tr("Обновить данные\nслоя"));
    butUpdateData->setEnabled(false);
    butFieldManager = createMainMenuButton(tabTools,layTools,
                  ":/img/fields.png",tr("Менеджер полей"));
    butFieldManager->setEnabled(false);
    layProject->addStretch();
    layDevice->addStretch();
    layTools->addStretch();
    connect(butNewVoid,SIGNAL(clicked()),this,SLOT(onNewVoidClick()));
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
    connect(butImportControls,SIGNAL(clicked()),this,SLOT(onImportControls()));
    connect(butUpdateData,SIGNAL(clicked()),this,SLOT(onUpdateData()));
    connect(butFieldManager,SIGNAL(clicked()),this,SLOT(onFieldManager()));

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
                                  "icon-size: 20px;"
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
                                  //"text-decoration: underline;"
                                  "color: rgb(100, 100, 100);"
                              "}"
                              "QTreeWidget::item:!has-children"
                              "{"
                                  "color: black;"
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
                              "QTreeWidget::item:!has-children:selected"
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
                                   "color: rgb(100,100,100);"
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
//                              Настройки
//---------------------------------------------------------------------------

    QHBoxLayout *laySettings = new QHBoxLayout(tabSettings);
    laySettings->setContentsMargins(10,5,10,5);
    laySettings->setSpacing(30);

    comboLang = new QComboBox(tabSettings);
    comboLang->addItem(tr("English"),QString("en_GB"));
    comboLang->addItem(tr("Русский"),QString("ru_RU"));
    // Смотрим строку, которая была задана в настройках, чтобы выбрать в этом комбобоксе
    // нужный язык.
    for (int i=0; i<comboLang->count(); i++)
    {
        // Выбранное в настройках значение будет либо en_GB (по умолчанию), либо
        // другим, но не пустым.
        if (comboLang->itemData(i,Qt::UserRole).toString() == strLastLangSelected)
        {
            comboLang->setCurrentIndex(i);
            break;
        }
    }
    connect(comboLang,SIGNAL(currentIndexChanged(int)),this,SLOT(onLanguageSelected(int)));

    QLabel *labSetts = new QLabel(tabSettings);
    labSetts->setText(tr("Язык") + ":");
    labSetts->setAlignment(Qt::AlignCenter);
    labSetts->setFont(QFont("Candara",9));
    labSetts->setStyleSheet("QLabel {color: rgb(170,170,170)}");
    labSetts->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    QHBoxLayout *vSetts1 = new QHBoxLayout();
    vSetts1->setContentsMargins(0,0,0,0);
    vSetts1->setSpacing(10);
    vSetts1->addWidget(labSetts);
    vSetts1->addWidget(comboLang);
    laySettings->addLayout(vSetts1);

    laySettings->addStretch();

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
    if (strLastLangSelected == "ru_RU")
    {
        labAboutUrl->setText(QString("<a href=\"") + FB_NEXTGIS_RU_URL + QString("\">")
                         + tr("Официальная страница программы") + "</a>");
    }
    else
    {
        labAboutUrl->setText(QString("<a href=\"") + FB_NEXTGIS_EN_URL + QString("\">")
                         + tr("Официальная страница программы") + "</a>");
    }
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
    //    groupItem->setText(0,tr("Компоновка"));
    //    treeWidget->addTopLevelItem(groupItem);
    //    QTreeWidgetItem *positionItem = new QTreeWidgetItem();
    //    positionItem->setText(0,tr("Расположение"));
    //    treeWidget->addTopLevelItem(positionItem);

        FBFactory *item;
        FBFactory *item2;

        //Текст
        item = new FBTextLabelFactory(true);
        decorationItem->addChild(item);
        item2 = new FBTextLabelFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Изображение
        //...

        //Пробел
        item = new FBSpaceFactory(true);
        decorationItem->addChild(item);
        item2 = new FBSpaceFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Текстовое поле
        item = new FBTextEditFactory(true);
        inputItem->addChild(item);
        item2 = new FBTextEditFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Список
        item = new FBComboBoxFactory(true);
        inputItem->addChild(item);
        item2 = new FBComboBoxFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Двойной список
        item = new FBDoubleComboBoxFactory(true);
        inputItem->addChild(item);
        item2 = new FBDoubleComboBoxFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Флажок
        item = new FBCheckBoxFactory(true);
        inputItem->addChild(item);
        item2 = new FBCheckBoxFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Радио-группа
        item = new FBRadioGroupFactory(true);
        inputItem->addChild(item);
        item2 = new FBRadioGroupFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Кнопка
        item = new FBButtonFactory(true);
        inputItem->addChild(item);
        item2 = new FBButtonFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Сетка кнопок с изображениями
        //...

        //Пикер с барабанами
        //...

        //Инкрементный счётчик
        //...

        //Дата и время
        item = new FBDateTimeFactory(true);
        inputItem->addChild(item);
        item2 = new FBDateTimeFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Фото
        item = new FBPhotoFactory(true);
        inputItem->addChild(item);
        item2 = new FBPhotoFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Официальная подпись
        item = new FBSignatureFactory(true);
        inputItem->addChild(item);
        item2 = new FBSignatureFactory(false);
        treeWidget2->addTopLevelItem(item2);

        //Компас
     //   item = new FBCompassFactory(true);
     //   inputItem->addChild(item);
     //   item2 = new FBCompassFactory(false);
     //   treeWidget2->addTopLevelItem(item2);

        //Вкладки
        //...

        //Группа
    //    item = new FBGroupFactory(true);
    //    groupItem->addChild(item);
    //    item2 = new FBGroupFactory(false);
    //    treeWidget2->addTopLevelItem(item2);

        //Вертикальная раскладка
        //...

        //Горизонтальная раскладка
        //...
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

    // Добавляем в конец раскладки растягивающий не-виджет элемент. Не забываем
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
    if (CUR_PROJECT == NULL)
        return;
    setLeftWidgetVisible(!treeWidget->isVisible());
}

void FB::onRightArrowClicked ()
{
    if (CUR_PROJECT == NULL)
        return;
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
    // не-виджет элемент ~ Vertical Spacer.
    layScreen->insertWidget(layScreen->count()-1, elem);
    FBInsertWidget *insWidget2 = new FBInsertWidget(widScreen);
    layScreen->insertWidget(layScreen->count()-1, insWidget2);

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
        //itemAlias->setFlags(Qt::ItemIsEditable);
        itemAlias->setFlags(Qt::ItemIsEnabled);
        itemAlias->setToolTip(alias);
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



// Переобпределённое событие - нажатие кнопки во всём приложении.
void FB::keyPressEvent (QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
    {
        onDeleteElemClick();
        return;
    }

    // Из доков: If you reimplement this handler, it is very important that you call
    // the base class implementation if you do not act upon the key.
    QWidget::keyPressEvent(event);
}



void FB::onImportControls ()
{
    butImportControls->setDown(true);

    if (FB::CUR_PROJECT != NULL)
    {
        int ret = showAlertBox(tr("При импорте элементов из другого проекта "
                                  "текущие элементы формы будут удалены. Продолжить?"));
        if (ret != QMessageBox::Ok)
        {
            butImportControls->setDown(false);
            return;
        }
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*."+QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Импортировать элементы формы. Выберите файл"
                  " с расширением .") + QString(FB_PROJECT_EXTENSION) + " ...");

    // TODO: сделать подстановку (а до этого запоминание) последнего файла для импорта?

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
            Json::Value jsonOtherForm;
            Json::Value jsonOtherMeta;

            // Открываем выбранный проект и возвращаем его форму+метаданные.
            if (CUR_PROJECT->readNgfpJsonMeta(strFullPath,jsonOtherMeta)
                && CUR_PROJECT->readNgfpJsonForm(strFullPath,jsonOtherForm))
            {
                // Очищаем форму от старых элементов. //см. onClearScreenClick();
                clearElemSelection();
                clearAndroidScreen();

                // Добавляем на форму новые элементы.
                // Но в атрибутах "поле слоя" у элементов будут записаны значения из другого
                // проекта, несмотря на то что в программе список полей слоя - для текущего
                // проекта.
                QList<FBElem*> formElems = fillForm(jsonOtherForm);

                // Заменяем значение атрибута "поле слоя" на неопределённое, если поле из
                // старого проекта не соответствует полю из текущего.
                // Просматриваем все только что добавленные элементы.
                for (int i=0; i<formElems.size(); i++)
                {
                    // Просматриваем все атрибуты элемента.
                    QMap<QString,FBAttr*>::const_iterator it = formElems[i]->mapAttrs.constBegin();
                    while(it != formElems[i]->mapAttrs.constEnd())
                    {
                        FBAttr* attr = it.value();

                        // Для каждого атрибута "Поле слоя":
                        // NOTE: если добавятся ещё атрибуты полей вида "поле слоя" - добавить
                        // их в проверку сюда.
                        if (attr->strJsonName == FB_JSON_FIELD
                            || attr->strJsonName == FB_JSON_FIELD_LEVEL1
                                || attr->strJsonName == FB_JSON_FIELD_LEVEL2 )
                        {
                            // keyname поля хранится в атрибуте.
                            QString keyname = static_cast<FBFieldAttr*>(attr)->getValue();

                            // Находим в импортируемом проекте описание этого поля по его keyname.
                            Json::Value otherJson;
                            for (int k=0; k<jsonOtherMeta[FB_JSON_META_FIELDS].size(); ++k)
                            {
                                otherJson = jsonOtherMeta[FB_JSON_META_FIELDS][k];
                                if (QString::fromUtf8(otherJson[FB_JSON_META_KEYNAME].asString().data())
                                        == keyname)
                                {
                                    break;
                                }
                            }

                            // Ищем, есть ли такое поле в текущем проекте.
                            bool found = false;
                            Json::Value jsonThisMeta = CUR_PROJECT->getJsonMeta();
                            for (int k=0; k<jsonThisMeta[FB_JSON_META_FIELDS].size(); ++k)
                            {
                                Json::Value thisJson = jsonThisMeta[FB_JSON_META_FIELDS][k];
                                if (otherJson == thisJson)
                                {
                                    found = true;
                                    break;
                                }
                            }

                            // Заменяем строку с названием старого поля на "неопределённую"
                            // строку.
                            if (!found)
                            {
                                static_cast<FBFieldAttr*>(attr)->setUndefinedValue();

                                // NOTE: В таблице атрибутов этот элемент визуально УЖЕ имеет
                                // неопределённое значение, т.к. при формировании
                                // комбобокса с выбором поля - использовался список полей
                                // слоя, связанного с ЭТИМ проектом, и метод по выдаче комбобокса -
                                // не нашёл такого названия поля среди текущих.
                                // См. FBFieldAttr::getWidget()
                            }

                        }

                        ++it;
                    }
                }

                // Выводим сообщение со списком тех полей слоя, которые не были назначены
                // в соответствие элементам.

                // TODO: доделать это.

            }

            // else - сообщения об ошибках уже выведены
        }
    }

    butImportControls->setDown(false);
}



void FB::onUpdateData ()
{
    butUpdateData->setDown(true);

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix("shp");
    dialog.setNameFilter("*.shp");
    dialog.setWindowTitle(tr("Обновить данные в проекте. Выберите Shapefile"));

    if (dialog.exec())
    {
        QStringList sPaths = dialog.selectedFiles();
        QString strShapefilePath = sPaths[0];

        // Создаём временный проект для выбранного слоя - для проверки правильности
        // его структуры.
        FBProject* tempProj = new FBProject(this);
        connect(tempProj,SIGNAL(sendMsg(QString)),
                this,SLOT(showMsgBox(QString)));
        QByteArray baPath = strShapefilePath.toUtf8();
        if (tempProj->init(baPath.data()))
        {
            // Сравниваем структуры полей текущего проекта и выбранного слоя.
            Json::Value jsonOtherMeta = tempProj->getJsonMeta();
            Json::Value jsonThisMeta = CUR_PROJECT->getJsonMeta();
            delete tempProj;
            if (jsonOtherMeta[FB_JSON_META_FIELDS] != jsonThisMeta[FB_JSON_META_FIELDS])
            {
                butUpdateData->setDown(false);
                showMsgBox(tr("Ошибка. Данные обновлены не будут так как "
                              "структура полей выбранного слоя отличается от структуры "
                              "полей текущего проекта. Выберите другой Shapefile"));
            }
            else
            {
                // Пересохраняем данные слоя.
                // Ставим путь для взятия данных слоя при сохранении - один раз,
                // после этого он будет сброшен. См. метод сохранения проекта.
                // Так же будут пересохранены и остальные файлы проекта (созданы заново),
                // т.к. в зип-архиве нельзя заменить только 1 файл.
                CUR_PROJECT->resetSrcDatasetPath(strShapefilePath);

                // Далее всё аналогично методу onSaveAsClick
                dlgProgress->setWindowTitle(tr("Сохранение ..."));
                FBSaveAsThread *thread = new FBSaveAsThread(this,CUR_PROJECT->getFullPath());
                connect(thread, &FBSaveAsThread::resultReady, this, &FB::onSaveAsEnded);
                connect(thread, &FBSaveAsThread::finished, thread, &QObject::deleteLater);
                thread->start();
                dlgProgress->exec();
            }
        }
    }
    else
    {
        butUpdateData->setDown(false);
    }
}



// Вызов менеджера полей.
void FB::onFieldManager ()
{
    butFieldManager->setDown(true);
    FBFieldManagerDialog dialog(this);

    // Если пользователь потдвердил изменения, изменяем структуру полей в программе,
    // но пока не в файле .ngfp - для этого надо сохранить проект. Именно тогда бдут
    // реально удалены (вместе с данными) и добавлены поля в реальном GeoJSON источнике
    // данных.
    if (dialog.exec())
    {
        clearElemSelection();

        // Формируем новый список полей в метаданных, просто заменяя старый.
        Json::Value json_mas;
        for (int i=0; i<dialog.tree->topLevelItemCount(); i++)
        {
            Json::Value json_mas_elem;
            QByteArray baName;
            QByteArray baType;
            baName = dialog.tree->topLevelItem(i)->text(0).toUtf8();
            baType = dialog.tree->topLevelItem(i)->text(1).toUtf8();
            json_mas_elem[FB_JSON_META_KEYNAME] = baName.data();
            json_mas_elem[FB_JSON_META_DATATYPE] = baType.data();
            json_mas_elem[FB_JSON_META_DISPLAY_NAME] = baName.data();
            json_mas.append(json_mas_elem);
        }
        CUR_PROJECT->resetJsonMetaFields(json_mas);

        // Просматриваем все элементы формы и снимаем их соответствия тем полям,
        // которые были удалены.
        QMap<int,FBElem*> mapElems = getFormElems();
        QMap<int,FBElem*>::const_iterator it1 = mapElems.constBegin();
        while (it1 != mapElems.constEnd())
        {
            FBElem* elem = it1.value();
            // Просамтриваем все атрибуты элемента, т.к. атрибутов-связей с полями
            // может быть несколько.
            QMap<QString,FBAttr*>::const_iterator it2 = elem->mapAttrs.constBegin();
            while(it2 != elem->mapAttrs.constEnd())
            {
                FBAttr* attr = it2.value();
                if (attr->strJsonName == FB_JSON_FIELD
                    || attr->strJsonName == FB_JSON_FIELD_LEVEL1
                        || attr->strJsonName == FB_JSON_FIELD_LEVEL2 )
                {
                    QString fieldName = static_cast<FBFieldAttr*>(attr)->getValue();
                    // TODO: пока что для каждого элемента смотрим весь список удалённых полей,
                    // поскольку в программе вообще нет проверок на то, что пользователь задал
                    // одно поле нескольким элементам формы. Можно исправить это в будущем, если
                    // добавятся такие проверки - исправить удалением уже найденного поля из множества.
                    if (dialog.fieldsDeleted.contains(fieldName))
                    {
                        static_cast<FBFieldAttr*>(attr)->setUndefinedValue();
                    }
                }

                ++it2;
            }

            ++it1;
        }

        // Сохраняем список удалённых полей - чтобы в последствии при сохранении всего
        // проекта отразить это на реальном GeoJSON источнике данных.
        // Добавленные поля сохранять не надо, т.к. они уже числятся в обновлённых
        // метаданных, а поскольку имена полей уникальны, это означает, что до этого
        // поля с такими именами были удалены, а значит их имена в списке удалённых.
        CUR_PROJECT->updateDeletedFields(dialog.fieldsDeleted);

    }

    butFieldManager->setDown(false);
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


void FB::onNewVoidClick ()
{
    butNewVoid->setDown(true);
    onNewAnyClick(0);
    butNewVoid->setDown(false);
}
void FB::onNewShpClick ()
{
    butNewShp->setDown(true);
    onNewAnyClick(1);
    butNewShp->setDown(false);
}
void FB::onNewNgwClick ()
{
    butNewNgw->setDown(true);
    onNewAnyClick(2);
    butNewNgw->setDown(false);
}
void FB::onNewAnyClick (int newProjectType)
{
    if (newProjectType != 0 && newProjectType != 1 && newProjectType != 2)
        return;

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
    if (newProjectType == 2)
    {
        dialog = new FBNgwDialog(this);
    }
    else if (newProjectType == 1)
    {
        dialog = new QFileDialog(this);
        static_cast<QFileDialog*>(dialog)->setAcceptMode(QFileDialog::AcceptOpen);
        static_cast<QFileDialog*>(dialog)->setFileMode(QFileDialog::ExistingFile);
        static_cast<QFileDialog*>(dialog)->setViewMode(QFileDialog::List);
        static_cast<QFileDialog*>(dialog)->setDefaultSuffix("shp");
        static_cast<QFileDialog*>(dialog)->setNameFilter("*.shp");
        static_cast<QFileDialog*>(dialog)
                ->setWindowTitle(tr("Создание нового проекта: выберите Shapefile ..."));
        // Ставим последнюю выбранную директорию, если она ещё существует. Сам ранее
        // выбранный Шейп не выделяем.
        if (strLastNewShapeFile != "")
        {
            QFileInfo fileInfo(strLastNewShapeFile);
            static_cast<QFileDialog*>(dialog)->setDirectory(fileInfo.absoluteDir());
        }
        else
        {
            static_cast<QFileDialog*>(dialog)->setDirectory(QDir());
        }
    }
    else
    {
        dialog = new FBNewVoidDialog(this);
    }

    // Запускаем диалог и пользователь делает выбор.
    if (dialog->exec())
    {
        QString str;
        QString strUrl, strLogin, strPass, strId;
        Json::Value jsonMeta;

        if (newProjectType == 2)
        {
            str = static_cast<FBNgwDialog*>
                    (dialog)->selectedNgwResource(strUrl,strLogin,strPass,strId,jsonMeta);
        }
        else if (newProjectType == 1)
        {
            QStringList sPaths = static_cast<QFileDialog*>(dialog)->selectedFiles();
            str = sPaths[0];
        }
        else
        {
            // Тут задаём не путь, а некаю строку для описания что за проект открыт (
            // будет выведена в нижнюю статусную строку).
            //str = ;
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
        QByteArray baPath = str.toUtf8();
        bool ok = false;
        if (newProjectType == 2)
        {
            ok = newProject->initFromNgw(baPath.data(),strUrl,strLogin,strPass,strId,jsonMeta);
        }
        else if (newProjectType == 1)
        {
            ok = newProject->init(baPath.data());
        }
        else
        {
            ok = true;
            newProject->initVoid(
                 static_cast<FBNewVoidDialog*>(dialog)->comboGeom->currentData().toString());
                 //,static_cast<FBNewVoidDialog*>(dialog)->comboSrs->currentData().toString());
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
            //setBottomProjectString(baPath.data());
            setBottomProjectString(str);

            // Очищаем старый экран и изменяем интерфейс.
            clearElemSelection();
            recreateAndroidScreen();
            setLeftWidgetVisible(true);
            setRightWidgetVisible(true);
            butSave->setEnabled(false);
            butSaveAs->setEnabled(true);
            butClearScreen->setEnabled(true);
            butImportControls->setEnabled(true);
            butUpdateData->setEnabled(true);
            butFieldManager->setEnabled(true);

            // Изменяем последний выбранный каталог для выбора шейпфайла. Это запишется
            // в настройки приложения в деструкторе класса.
            if (newProjectType == 1)
            {
                strLastNewShapeFile = str;
            }
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

    //dialog.setLabelText(QFileDialog::LookIn,global_translate(5));
    //dialog.setLabelText(QFileDialog::FileName,global_translate(4));
    //dialog.setLabelText(QFileDialog::FileType,global_translate(3));
    //dialog.setLabelText(QFileDialog::Accept,global_translate(0));
    //dialog.setLabelText(QFileDialog::Reject,global_translate(2));
    //dialog.setLabelText(QFileDialog::LookIn,tr());
    //dialog.setLabelText(QFileDialog::FileName,tr());
    //dialog.setLabelText(QFileDialog::FileType,tr());
    //dialog.setLabelText(QFileDialog::Accept,tr());
    //dialog.setLabelText(QFileDialog::Reject,tr());

    dialog.setWindowTitle(tr("Открыть проект. Выберите файл"
                  " с расширением .") + QString(FB_PROJECT_EXTENSION) + " ...");

    if (strLastOpenFile != "")
    {
        QFileInfo fileInfo(strLastOpenFile);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    else if (strLastSaveAsFile != "")
    {
        // TODO: Исправить - если будет уже несуществующая директория (но не ""), то
        // он почему-то откроет Documents (под Windows).
        QFileInfo fileInfo2(strLastSaveAsFile);
        dialog.setDirectory(fileInfo2.absoluteDir());
    }
    else
    {
        dialog.setDirectory(QDir());
    }

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
                butImportControls->setEnabled(true);
                butUpdateData->setEnabled(true);
                butFieldManager->setEnabled(true);

                strLastOpenFile = strFullPath;
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

    //dialog.setLabelText(QFileDialog::LookIn,global_translate(5));
    //dialog.setLabelText(QFileDialog::FileName,global_translate(4));
    //dialog.setLabelText(QFileDialog::FileType,global_translate(3));
    //dialog.setLabelText(QFileDialog::Accept,global_translate(1));
    //dialog.setLabelText(QFileDialog::Reject,global_translate(2));
    //dialog.setLabelText(QFileDialog::LookIn,tr());
    //dialog.setLabelText(QFileDialog::FileName,tr());
    //dialog.setLabelText(QFileDialog::FileType,tr());
    //dialog.setLabelText(QFileDialog::Accept,tr());
    //dialog.setLabelText(QFileDialog::Reject,tr());

    dialog.setWindowTitle(tr("Сохранить проект как ..."));

    // Если этот проект уже сохранялся - берём путь по которому ЭТОТ проект уже был сохранён.
    if (CUR_PROJECT->getStrPath() != "")
    {
        dialog.setDirectory(QDir(CUR_PROJECT->getStrPath()));
        dialog.selectFile(QString(FB_INIT_PROJECT_NAMEBASE)
                          + "." + QString(FB_PROJECT_EXTENSION));
    }
    // Если это новый, ни разу не сохранявшийся проект - выдавать последний путь.
    else if (strLastSaveAsFile != "")
    {
        QFileInfo fileInfo(strLastSaveAsFile);
        dialog.setDirectory(fileInfo.absoluteDir());
        dialog.selectFile(QString(FB_INIT_PROJECT_NAMEBASE)
                          + "." + QString(FB_PROJECT_EXTENSION));
    }
    else
    {
        dialog.setDirectory(QDir());
    }

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
        strLastSaveAsFile = CUR_PROJECT->getFullPath();
    }
    else
    {

    }
    butSave->setDown(false);
    butSaveAs->setDown(false);
    butUpdateData->setDown(false);
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

    // Получаем элементы формы в нужном порядке.
    // Формируем словарь, который будет автоматически упорядочен по ключу, где ключ -
    // Y координата элемента в расклаке.
    QMap<int,FBElem*> mapElems = getFormElems();

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
        while (it2 != elem->mapAttrs.constEnd())
        {
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


// Возвращает все элементы формы.
// Использует метод QLayout::itemAt(int index) для просмотра всех виджетов
// в раскладке с последующим извлечением координаты У и сортировке по ней.
QMap<int,FBElem*> FB::getFormElems ()
{
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
    return mapElems;
}


// Вызывать этот метод только при пустом экране!!!
// Элементы для создания (фабрики) в левом tree-widget-е должны быть уже созданы!!!
QList<FBElem*> FB::fillForm (Json::Value jsonForm)
{
    QList<FBElem*> retFormElems;

    if (jsonForm.isNull())
        return retFormElems; // пустой

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

            //if (!jsonAttr.isNull())
            //{
                // Если значение атрибута в файле == null, то это тоже нужно считать, поскольку
                // некоторые атрибуты могут быть настроены из-за этого по-особому.
                it2.value()->fromJson(jsonAttr);
            //}

            ++it2;
        }

        // Обязательно изменям внешний вид элемента, т.к. сейчас он имеет стандартный,
        // а его атрибуты изменились.
        elem->updateAppearance();

        // Ещё раз нажимаем на последний добавленный элемент, чтобы обновить таблицу
        // атрибутов, т.к. там сейчас значения, заданные при создании элемента.
        onPressElem();

        // Формируем массив всех добавленных элементов - для возврата.
        retFormElems.append(elem);
    }

    return retFormElems;
}



void FB::onLanguageSelected (int index)
{
    strLastLangSelected = comboLang->currentData(Qt::UserRole).toString();
    showMsgBox(tr("Для изменения языка необходимо перезапустить приложение"));
}


void FB::writeSettings()
{
    QSettings settings (QSettings::IniFormat, QSettings::UserScope, "NextGIS", "FormBuilder");
    // Для Windows запишется например в: C:\Users\Mikhail\AppData\Roaming\NextGIS
    settings.setValue("last_new_shp_file",strLastNewShapeFile);
    settings.setValue("last_open_file",strLastOpenFile);
    settings.setValue("last_save_as_file",strLastSaveAsFile);
    settings.setValue("last_language_selected",strLastLangSelected);
    settings.setValue("last_ngw_url",strLastNgwUrl);
    settings.setValue("last_ngw_login",strLastNgwLogin);
}
void FB::readSettings()
{
    QSettings settings (QSettings::IniFormat, QSettings::UserScope, "NextGIS", "FormBuilder");
    strLastNewShapeFile = settings.value("last_new_shp_file","").toString();
    strLastOpenFile = settings.value("last_open_file","").toString();
    strLastSaveAsFile = settings.value("last_save_as_file","").toString();
    strLastLangSelected = settings.value("last_language_selected","en_GB").toString();
    strLastNgwUrl = settings.value("last_ngw_url","").toString();
    strLastNgwLogin = settings.value("last_ngw_login","").toString();
}


QString FB::shortenStringForOutput(QString inStr)
{
    if (inStr.size() > 23)
    {
        inStr.remove(0,inStr.size()-23);
        inStr.append("...");
    }
    return inStr;
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
