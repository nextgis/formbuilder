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

#include "commontablewidget.h"

#include <QWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QPushButton>

namespace Fb
{
namespace Gui
{


/*!
 * \brief A widget-wrapper around CommonTableWidget with all necessary buttons and behavior.
 */
class ItemsWidget: public QWidget
{
    Q_OBJECT

    public:

     explicit ItemsWidget (QWidget *parent, QString caption, const QStringList &column_names,
                           int min_rows, int max_rows, bool must_have_default_row);
     virtual ~ItemsWidget ();

     inline QGridLayout *getMainLayout () const { return lay_grid; }
     inline CommonTableWidget *getTable () const { return table; }

     void hideCsvButton () { but_csv->hide(); }

    protected slots:

     void onClearClicked ();
     void onCsvClicked ();
     void onAddClicked ();
     void onRemoveClicked ();
     void onSetDefaultClicked ();

    protected:

     template <typename Slot>
     QToolButton *createToolButton (QString icon_path, QString hint_text, Slot slot);

     QGridLayout *lay_grid;
     CommonTableWidget *table;

     QToolButton *but_csv;
};


}
}



