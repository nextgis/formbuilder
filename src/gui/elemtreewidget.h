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

#pragma once

#include <QTreeWidget>

namespace Fb
{
namespace Gui
{

class MainWindow;


/*!
 * @brief ElemTreeWidget is a tree which loads all available elements and allows to drag/place them
 * somewhere (e.g. to the "screen").
 */
class ElemTreeWidget final: public QTreeWidget
{
    Q_OBJECT

    public:

     explicit ElemTreeWidget (QWidget *parent, QString icons_dir);
     ~ElemTreeWidget ();

     inline QString getElemNamePressed () const { return elem_name_pressed; }

     void setCtrlPressed (bool is_pressed);

     void fillSelf ();

     inline void setBlockUnsupported (bool do_block) { block_unsupported = do_block; }
     inline bool getBlockUnsupported () { return block_unsupported; }

    signals:

     void elemItemPressed ();
     void elemItemMoved (QPoint coords);
     void elemItemReleased ();

     void elemItemCtrlClicked ();

    private:

     void mousePressEvent (QMouseEvent *event) override;
     void mouseMoveEvent (QMouseEvent *event) override;
     void mouseReleaseEvent (QMouseEvent *event) override;

     QString icons_dir;
     QString elem_name_pressed;

     bool is_ctrl_pressed;

     QCursor last_cursor;

     bool block_unsupported;
};


}
}



