/***************************************************************************************************
 *    Project:  NextGIS Formbuilder
 *    Author:   Mikhail Gusev, gusevmihs@gmail.com
 ***************************************************************************************************
 *    Copyright (C) 2014-2017 NextGIS
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
 **************************************************************************************************/

#ifndef CSVCOLUMNSDIALOG_H
#define CSVCOLUMNSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QGridLayout>

namespace Fb
{
namespace Gui
{


/*!
 * @brief A dialog which aim to select the columns for the items table from a csv file.
 */
class FbCsvColumnsDialog: public QDialog
{
    Q_OBJECT

    public:
     FbCsvColumnsDialog (QWidget *wParent, QString sFile, const QStringList &listColumns);
     virtual ~FbCsvColumnsDialog ();

    protected slots:
     void onButOkClicked ();
    protected:
     QLabel *m_wLabFile;
     QGridLayout *m_lgGrid;
     QPushButton *m_wButOk;
};


}
}

#endif //CSVCOLUMNSDIALOG_H



