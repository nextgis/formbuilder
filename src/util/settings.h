/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
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

#include <QSettings>
#include <QCoreApplication>

#define FB_STS_LANGUAGE "language"
#define FB_STS_LAYERDLG_W "layerdlg_w"
#define FB_STS_LAYERDLG_H "layerdlg_h"
#define FB_STS_LAYERDLG_COL0_W "layerdlg_col0_w"
#define FB_STS_DITMDLG_W "ditmdlg_w"
#define FB_STS_DITMDLG_H "ditmdlg_h"
#define FB_STS_DITMDLG_COL0_W "ditmdlg_col0_w"
#define FB_STS_TITMDLG_W "titmdlg_w"
#define FB_STS_TITMDLG_H "titmdlg_h"
#define FB_STS_TITMDLG_COL0_W "titmdlg_col0_w"
#define FB_STS_TITMDLG_COL1_W "titmdlg_col1_w"
#define FB_STS_DEPITMDLG_W "depitmdlg_w"
#define FB_STS_DEPITMDLG_H "depitmdlg_h"
#define FB_STS_DEPITMDLG_COL0_W_L "depitmdlg_col0_w_l"
#define FB_STS_DEPITMDLG_COL0_W_R "depitmdlg_col0_w_r"
#define FB_STS_TPAGESDLG_W "tpagesdlg_w"
#define FB_STS_TPAGESDLG_H "tpagesdlg_h"
#define FB_STS_ABOUTDLG_W "aboutdlg_w"
#define FB_STS_ABOUTDLG_H "aboutdlg_h"
#define FB_STS_NGWDLG_W "ngwdlg_w"
#define FB_STS_NGWDLG_H "ngwdlg_h"
#define FB_STS_NGWDLG_COL0_W "ngwdlg_col0_w"
#define FB_STS_NGW_URL "ngw_url"
#define FB_STS_NGW_GUEST "ngw_guest"
#define FB_STS_NGW_LOGIN "ngw_login"
#define FB_STS_NGW_PASSWORD "ngw_password"
#define FB_STS_CSV_FILE "csv_file"
#define FB_STS_SHAPE_FILE "shape_file"
#define FB_STS_NGFP_FILE "ngfp_file"

namespace Fb
{


inline QSettings *g_getSettings ()
{

#ifdef Q_OS_MACOS
    static QSettings settings(QSettings::NativeFormat,
                              QSettings::UserScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName());
#else
    static QSettings settings(QSettings::IniFormat,
                              QSettings::UserScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName());
#endif

/*
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                              "NextGIS_temp", "Formbuilder_temp");
*/

    return &settings;
}


}



