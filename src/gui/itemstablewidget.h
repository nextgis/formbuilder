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

#ifndef ITEMSTABLEWIDGET_H
#define ITEMSTABLEWIDGET_H

#include <QTableWidget>
#include <QColor>

namespace Fb
{
namespace Gui
{


const int MAX_ITEMSTABLE_ROWS = 65535;
const int MAX_ITEMSTABLE_STRINGSIZE = 255;
const QColor COLOR_ITEMSTABLE_NOTSEL = QColor(255,255,255);
const QColor COLOR_ITEMSTABLE_DEF = QColor(139,183,224);
const QColor COLOR_ITEMSTABLE_LAST = QColor(255,255,150);


/*!
 * @brief A common table widget for entering a list of items. Except the specific style this table
 * also has the obligatiry row at the end which serves as the field for entering new items.
 */
class FbItemsTableWidget: public QTableWidget
{
    Q_OBJECT

    public:
     FbItemsTableWidget (QWidget *wParent);
     virtual ~FbItemsTableWidget ();
     bool addEnterRow ();
     void switchToEnterRow ();
     void removeAllItems ();
    signals:
     void needShowMessage (QString, bool);
     void needShowQuestion (QString);

    protected slots:
     void addRow ();
     void deleteRow ();
     void makeDefaultRow ();
     void onSelectionChanged ();
     void onCellChanged (int nRow, int nCol);
    protected:
     void keyPressEvent (QKeyEvent *pEvent);
    protected:
     virtual void completeRow (int nRow);
     void unmarkDefaultRow ();
     void markDefaultRow (int nRow);
     bool isRowVoid (int nRow);
     bool isOneInRowVoid (int nRow);
     static bool s_isStringVoid (QString str);
     static void s_reduceString (QString &str);
    protected:
     QAbstractItemView::EditTriggers m_eEditTrigs;
     int m_nMinRows;
     int m_nMaxRows;
     bool m_wasEnterRowAdded;

    private:
     void u_setDefaultEditTriggers ();
     bool u_commitAndClosePersistentEditor (QTableWidgetItem* pItem);
};


}
}

#endif //ITEMSTABLEWIDGET_H



