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
 
#include "splitcomboitemsdialog.h"

using namespace Fb::Gui;


/// Constructor.
FbSplitComboItemsDialog::FbSplitComboItemsDialog (QWidget *wParent):
    FbItemsDialog(wParent)
{
    QStringList hdrs;
    hdrs<<tr("Inner")<<tr("Displayed 1")<<tr("Displayed 2");

    m_wTable->setColumnCount(3);
    m_wTable->setHorizontalHeaderLabels(hdrs);
    m_wTable->addEnterRow();
}


/// Destructor.
FbSplitComboItemsDialog ::~FbSplitComboItemsDialog ()
{
}



