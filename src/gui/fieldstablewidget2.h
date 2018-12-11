/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2018 NextGIS                                                   *
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

#include "core/field.h"

#include <QTableWidget>
#include <QLabel>

namespace Fb
{
namespace Gui
{


class FieldsTableWidget2 final: public QTableWidget
{
    Q_OBJECT

    public:

     explicit FieldsTableWidget2 (QWidget *parent);
     ~FieldsTableWidget2 ();

     void updateSelf (const QList<Core::Field*> &fields, const Core::Elem *selected_elem);
     void clearSelf ();

    signals:

     void fieldAliasChanged (const QString &text);
     void fieldTypeChanged (int index);

    private:

     void stretchColumns ();
};


}
}



