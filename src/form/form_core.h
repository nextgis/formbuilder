/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  all core form definitions
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

#ifndef FORM_CORE_H
#define FORM_CORE_H

#include <QPair>
#include <QList>
#include <QDateTime>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "json/json.h"

#define FB_FORM_INSWIDGETHEIGHT 8
#define FB_ELEM_DEFHEIGHT 30
#define FB_ELEM_DEFFONTTYPE "Segoe UI"
#define FB_ELEM_DEFFONTSIZE 12

// Reserved string constants for project's form file.
#define FB_JSONKEY_ELEM_TYPE "type"
#define FB_JSONKEY_ELEM_ATTRS "attributes"
#define FB_JSONKEY_ELEM_ELEMS "elements"
#define FB_JSONKEY_ATTRVAL_KEYNAME "name"
#define FB_JSONKEY_ATTRVAL_DISPNAME "alias"
#define FB_JSONKEY_ATTRVAL_DEFAULT "default"
#define FB_JSONKEY_ATTRVAL_VALUES "values"
#define FB_JSONVALUE_ATTRVAL_NOKEYNAME "-1"
#define FB_JSONVALUE_ATTRVAL_NODISPNAME "--"
#define FB_DEFVALUE_NOTSELECTED "-"

// Element & attribute names.
// IMPORTANT. Change or add these names carefully, because this is critical for
// NextGIS Mobile.
// NOTE: keynames of elements and atributes are also usually used to define their
// styles in the ScreenX classes.
#define FB_ELEMNAME_TEXT_LABEL "text_label"
#define FB_ELEMNAME_IMAGE "image"
#define FB_ELEMNAME_TEXT_EDIT "text_edit"
#define FB_ELEMNAME_BUTTON "button"
#define FB_ELEMNAME_COMBOBOX "combobox"
#define FB_ELEMNAME_CHECKBOX "checkbox"
#define FB_ELEMNAME_RADIOGROUP "radio_group"
#define FB_ELEMNAME_COMPASS "compass"
#define FB_ELEMNAME_DATE_TIME "date_time"
#define FB_ELEMNAME_DOUBLE_COMBOBOX "double_combobox"
#define FB_ELEMNAME_PHOTO "photo"
#define FB_ELEMNAME_SIGNATURE "signature"
#define FB_ELEMNAME_SPACE "space"
#define FB_ELEMNAME_COUNTER "counter"
#define FB_ELEMNAME_COORDINATES "coordinates"
//#define FB_ELEMNAME_TABS "tabs"
//#define FB_ELEMNAME_GROUP "group"
//#define FB_ELEMNAME_VERTICAL_LAYOUT "ver_layout"
//#define FB_ELEMNAME_HORIZONTAL_LAYOUT "hor_layout"
#define FB_ATTRNAME_FIELD "field"
#define FB_ATTRNAME_FIELD_dcmb_1 "field_level1"
#define FB_ATTRNAME_FIELD_dcmb_2 "field_level2"
#define FB_ATTRNAME_VALUE "value"
#define FB_ATTRNAME_VALUE_mult "values"
#define FB_ATTRNAME_TEXT "text"
#define FB_ATTRNAME_INITVALUE "init_value"
#define FB_ATTRNAME_INITVALUE_datetime "datetime"
#define FB_ATTRNAME_STORELAST "last"
//#define FB_ATTRNAME_REQUIRED "required"
//#define FB_ATTRNAME_OTHERINPUT_dialog "is_dialog"
//#define FB_ATTRNAME_OTHERINPUT_dcmb_1 "big_list_level1"
//#define FB_ATTRNAME_OTHERINPUT_dcmb_2 "big_list_level2"
#define FB_ATTRNAME_ONLYNUMBERS "only_figures"
#define FB_ATTRNAME_MAXSTRINGCOUNT "max_string_count"
#define FB_ATTRNAME_DATETYPE "date_type"
#define FB_ATTRNAME_INPUTSEARCH "input_search"
#define FB_ATTRNAME_ALLOWADDING "allow_adding_values"
#define FB_ATTRNAME_GALLERYSIZE "gallery_size"
#define FB_ATTRNAME_NGW_ID "ngw_id"
#define FB_ATTRNAME_NGWLOGIN "ngw_login"
#define FB_ATTRNAME_PREFIX "prefix"
#define FB_ATTRNAME_SUFFIX "suffix"
#define FB_ATTRNAME_INCREMENT "increment"
#define FB_ATTRNAME_CRS "crs"
#define FB_ATTRNAME_FORMAT "format"
#define FB_ATTRNAME_HIDDEN "hidden"


enum FBElemtype
{
    FBDecoration, FBInput, FBGrouping, FBLayout, FBMetric, FBSpecific
};

enum FBAttrrole
{
    FBNoRole, FBImportant, FBOtherAttrChange, //FBStructureChange,
};

struct FBDatetimeFormat
{
    QString name;
    QString strDisplayEn;
    QString strDisplayRu;
    int codeNgfp;
    QString strNgfp;
    FBDatetimeFormat (QString n, QString sde, QString sdr, int c, QString sn)
    {
        name = n;
        strDisplayEn = sde;
        strDisplayRu = sdr;
        codeNgfp = c;
        strNgfp = sn;
    }
    ~FBDatetimeFormat () { }
};

typedef QPair<QList<QPair<QString,QString> >, int> FBListValue;
Q_DECLARE_METATYPE(FBListValue);
typedef QPair<QList<QList<QPair<QString,QString> > >, QList<int> > FBListsValues;
Q_DECLARE_METATYPE(FBListsValues);
typedef QPair<FBListValue,FBListsValues> FBDoublelistValue;
Q_DECLARE_METATYPE(FBDoublelistValue);
//typedef QPair<QDateTime,bool> FBDatetimeValue;
//Q_DECLARE_METATYPE(FBDatetimeValue);

class FBForm;
class FBElem;
class FBFctelem;


/**
 * Abstract attribute.
 * Each concrete FBAttrX class - is an "attribute type" (not "attribute instance")
 * because different elements can have the same attribute types, but with different
 * display names and used for different purposes.
 *
 * Each attribute must be able to:
 * 1. Store its value;
 * 2. Save/load self to/from JSON;
 * 3. Return its value (may be complex) so element, which is the owner of this attr
 * can change its appearance or change other attribute's value.
 * 4. Return the QWidget so it can be possible for user to work with value (firstly
 * read and display, secondly write);
 *
 * One attribute equals to one SINGLE value in the project (in form.json of ngfp file).
 * That's why complex attribute (e.g. lists of values for Double-list elem) is
 * usually edited in complex way: with the help of special dialogues, which are
 * opened by pressing BUTTON in the attrs table.
 *
 * FOR DEVELOPERS: add new attribute by subclassing from one of the FBAttrX classes
 * or FBAttr class. The getWidget() method must return a QWidget which is inited
 * without parent. The actual parent may be table in main app's GUI which will delete
 * this widget self.
 */
class FBAttr: public QObject
{
    Q_OBJECT

    public:

     FBAttr (FBElem *parentElem, QString keyName, QString displayName, QString descr,
             FBAttrrole role);
     virtual ~FBAttr () { }

     virtual Json::Value toJson () = 0;
     virtual bool fromJson (Json::Value jsonVal) = 0;

     virtual QWidget *getWidget () = 0; // its the caller responsib. to delete it

     virtual QVariant getValue () = 0;

     QString getKeyName () { return keyName; }
     QString getDisplayName () { return displayName; }
     QString getDescription () { return description; }
     FBAttrrole getRole () { return role; }

    signals:

     void changeOtherAttr (FBAttr* thisAttr); // in order to signalize to some other
                                 //attrs of this element that they should be changed
     void changeAppearance (FBAttr* thisAttr); // indicates that changing this attr
                                               // changes elem's appearance
    protected:

     FBElem *elemPtr; // "parent" elem

     QString keyName;
     QString displayName;
     QString description;
     FBAttrrole role;
};


/**
 * Default decorator class.
 * Decorators are used to set the visual apperance to elememnts of the form.
 *
 * IMPORTANT FOR DEVELOPERS. Decorate elements only with the help of other widgets:
 * add them to the main layout of elem via according methods and change their
 * appearance. DO NOT CHANGE FBElem widget itself (except size policy and hint).
 * TODO: maybe somehow make FBElem protected-inherited from QWidget and implement
 * according interface, but that is probably not possible because all QWidget-s have
 * their copy-constructors declared private.
 *
 * IMPORTANT FOR DEVELOPERS. In overriden methods ensure that all added to the elem
 * decorational widgets/layouts/etc are finally the child widgets for the elem, so
 * there will be no memory leaks after the clearing or deletion the elem.
 */
class FBDecorator
{
    public:
     FBDecorator () { }
     virtual ~FBDecorator () { }
     virtual void redecor (FBElem* elem);
     virtual void update (FBElem *elem);
};


/**
 * Abstract element.
 *
 * Each concrete element has: a list of attributes (but can be void) and (optionally)
 * describes how attributes' changings have influence on other attributes of this elem
 * and on elem's appearance. Each concrete elem must contain only unique set of attributes
 * - i.e. their keynames must be unique in the context of this elem, but the FBAttrX
 * classes can be any.
 *
 * Every element must fully describe itself. It should be possible to create element
 * directly via constructor or via factory in a common way.
 * IMPORTANT. Each FBElemX should have the FBFctelemX pair in order the form can read
 * and create element from the project file!
 *
 * Every element IS NOT responsible for its visual appearance. This is done by decorators
 * - see FBDecorator class. Decorational widgets will belong to elem as the children
 * widgets and will be deleted with it. Reason of such approach: each concrete screen/style
 * can create very different set of widgets for decoring and also because elements are
 * not "for visual purpose" while "for aggregating attributes" + "for standard form
 * manipulations".
 *
 * FOR DEVELOPERS: add new element by subclassing FBElem or derived classes. Do not
 * forget to put Q_OBJECT macro to the class, while in FB it will be used for type
 * casting for implementing common actions for elems. If you want to set an elem's
 * style for some screen - see FBDecoratorX classes.
 * IMPORTANT. After adding new elem do not forget to create its factory and register
 * it in FBForm during runtime.
 */
class FBElem: public QWidget
{
    friend class FBForm;

    Q_OBJECT // we need this also for type-casting with qobject_cast!

    signals:

     void pressed (FBElem *thisElem);
     void moved (QMouseEvent *event);
     void released ();

    public:

     // Main:
     FBElem ();
     virtual ~FBElem () { }

     // Json:
     virtual void modifyJsonOut (Json::Value &jsonValue) { return; }
     virtual void modifyElemIn (Json::Value jsonValue) { return; }

     // For decoring:
     void clearContents ();
     bool addAsDecor (QWidget *widget, QString regName = "");
     void addAsDecor (QLayout *layout);
     bool registerAsDecor (QWidget *widget, QString regName);
     QWidget *findAsDecor (QString regName);
     void setDecorator (FBDecorator *decorator);
     void redecorateSelf () { decoratorPtr->redecor(this); }
     void updateSelf () { decoratorPtr->update(this); }

     // Info:
     QMap<QString,FBAttr*> getAttrs () { return attrs; }
     FBAttr* getAttr (QString attrKeyName);
     virtual QString getKeyName () = 0;
     virtual QString getDisplayName () = 0;
     virtual QString getDescription () = 0;
     virtual FBElemtype getType () = 0;

    protected slots:

     virtual void onChangeAttrValue (FBAttr *attr);
     virtual void onChangeAppearance (FBAttr *attr);

    protected: // methods

     void setSelectStyle ();
     void setDeselectStyle ();

     void mousePressEvent (QMouseEvent *event);
     void mouseReleaseEvent (QMouseEvent *event);
     void mouseMoveEvent (QMouseEvent *event);
     void paintEvent (QPaintEvent *event);
    
    protected: // fields

     QMap<QString,FBAttr*> attrs;
     QVBoxLayout *lvMain; // store it for deleting style decorations & inner elems

     // We store the pointer to decoartor so not to connect every elem with screen
     // to change its appearance each time.
     FBDecorator *decoratorPtr; // FBElem is not the owner of the decorator!
};


/** 
 * Insert widget.
 * Helper class for elements movement.
 *
 * WARNING. FBInsertWidget class name will be used to determine widget type! Do
 * not change it.
 * TODO: may be use other way to determine the Insert Widget, e.g. qobject_cast<>.
 */
class FBInsertWidget: public QWidget
{
    friend class FBForm;
    Q_OBJECT  // required for widget type determination
    public:
     FBInsertWidget (QWidget* parent);
     ~FBInsertWidget () { }
     void paintEvent (QPaintEvent *event);
     void setShowStyle ();
     void setHideStyle ();
};


/**
 * Abstract elem's factory.
 * See FBElem description.
 */
class FBFctelem
{
    public:
     FBFctelem () { }
     virtual ~FBFctelem () { }
     virtual FBElem *create () = 0;
};

/**
 * Abstract factory for elems which attributes need application widget for displaying
 * its dialogs.
 * See FBFctelem description.
 */
class FBFctelemAppwidget: public FBFctelem
{
    public:
     FBFctelemAppwidget (QWidget *appWidget): FBFctelem ()
     {
         this->appWidget = appWidget;
     }
     virtual ~FBFctelemAppwidget () { }
    protected:
     QWidget *appWidget;
};


/**
 * Form.
 * Final form class. Form contains elements and provides the capability to
 * manage them by user, using app events (via mouse).
 *
 * Visually form is "transparent" and only its elements are rendered to the
 * screen. Technically form is a QWidget which contains elements in its layout
 * like any simple Qt-widget, so it allows to read their strict order and to
 * render it to the screen.
 *
 * All elements are always followed each other in the form vertically. This
 * is because of the structure of the final JSON file where form is saved, and
 * also because that's why we can determine the elems' order by their Y coordinate.
 * All other layouts and groupings of elems are made via specific "container"
 * elements.
 *
 * The special insert-element is placed between any pair of elements  so it can
 * be possible to move them via mouse.
 *
 * The form, as well as its elements and attributes, has specific methods of
 * converting self to/from JSON.
 */
class FBForm: public QWidget
{ 
    Q_OBJECT

    public:

     static QList<FBDatetimeFormat*> DATETIME_FORMATS;
     static void initDateTimeFormats ();
     static void deinitDateTimeFormats ();

     static bool registerElem (FBFctelem *fct);
     //static void registerAll ();
     static FBElem* createElem (QString elemKeyName);

    signals: // can be used by GUI to update its menus

     void elemPressed ();
     void elemMoved ();

    public:

     FBForm ();
     ~FBForm ();

     // manipulation
     void addElem (FBElem *newElem, FBElem *afterThisElem = NULL);
     void moveElem (FBElem* elem, FBInsertWidget *insteadInsWidget);
     void deleteElem (FBElem* elem);
     void deleteSelected ();
     void resetContents ();

     // selection
     void selectElem (FBElem *elem);
     void deselectElem ();

     // info
     bool isVoid ();
     bool isSaveRequired ();
     QMap<int,FBElem*> getTopElems ();
     QList<FBElem*> getAllElems ();
     QList<FBElem*> getSelectedElems ();
     FBDecorator *getDefaultDecoratorPtr () { return decoratorDefault; }

     // json
     Json::Value toJson ();
     static bool parseJson (Json::Value jsonVal, QList<FBElem*> &retList);
     bool fromJson (Json::Value jsonVal);

    private slots: // used by form to work with global elem variables

     void onElemPressed (FBElem *elem);
     void onElemReleased ();
     void onElemMoved (QMouseEvent *event);

    private:

     bool modified; // will be set to true/false in order to signalize that the
                    // form's elements structure has been changed and there is
                    // a need to save it
     QVBoxLayout *lvForm; // the layout of the form, i.e. the layout of this widget

     // global variables for elems manipulating. Not static!
     FBElem *SELECTED;
     //QList<FBElem*> SELECTED;
     bool IS_SELECTED_MOVING;
     FBInsertWidget* INSERT_SHOWED;

     static QMap<QString,FBFctelem*> FACTORIES; // registered elements

     FBDecorator *decoratorDefault;
};


#endif //FORM_CORE_H
