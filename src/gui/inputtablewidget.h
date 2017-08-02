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

#ifndef INPUTTABLEWIDGET_H
#define INPUTTABLEWIDGET_H

#include <QTableWidget>
#include <QColor>

namespace Fb
{
namespace Gui
{


const int MAX_INPUTTABLE_ROWS = 65535;
const int MAX_INPUTTABLE_STRINGSIZE = 255;
const QColor COLOR_INPUTTABLE_NOTSEL = QColor(255,255,255);
const QColor COLOR_INPUTTABLE_DEF = QColor(139,183,224);
const QColor COLOR_INPUTTABLE_LAST = QColor(255,255,150);


/*!
 * @brief A common table widget for entering a list of items. The table has an obligatiry row at the
 * end ("input" row) which serves for entering new items.
 * WARNING. The manual change of columns/rows count of this table outside can lead to undefined
 * behavior. You should define the appropriate columns count in a constructor and add rows with
 * the help of specific methods.
 */
class FbInputTableWidget: public QTableWidget
{
    Q_OBJECT

    public:
     FbInputTableWidget (QWidget *wParent, const QStringList &listColumns);
     virtual ~FbInputTableWidget ();
    public:
     bool putItems (const QList<QStringList> &listItems, int nDefaultRow = -1);
     void getItems (QList<QStringList> &listItems, int &nDefaultRow, bool bGetFromInputRow = false);
     void clearItems ();
     void makeCurrentRowDefault ();
     QStringList getHorizontalHeaderItems () const;
     static bool s_isStringVoid (QString str);
     static bool s_chopString (QString &str);

    protected:
     bool addInputRow ();
     bool hasInputRow () const { return m_bHasInputRow; }
     void switchToInputRow ();
     void addItemsFromInputRow ();
     void deleteCurrentRow ();
     void completeRow (int nRow);
     bool isRowVoid (int nRow) const;
     bool isOneInRowVoid (int nRow) const;
    protected: // events
     virtual void keyPressEvent (QKeyEvent *pEvent);
    protected slots:
     void onCellChanged (int nRow, int nCol);

    private:
     void colorRow (int nRow, QColor oColor);
     void u_setDefaultEditTriggers ();
     bool u_commitAndClosePersistentEditor (QTableWidgetItem* pItem);
    private:
     bool m_bHasInputRow;
     int m_nDefaultRow;
     QAbstractItemView::EditTriggers m_eEditTrigs;
};


}
}

#endif //INPUTTABLEWIDGET_H



