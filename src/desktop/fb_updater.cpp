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

#include "fb_updater.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>


const QStringList PACKAGES = (QStringList() << "GSL" << "QCA" << "QScintilla" <<
                              "Qt4" << "QWT" << "YAML" << "Python" <<
                              "Operation for all python" << "QGIS" << "Utilities" <<
                              "SQLite3 utilities" << "QHULL utilities" <<
                              "QCA utilities" << "Proj4 utilities" <<
                              "OpenCAD utilities" << "GIF utilities" <<
                              "GDAL utilities" << "EXPAT utilities" <<
                              "CURL utilities" << "Operation for all utils" <<
                              "Spatial libraries" << "SpatiaLite" <<
                              "SpatialIndex" << "SpatialIndex C" << "QGIS Python" <<
                              "QGIS plugins" << "Python library" << "Gui library" <<
                              "Core library" << "App library" <<
                              "Analysis library" << "Documentation" <<
                              "QGIS documentation (English)" <<
                              "QGIS documentation (Russian)" << "PyYaml" <<
                              "Subproces32" << "PySpatialite" << "Six" << "SIP" <<
                              "Requests" << "Raven" << "PyQt4" << "PyQSci" <<
                              "PyQGIS" << "Pytz" << "Python27" << "Pygments" <<
                              "Psycopg" << "Pyparsing" << "OWSLib" << "Numpy" <<
                              "Nose2" <<  "Matplotlib" << "MarkupSafe" << "Kiwi" <<
                              "Jinja2" << "HttpLib2" << "GDAL python" << "future" <<
                              "backports.functools_lru_cache" << "ExifRead" <<
                              "DateUtil" << "Cycler" << "ContextLib2");


FbUpdater::FbUpdater (QWidget *parent):
    NGUpdater(parent)
{
}

const QStringList FbUpdater::ignorePackages ()
{
    return PACKAGES;
}

const QString FbUpdater::updaterPath ()
{
    QFileInfo app_dir(qApp->applicationDirPath());

#if defined Q_OS_WIN
    QDir updater_dir = app_dir.dir();
    //updater_dir.cdUp();
    return updater_dir.absoluteFilePath(NGUpdater::updaterPath());
#elif defined(Q_OS_MACX)
    // 4 level up
    QDir updater_dir = app_dir.dir();
    updater_dir.cdUp();
    updater_dir.cdUp();
    updater_dir.cdUp();
    return updater_dir.absoluteFilePath(NGUpdater::updaterPath());
#else
    return NGUpdater::updaterPath();
#endif
}

