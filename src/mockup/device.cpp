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

#include "device.h"
#include "ui_device.h"

#include "util/common.h"

// TEMP:
#include "decorators/androidanydecorator.h"

using namespace Fb;
using namespace Mockup;


Device::Device (QWidget *parent, DeviceData device_data):
    CustomWidget(parent),
    ui(new Ui::Device),
    device_data(device_data)
{
    ui->setupUi(this);

    // TEMP.
    // Create new screen.
    screen = new Screen(this);
    ui->lay_cen->insertWidget(1, screen);

    // TEMP.
    // Set body style sheet.
    QString body_style_sheet = Util::g_loadStyleSheet(":/styles/black_smartphone.qss");
    this->setStyleSheet(body_style_sheet);

    // TEMP.
    // Install OS decorator and set according style sheet.
    Decorator *decorator = new AndroidAnyDecorator();
    screen->setDecorator(decorator);
    QString os_style_sheet = Util::g_loadStyleSheet(":/styles/android_any.qss");
    screen->setStyleSheet(os_style_sheet);

    auto screen_size = Screen::calculateSize(device_data.width_res, device_data.height_res,
                                             device_data.diagonal);

    // Define some initial position/orientation of a device (not important, can be changed by user).
    if (device_data.can_rotate) // e.g. phones, tablets => start vertical
        this->resizeScreen(screen_size.second, screen_size.first);
    else // e.g. monitors => start horizontal
        this->resizeScreen(screen_size.first, screen_size.second);
}

Device::~Device ()
{
    delete ui;
}


void Device::resizeScreen (int width, int height)
{
    // Note: a QScrollArea which owns the Device widget must have widgetResizable = false.
    screen->setFixedSize(width, height);
    this->adjustSize();
}



