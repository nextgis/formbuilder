#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <QLabel>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>


enum FBEType
//enum FBElemType
{
    FBNone,
    FBSpace,
    //FBVLayout,
    //FBHLayout,
    FBCombobox,
    FBTextedit,
    FBCheckbox,
    FBRadiobutton,
    FBDateTime,
    FBDoubleList,
    FBText,
    FBButton,
    FBCompass,
    FBPhoto,
};


// То, что перемещается с панели инструментов (левой колонки) на "экран"
// телефона.
class FBLabelToDrag: public QLabel
{
    public:
     //FBElemType type;
     int elemArrayIndex;

     FBLabelToDrag (QWidget* pParent);
     //virtual void dragEnterEvent(QDragEnterEvent *event);
     virtual void mousePressEvent (QMouseEvent *event);

     static int eCurElemArrayIndex;
};


// То, что рисуется на "экране" телефона как элемент интерфейса.
/*
class FBLabel: public QLabel
{
    Q_OBJECT
    signals:
     void elemSelected (FBLabel *pElem);
    public:
     FBLabel(QLabel* pParent): QLabel (pParent) {}
     QList<QPair<QPair<QString,QString>, QString> > vParamValues; // name, type, value
     virtual void mousePressEvent (QMouseEvent *event);
     static FBLabel *pLastSelected;
     virtual void select ();
     virtual void deselect ();
};
*/


// Общий лейбл для отрисовки всего элемента на "экране" телефона - в нём располагаются
// его пользовательское название и компоненты.
class FBElem: public QLabel
{
    Q_OBJECT

    signals:
     void elemSelected (FBElem *pElem);

    public:
     FBElem(QLabel* pParent): QLabel (pParent) {}
     ~FBElem();
     virtual void mousePressEvent (QMouseEvent *event);
     virtual void select ();
     virtual void deselect ();

     QString caption;
     // их порядковые номера в вертикальной раскладке, т.к. без этого при считывании
     // элементов родительского лейбла - они считаются в порядке добавления.
     QList<QLabel*> images; // Изображения компонентов элемента.
     QList<QPair<QPair<QString,QString>, QString> > vParamValues; // name, alias, value

     static FBElem *pLastSelected;
};


/*
class FBLabelVLayoutElem: public FBLabel
{
    public:
     FBLabelVLayoutElem(QLabel* pParent): FBLabel (pParent) {}
     virtual void select ();
     virtual void deselect ();
};
class FBLabelHLayoutElem: public FBLabel
{
    public:
     FBLabelHLayoutElem(QLabel* pParent): FBLabel (pParent) {}
     virtual void select ();
     virtual void deselect ();
};
*/


// Тип элемента интерфейса. Каждый такой тип создаётся один раз в
// начале работы программы. По сути - фабрики, выдающие элементы.
class FBElemType
//class FBElem
{
    public:
     //FBElemType type;
     FBEType type;
     QString caption;
     //QString imgPath;
     QList<QString> imgPaths;
     //virtual FBLabel *CreateElem(QLabel *pParent, FBElemType eType);
     FBElem *CreateElem (QLabel *pParent, FBEType eType);
     QList<QPair<QString,QString> > vParams; // name, alias

     static long idCounter;
     static long GetNextId ();
};




#endif // ELEMENTS_H
