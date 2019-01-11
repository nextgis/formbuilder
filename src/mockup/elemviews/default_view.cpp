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

#include "default_view.h"
#include "core/elem_registrar.h"

using namespace Fb;
using namespace Mockup;

#include <QLabel>


DefaultView::DefaultView (const Core::Elem *elem):
    ElemView(elem)
{
    QString text = tr("<unknown element>");

    auto elem_data = Core::ElemRegistrar::get(elem->getKeyName());
    if (elem_data != nullptr)
    {
        text = elem_data->display_name;
    }

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(40);

    w_base->setObjectName("w_default_base");

    QLabel *lab = new QLabel(w_base);
    lab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lab->setText(text);
    lab->setStyleSheet("background: white;"
                       "color: black;"
                       "border: 1px solid black;"
                       "font-family: \"Arial\";"
                       "font-size: 13px;");

    lay_base->addWidget(lab);
}

DefaultView::~DefaultView ()
{
}



