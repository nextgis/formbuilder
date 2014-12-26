/******************************************************************************
 *
 * Name:     formbuilder.h
 * Project:  NextGIS Form Builder
 * Purpose:  General declarations.
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

#ifndef FORMBUILDER_H
#define FORMBUILDER_H

#include <QWidget>
#include <QPicture>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QScrollArea>
#include <QPushButton>
#include <QTabWidget>
#include <QAction>
#include <QFileDialog>
#include <QComboBox>
#include <QListWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QTreeWidget>
#include <QProgressBar>
#include <QGroupBox>

#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>

#include <QMap>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QUrl>

#include <QDialog>

#include "ogrsf_frmts.h"

#include "json/json.h"

// Различные константные размеры.
#define FBParentLabelVertWidth 289
#define FBParentLabelVertStartHeight 500
#define FBParentLabelHorWidth 488
#define FBParentLabelHorStartHeight 300

// Различные ограничительные константы.
#define FBMaxPageCount 10
#define FBMaxElemInPageCount 50
#define FBMaxTabNameCharCount 25
#define FBMaxComboElemsCount 100

// Имена атрибутов элементов интерфейса.

#define FBATTR_field "field"
#define FBATTR_field_name "field_name"
#define FBATTR_field_azimuth "field_azimuth"
#define FBATTR_field_datetime "field_datetime"
#define FBATTR_field_level1 "field_level1"
#define FBATTR_field_level2 "field_level2"
#define FBATTR_value "value"
#define FBATTR_values "values"
#define FBATTR_text "text"
#define FBATTR_only_figures "only_figures"
#define FBATTR_max_string_count "max_string_count"


#define FBLIST_new_name "new_elem"
#define FBLIST_new_alias "<новый элемент>"


#define FB_current_version "1.0"

// Ключи для JSON.
#define FBJSONKEY_version "version"
#define FBJSONKEY_dataset "dataset"
#define FBJSONKEY_ngw_connection "ngw_connection"
#define FBJSONKEY_tabs "tabs"
#define FBJSONKEY_name "name"
#define FBJSONKEY_portrait_elements "portrait_elements"
#define FBJSONKEY_album_elements "album_elements"
#define FBJSONKEY_type "type"
#define FBJSONKEY_attributes "attributes"
#define FBJSONKEY_values "values"
#define FBJSONKEY_default "default"
#define FBJSONKEY_alias "alias"


// Следующие константы используются для выбора ресурса в древе доступных ресурсов,
// полученного с сервера NGW.
#define FB_NGW_ITEM_TYPE_UNDEFINED 0
#define FB_NGW_ITEM_TYPE_RESOURCEGROUP 1
#define FB_NGW_ITEM_TYPE_VECTORLAYER 2
#define FB_NGW_ITEM_TYPE_POSTGISLAYER 3


#define FBELEMTYPE_text_label "text_label"
#define FBELEMTYPE_text_edit "text_edit"
#define FBELEMTYPE_button "button"
#define FBELEMTYPE_combobox "combobox"
#define FBELEMTYPE_checkbox "checkbox"
#define FBELEMTYPE_radio_group "radio_group"
#define FBELEMTYPE_compass "compass"
#define FBELEMTYPE_date_time "date_time"
#define FBELEMTYPE_double_combobox "double_combobox"
#define FBELEMTYPE_photo "photo"
#define FBELEMTYPE_space "space"
#define FBELEMTYPE_group_start "group_start"
#define FBELEMTYPE_group_end "group_end"

struct FBAlias
{
    QString ru;
    QString en;
    FBAlias (QString lRu) {ru = lRu, en = "en";}
    FBAlias (QString lRu, QString lEn) {ru = lRu, en = lEn;}
    FBAlias () {ru = ""; en = "";}
};


// Всё то, что должно изменяться при открытии, сохранении, создании нового проекта.
struct FBProject
{
    QString path;
    QString name_base;
    OGRDataSource* poDS;
    QStringList availableFields;
    std::string NGWConnection;

    FBProject();
    ~FBProject();
};


namespace Ui
{
    class FormBuilder;
}

class FBParentLabel;


class FBElemType;

// Общий лейбл для отрисовки всего элемента на "экране" телефона - в нём располагаются
// его пользовательское название и компоненты.
class FBElem: public QLabel
{
    Q_OBJECT

    public:

     static FBElem* CURRENT;

     static long idCounter;

     FBElemType *elemType; // Указатель на тип, чтобы достать имена,  алиасы, ...

     // их порядковые номера в вертикальной раскладке, т.к. без этого при считывании
     // элементов родительского лейбла - они считаются в порядке добавления.
     QList<QLabel*> images; // Изображения компонентов элемента.

     QList<QPair<QString,Json::Value> > attributes; // строковое имя и значение в особом формате

    public:

     static long GetNextId ();

     FBElem(): QLabel () {}
     ~FBElem();

     virtual void mousePressEvent (QMouseEvent *event);

     virtual void select ();
     virtual void deselect ();

    signals:
     void elemPressed ();
};


// Тип элемента интерфейса. Каждый такой тип создаётся один раз в
// начале работы программы. По сути - фабрики, выдающие элементы.
// Он же - то, что перемещается с панели инструментов (левой колонки) на "экран"
// телефона.
class FBElemType: public QPushButton
{
    Q_OBJECT

    public:
      static FBElemType* CURRENT;

      QString name;
      FBAlias alias;
      QList<QString> imgPaths;
      QList<QPair<QString, FBAlias> > attributeNames; // name, aliases

    public:
     FBElemType (QWidget* pParent): QPushButton(pParent) {}
     FBElem *CreateElem ();
     virtual void mousePressEvent (QMouseEvent *event);

    signals:
     void elemTypePressed ();
};


class FBParentLabel: public QLabel
{
    Q_OBJECT

    public:
     QList <FBElem*> elements;
     // Сохраняем логическое значение - в какой ориентации был создан этот лейбл.
     bool isVertical;

    public:
     bool InsertElem (FBElem *elem, int index = -1);
     void DeleteElem (FBElem *elem);
     void MoveElem (FBElem *elem, bool isUp);
     FBParentLabel(QScrollArea *parent);

    public slots:
};


class FBTabWidget: public QTabWidget
{
    Q_OBJECT

    public:

     FBTabWidget(QWidget *parent) : QTabWidget (parent) {}

     // Для снятия выделения с элемента.
     virtual void mousePressEvent (QMouseEvent *event);

    signals:

     void tabPressed ();
};


class FormBuilder : public QWidget
{
    Q_OBJECT

    public:

     explicit FormBuilder(QWidget *parent = 0);
     ~FormBuilder(); 

    private:

     Ui::FormBuilder *ui;

     FBTabWidget *tabWidget;
     FBTabWidget *tabHorWidget;

     bool isCurrentVertical;

     // Список соответствий - каждый родительский лейбл + лейбл экрана соответствуют
     // некой странице.
     // ВНИМАНИЕ: не путать родительский лейбл с лейблом экрана. Лейбл экрана содержит
     // фоновую картинку и один виджет - прокручиваемую область, которая сама по себе
     // и содержит родительский лейбл для всех элементов интерфейса. Здесь
     // соответствие 1 к 1: один виджет -> один экранный -> один родительский лейбл.
     QMap <QWidget*,QPair<QLabel*,FBParentLabel*> > correspondence;

     QMenuBar* menuBar;
     QMenu *menuFile;
       QMenu *menuNew;
         QAction *actionNewVoid;
         QAction *actionNewDataset;
         QAction *actionNewNGW;
       QAction *actionOpen;
       QAction *actionSave;
     QMenu *menuScreen;
       QMenu *menuOrtn;
         QAction *actionOrtnPrt;
         QAction *actionOrtnAlb;
       QAction *actionAddPage;

     FBProject *CUR_PRJ;
     FBProject *NEW_PRJ;

     // Список типов элементов. На экране может быть создано любое
     // количество таких элементов.
     QList<FBElemType*> vElemTypes;

     FBElemType* pElemTypeGroupStart;
     FBElemType* pElemTypeGroupEnd;

    private:

     void CreateElemTypes ();

     // Создаёт и возвращает экранный лейбл (с родительским внутри).
     // Этот метод нужен отдельно от создания самого виджета вкладки,
     // т.к. он используется при очистке вкладки, когда виджет не
     // удаляется.
     QLabel *CreateScreenLabel (QWidget *pageWidget, bool isVertical);

     void CreatePage();
     void ClearPage();
     void DeletePage();
     void ChangeTabName (QString newName);

     // Различные действия после того как выделение элемента должно быть снято.
     void AfterDeselectElement ();

     // Очищает существующие вкладки, удаляя все, кроме одной, создавая по сути
     // новый проект.
     // Нельзя использовать как метод по созданию вкладок в самом начале!
     void ClearAll ();

     void ShowMsgBox (QString msg);

    public slots:

     void OnElemPressed ();
     void OnElemTypePressed ();

     void on_pushButton_2_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_4_clicked();

     void OnTabIndexChanged (int index);

     void OnActionNewVoid ();
     void OnActionNewDataset ();
     void OnActionNewNGW ();
     bool _initGdalDataset (char *datasetName);
     void OnActionOpen ();
     void OnActionSave ();
     bool AddFileToZip(const CPLString &szFilePathName,
                       const CPLString &szFileName,
                       void* hZIP, GByte **pabyBuffer, size_t nBufferSize);
     void OnActionOrtnPrt ();
     void OnActionOrtnAlb ();
     void OnActionAddPage ();
     void on_toolButton_clicked();
     void on_toolButton_2_clicked();
     void on_toolButton_3_clicked();

     void DeselectElement ();

     void ShowDoubleComboDialog();
     void ShowComboOrRadioDialog();

private slots:
     void on_toolButton_4_clicked();
};


class FBConnectButton : public QPushButton
{
    Q_OBJECT

    public:

     FBConnectButton(QWidget *Parent,
             QLineEdit* inUrl, QLineEdit* inLog,
                     QLineEdit* inPas, QTreeWidget *outTree,
                     QPushButton *cancelButton,QProgressBar* progBar,QLabel* statusLabel,
                     QPushButton *selectButton);

    private:

     QLineEdit *_inUrl;
     QLineEdit *_inLog;
     QLineEdit *_inPas;
     QTreeWidget *_outTree;
     QPushButton *_cancelButton;
     QProgressBar* _progBar;
     QLabel* _statusLabel;
     QPushButton *_selectButton;

     QNetworkAccessManager httpManager;
     QNetworkReply *httpAuthReply;
     QNetworkReply *httpReply;
     QNetworkReply *httpResourceReply;

     std::string receivedJson;

     QTreeWidgetItem *_itemToExpand;

     QList<QTreeWidgetItem*> _ParseJsonReply(QNetworkReply *reply);

     std::map<int,int> _itemTypes; // <item_id, item_type>

    public slots:

     void OnClicked ();
     void HttpOnItemExpended (QTreeWidgetItem *treeItem);
     void HttpOnItemCollapsed(QTreeWidgetItem *treeItem);
     void HttpOnItemClicked(QTreeWidgetItem *treeItem, int treeItemColumn);

     void HttpReadyAuthRead ();
     void HttpReadyRead ();
     void HttpReadyResourceRead ();

     void HttpAuthFinished ();
     void HttpFinished ();
     void HttpResourceFinished ();
};


class FBListDialog: public QDialog
{
    Q_OBJECT

    public:

     FBListDialog(QString listType, QWidget *parent = 0);

     QString listType;

     // Из следующих полей будут браться значения для формирования JSON-значения
     // единственного атрибута "values" этого списка.

     QLabel *labelLeft;
     QListWidget *listLeft;
     QPushButton *buttonAddLeft;
     QPushButton *buttonRemoveLeft;
     QGroupBox *groupLeft;
     QLabel *labelNameLeft;
     QLabel *labelAliasLeft;
     QLineEdit *editNameLeft;
     QLineEdit *editAliasLeft;
     QPushButton *buttonModifyLeft;
     QLabel *labelComboLeft;
     QComboBox *comboLeft;

     QLabel *labelRight;
      QHBoxLayout *listsLayoutRight;
      QListWidget *pListRight;
      QList<QListWidget*> listsRight;
     QPushButton *buttonAddRight;
     QPushButton *buttonRemoveRight;
     QGroupBox *groupRight;
     QLabel *labelNameRight;
     QLabel *labelAliasRight;
     QLineEdit *editNameRight;
     QLineEdit *editAliasRight;
     QPushButton *buttonModifyRight;
     QLabel *labelComboRight;
      QHBoxLayout *combosLayoutRight;
      QList<QComboBox*> combosRight;
      QComboBox *pComboRight;

     QPushButton *buttonOk;

    public slots:

     void OnOkClicked();
     void OnCancelClicked();

     void OnLeftItemSelected(QListWidgetItem* item);
     void OnLeftAddClicked();
     void OnLeftDeleteClicked();
     void OnLeftFixClicked();

     void OnRightItemSelected(QListWidgetItem* item);
     void OnRightAddClicked();
     void OnRightDeleteClicked();
     void OnRightFixClicked();

    public:

     void refreshLeftCombo(int elemIndex, QString actionWithElem);

     void refreshRightCombo(int elemIndex, QString actionWithElem);
};

#endif // FORMBUILDER_H
