/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
 *                                                                                                *
 *    This program is free software: you can redistribute it and/or modify it under the terms     *
 *    of the GNU General Public License as published by the Free Software Foundation, either      *
 *    version 2 of the License, or (at your option) any later version.                            *
 *                                                                                                *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
 *    See the GNU General Public License for more details.                                        *
 *                                                                                                *
 *    You should have received a copy of the GNU General Public License along with this program.  *
 *    If not, see http://www.gnu.org/licenses/.                                                   *
 *                                                                                                *
 **************************************************************************************************/

#pragma once

#include "core/elem_registrar.h"
#include "core/field.h"
#include "attrwatcher_registrar.h"

#include <QTableWidget>
#include <QComboBox>

namespace Fb
{
namespace Gui
{


class FieldComboBox final: public QComboBox
{
    Q_OBJECT

    friend class PropsTableWidget;

    public:

     FieldComboBox ();
     ~FieldComboBox ();

    public slots:

     void onFieldIndexChange (int index);

    signals:

     void fieldSelected (int field_index, const Core::Elem *elem, QString field_slot);

    private:

     const Core::Elem *elem;
     QString field_slot;
};


/*!
 * @brief PropsTableWidget is a table which shows attributes/fields for the selected element and
 * allows to edit them.
 */
class PropsTableWidget final: public QTableWidget
{
    Q_OBJECT

    public:

     explicit PropsTableWidget (QWidget *parent, MainWindow *window);
     ~PropsTableWidget ();

     void fillSelf (const Core::Elem *elem, const QList<Core::Field*> &fields);
     void clearSelf ();
     void updateSelf (const Core::Elem *elem, const QList<Core::Field*> &fields);

    signals:

     void fieldSelected (int field_index, const Core::Elem *elem, QString field_slot);

    private:

//     void resizeEvent (QResizeEvent *event) override;

     void stretchColumns ();
     QWidget *createAttrWidget (const AttrWatcher *attr_watcher);
     FieldComboBox *createFieldCombo (const Core::Elem *elem, QString field_slot,
                                      const QList<Core::Field*> &fields);

     MainWindow *window; // for attr watcher dialogs
};


}
}



