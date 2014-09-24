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
#include <QMouseEvent>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QComboBox>

#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QDomComment>

#include "elements.h"

#include "ogrsf_frmts.h"

#define FBParentLableVertWidth 289
#define FBParentLableVertStartHeight 500
#define FBParentLableHorWidth 488
#define FBParentLableHorStartHeight 300

#define FBMaxPageCount 10
#define FBMaxElemInPageCount 50
#define FBMaxTabNameCharCount 25

#define FBXML_Root_Form "Form"
#define FBXML_Node_Tab "Tab"
#define FBXML_Attr_TabName "Name"
#define FBXML_Node_Port "Portrait"
#define FBXML_Node_Alb "Album"
#define FBXML_Node_Elem "Element"
#define FBXML_Attr_Type "Type"
#define FBXML_Node_Prop "Property"
#define FBXML_Attr_Name "Name"
#define FBXML_Attr_Alias "Alias"
#define FBXML_Attr_Value "Value"

namespace Ui
{
    class FormBuilder;
}

class FormBuilder : public QWidget
{
    Q_OBJECT

    public:
     explicit FormBuilder(QWidget *parent = 0);
     ~FormBuilder();

     void CreateElemTypes ();

     // Определяет где мышь отпустилась для всего виджета.
     virtual void mouseReleaseEvent (QMouseEvent *event);

    public slots:
     //void OnElemSelected (FBLabel *pElemLabel);
     void OnElemSelected (FBElem *pElem);

    private slots:
     void on_pushButton_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_2_clicked();

    private:
     Ui::FormBuilder *ui;

     QTabWidget *tabWidget;
     QTabWidget *tabHorWidget;

     bool isCurrentVertical;

     // Список соответствий - каждый родительский лейбл + лейбл экрана соответствуют
     // некой странице.
     // ВНИМАНИЕ: не путать родительский лейбл с лейблом экрана. Лейбл экрана содержит
     // фоновую картинку и один виджет - прокручиваемую область, которая сама по себе
     // и содержит родительский лейбл для всех элементов интерфейса.
     QList <QPair<QWidget*,QPair<QLabel*,QLabel*> > > screenParentLabels;

     QMenuBar* menuBar;
     QMenu *menuFile;
       //QAction *actionNew;
       QMenu *menuNew;
         QAction *actionNewVoid;
         QAction *actionNewDataset;
       QAction *actionOpen;
       QAction *actionSave;
     QMenu *menuScreen;
       QMenu *menuOrtn;
         QAction *actionOrtnPrt;
         QAction *actionOrtnAlb;
       QAction *actionAddPage;

     // Список типов элементов. На экране может быть создано любое
     // количество таких элементов.
     QList<FBElemType> vElemTypes;

     // Создаёт и возвращает родительский лейбл на родительском лейбле.
     QLabel *CreateScreenLabel (QWidget *pageWidget, bool isVertical);

     // Различные действия после того как выделение элемента должно быть снято.
     void OnDeselectElement ();

     QFileDialog* dialogSave;
     QFileDialog* dialogNewDataset;

     GDALDataset* poDS;

     QStringList availableFields;

     // Очищает существующие вкладки, создавая по сути новый проект.
     // Нельзя использовать как метод по созданию вкладок в самом начале!
     void ClearAll ();

    private slots:
     // Обработчики событий меню:
     //void OnActionNew ();
     void OnActionNewVoid ();
     void OnActionNewDataset ();
     void OnActionOpen ();
     void OnActionSave ();
     void OnActionOrtnPrt (bool is);
     void OnActionOrtnAlb (bool is);

     // Общая процедура создание новой страницы.
     void OnActionAddPage ();

     void OnTabIndexChanged (int index);
     void on_pushButton_4_clicked();
     void on_pushButton_5_clicked();
};

#endif // FORMBUILDER_H
