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

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <QMouseEvent>

// See paintEvent() from widgets with Q_OBJECT.
//http://stackoverflow.com/questions/18344135/why-do-
//stylesheets-not-work-when-subclassing-qwidget-and-using-q-object
#include <QPainter>
#include <QStyleOption>

#include "json/json.h"

#define FB_STYLENAME_DEFAULT "default"
#define FB_STYLENAME_ANDROID "android"
#define FB_STYLENAME_IOS "ios"
#define FB_STYLENAME_WEB "web"
#define FB_STYLENAME_QGIS "qgis"

#define FB_FORMSIZE_ELEMSPACING 9 //13

#define FB_ANDROIDSCREEN_FONTSIZE_NORMAL 12 //14
#define FB_ANDROIDSCREEN_FONTTYPE "Segoe UI" //"Candara"

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

#define FB_COLOR_LIGHTGREY "rgb(238,238,238)"
#define FB_COLOR_LIGHTMEDIUMGREY "rgb(210,210,210)"
#define FB_COLOR_MEDIUMGREY "rgb(170,170,170)"
#define FB_COLOR_DARKGREY "rgb(100,100,100)"
#define FB_COLOR_VERYDARKGREY "rgb(46,46,46)"
#define FB_COLOR_LIGHTBLUE "rgb(139,183,224)"
#define FB_COLOR_DARKBLUE "rgb(23,111,193)"


// Element & attribute names.
// IMPORTANT. Change or add these names carefully, because this is critical for
// NextGIS Mobile.
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


enum FBElemtype
{
    FBDecoration, FBInput, FBGrouping, FBLayout
};

enum FBAttrrole
{
    FBNoRole, FBImportant, FBOtherAttrChange, FBStructureChange,
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


class FBForm;
class FBElem;
class FBFactory;


/**
 * Attribute.
 * Each FBAttrX class - is an "attribute type" (not "attribute instance") because
 * different elements can have the same attribute types, but with different display
 * names and used for different purposes.
 *
 * Each attribute must be able to:
 * 1. Store its value;
 * 2. Return the QWidget so it can be possible for user to work with value (firstly
 * read and display, secondly write);
 * 3. Save/load self to/from JSON;
 * 4. (Optionally) return its value (may be complex) so element, which is the owner
 * of this attr, can change its appearance.
 *
 * One attribute equals to one value in the project (in form.json of ngfp file).
 * That's why complex attribute (e.g. lists of values for Double-list elem) is
 * usually edited in complex way: with the help of special dialogues, which are
 * opened by pressing BUTTON in the attrs table.
 *
 * The getWidget() method must return a QWidget which is inited without parent. The
 * actual parent may be table in main app's GUI which will delete this widget self.
 *
 * FOR DEVELOPERS: add new attribute by subclassing from one of the FBAttrX classes
 * or FBAttr class.
 * WARNING. Each attribute must have a unique key name.
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
     QString getKeyName () { return keyName; }
     QString getDisplayName () { return displayName; }
     QString getDescription () { return description; }
     FBAttrrole getRole () { return role; }
    signals:
     void changeOtherAttr (); // in order to signalize to other attrs of this element
                          // that they can be changed
     void changeAppearance (); // indicates that changing this attr changes elem's
                               // appearance
    protected:
     FBElem *elemPtr; // parent elem
     QString keyName;
     QString displayName;
     QString description;
     FBAttrrole role;
};


/**
 * Element.
 * Each element has:
 * 1. List of attributes (but can be void);
 * 2. Its own graphical representation which can vary among form styles;
 * 3. Common way to convert itself to/from JSON: basically it is just a listing of
 * its attributes.
 *
 * FOR DEVELOPERS: add new element by subclassing one of the FBElemX classes.
 * See FBFactory class how to add new element. Do not forget to put Q_OBJECT macro
 * to each FBElem class, while in FB it will be used for type casting for
 * implementing common actions for elems.
 */
class FBElem: public QWidget
{
    friend class FBForm;

    Q_OBJECT // we need this also for type-casting with qobject_cast

    public:
     FBElem (FBFactory *fctPtr);
     virtual ~FBElem () { }
     virtual Json::Value toJson ();
     virtual bool fromJson (Json::Value jsonValue);
     QSet<FBAttr*> getAttrs () { return attrs; }
     FBFactory *getFctPtr () { return fctPtr; }
     QString getDisplayName ();

    protected slots:
     virtual void changeAttrValue () = 0;
     virtual void updateAppearance () = 0;

    protected:
     virtual void changeStyle (QString styleName);
     void clearContents ();
     void setSelectStyle ();
     void setDeselectStyle ();
     void mousePressEvent (QMouseEvent *event);
     void mouseReleaseEvent (QMouseEvent *event);
     void mouseMoveEvent (QMouseEvent *event);
     void paintEvent (QPaintEvent *event);

    signals:
     void pressed (FBElem *thisElem);
     void moved (QMouseEvent *event);
     void released ();
    
    protected: // fields
     FBFactory *fctPtr; // parent factory
     QSet<FBAttr*> attrs;
     QVBoxLayout *lvMain; // store it for deleting style decorations & inner elems
};


/** 
 * Insert widget.
 * Final helper class for elements movement.
 *
 * WARNING. FBInsertWidget class name will be used to determine widget type! Do
 * not change it.
 * TODO: may be use other way to determine the Insert Widget, e.g. qobject_cast<>.
 */
class FBInsertWidget: public QWidget 
{
    Q_OBJECT  // required for widget type determination
    public:
     FBInsertWidget (QWidget* parent);
     ~FBInsertWidget () { }
     void paintEvent (QPaintEvent *event);
     void setShowStyle ();
     void setHideStyle ();
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
 * also because that's why we can get all form's elems by their Y coordinate.
 * All other layouts and groupings of elems are made via specific elements.
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
     static void initEnv ();
     static void deinitEnv ();

    public:
     FBForm ();
     ~FBForm () { }

     // manipulation
     void addElem (FBElem *newElem, FBElem *afterThisElem = NULL);
     void moveElem (FBElem* elem, FBInsertWidget *insteadInsWidget);
     void deleteElem (FBElem* elem);
     void deleteSelected ();
     void clear ();

     // selection
     void selectElem (FBElem *elem);
     void deselectElem ();

     // info
     bool isVoid ();
     QMap<int,FBElem*> getTopElems ();
     QList<FBElem*> getAllElems ();
     QList<FBElem*> getSelectedElems ();

     // json
     Json::Value toJson ();
     static bool parseJson (Json::Value jsonVal, QList<FBElem*> &retList);
     bool fromJson (Json::Value jsonVal);

     // visual
     void updateStyle (QString styleName);

    signals: // used by GUI to update its menus
     void elemPressed ();
     void elemMoved ();

    private slots: // used by form to work with global elems variables
     void onElemPressed (FBElem *elem);
     void onElemReleased ();
     void onElemMoved (QMouseEvent *event);

    private:

     bool modified; // will be set to true/false in order to signalize that the
                    // form's elements structure has been changed and there is
                    // a need to save it

     QVBoxLayout *lvForm; // the layout of the form, i.e. the layout of this widget

     // global variables for elems manipulating
     FBElem *SELECTED; //QList<FBElem*> SELECTED;
     bool IS_SELECTED_MOVING;
     FBInsertWidget* INSERT_SHOWED;

     QString curStyle;
};


/**
 * Factory. For each elem must be its own concrete factory.
 * The main purpose of factory is to create elems in a common way. Factory also stores
 * static data of the elems.
 *
 * FOR DEVELOPERS: create new FBElemX and its FBFactoryX implementations. After this
 * add FBFactoryX to static method initAll() to register it.
 * WARNING. Each element must have a unique key name.
 */
class FBFactory
{
    public:

     static void initAll (QWidget *appWidget = NULL);
     static void deinitAll ();
     static FBFactory *getFctByName (QString keyName);
     static QList<FBFactory*> getAllFcts () { return fctsElem; }

    public:

     FBFactory (QString keyName, QString displayName,
                FBElemtype type, QString imgPath)
        { this->keyName = keyName; this->displayName = displayName;
          this->type = type; this->imgPath = imgPath; }
     virtual ~FBFactory () {}
     virtual FBElem *create () = 0;
     QString getKeyName () { return keyName; }
     QString getDisplayName () { return displayName; }
     FBElemtype getType () { return type; }
     QString getImgPath () { return imgPath; }

    protected:

     QString keyName;
     QString displayName;
     FBElemtype type;
     QString imgPath;

     static QList<FBFactory*> fctsElem;
};


#endif //FORM_CORE_H
