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

#include "core/field.h"

#include <QTableWidget>
#include <QLabel>

namespace Fb
{
namespace Gui
{


/*!
 * @brief FieldsTableWidget is a table which shows the bound/unbound state of fields.
 */
class FieldsTableWidget final: public QTableWidget
{
    Q_OBJECT

    public:

     explicit FieldsTableWidget (QWidget *parent);
     ~FieldsTableWidget ();

     void updateSelf (const QList<Core::Field*> &fields);
     void clearSelf ();

    private:

     QList<QLabel*> label_ptrs;
};


}
}



