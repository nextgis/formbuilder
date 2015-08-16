/******************************************************************************
 *
 * Name:     fb.h
 * Project:  NextGIS Form Builder
 * Purpose:  Main widget (application window) class declaration.
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

#ifndef FB_H
#define FB_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QTemporaryDir>
//#include <QTemporaryFile>
#include <QFile>
#include <QTextStream>
#include <QProgressDialog>
#include <QThread>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
//#include <QAuthenticator>
#include <QSettings>

//#include "gdal/ogrsf_frmts.h"
#include "ogrsf_frmts.h"
#include "json/json.h"

#define FB_VERSION 2.0

#define FB_GDAL_2_X 1 // Использовать классы GDAL 2.X версии. Если закомментить - GDAL 1.X.
// Важно: на самом деле не использовать GDAL < 2.0.1, т.к. только после этой версии
// исправлен GDAL-баг с кэшированием при работе с ИД через /vcizip/, что приводило
// здесь к невозможности повторного сохранения проекта в тот же файл-архив.
// См. https://trac.osgeo.org/gdal/ticket/6005

//#define FB_GDAL_DEBUG "D:/nextgis/formbuilder-2.0/gdal_log.txt" // Куда сохранять лог.
//#define FB_TEST 1 // Если раскомменчено: включить тестовый код.
#define FB_NGW_API_VERS 1 // Какую версию NextGIS Web API использовать. TODO: реализовать по-настоящему.

#define FB_STR_NOT_SELECTED "-"
#define FB_PROJECT_EXTENSION "ngfp"
#define FB_PROJECT_FORM_FILENAME "form.json"
#define FB_PROJECT_DATA_FILENAME "data.geojson"
#define FB_PROJECT_META_FILENAME "meta.json"
#define FB_INIT_ATTR_MAXSTRINGS 1
#define FB_INIT_PROJECT_NAMEBASE "form" //"форма"
#define FB_NGW_ITEM_TYPE_UNDEFINED 0
#define FB_NGW_ITEM_TYPE_RESOURCEGROUP 1
#define FB_NGW_ITEM_TYPE_VECTORLAYER 2
#define FB_NGW_ITEM_TYPE_POSTGISLAYER 3
#define FB_NEXTGIS_URL "http://nextgis.com/nextgis-formbuilder"

// Ограничения:
#define FB_LIMIT_BOTTOM_STRING_LEN_SHORT 40
#define FB_LIMIT_BOTTOM_STRING_LEN_LONG 70
#define FB_LIMIT_COMBOBOX_ELEMS 65535
#define FB_LIMIT_RADIOGROUP_ELEMS 15

// Ключи для json-файла метаданных:
#define FB_JSON_META_VERSION "version"
#define FB_JSON_META_FIELDS "fields"
#define FB_JSON_META_KEYNAME "keyname"
#define FB_JSON_META_DATATYPE "datatype"
#define FB_JSON_META_DISPLAY_NAME "display_name"
#define FB_JSON_META_GEOMETRY_TYPE "geometry_type"
#define FB_JSON_META_NGW_CONNECTION "ngw_connection"
#define FB_JSON_META_SRS "srs"
#define FB_JSON_META_ID "id"
#define FB_JSON_META_LOGIN "login"
#define FB_JSON_META_PASSWORD "password"
#define FB_JSON_META_URL "url"

// Ключи для json-файла формы:
// - общие ключевые слова:
#define FB_JSON_ATTRIBUTES "attributes"
#define FB_JSON_TYPE "type"
#define FB_JSON_ELEMENTS "elements" // Для составных элементов (например вкладок).
// - названия элементов:
#define FB_JSON_TEXT_LABEL "text_label"
#define FB_JSON_IMAGE "image"
#define FB_JSON_TEXT_EDIT "text_edit"
#define FB_JSON_BUTTON "button"
#define FB_JSON_COMBOBOX "combobox"
#define FB_JSON_CHECKBOX "checkbox"
#define FB_JSON_RADIOGROUP "radio_group"
#define FB_JSON_COMPASS "compass"
#define FB_JSON_DATE_TIME "date_time"
#define FB_JSON_DOUBLE_COMBOBOX "double_combobox"
#define FB_JSON_PHOTO "photo"
#define FB_JSON_SIGNATURE "signature"
#define FB_JSON_SPACE "space"
#define FB_JSON_GROUP_START "group_start"
#define FB_JSON_GROUP_END "group_end"
#define FB_JSON_TABS "tabs"
#define FB_JSON_VERTICAL_LAYOUT "vertical_layout"
#define FB_JSON_HORIZONTAL_LAYOUT "horizontal_layout"
#define FB_JSON_GRID_LAYOUT "grid_layout"
// - названия атрибутов:
// --- для связи со слоем:
#define FB_JSON_FIELD "field" // если добавятся новые field_... атрибуты, то добавить их
#define FB_JSON_FIELD_LEVEL1 "field_level1" // в метод "импорт контролов"
#define FB_JSON_FIELD_LEVEL2 "field_level2"
// --- для способа ввода:
#define FB_JSON_IS_DIALOG "is_dialog"
#define FB_JSON_IS_DIALOG_LEVEL1 "big_list_level1"
#define FB_JSON_IS_DIALOG_LEVEL2 "big_list_level2"
// --- остальное:
#define FB_JSON_VALUE "value"
#define FB_JSON_VALUES "values"
#define FB_JSON_INIT_VALUE "init_value"
#define FB_JSON_TEXT "text"
#define FB_JSON_ONLY_FIGURES "only_figures"
#define FB_JSON_MAX_STRING_COUNT "max_string_count"
#define FB_JSON_LAST "last"
#define FB_JSON_DATE_TYPE "date_type"
#define FB_JSON_REQUIRED "required"
#define FB_JSON_GALLERY_SIZE "gallery_size"
// - ещё для атрибутов:
#define FB_JSON_ALIAS "alias"
#define FB_JSON_NAME "name"
#define FB_JSON_DEFAULT "default"
#define FB_JSONVALUE_NONAME "-1"
#define FB_JSONVALUE_NOALIAS "--"


#include "factories.h"

namespace Ui
{
    class FB;
}

class FBProgressDialog;
class FB;


// Один проект = вся текущая информация о единственной открытой форме: метаданные и т.д., кроме
// самих элементов формы.
class FBProject: public QObject
{
    Q_OBJECT

    public:
     FBProject (FB *fbPtr);
     ~FBProject ();
     QString getFullPath ();
     static void getPathComponents (QString strFullPath, QString &strPath, QString &strNameBase);
     bool init (char *datasetName);
     bool initFromNgw (char *datasetName, QString strUrl, QString strLogin, QString strPass,
                       QString strId, Json::Value jsonMeta);
     bool open (QString strFullPath, Json::Value &retForm);
     // Для вызова методов в отдельном потоке. Все следующие методы считаются долгими, т.к. может
     // идти работа с большими объёмами данных через GDAL.
     bool saveAs (QString strFullPath);

     bool getNgfpJsonMeta (QString strNgfpFullPath, Json::Value &retValue);
     bool getNgfpJsonForm (QString strNgfpFullPath, Json::Value &retValue);

    private:
     FB *fbPtr; // там указатель на раскладку из которой брать элементы при сохранении.
     // Далее поля: для сторонних классов - только для чтения, а задаются внутри своих методов.
     Json::Value jsonMeta; // метаданные проекта
     QString strPath; // каталог, где расположен файл проекта
     QString strNameBase; // имя файла проекта (без пути)
     QString strFirstTimeDataset; // путь к ИД, но только при создании нового проекта. В остальных случая строка пустая.
     QStringList strsNewImgsToSave; // аналогично для изображений.
     QStringList strsNewImgsToDelete;
    public:
     QString getStrPath () { return strPath; }
     Json::Value getJsonMeta () { return jsonMeta; }
     void resetSrcDatasetPath (QString strPath) { strFirstTimeDataset = strPath; }

    private:
     bool addFileToZip (const CPLString &szFilePathName, const CPLString &szFileName,
                       void* hZIP, GByte **pabyBuffer, size_t nBufferSize);

    signals:
     void sendMsg (QString msg);
     void changeProgress (int value);
};


// Класс потока для выполнения одного долгого действия: Сохранить Как.
class FBSaveAsThread: public QThread
{
    friend class FBProject; // для вызова соответствующего метода по сохранению.
    Q_OBJECT
     void run () Q_DECL_OVERRIDE;
    signals:
     void resultReady (bool result);
    private:
     QString strFullPath;
    public:
     FBSaveAsThread(QObject *parent, QString strFullPath);
};

// class FBOpenThread: public QThread


// Виджет для вставки элементов интерфеса (сам не является элементом).
// При отпускании кнопки на этом виджете, новый элемент интерфейса
// добавляется на экран вместо него.
class FBInsertWidget: public QWidget
{
    Q_OBJECT  // Q_OBJECT здесь требуется ещё и для определения типа виджета взятого из общей раскладки экрана

    public:
     FBInsertWidget(QWidget* parent);
     //~FBInsertWidget();
     virtual void paintEvent(QPaintEvent *pe);
     void setShowStyle();
     void setHideStyle();

    public:
     static FBInsertWidget* SHOWED;
};


// Окно приложения. Главный класс интерфейса пользователя.
// Содержит в себе все виджеты GUI, а так же саму редактируемую форму с её элементами.
class FB: public QWidget
{
    Q_OBJECT

    public:
     explicit FB (QWidget *parent = 0);
     ~FB();
     Json::Value formToJson ();
     static QString shortenStringForOutput (QString inStr);

    public:
     static FBProject *CUR_PROJECT; // Текущий проект - переменная доступна всем извне.

    public slots:
     void showMsgBox (QString msg);
     int showAlertBox (QString msg);

    private:
     Ui::FB *ui;
     QToolButton *butNewShp;
     QToolButton *butNewNgw;
     QToolButton *butOpen;
     QToolButton *butSave;
     QToolButton *butSaveAs;
     QToolButton *butAndrAll;
     QToolButton *butPhoneAndrVer;
     QToolButton *butPhoneAndrHor;
     QToolButton *butTabletAndrVer;
     QToolButton *butTabletAndrHor;
     QToolButton *butWeb;
     QToolButton *butQgis;
     QToolButton *butClearScreen;
     QToolButton *butDeleteElem;
     QToolButton *butImportControls;
     QToolButton *butUpdateData;
     QToolButton *butAboutGraphics;
     QPushButton* leftArrow;
     QPushButton* rightArrow;
     QTreeWidget* treeWidget; // Расширенный (основной).
     QTreeWidget* treeWidget2; // Минималистичный (дополнительный).
     QTableWidget* tableWidget;
     QLabel *labTableCaption;
     QVBoxLayout *layCenter;
     QScrollArea *wScrollArea;
     QWidget *widScreen;
     QVBoxLayout *layScreen; // потребуется доступ извне, чтобы читать элементы формы
     FBProgressDialog *dlgProgress;
     // Ставится в true только чтобы при добавлении нового элемента в конец - экран
     // проскроллился на него. При удалении (и во всех других случаях изменения формы)
     // скроллить не нужно - и эта переменная = false.
     bool canScrollToEnd;
     // Эти пути читаются и пишутся через QSettings. Запоминаем именно полный абсолютный
     // путь до файлов.
     QString strLastNewShapeFile;
     QString strLastOpenFile;
     QString strLastSaveAsFile;
    public:
     QVBoxLayout *getLayScreenPtr () { return layScreen; }

    private:
     void recreateFactories ();
     void clearScreen ();
     void recreateAndroidScreen ();
     void clearAndroidScreen ();
     void clearElemSelection ();
     void clearAttrTable ();
     QToolButton *createMainMenuButton (QWidget *parent, QLayout *layout,
                                  QString imgPath, QString text);
     void setRightWidgetCaption (bool isElemSelected);
     void setLeftWidgetVisible (bool isVisible);
     void setRightWidgetVisible (bool isVisible);
     void setBottomProjectString (QString datasetPath);
     void onNewAnyClick (bool isNgw);
     QList<FBElem*> fillForm (Json::Value jsonForm);
     void writeSettings ();
     void readSettings ();

    private slots:
     void onLeftArrowClicked ();
     void onRightArrowClicked ();
     void onNewShpClick ();
     void onNewNgwClick ();
     void onOpenClick ();
     void onSaveClick ();
     void onSaveAsClick ();
     void onAndrAllClick ();
     void onAndrPhoneVerClick ();
     void onAndrPhoneHorClick ();
     void onAndrTabletVerClick ();
     void onAndrTabletHorClick ();
     void onClearScreenClick ();
     void onDeleteElemClick ();
     void onImportControls ();
     void onUpdateData ();
     void onAboutGraphicsClick ();
     FBElem *onTreeItemClicked (QTreeWidgetItem* item, int column);
     void onAddElem (FBElem *elem);
     void onMoveElem (FBElem *elem, FBInsertWidget *insWidget);
     void onPressElem ();
     void onSaveAsEnded (bool result);
     void moveScrollBarToBottom (int min, int max);
};


// Диалог для подключения к NextGIS Web.
// Цель диалога - выбрать источник данных (слой) для создания формы.
class FBNgwDialog: public QDialog
{
    Q_OBJECT

    public:

     FBNgwDialog(QWidget *parent);
     QString selectedNgwResource (QString &strUrl, QString &strLogin,
                                  QString &strPass, QString &strId, Json::Value &jsonMeta);

    private:

     // Текущие выбранные параметры:
     QString strUrl;
     QString strLogin;
     QString strPass;
     QString strId;
     Json::Value jsonMeta;

     QLineEdit *wEditUrl;
     QLineEdit *wEditLogin;
     QLineEdit *wEditPass;
     QPushButton *wButConnect;
     // Временно не показываем кнопку отмены соединения, т.к. всё асинхронно и всегда
     // можно закрыть диалог или заного нажать кнопку "соединить".
     // TODO: сделать нормальную кнопку "отменить".
     //QPushButton *wButCancel;
     QPushButton *wButSelect;
     QTreeWidget *wTree;
     QProgressBar *wProgBar;
     QLabel *wLabelStatus;

     QNetworkAccessManager httpManager;
     QNetworkReply *httpAuthReply;
     QNetworkReply *httpReply;
     QNetworkReply *httpResourceReply;
     QNetworkReply *httpSelectedReply;
     std::string receivedJson;
     QTreeWidgetItem *itemToExpand;
     std::map<int,int> itemTypes; // <item_id, item_type>

    private:

     QList<QTreeWidgetItem*> parseJsonReply (QNetworkReply *reply);

    private slots:

     void onConnectClicked ();
     void onSelectClicked ();

     void httpOnItemExpended (QTreeWidgetItem *treeItem);
     void httpOnItemCollapsed (QTreeWidgetItem *treeItem);
     void httpOnItemClicked (QTreeWidgetItem *treeItem, int treeItemColumn);

     void httpReadyAuthRead ();
     void httpReadyRead ();
     void httpReadyResourceRead ();
     void httpReadySelectedRead ();

     void httpAuthFinished ();
     void httpFinished ();
     void httpResourceFinished ();
     void httpSelectedFinished ();

};


// Диалог для редактирования значений списка: комбобокса, радиогруппы и т.д.
class FBListValuesDialog: public QDialog
{
    Q_OBJECT

    public:
     FBListValuesDialog (QString elemName);

    public: // следующие виджеты - public для чтения извне (при сохраннеии в атрибут)
     QListWidget *listL;
     QToolButton *butAddL; // +
     QToolButton *butRemoveL; // -
     QToolButton *butChangeL; // #
     QLineEdit *editInnerL;
     QLineEdit *editOuterL;
     QComboBox *comboL;
     QPushButton *butOk;

    public slots:
     void onOkClicked ();
     void onCancelClicked ();
     void onLeftClicked (QListWidgetItem *item);
     void onLeftAddClicked ();
     void onLeftRemoveClicked ();
     void onLeftChangeClicked ();

    private:
     QString elemName;
};


// Диалог для ввода значений сдвоенного списка.
class FBDoubleListValueDialog: public QDialog
{
    Q_OBJECT

    public:
     FBDoubleListValueDialog ();

    public:
     QTableWidget *table1;
     QComboBox *combo1;
     QLabel *label2;
     QList<QTableWidget*> tables2;
     QList<QComboBox*> combos2;
     QPushButton *butOk;

    public slots:
     void onOkClicked ();
     void onTable1SelectionChanged ();
     void onTable2SelectionChanged ();
     void onCell1Changed (int row, int col);
     void onCell2Changed (int row, int col);

    private:
     void addTable2();
     void removeTable2 (int row);
     void showTable2 (int row);
     void showMsgBox (QString msg);
     QString getTable2String (QString srcStr);

    private:
     QVBoxLayout *vlTable2;
     QHBoxLayout *hlCombo2;
     QTableWidget *curTable2Ptr;
     QComboBox *curCombo2Ptr;
};


// Специальный диалог для показа процесса некотрого действия без возможности его отменить
// - например Сохранение. Для действия открытия проекта использовать QProgressDialog.
class FBProgressDialog: public QDialog
{
    Q_OBJECT
    public:
     FBProgressDialog(QWidget *parent);
     ~FBProgressDialog();
    public: // public - для задания полей извне
     QProgressBar *bar;
    public slots:
     void onChangeProgress (int value) {bar->setValue(value);}
};


class FBAboutDialog: public QDialog
{
    // Если не поставить здесь, и в других местах Q_OBJECT, то при переводе на другой язык
    // через tr() - строки не будут переведены.
    Q_OBJECT
    public:
     FBAboutDialog(QWidget *parent);
     ~FBAboutDialog();
};




#endif // FB_H
