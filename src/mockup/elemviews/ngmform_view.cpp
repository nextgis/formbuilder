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

#include "ngmform_view.h"

#include "container.h"

#include <QScrollArea>
#include <QScrollBar>

using namespace Fb;
using namespace Mockup;


NgmFormView::NgmFormView (const Core::Elem *elem):
    ElemView(elem),
    can_scroll_to_bottom(false)
{
    scr_main = new QScrollArea(w_base);
    scr_main->setObjectName("scr_main");
    scr_main->setAlignment(Qt::AlignCenter);
    scr_main->setAutoFillBackground(true);
    scr_main->setWidgetResizable(true);

    QScrollBar* scrollbar = scr_main->verticalScrollBar();
    connect(scrollbar, &QScrollBar::rangeChanged, this, &NgmFormView::onScrollBarRangeChanged);

    lay_base->addWidget(scr_main);

    container = new Container(scr_main, ContainerType::Column, this);
    scr_main->setWidget(container);

    // TEMP.
    scr_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    container->setMaximumWidth(267 - 8);
}

NgmFormView::~NgmFormView ()
{
}


void NgmFormView::onScrollBarRangeChanged (int min, int max)
{
    Q_UNUSED(min);

    if (can_scroll_to_bottom)
    {
        scr_main->verticalScrollBar()->setValue(max);
        can_scroll_to_bottom = false;
    }
}
