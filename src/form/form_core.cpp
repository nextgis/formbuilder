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
                QString descr, FBAttrrole role):
    QObject()
{
    this->elemPtr = parentElem;
    this->keyName = keyName;
    this->displayName = displayName;
    this->description = descr;
    this->role = role;
    // Note: the connection of two main signals is made inside elements, because
    // only elements can know how their attributes have influence on them and on
    // each other.
}


/*****************************************************************************/
/*                                                                           */
/*                               FBDecorator                                 */
/*                                                                           */
/*****************************************************************************/

// Redecorate element.
// Fully clear all decorations and set new ones.
// By default an element - is the white rectangle with black border and with its
// name inside.
// See also important comments from the class declaration.
void FBDecorator::redecor (FBElem *elem)
{
    if (elem == NULL) return;
    elem->clearContents();
    elem->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    elem->setFixedHeight(FB_ELEM_DEFHEIGHT);
    QLabel *labText = new QLabel(elem);
    labText->setStyleSheet("QLabel"
                          "{border: 1px solid black;"
                          "border-radius: none;"
                          "background: white;" // so could be displayed on any background
                          "color: black;}");
    labText->setFont(QFont(FB_ELEM_DEFFONTTYPE,
                           FB_ELEM_DEFFONTSIZE));
    labText->setText(elem->getDisplayName());
    labText->setAlignment(Qt::AlignCenter);
    elem->addAsDecor(labText);
}

// Update element.
// Get the attributes of the element and update the look of it according to their values.
// This method is implemented separetly of redecor() because some styles have common way
// to update self but different way to redecorate self.
// By default there is no updating actions.
// See also important comments from the class declaration.
void FBDecorator::update (FBElem *elem)
{
    return;
}


/*****************************************************************************/
/*                                                                           */
/*                                  FBElem                                   */
/*                                                                           */
/*****************************************************************************/

FBElem::FBElem ():
    QWidget()
{
    decoratorPtr = NULL;
    this->setDeselectStyle();
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setFixedHeight(FB_ELEM_DEFHEIGHT); // to give to the elem some volume
    this->setCursor(Qt::PointingHandCursor);
    lvMain = new QVBoxLayout(this);
    lvMain->setContentsMargins(2,2,2,2); // so can be properly selected with red frame
    lvMain->setSpacing(0);
}


// Will delete all widgets in the element: decorational and other elements.
// This is equivalent to element re-creation - it will be fully clear at the end.
// See FBForm::clear() for comments.
void FBElem::clearContents ()
{
    QObjectList list = this->children();
    for (int i=0; i<list.size(); i++)
    {
        if (list[i]->isWidgetType())
            delete list[i]; // will delete all child widgets of these widget also
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

//...
// Pass only the widget which is a child of this FBElem already.
// Returns whether widget was registered or not.
bool FBElem::addAsDecor (QWidget *widget, QString regName)
{
    lvMain->addWidget(widget);
    return this->registerAsDecor(widget,regName);
}

//...
void FBElem::addAsDecor (QLayout *layout)
{
    lvMain->addLayout(layout);
}

//...
// Returns whether widget was registered or not.
bool FBElem::registerAsDecor (QWidget *widget, QString regName)
{
    if (regName != "")
    {
        QWidget* child = this->findChild<QWidget*>(regName,Qt::FindDirectChildrenOnly);
        if (child == NULL && widget->parent() == this)
        {
            widget->setObjectName(regName);
            return true;
        }
    }
    return false;
}

//...
QWidget *FBElem::findAsDecor (QString regName)
{
    return this->findChild<QWidget*>(regName,Qt::FindDirectChildrenOnly);
}

//...
void FBElem::setDecorator (FBDecorator *decorator)
{
    if (decorator == NULL) // common situation: there is no corresponding to this elem
        return;            // decorator for some screen
    decoratorPtr = decorator;
}


// Return attribute of its elem by keyname (keynames of attrs are unique).
FBAttr* FBElem::getAttr (QString attrKeyName)
{
    QMap<QString,FBAttr*>::iterator it = attrs.find(attrKeyName);
    if (it != attrs.end())
        return it.value();
    return NULL;
}


void FBElem::onChangeAttrValue (FBAttr *attr)
{
    return;
}

void FBElem::onChangeAppearance (FBAttr *attr)
{
    this->updateSelf();
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

// Why we need paintEvent() and Q_OBJECT macro to paint widgets:
// http://stackoverflow.com/questions/18344135/why-do-
// stylesheets-not-work-when-subclassing-qwidget-and-using-q-object
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
    this->setMinimumHeight(FB_FORM_INSWIDGETHEIGHT);
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


// See: FBElem::paintEvent()
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

QMap<QString,FBFctelem*> FBForm::FACTORIES;

QList<FBDatetimeFormat*> FBForm::DATETIME_FORMATS;

void FBForm::initDateTimeFormats () // STATIC
{
    FBForm::deinitDateTimeFormats();
    DATETIME_FORMATS.append(
      new FBDatetimeFormat("Date","yyyy.MM.dd","",
                           0,"yyyy-MM-dd"));
    DATETIME_FORMATS.append(
      new FBDatetimeFormat("Time","HH:mm:ss","",
                           1,"HH:mm:ss"));
    DATETIME_FORMATS.append(
      new FBDatetimeFormat("Date and time","yyyy.MM.dd  HH:mm:ss","",
                           2,"yyyy-MM-dd HH:mm:ss"));
}
void FBForm::deinitDateTimeFormats () // STATIC
{
    for (int i=0; i<DATETIME_FORMATS.size(); i++)
    {
        delete DATETIME_FORMATS[i];
    }
    DATETIME_FORMATS.clear();
}

// Register elem's factory.
// The factory is registered by the elem's keyname which is obtained automatically. This
// name should be unique in order the factory can be successfully registered.
// False is returend if there is already a factory with such keyname.
bool FBForm::registerElem (FBFctelem *fct) // STATIC
{
    if (fct == NULL)
        return false;
    FBElem *testElem = fct->create();
    QString elemKeyName = testElem->getKeyName();
    delete testElem;
    if (FACTORIES.find(elemKeyName) != FACTORIES.end())
        return false;
    FACTORIES.insert(elemKeyName,fct);
    return true;
}

// Create and return new element. Does not add it to any form.
// See FBForm::registerElem().
// It is the caller's responsibility to delete the returned elem, but usually it is
// done by the form to which the elem is usually added after creation.
// NULL is returned if there is no factory registered with the passed keyname.
FBElem* FBForm::createElem (QString elemKeyName) // STATIC
{
    if (FACTORIES.find(elemKeyName) != FACTORIES.end())
    {
        FBElem *elem = FACTORIES.value(elemKeyName)->create();
        return elem;
    }
    return NULL;
}


FBForm::~FBForm ()
{
    if (decoratorDefault != NULL)
        delete decoratorDefault;
}


FBForm::FBForm ():
    QWidget()
{
    SELECTED = NULL;
    IS_SELECTED_MOVING = false;
    INSERT_SHOWED = NULL;

    decoratorDefault = new FBDecorator();

    // Set transparent background.
    this->setStyleSheet("QWidget{background-color:rgba(0,0,0,0);}");

    lvForm = new QVBoxLayout(this);
    lvForm->setContentsMargins(0,0,0,0);
    lvForm->setSpacing(0);

    // Add initial form components.
    this->resetContents();
}


// Add already outside-created elem to the form after the passed element
// or at the end.
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
        //...
    }

    // Connect all elem and form events.
    QObject::connect(newElem, SIGNAL(pressed(FBElem*)),
                     this, SLOT(onElemPressed(FBElem*)));
    QObject::connect(newElem, SIGNAL(moved(QMouseEvent*)),
                     this, SLOT(onElemMoved(QMouseEvent*)));
    QObject::connect(newElem, SIGNAL(released()),
                     this, SLOT(onElemReleased()));

    // Set default decorator (~ just assign pointer, no decoring actions).
    newElem->setDecorator(decoratorDefault);

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
void FBForm::resetContents ()
{
    this->deselectElem();

    // Delete all elements. We must do this, because layouts do not delete their
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
    // The form is void only if there is no elems inside it (but there can be other
    // helper widgets).
    bool found = true;
    for (int i=0; i<lvForm->count(); ++i)
    {
        QWidget *w = lvForm->itemAt(i)->widget();
        FBElem *e = qobject_cast<FBElem*>(w);
        if (e != NULL)
        {
            found = false;
            break;
        }
    }
    return found;
}


bool FBForm::isSaveRequired ()
{
    return true;
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
// so on. The order of elems in the list can be any.
QList<FBElem*> FBForm::getAllElems ()
{
    QList<FBElem*> elemsAll;
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
// follow each other vertically in the form. For each element tha name (so in further
// reading it can be possible to identify and create elem by its name) and the set of
// attributes is converted. Additionally each element can modify its json representation.
Json::Value FBForm::toJson ()
{
    Json::Value jsonRootArray;

    // Firstly set the root value to array type, so if the form is void, the void
    // array (not null value) is returned.
    Json::Value jsonStub = 1;
    jsonRootArray.append(jsonStub);
    jsonRootArray.clear();

    QMap<int,FBElem*> mapTopElems = this->getTopElems(); // container elems will write
                                                         // their child elements
    QMap<int,FBElem*>::const_iterator it = mapTopElems.constBegin();
    while (it != mapTopElems.constEnd())
    {
        FBElem* elem = it.value();
        Json::Value jsonElem;

        // 1. Put attributes of an element.
        QMap<QString,FBAttr*> attrs = elem->getAttrs();
        QMap<QString,FBAttr*>::const_iterator it2 = attrs.constBegin();
        Json::Value jsonAttrsSet;
        while (it2 != attrs.constEnd())
        {
            QByteArray baKn;
            baKn = (*it2)->getKeyName().toUtf8();
            jsonAttrsSet[baKn.data()] = ((*it2)->toJson());
            ++it2;
        }
        jsonElem[FB_JSONKEY_ELEM_ATTRS] = jsonAttrsSet;

        // 2. Put keyname of an element.
        // We add here keyname of the element, because it's also a form's responsibility
        // to read and create an element of specific FBElem class in a parseJson()
        // method by its name.
        QByteArray ba;
        ba = elem->getKeyName().toUtf8();
        jsonElem[FB_JSONKEY_ELEM_TYPE] = ba.data();

        // 3. Modify json for the concrete elem.
        // E.g. convert inner elems of the container elem.
        elem->modifyJsonOut(jsonElem);

        // Finally append json representation of an elem to the array.
        jsonRootArray.append(jsonElem);
        ++it;
    }
    return jsonRootArray;
}


// Returns the list of elements (in the passed parameter) by their order reading
// them from given JSON. Returning void list is correct - the form has no elements
// in that case.
// See FBForm::toJson() how the form was converted to JSON.
// Elements are created by their keynames.
// If errors occur false will be returned. Error is returned even if at least one
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
        FBElem *elem = FBForm::createElem(keyElemName);
        if (elem == NULL)
        {
            ok = false; // Elem's name is undefined OR JUST NOT REGISTERED!
            break;
        }

        // 2. Get elem's attributes.
        // Note: here we do not check jsonAttrsSet and jsonAttrsSet[...] for nulls
        // because fore some elems it is ok to have no attrs at all and for some
        // attrs - no values. The correctness of this was checked during the reading
        // of the form (where all syntax checks are made).
        Json::Value jsonAttrsSet = jsonVal[k][FB_JSONKEY_ELEM_ATTRS];
        QMap<QString,FBAttr*> attrs = elem->getAttrs();
        QMap<QString,FBAttr*>::const_iterator it2 = attrs.constBegin();
        while (it2 != attrs.constEnd())
        {
            QByteArray baKn;
            baKn = (*it2)->getKeyName().toUtf8();
            if (!((*it2)->fromJson(jsonAttrsSet[baKn.data()])))
            {
                // We can reach this only if there is an attribute in the elem's structure
                // which does not exist at all in the passed json (json is correct because
                // we checked this in the readForm method). Such situiation is possible
                // only if we read the file of earlier version than the program's one.

                //ok = false;
                //break;
            }
            ++it2;
        }
        if (!ok)
        {
            break;
        }

        // 3. Additionally modify elem if needed.
        // E.g. add inner elems to container elem.
        elem->modifyElemIn(jsonVal[k]);

        retList.append(elem);
    }

    // Delete all already created elems if any error occured.
    if (!ok)
    {
        for (int i=0; i<retList.size(); i++)
        {
            delete retList[i];
        }
        retList.clear();

        return false; // TODO: do we need it here?
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
    this->resetContents(); // clear form and add initial content
    for (int i=0; i<list.size(); i++)
    {
        // Add each elem to the end of form.
        this->addElem(list[i]);
    }
    return true;
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


