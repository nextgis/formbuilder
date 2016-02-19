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

FBDialogProjectNew::FBDialogProjectNew (QWidget *parent): QDialog(parent)
{
    this->setWindowTitle(tr("New project parameters ..."));

    // TODO: set OS-specific colors and fonts (now are parent colors).
    this->setStyleSheet("");
    this->setStyleSheet("QWidget { color: black }");
    this->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));

    QLabel *label1 = new QLabel(this);
    label1->setText(tr("Geometry type: "));

    comboGeom = new QComboBox(this);

    QMap<QString,FBGeomType>::const_iterator it
            = FBProject::GEOM_TYPES.constBegin();
    while (it != FBProject::GEOM_TYPES.constEnd())
    {
        comboGeom->addItem(it.key());
        ++it;
    }

    QPushButton *but1 = new QPushButton(this);
    but1->setText(tr("OK"));
    connect(but1,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *vlall = new QVBoxLayout(this);
    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(label1);
    hl1->addWidget(comboGeom);
    vlall->addLayout(hl1);
    vlall->addWidget(but1);
}
