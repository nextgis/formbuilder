/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  basic form implementations
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "form_core.h"


/*****************************************************************************/
/*                                                                           */
/*                                  FBAttr                                   */
/*                                                                           */
/*****************************************************************************/

FBAttr::FBAttr (FBElem *parentElem)
{
    elemPtr = parentElem;
}

FBAttr::~FBAttr ()
{
}


/*****************************************************************************/
/*                                                                           */
/*                                  FBElem                                   */
/*                                                                           */
/*****************************************************************************/

FBElem::~FBElem ()
{
}

FBElem::FBElem (FBFactory *fctPtr): QWidget()
{
    this->fctPtr = fctPtr;

    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    lvMain = new QVBoxLayout(this);
}

Json::Value FBElem::toJson ()
{
    Json::Value jsonFinal;

    return jsonFinal;
}

FBErr FBElem::fromJson (Json::Value jsonValue)
{

    return FBErrNone;
}

QList<QList<QPair<QString,FBAttr*> > > FBElem::getAttrsLists()
{
    QList<QList<QPair<QString,FBAttr*> > > retList;
    QList<QPair<QString,FBAttr*> > list;
    QMap<QString,FBAttr*>::const_iterator it = mapAttrs.constBegin();
    while (it != mapAttrs.constEnd())
    {
        list.append(QPair<QString,FBAttr*>(it.key(),it.value()));
        ++it;
    }
    retList.append(list);
    return retList;
}

QString FBElem::getDisplayName ()
{
    return fctPtr->getDisplayName();
}

void FBElem::changeStyle (QString styleName)
{

}

// Will delete all widgets in the element: decorational and other elements.
void FBElem::clearContents ()
{
    QLayoutItem *child;
    while ((child = lvMain->takeAt(0)) != 0)
    {
        // TODO: test such deletion - when the child layouts (not widgets) will
        // be deleted.

        if (child->widget() != 0)
            delete child->widget(); // we must call widget() for its destructor
        else
            delete child;
    }
}

void FBElem::setSelectStyle ()
{
    // We must set select and deselect style only for top elem widget (i.e. this
    // widget), but not for its decoration inner widgets.
    this->setStyleSheet("QWidget"//#" + this->objectName() +
                        "{background-color: rgba(0,0,0,0); "
                        "border: 2px dashed red;}");
}
void FBElem::setDeselectStyle ()
{
    this->setStyleSheet("QWidget"//#" + this->objectName() +
                        "{background-color: rgba(0,0,0,0); "
                        "border: 2px dashed rgba(0,0,0,0);}");
}

void FBElem::mousePressEvent (QMouseEvent *event)
{
    // If this elem contains other elems it will receive this event
    // only if the click was not inside them.
    emit pressed(this);
}

void FBElem::mouseReleaseEvent (QMouseEvent *event)
{
    emit released();
}

void FBElem::mouseMoveEvent (QMouseEvent *event)
{
    emit moved(event);
}

void FBElem::paintEvent (QPaintEvent *event)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

/*****************************************************************************/
/*                                                                           */
/*                             FBInsertWidget                                */
/*                                                                           */
/*****************************************************************************/

FBInsertWidget::FBInsertWidget (QWidget* parent): QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMinimumHeight(13);
    this->setHideStyle();
}

FBInsertWidget::~FBInsertWidget ()
{
}

void FBInsertWidget::setShowStyle()
{
    this->setStyleSheet("QWidget"
                        "{background-color: rgba(0,0,0,0);"
                        "border: 2px dashed red;}");
}

void FBInsertWidget::setHideStyle()
{
    this->setStyleSheet("QWidget"
                        "{background-color: rgba(0,0,0,0);"
                        "border: 2px dashed rgba(0,0,0,0);}");
}

void FBInsertWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}



/*****************************************************************************/
/*                                                                           */
/*                                  FBForm                                   */
/*                                                                           */
/*****************************************************************************/


FBForm::~FBForm ()
{
}


FBForm::FBForm (): QWidget()
{
    SELECTED = NULL;
    IS_SELECTED_MOVING = false;
    INSERT_SHOWED = NULL;

    // Set transparent background.
    this->setStyleSheet("QWidget{background-color:rgba(0,0,0,0);}");

    lvForm = new QVBoxLayout(this);
    lvForm->setContentsMargins(0,0,0,0);
    lvForm->setSpacing(0);

    // Add initial form components.
    this->clear();

    curStyle = FB_STYLENAME_DEFAULT;
}


// Add already outside-created elem to the form after the passed element
// or at the end.
// WARNING. Do not pass the elem which is already in the form to this method.
void FBForm::addElem (FBElem *newElem, FBElem *afterThisElem)
{
    if (newElem == NULL)
        return;

    // TODO: do the check if the passed newElem is already the form's one.

    // We do not use addWidget() because the last item in layout is stretch item.
    if (afterThisElem == NULL)
    {
        lvForm->insertWidget(lvForm->count()-1, newElem);
        FBInsertWidget *insWidget2 = new FBInsertWidget(this);
        lvForm->insertWidget(lvForm->count()-1, insWidget2);
    }
    else
    {

    }

    // Visualize elem.
    newElem->changeStyle(curStyle);

    // Connect all elem and form events.
    QObject::connect(newElem, SIGNAL(pressed(FBElem*)),
                     this, SLOT(onElemPressed(FBElem*)));
    QObject::connect(newElem, SIGNAL(moved(QMouseEvent*)),
                     this, SLOT(onElemMoved(QMouseEvent*)));
    QObject::connect(newElem, SIGNAL(released()),
                     this, SLOT(onElemReleased()));

    // Select element right after addition and signalize about it.
    this->selectElem(newElem);
    emit elemPressed();
}


// WARNING. Pass only the elem which belongs to the form.
void FBForm::moveElem (FBElem* elem, FBInsertWidget *insteadInsWidget)
{
    if (elem == NULL || insteadInsWidget == NULL)
        return;

    // TODO: check if the passed elem does not belong to the form.

    int indexOfElemWidget = lvForm->indexOf(elem);
    int indexOfInsWidget = lvForm->indexOf(insteadInsWidget);

    // There is no need to move element after or before itself.
    if ((indexOfElemWidget - indexOfInsWidget == -1)
      || (indexOfElemWidget - indexOfInsWidget == 1))
        return;

    // Firstly remove element and its insert widget (which follows it) and
    // than insert this pair right after selected insert widget.
    // Ownership of removed widgets will be the same, so we can add them simply
    // again.
    FBInsertWidget* insWid
            = (FBInsertWidget*)(lvForm->itemAt(indexOfElemWidget+1)->widget());
    lvForm->removeWidget(elem);
    lvForm->removeWidget(insWid);
    indexOfInsWidget = lvForm->indexOf(insteadInsWidget);
    lvForm->insertWidget(indexOfInsWidget+1,elem);    // indexes in the layout were
    lvForm->insertWidget(indexOfInsWidget+2,insWid);  // changed automatically
}


void FBForm::deleteElem (FBElem* elem)
{
    if (elem == NULL)
        return;
    if (elem == SELECTED)
        this->deselectElem();
    int indexOfElemWidget = lvForm->indexOf(elem);
    // Delete the pair: element widget and its insert widget, which follows it.
    QWidget *w;
    w = lvForm->takeAt(indexOfElemWidget)->widget();
    delete w;
    w = lvForm->takeAt(indexOfElemWidget)->widget(); // layout's indexes were changed
    delete w;
}


void FBForm::clear ()
{
    // Delete ALL items which are placed in the main form layout: form elements,
    // insert widgets and tha last stretch layout.
    QLayoutItem *child;
    while ((child = lvForm->takeAt(0)) != NULL)
    {
        // TODO: think about why here we can delete only casting to QWidget, while
        // in other method e.g. ~ recreateVoidScreen() we can delete without it.
        // Possible reason: if children are layouts they delete their items self,
        // but here all children are widgets.

        // TODO: why the last stretch item (not widget) is deleted without error?

        delete child->widget();
    }

    // Firstly we add one insert widget so it can be possible to add elems to the form.
    // Secondly add the stretch-item to the end of form. All form elems must be added
    // just before it.
    // Do not forget, that this item is not a widget, while later we will often
    // look through all form layout items and cast them to widgets.
    FBInsertWidget *insWidget = new FBInsertWidget(this);
    lvForm->addWidget(insWidget);
    lvForm->addStretch();
}


void FBForm::selectElem (FBElem* elem)
{
    if (SELECTED != NULL)
        SELECTED->setDeselectStyle();
    elem->setSelectStyle();
    SELECTED = elem;
}


void FBForm::deselectElem ()
{
    if (SELECTED != NULL)
        SELECTED->setDeselectStyle();
    SELECTED = NULL;
    IS_SELECTED_MOVING = false; // TODO: check if it is necessary
}


bool FBForm::isVoid ()
{
    return false;
}


QList<FBElem*> FBForm::getSelectedElems()
{
    QList<FBElem*> list;
    if (SELECTED != NULL)
        list.append(SELECTED);
    return list;
}


// Returns all form elements in a map, which is sorted by Y coordinate.
// The map will contain all FBElem elements of top level, where containers
// will hold their inner elements inside.
// The map will be void if there is no elems in a form.
QMap<int,FBElem*> FBForm::getElems ()
{
    QMap<int,FBElem*> map;
    for (int i = 0; i < lvForm->count(); ++i) // iterate all items in layout
    {
        QWidget *w = lvForm->itemAt(i)->widget();
        FBElem *e = qobject_cast<FBElem*>(w); // select only elements from it
        if (e != NULL)
        {
            // Y coordinate will be unique because all elems are separated with
            // insert-widgets.
            map.insert(e->y(),e); // it will automatically sort elems by its Y
        }
    }
    return map;
}


// Convert form to JSON.
// Strategy: each json-form is just a one dimentional array of json-elements. Each
// form element knows how to convert itself to JSON.
Json::Value FBForm::toJson ()
{
    Json::Value jsonRootArray;
    QMap<int,FBElem*> mapElems = this->getElems();
    QMap<int,FBElem*>::const_iterator it = mapElems.constBegin();
    while (it != mapElems.constEnd())
    {
        // 1. Get JSON representation of an element.
        FBElem* elem = it.value();
        Json::Value jsonElem = elem->toJson();

        // 2. Add to this representation form's syntax.
        // We add here keyname of the element, because it is a form's responsibility
        // to read and create an element of specific FBElem class in a parseJSON()
        // method by its name.
        QByteArray ba;
        ba = elem->getFctPtr()->getKeyName().toUtf8();
        jsonElem[FB_JSONKEY_ELEM_TYPE] = ba.data();
        jsonRootArray.append(jsonElem);
        ++it;
    }
    return jsonRootArray;
}


// Returns the list of elements by their order reading them from given JSON.
// See FBForm::toJson() how the form was converted to JSON.
// Elements are created by their names.
// If errors occur the list will be returned void. Error is returned if at least one
// of the elems in JSON array is incorrect.
// WARNING. It is responsibility of the caller to delete all returned elems, but it
// could be done by form if one add elems to it after creation.
QList<FBElem*> FBForm::parseJson (Json::Value jsonVal)
{
    QList<FBElem*> list;

    if (jsonVal.isNull() || !jsonVal.isArray())
        return list; // void

    // Look through all json items.
    bool ok = true;
    for (int i=0; i<jsonVal.size(); ++i)
    {
        // Create elem by its name.

        // 1. Firstly we need to get elem's keyname and create FBElem concrete derived
        // class by it.
        Json::Value jsonValItem = jsonVal[i][FB_JSONKEY_ELEM_TYPE];
        if (!jsonValItem.isNull()) // TODO: how else to check correctness?
        {
            ok = false;
            break;
        }
        QString keyElemName = QString::fromUtf8(jsonValItem.asString().data());
        FBFactory *fct = FBFactory::getFctByName(keyElemName);
        FBElem *elem = fct->create();

        // 2. And only than the element "will know" how to convert itself from JSON.
        FBErr err = elem->fromJson(jsonVal[i]);
        if (err != FBErrNone)
        {
            ok = false;
            break;
        }

        list.append(elem);
    }

    // Delete all already created elems if error occured.
    if (!ok)
    {
        for (int i=0; i<list.size(); i++)
        {
            delete list[i];
        }
        list.clear();
    }

    return list;
}


// Converts form from JSON, using FBForm::parseJson().
// Will clear all elems in a form before adding new!
FBErr FBForm::fromJson (Json::Value jsonVal)
{
    QList<FBElem*> list = this->parseJson(jsonVal);
    if (list.isEmpty())
        return FBErrIncorrectJson;

    this->clear();

    for (int i=0; i<list.size(); i++)
    {
        // Add each elem to the end of form.
        this->addElem(list[i]);

        // Change the appearance of elem, while its attrs have been changed.
        list[i]->updateAppearance();
    }

    return FBErrNone;
}


void FBForm::updateStyle (QString styleName)
{
    // Form itself has no style.
    // Update style for all form elements.
    QMap<int,FBElem*> elems = this->getElems();
    QMap<int,FBElem*>::const_iterator it = elems.constBegin();
    while (it != elems.constEnd())
    {
        it.value()->changeStyle(styleName);
        ++it;
    }

    curStyle = styleName;
}


void FBForm::onElemPressed (FBElem *elem)
{
    this->selectElem(elem);

    // Signalize about it to others: e.g. to show elem's attributes table in GUI.
    emit elemPressed();

    IS_SELECTED_MOVING = true;
}


void FBForm::onElemMoved (QMouseEvent *event)
{
    // Here we use the following way to move elems on form via mouse:
    // using the coords of parent widget (form or widget-container) we search an
    // insert widget, above which the mouse cursor now is situated and select it.
    // At one time only one insert widget can be selected. The "grab" of mouse
    // will always hang on that elem widget which was pressed, so all drawing
    // work can be done inside 3 widget events: press, move and release.

    // There is no other way to do elements moving with mouse - because of the fact,
    // that Qt grabs widget after the press on it and all other widgets in application
    // do not receive mouse events at all.
    // All the following ways do not work:
    // * to call releaseMouse() inside mousePress() of element;
    // * to ignore event with event->ignore() in all mouse events;
    // * to set eventFilter for elem widgets;
    // * to use setAttribute(Qt::WA_TransparentForMouseEvents);
    // * QCoreApplication::postEvent() for neighbour insert-widget;
    // * any combination of them.

    if (IS_SELECTED_MOVING)
    {
        QWidget* parentWidget = SELECTED->parentWidget();
        QPoint coords = parentWidget->mapFromGlobal(QPoint(event->globalX(),
                                                           event->globalY()));

        // See through the hierarchy of widgets to see what widget is under the
        // mouse cursor - form widget or container widget.
        //if ()
        //{
        //    parentWidget = parentWidget->parentWidget();
        //}

        QWidget* widget = parentWidget->childAt(coords);
        if (widget == NULL)
            return;

        // TODO: use qobject_cast<FBInsertWidget*> instead of class name.
        QString className = widget->metaObject()->className();
        if (className == "FBInsertWidget")
        {
            if (static_cast<FBInsertWidget*>(widget) == INSERT_SHOWED)
                return;
            if (INSERT_SHOWED != NULL)
                INSERT_SHOWED->setHideStyle();
            static_cast<FBInsertWidget*>(widget)->setShowStyle();
            INSERT_SHOWED = static_cast<FBInsertWidget*>(widget);
        }
    }
}


void FBForm::onElemReleased ()
{
    IS_SELECTED_MOVING = false;

    if (INSERT_SHOWED == NULL)
        return;

    // Deselect insert-widget.
    INSERT_SHOWED->setHideStyle();
    FBInsertWidget* insWidget = INSERT_SHOWED;
    INSERT_SHOWED = NULL;

    // Insert moving elem instead of selected insert-widget.
    this->moveElem(SELECTED, insWidget);
}


