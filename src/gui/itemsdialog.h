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

#ifndef ITEMSDIALOG_H
#define ITEMSDIALOG_H

#include "inputtablewidget.h"

    #include "ogrsf_frmts.h" // TEMP

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

namespace Fb
{
namespace Gui
{


/*!
 * @brief A common dialog which aim to enter lists of items.
 */
class FbItemsDialog: public QDialog
{
    Q_OBJECT

    public:
     FbItemsDialog (QWidget *wParent, const QStringList &listColumns);
     virtual ~FbItemsDialog ();
     bool putItems(const QList<QStringList> &listItems) { return m_wTable->putItems(listItems); }
     void getItems (QList<QStringList> &listItems) { m_wTable->getItems(listItems, true); }

    protected slots:
     void showMessage (QString sText, bool bIsCritical);
     void showQuestion (QString sText);
     void onButClearClicked ();
     void onButCsvClicked ();
     void onButOkClicked ();
    protected:
     FbInputTableWidget *m_wTable;
     QLabel *m_wLabCaption;
     QToolButton *m_wButClear;
     QToolButton *m_wButCsv;
     QPushButton *m_wButOk;

    private:
     bool u_areCsvFieldIndexesSet (QList<int> listCsvFieldIndexes) const;
    private:
     // TODO: maybe move some common work with GDAL to the specific file/module. Anyway rewrite
     // these methods in future during refactoring.
     GDALDataset *temp_getGdalDataset (QString sPath, int nTableColumnCount);
     QStringList temp_getCsvColumns (GDALDataset *poDS);
     QList<QStringList> temp_getCsvData (GDALDataset *poDS, QList<int> listFieldIndexes);
};


}
}

#endif //ITEMSDIALOG_H



