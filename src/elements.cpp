#include "elements.h"


// ************************************************************************** //
// ******************************* FBLabelToDrag ********************************* //
// ************************************************************************** //

//FBElemType FBLabelToDrag::eDragElemType = FBNone;
int FBLabelToDrag::eCurElemArrayIndex = -1;

FBLabelToDrag::FBLabelToDrag(QWidget* pParent): QLabel(pParent)
{
}

//void FBLabelToDrag::dragEnterEvent(QDragEnterEvent *event)
//{
//}

void FBLabelToDrag::mousePressEvent (QMouseEvent *event)
{
    //eDragElemType = this->type;
    //nCurElemTypeNum = this->
    eCurElemArrayIndex = this->elemArrayIndex;
}


// ************************************************************************** //
// ******************************* FBElemType ********************************* //
// ************************************************************************** //

long FBElemType::idCounter = 0;

// Создаём элемент интерфейса для экрана.
FBElem *FBElemType::CreateElem (QLabel *pParent, FBEType eType)
{
    FBElem *newElem = new FBElem(pParent);
    QVBoxLayout *newElemLayout = new QVBoxLayout(newElem);
    newElemLayout->setSpacing(0);
    newElemLayout->setContentsMargins(0,0,0,0);

    int totalHeight = 0;

    newElem->caption = this->caption;

    // Создаём компоненты элемента.
    for (int i=0; i<imgPaths.count(); i++)
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

    // Задаём уникальное имя. Используется как минимум для того, чтобы
    // можно было задавать стили только для конкретных виджетов, не трогая
    // их дочерние виджеты.
    newElem->setObjectName(QString("e_") +
                           QString::number(eType) +
                           QString("_") +
                           QString::number(GetNextId()));

    // Формируем внешний вид элемента интерфейса. Это необходимо делать после
    // задания имени объекта, т.к. стиль будет задаваться по имени.
    newElem->deselect(); // Задаём стиль контура - элемент не выделен.

    // Копируем названия и поля свойств объекта.
    for (int i=0; i<vParams.count(); i++)
    {
        newElem->vParamValues.append(QPair<QPair<QString,QString>,QString>
                                     (vParams[i],""));
    }

    // TODO: проверять, является ли родительский виджет - виджетом раскладки
    // Добавляем элемент в родительскую раскладку.
    QVBoxLayout *parentLayout = (QVBoxLayout *)pParent->layout();
    if (parentLayout != NULL)
    {
        // Вставляем а не добавляем виджет, т.к. в родителе присутствует растягиватль
        // в конце раскладки.
        //newElem->setText("rrrrrrrrrrrrrrrrrrrrrr");
        parentLayout->insertWidget(0, newElem);
    }
    else
    {
        delete newElem;
        return NULL;
    }

    return newElem;
}

long FBElemType::GetNextId ()
{
    idCounter++;
    return idCounter - 1;
}


// ************************************************************************** //
// ******************************* FBElem ********************************* //
// ************************************************************************** //

FBElem *FBElem::pLastSelected = NULL;

FBElem::~FBElem()
{
    //if (caption != NULL)
       // delete caption;

    for (int i=0; i<images.count(); i++)
    {
        if (images[i] != NULL)
            delete images[i];
    }
    images.clear();
}

void FBElem::mousePressEvent (QMouseEvent *event)
{
    if (pLastSelected != NULL && pLastSelected != this)
    {
        pLastSelected->deselect();
    }

    this->select();
    pLastSelected = this;

    emit elemSelected (this);
}

void FBElem::select ()
{
    setStyleSheet("QLabel#" + this->objectName() + " { border: 2px solid red}");
}

void FBElem::deselect ()
{
    setStyleSheet("QLabel#" + this->objectName() + " { border: none}");
}

