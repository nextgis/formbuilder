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

#include "elemtreewidget.h"

#include "core/elem_registrar.h"
#include "gui/sizes.h"

#include "framework/access/access.h"

#include <QMouseEvent>

//#define FB_UTF8CHAR_LOCK "\xF0\x9F\x94\x92"
//#define FB_UTF8CHAR_LOCK " \xF0\x9F\x94\x92\0"
//#define FB_UTF8CHAR_LOCK 0x1F512

using namespace Fb;
using namespace Gui;


ElemTreeWidget::ElemTreeWidget (QWidget *parent, QString icons_dir):
    QTreeWidget(parent),
    icons_dir(icons_dir),
    elem_name_pressed(""),
    is_ctrl_pressed(false),
    block_unsupported(true)
{
    this->setObjectName("tree_elems");
    this->setHeaderHidden(true);
    this->setColumnCount(1);
    this->setFocusPolicy(Qt::NoFocus); // so not to show dotted frame
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setItemsExpandable(false);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setIndentation(TREE_INDENT);
    this->setIconSize({TREE_ICON_SIZE, TREE_ICON_SIZE}); // TEMP: do this for NGAccess::lockIcon()
}

ElemTreeWidget::~ElemTreeWidget ()
{
}


void ElemTreeWidget::setCtrlPressed (bool is_pressed)
{
    if (elem_name_pressed != "")
        return;

    if (is_pressed)
    {
        last_cursor = this->cursor();
        this->setCursor(Qt::PointingHandCursor);
        is_ctrl_pressed = true;
    }
    else
    {
        this->setCursor(last_cursor);
        is_ctrl_pressed = false;
    }
}


void ElemTreeWidget::fillSelf ()
{
    this->clear();

    auto elems = Core::ElemRegistrar::getAll();
    for (auto &pair: elems)
    {
        auto elem_key = pair.first;
        auto elem_data = pair.second;

        if (elem_data.is_system)
            continue;

//        QString text;
//        if (block_unsupported && elem_data.is_for_supported)
//            text = QString("🔒 %1").arg(elem_data.display_name);
//        else
//            text = elem_data.display_name;

        QIcon icon;
        if (block_unsupported && elem_data.is_for_supported)
            icon = NGAccess::lockIcon(QIcon(icons_dir + elem_data.icon_dis_file_name),
                                      //this->iconSize()); // at this point no size yet
                                      {TREE_ICON_SIZE, TREE_ICON_SIZE},
                                      QIcon(":/images/lock.svg"));
        else
            icon = QIcon(icons_dir + elem_data.icon_file_name);

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0, Qt::UserRole, QVariant::fromValue(elem_key));
        item->setText(0, elem_data.display_name);
        item->setToolTip(0, QString("<p>%1</p>").arg(elem_data.description));
        item->setIcon(0, icon);

        this->addTopLevelItem(item);
    }

    this->sortItems(0, Qt::AscendingOrder);
}


// If Control button is not pressed:
// We need to create new elements on the Screen by dragging them from this widget to the Screen. We
// do not use Qt drag-and-drop mechanics here because of it's low performance. Instead we use mouse
// events with signals emmited to the MainWindow (which holds Screen widget).
// If Control button is pressed:
// ...
void ElemTreeWidget::mousePressEvent (QMouseEvent *event)
{
    QTreeWidgetItem *item = this->itemAt(event->pos());
    if (item != nullptr)
    {
        QVariant var = item->data(0, Qt::UserRole);
        elem_name_pressed = var.toString();

        if (is_ctrl_pressed)
        {
            emit elemItemCtrlClicked();
            elem_name_pressed = "";
        }
        else
        {
            last_cursor = this->cursor();
            this->setCursor(Qt::ClosedHandCursor);

            emit elemItemPressed();
        }
    }

    QTreeWidget::mousePressEvent(event);
}

// See mousePressEvent() for an explanation.
void ElemTreeWidget::mouseMoveEvent (QMouseEvent *event)
{
    if (is_ctrl_pressed)
        return;

    if (elem_name_pressed == "")
        return;

    QPoint coords = {event->globalX(), event->globalY()};
    emit elemItemMoved(coords);
}

// See mousePressEvent() for an explanation.
void ElemTreeWidget::mouseReleaseEvent (QMouseEvent *event)
{
    if (!is_ctrl_pressed)
    {
        if (elem_name_pressed != "")
            emit elemItemReleased();
        elem_name_pressed = "";

        this->setCursor(last_cursor);
    }

    QTreeWidget::mouseReleaseEvent(event);
}



