/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  different devices
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

#ifndef DEVICES_H
#define DEVICES_H

#include "screen_core.h"

// Has two states: portrait and landscape.
class FBDeviceMobile: public FBDevice
{
    public:
     FBDeviceMobile (QPair<int,int> resolution, float diagonal, float dpi,
                     QString name, QString imgPath):
         FBDevice(resolution,diagonal,dpi,name,imgPath)
     {
         states.append(FBState(QObject::tr("Portrait"),
                               QObject::tr("Portrait orientation"),":/img/phone_port.png"));
         states.append(FBState(QObject::tr("Landscape"),
                               QObject::tr("Landscape orientation"),":/img/phone_land.png"));
     }
     virtual ~FBDeviceMobile() {}
};
class FBDeviceTablet: public FBDevice
{
    public:
     FBDeviceTablet (QPair<int,int> resolution, float diagonal, float dpi,
                     QString name, QString imgPath):
         FBDevice(resolution,diagonal,dpi,name,imgPath)
     {
         states.append(FBState(QObject::tr("Portrait"),
                               QObject::tr("Portrait orientation"),":/img/tablet_port.png"));
         states.append(FBState(QObject::tr("Landscape"),
                               QObject::tr("Landscape orientation"),":/img/tablet_land.png"));
     }
     virtual ~FBDeviceTablet() {}
};


#endif //DEVICES_H
