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

#include "custom_widget.h"
#include "screen.h"
#include "device_data.h"

namespace Ui { class Device; }

namespace Fb
{
namespace Mockup
{


/*!
 * @brief A mock-up of a "device with a screen". The main ability of the device is to hold a Screen.
 */
class Device: public CustomWidget
{
    Q_OBJECT

    public:

     explicit Device (QWidget *parent, DeviceData device_data);
     virtual ~Device ();

     inline Screen *getScreen () const { return screen; }

    protected:

     void resizeScreen (int width, int height);

     Ui::Device *ui;
     Screen *screen;
     DeviceData device_data;
};


}
}
