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
//#include <QMouseEvent>
#include <QFileDialog>
#include <QComboBox>
#include <QListWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QTreeWidget>
#include <QProgressBar>

#include <QFile>
#include <QTextStream>

#include <QMap>

#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QDomComment>
#include <QDomNode>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QUrl>

#include "ogrsf_frmts.h"

// Различные константные размеры.
#define FBParentLabelVertWidth 289
#define FBParentLabelVertStartHeight 500
#define FBParentLabelHorWidth 488
#define FBParentLabelHorStartHeight 300

// Различные ограничительные константы.
#define FBMaxPageCount 10
#define FBMaxElemInPageCount 50
#define FBMaxTabNameCharCount 25

// Имена атрибутов элементов интерфейса.
#define FBPARAM_field "field"
#define FBPARAM_caption "caption"
#define FBPARAM_default "default"
#define FBPARAM_values "values"
#define FBPARAM_only_figures "only_figures"
#define FBPARAM_max_char_count "max_char_count"
#define FBPARAM_values_sets "values_sets"
#define FBPARAM_defaults "defaults"
#define FBPARAM_field_name "field_name"
#define FBPARAM_field_azimuth "field_azimuth"
#define FBPARAM_field_datetime "field_datetime"

// Теги для XML файла.
#define FBXML_Value_CURRENTVERSION "1.0"
#define FBXML_Root_Form "Form"
#define FBXML_Attr_Version "Version"
#define FBXML_Attr_Dataset "Dataset"
#define FBXML_Value_DatasetYes "Yes"
#define FBXML_Value_DatasetNo "No"
#define FBXML_Node_Tab "Tab"
#define FBXML_Attr_TabName "Name"
#define FBXML_Node_Port "Portrait"
#define FBXML_Node_Alb "Album"
#define FBXML_Node_Elem "Element"
#define FBXML_Attr_Type "Type"
#define FBXML_Attr_NameRus "NameRus"
#define FBXML_Attr_NameEng "NameEng"
#define FBXML_Node_Prop "Property"
#define FBXML_Attr_Name "Name"
#define FBXML_Attr_Alias "Alias"
#define FBXML_Attr_Value "Value"

//#define FB_DEFAULT_NGW_URL "https://demo.nextgis.ru/rekod/"
//#define FB_DEFAULT_NGW_URL "http://demo.nextgis.ru/rekod"
//#define FB_DEFAULT_NGW_URL "http://bishop.gis.to"
#define FB_DEFAULT_NGW_URL "http://demo.nextgis.ru/ngw"
#define FB_DEFAULT_NGW_LOGIN "administrator"
#define FB_DEFAULT_NGW_PASSWORD "admin"

// Следующие константы используются для выбора ресурса в древе доступных ресурсов,
// полученного с сервера NGW.
#define FB_NGW_ITEM_TYPE_UNDEFINED 0
#define FB_NGW_ITEM_TYPE_RESOURCEGROUP 1
#define FB_NGW_ITEM_TYPE_VECTORLAYER 2
#define FB_NGW_ITEM_TYPE_POSTGISLAYER 3

// Константы состояний.
enum FBParentLabelAction
{
    FBActionNone,
    FBActionCreating,
    FBActionMoving,
};

// Номер типа элемента.
enum FBType
{
    FBNone,
    FBSpace,
    FBCombobox,
    FBTextedit,
    FBCheckbox,
    FBRadiogroup,
    FBDateTime,
    FBDoubleList,
    FBText,
    FBButton,
    FBCompass,
    FBPhoto,

    FBGroupStart,
    FBGroupEnd,
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

     //QString caption;
     FBElemType *elemType; // Указатель на тип, чтобы достать имена,  алиасы, ...

     // их порядковые номера в вертикальной раскладке, т.к. без этого при считывании
     // элементов родительского лейбла - они считаются в порядке добавления.
     QList<QLabel*> images; // Изображения компонентов элемента.
     QList<QPair<QPair<QString,QString>, QString> > vParamValues; // name, alias, value

    public:

     static long GetNextId ();

     //FBElem(FBParentLabel* pParent): QLabel ((QLabel*)pParent) {}
     FBElem(): QLabel () {}
     ~FBElem();

     virtual void mousePressEvent (QMouseEvent *event);
     //virtual void mouseMoveEvent (QMouseEvent *event);
     //virtual void mouseReleaseEvent (QMouseEvent *event);

     virtual void select ();
     virtual void deselect ();

    signals:
     void elemPressed ();
};


// Тип элемента интерфейса. Каждый такой тип создаётся один раз в
// начале работы программы. По сути - фабрики, выдающие элементы.
// Он же - то, что перемещается с панели инструментов (левой колонки) на "экран"
// телефона.
class FBElemType: public QPushButton//QLabel
{
    Q_OBJECT

    public:
     static FBElemType* CURRENT;
     FBType type;
     //QString caption;
      QString name;
      QString alias_ru;
      QString alias_eng;
     QList<QString> imgPaths;
     QList<QPair<QString,QString> > vParams; // name, alias

    public:
     //FBElemType(): QLabel () {}
     FBElemType (QWidget* pParent): QPushButton(pParent) {}//QLabel(pParent) {}
     //FBElemType(const FBElemType &otherElem): QLabel(otherElem) {}
     FBElem *CreateElem ();
     virtual void mousePressEvent (QMouseEvent *event);
     //virtual void mouseMoveEvent (QMouseEvent *event);

    signals:
     void elemTypePressed ();
};


class FBParentLabel: public QLabel
{
    Q_OBJECT

    public:

     //static FBParentLabelAction currentAction;

     // УПОРЯДОЧЕННЫЙ список элементов и их геометрических средних линий в данном
     // родительском лейбле.
     //QList <QPair<FBElem*,int> > elements;
     QList <FBElem*> elements;

     //int posToInsertWidget;
     //QLabel *selector;

     // Сохраняем логическое значение - в какой ориентации был создан этот лейбл.
     bool isVertical;

    public:

     // Вставляет элемент в раскладку на позицию курсора.
     // Если index не равен -1, то вставляет именно на эту позицию.
     //void InsertElem (FBElem *elem, int index = -1);
     bool InsertElem (FBElem *elem, int index = -1);

     void DeleteElem (FBElem *elem);

     void MoveElem (FBElem *elem, bool isUp);

     FBParentLabel(QScrollArea *parent);
     //~FBParentLabel(); // Деструктор не требуется.
     //virtual void mouseMoveEvent (QMouseEvent *event);
     //virtual void mouseReleaseEvent (QMouseEvent *event);

    public slots:
     /*
     void OnElemPressed ();
     void OnElemTypePressed ();
     */
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
     //QList <QPair<QWidget*,QPair<QLabel*,QLabel*> > > screenParentLabels;
     //QList <QPair<QWidget*,QPair<QLabel*,FBParentLabel*> > > screenParentLabels;
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

       GDALDataset* poDS;
       QStringList availableFields;

       QString lastSavePath;

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

     //virtual void mouseMoveEvent (QMouseEvent *event);
     //virtual void mouseReleaseEvent (QMouseEvent *event);

     void ShowMsgBox (QString msg);

    public slots:

     void OnElemPressed ();
     void OnElemTypePressed ();

     void on_pushButton_2_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_4_clicked();
     void on_pushButton_5_clicked();

     void OnTabIndexChanged (int index);

     void OnActionNewVoid ();
     void OnActionNewDataset ();
     void OnActionNewNGW ();
     void _openGdalDataset (char *datasetName);
     void OnActionOpen ();
     void OnActionSave ();
     void OnActionOrtnPrt ();
     void OnActionOrtnAlb ();
     void OnActionAddPage ();
     void on_toolButton_clicked();
     void on_toolButton_2_clicked();
     void on_toolButton_3_clicked();

     void DeselectElement ();

     //void OnNgwConnect (QString url, QString login, QString password);
     //void OnNgwSelect (QString inLayer, QString &outJson);
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

// TEST ---------------------------------------
//     QTextEdit* _testEdit;
// --------------------------------------------

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

#endif // FORMBUILDER_H
