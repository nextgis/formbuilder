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

FBAttr::FBAttr (FBElem *parentElem, QString keyName, QString displayName,
                FBAttrrole role):
    QObject()
{
    this->elemPtr = parentElem;
    this->keyName = keyName;
    this->displayName = displayName;
    this->role = role;

    // Connect two main signals to any Elem which owns this Attr.
//    QObject::connect(this,SIGNAL(changeAppearance()),
//                     parentElem,SLOT(updateAppearance()));
//    QObject::connect(this,SIGNAL(changeOtherAttr()),
//                     parentElem,SLOT(changeAttrValue()));
}


/*****************************************************************************/
/*                                                                           */
/*                                  FBElem                                   */
/*                                                                           */
/*****************************************************************************/

// Constructor.
// IMPORTANT. As usual the concrete elem's constructor creates the set of elem's
// attributes!
FBElem::FBElem (FBFactory *fctPtr):
    QWidget()
{
    this->fctPtr = fctPtr;

    this->setCursor(Qt::PointingHandCursor);
    this->setDeselectStyle();

    lvMain = new QVBoxLayout(this);
}

// Convert Elem to JSON.
// Each element in a form must at least convert its attributes to JSON.
Json::Value FBElem::toJson ()
{
    Json::Value jsonFinal;
    QSet<FBAttr*>::const_iterator it = attrs.constBegin();
    Json::Value jsonAttrsSet;
    while (it != attrs.constEnd())
    {
        QByteArray baKn;
        baKn = (*it)->getKeyName().toUtf8();
        jsonAttrsSet[baKn.data()] = ((*it)->toJson());
        ++it;
    }
    jsonFinal[FB_JSONKEY_ELEM_ATTRS] = jsonAttrsSet;
    return jsonFinal; // can be null = no attributes
}

// Convert Elem from JSON.
// Each elem must at least read its attributes from JSON.
// For now all attrs are stored in the Elem class while they had been created
// in its constructor.
bool FBElem::fromJson (Json::Value jsonValue)
{
    if (jsonValue.isNull())
        return false;
    Json::Value jsonAttrsSet = jsonValue[FB_JSONKEY_ELEM_ATTRS];
    // Note: here we do not check jsonAttrsSet and jsonAttrsSet[...] for nulls
    // because fore some elems it is ok to have no attrs at all and for some
    // attrs - no values. The correctness of this was checked during the reading
    // of the form (where all syntax checks are made).
    QSet<FBAttr*>::const_iterator it = attrs.constBegin();
    while (it != attrs.constEnd())
    {
        QByteArray baKn;
        baKn = (*it)->getKeyName().toUtf8();
        if (!((*it)->fromJson(jsonAttrsSet[baKn.data()])))
        {
            return false;
        }
        ++it;
    }
    return true;
}


// Get Elem's display name.
QString FBElem::getDisplayName ()
{
    return fctPtr->getDisplayName();
}


// Change elem's look (style).
// Usually it is done by creating some inner decorational widgets-children which represet
// the "visual part" of the elem.
// WARNING. FOR DEVELOPERS: Do not forget in the override changeStyle() methods to set the
// parent of ALL decorational widgets as the Elem itself, because clearContents() method
// will delete all child widgets and layouts of the Elem each time if there was a command
// to change its style.
void FBElem::changeStyle (QString styleName)
{
    // TODO: do we need the most common default style for all elems? Smth like "stub elem",
    // which will be shown if no style was defined in the derived elem class.
}


// Will delete all widgets in the element: decorational and other elements.
// This is equivalent to element re-creation - it will be fully clear at the end.
void FBElem::clearContents ()
{
    // See FBForm::clear() for comments.
    QObjectList list = this->children();
    for (int i=0; i<list.size(); i++)
    {
        if (list[i]->isWidgetType())
            delete list[i]; // will delete all child widgets of these widgets also
    }
    QLayoutItem *child;
    while ((child = lvMain->takeAt(0)) != 0)
    {
        if (child->layout() != 0)
        {
            delete child->layout();
        }
        else if (child->spacerItem() != 0)
        {
            delete child->spacerItem();
        }
        else
        {
            delete child;
        }
    }
}


void FBElem::setSelectStyle ()
{
    // We must set select and deselect style only for top elem widget (i.e. this
    // widget), but not for its decoration inner widgets.
    this->setStyleSheet("QWidget"
                        "{background-color: rgba(0,0,0,0); "
                        "border: 2px dashed red;}");
}
void FBElem::setDeselectStyle ()
{
    this->setStyleSheet("QWidget"
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

FBInsertWidget::FBInsertWidget (QWidget* parent):
    QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setMinimumHeight(13);
    this->setHideStyle();
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

QList<FBDatetimeFormat*> FBForm::DATETIME_FORMATS;
void FBForm::initEnv ()
{
    FBForm::deinitEnv();
    DATETIME_FORMATS.append(
      new FBDatetimeFormat("Date","yyyy.MM.dd","",0,"yyyy-MM-dd"));
    DATETIME_FORMATS.append(
      new FBDatetimeFormat("Time","HH:mm:ss","",1,"HH:mm:ss"));
    DATETIME_FORMATS.append(
      new FBDatetimeFormat("Date and time","yyyy.MM.dd  HH:mm:ss","",
                           2,"yyyy-MM-dd HH:mm:ss"));
}
void FBForm::deinitEnv ()
{
    for (int i=0; i<DATETIME_FORMATS.size(); i++)
    {
        delete DATETIME_FORMATS[i];
    }
    DATETIME_FORMATS.clear();
}

FBForm::FBForm ():
    QWidget()
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

    // Set the form as the parent widget, because the form must delete its
    // child widgets when it is deleted self.
    newElem->setParent(this);

    // We do not use addWidget() because the last item in layout is a stretch item.
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

    // Select element right after addition.
    this->selectElem(newElem);

    // Do not signaliaze about selection outside.
    // Reason: not to call the according slots of the caller many times when elems
    // are being read and created from a file. E.g. the FB class will create and
    // after that delete as many tables for elem attrs, as the amount of such calls.
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


void FBForm::deleteSelected ()
{
    if (SELECTED == NULL)
        return;
    this->deleteElem(SELECTED); // will deselect elem automatically
}


// Delete ALL items which are placed in the main form layout: form elements,
// insert widgets and the last stretch layout. After that insert initial items.
void FBForm::clear ()
{
    this->deselectElem();

    // Delete all widgets. We must do this, because layouts do not delete their
    // widgets self.
    QObjectList list = this->children();
    for (int i=0; i<list.size(); i++)
    {
        if (list[i]->isWidgetType())
            delete list[i]; // will delete all child widgets of these widgets also
    }

    // Clear main layout.
    QLayoutItem *child;
    while ((child = lvForm->takeAt(0)) != 0)
    {
        if (child->layout() != 0)
        {
            delete child->layout();
        }
        else if (child->spacerItem() != 0)
        {
            delete child->spacerItem();
        }
        else
        {
            delete child; // TODO: do we need this here?
        }
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


// Returns all TOP LEVEL form elements in a map, which is sorted by Y coordinate.
// The map will contain all FBElem elements of top level, where containers
// will hold their inner elements inside.
// The map will be void if there is no elems in a form.
// WARNING. If elements still have their x,y = 0,0 (at the moment when they just have
// been added to the form) the returned map will contain only last elem in the form,
// so use this method carefully - only after the elements acquire their proper coordinates.
QMap<int,FBElem*> FBForm::getTopElems ()
{
    //this->updateGeometry();
    //lvForm->update();
    //this->update();
    //this->repaint();

    QMap<int,FBElem*> map;
    for (int i = 0; i < lvForm->count(); ++i) // iterate all items in layout
    {
        QWidget *w = lvForm->itemAt(i)->widget();
        FBElem *e = qobject_cast<FBElem*>(w); // select only elements from it
        if (e != NULL)
        {
            // We need the strict elements order.
            // Y coordinate will be unique because all elems are separated with
            // insert-widgets.
            int y = e->y();
            map.insert(y,e); // it will automatically sort elems by its Y
        }
    }
    return map;
}


// Returns ALL ELEMENTS of the form. The returned list contains not only container
// elements, but also elements which are the child elements for these containers and
// so on.
QList<FBElem*> FBForm::getAllElems ()
{
    QList<FBElem*> elemsAll;

    // Firstly get top-level elems.

//    QMap<int,FBElem*> mapTopElems = this->getTopElems();
//    QMap<int,FBElem*>::const_iterator it = mapTopElems.constBegin();
//    while (it != mapTopElems.constEnd())
//    {
//        elemsAll.append(it.value());
//        ++it;
//    }

    for (int i = 0; i < lvForm->count(); ++i) // do not use getTopElems()
    {
        QWidget *w = lvForm->itemAt(i)->widget();
        FBElem *e = qobject_cast<FBElem*>(w);
        if (e != NULL)
        {
            elemsAll.append(e);

            // Get children for top-level elems recursively.
            //FBElemCompound *ec = qobject_cast<FBElemCompound*>(e);
            //...
        }
    }

    return elemsAll;
}


// Convert form to JSON.
// Strategy: each json-form is just a one-dimentional array of json-elements as they
// follow each other vertically in the form. Each element knows how to convert itself
// to JSON, but form ADDS TO THE FINAL ELEM'S JSON VALUE the common thing: the key-name
// of element, so in further reading it can be possible to identify and create elem by
// its name.
Json::Value FBForm::toJson ()
{
    Json::Value jsonRootArray;

    // Firstly set the root value to array type, so if the form is void, the void
    // array (not null value) is returned.
    Json::Value jsonStub = 1;
    jsonRootArray.append(jsonStub);
    jsonRootArray.clear();

    // Finally fill value with elements.
    QMap<int,FBElem*> mapTopElems = this->getTopElems(); // container elems will write
                                                         // their child elements
    QMap<int,FBElem*>::const_iterator it = mapTopElems.constBegin();
    while (it != mapTopElems.constEnd())
    {
        // 1. Get JSON representation of an element.
        FBElem* elem = it.value();
        Json::Value jsonElem = elem->toJson();

        // 2. Add to this representation form's syntax.
        // We add here keyname of the element, because it's also a form's responsibility
        // to read and create an element of specific FBElem class in a parseJson()
        // method by its name.
        QByteArray ba;
        ba = elem->getFctPtr()->getKeyName().toUtf8();
        jsonElem[FB_JSONKEY_ELEM_TYPE] = ba.data();
        jsonRootArray.append(jsonElem);
        ++it;
    }
    return jsonRootArray;
}


// Returns the list of elements (in the passed parameter) by their order reading
// them from given JSON. Returning void list is correct - the form has no elements
// in that case.
// See FBForm::toJson() how the form was converted to JSON.
// Elements are created by their names.
// If errors occur FALSE will be returned. Error is returned even if at least one
// of the elems in JSON array is incorrect.
// WARNING. Passed retList will be cleared even if the error occurs during the work
// of the method. It is resposibility of the caller to avoid any memory leaks if to
// pass not a void list to this method. It is also resposibility of the caller to
// delete all created elems, but usually it is done by the form which is intended to
// be the parent for them.
bool FBForm::parseJson (Json::Value jsonVal, QList<FBElem*> &retList) // STATIC
{
    retList.clear();

    if (jsonVal.isNull() || !jsonVal.isArray())
        return false; // void

    // Look through all json items.
    bool ok = true;
    for (int k=0; k<jsonVal.size(); ++k)
    {
        // Create elem by its name.

        // 1. Firstly we need to get elem's keyname and create FBElem concrete derived
        // class by it.
        Json::Value jsonKeyName = jsonVal[k][FB_JSONKEY_ELEM_TYPE];
        if (jsonKeyName.isNull()) // TODO: how else to check correctness?
        {
            ok = false;
            break;
        }
        QString keyElemName = QString::fromUtf8(jsonKeyName.asString().data());
        FBFactory *fct = FBFactory::getFctByName(keyElemName);
        if (fct == NULL) // some unknow elem's name
        {
            ok = false;
            break;
        }
        FBElem *elem = fct->create();

        // 2. And only than the element "will know" how to convert itself from JSON.
        if (!elem->fromJson(jsonVal[k]))
        {
            ok = false;
            break;
        }

        retList.append(elem);
    }

    // Delete all already created elems if error occured.
    if (!ok)
    {
        for (int i=0; i<retList.size(); i++)
        {
            delete retList[i];
        }
        retList.clear();
    }

    return true;
}


// Converts form from JSON, using FBForm::parseJson().
// Will clear all elems in a form before adding new!
bool FBForm::fromJson (Json::Value jsonVal)
{
    QList<FBElem*> list;
    bool ok = FBForm::parseJson(jsonVal,list);
    if (!ok)
    {
        return false;
    }
    this->clear();
    for (int i=0; i<list.size(); i++)
    {
        // Add each elem to the end of form.
        this->addElem(list[i]);
        // Change the appearance of elem, while its attrs have been changed.
        list[i]->updateAppearance();
    }
    return true;
}


void FBForm::updateStyle (QString styleName)
{
    // Form itself has no style.
    // Update style for all form elements.
    QMap<int,FBElem*> elems = this->getTopElems(); // container elems will update their
                                                   // child elems
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


