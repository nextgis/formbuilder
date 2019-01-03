/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  updater class
 * Author:   Dmitry Baryshnikov, dmitry.baryshnikov@nextgis.com
 ******************************************************************************
*   Copyright (C) 2018 NextGIS
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

#include "fbupdater.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>


const QStringList packages = (QStringList() << "GSL" << "QCA" << "QScintilla" <<
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

FBUpdater::FBUpdater(QWidget *parent) : NGUpdater (parent)
{

}

const QStringList FBUpdater::ignorePackages()
{
    return packages;
}

const QString FBUpdater::updaterPath()
{
    QFileInfo appDir(qApp->applicationDirPath());

#if defined Q_OS_WIN
    return appDir.dir().absoluteFilePath(NGUpdater::updaterPath());
#elif defined(Q_OS_MACX)
    // 4 level up
    QDir updaterDir = appDir.dir();
    updaterDir.cdUp();
    updaterDir.cdUp();
    updaterDir.cdUp();
    return updaterDir.absoluteFilePath(NGUpdater::updaterPath());
#else
    return NGUpdater::updaterPath();
#endif
}
