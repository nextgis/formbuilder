/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  new void project dialog
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

#include "fb.h"

FBDialogProjectNew::~FBDialogProjectNew ()
{
}

FBDialogProjectNew::FBDialogProjectNew (QWidget *parent):
    QDialog(parent)
{
    this->setWindowTitle(tr("New project parameters"));

    QLabel *label1 = new QLabel(this);
    label1->setText(tr("Geometry type: "));

    comboGeom = new QComboBox(this);
    for (int i=0; i<FBProject::GEOM_TYPES.size(); i++)
    {
        comboGeom->addItem(FBProject::GEOM_TYPES[i]->strDisplayName,
                           FBProject::GEOM_TYPES[i]->aliasNgw);
    }

    QPushButton *but1 = new QPushButton(this);
    but1->setText(tr("OK"));
    connect(but1,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *vlall = new QVBoxLayout(this);
    vlall->setSpacing(10);
    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(label1);
    hl1->addWidget(comboGeom);
    vlall->addLayout(hl1);
    vlall->addWidget(but1);
}

QString FBDialogProjectNew::getSelectedGeom ()
{
    return comboGeom->currentData(Qt::UserRole).toString();
}
