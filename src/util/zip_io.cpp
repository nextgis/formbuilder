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

#include "zip_io.h"

using namespace Fb;
using namespace Util;


void ZipIo::writeFiles (QString zip_path, QList<ZipFileInfo> files)
{
    QByteArray ba_path = zip_path.toUtf8();
    void *h_zip = CPLCreateZip(ba_path.data(), NULL);
    if (!h_zip)
        throw QObject::tr("Unable to create ZIP archive");

    size_t buffer_size = 1024 * 1024;
    GByte *paby_buffer = (GByte*)CPLMalloc(buffer_size);
    QByteArray ba_temp1;
    QByteArray ba_temp2;

    bool ok = true;
    for (int i = 0; i < files.size(); i++)
    {
        ba_temp1 = files[i].file_path.toUtf8();
        ba_temp2 = files[i].file_name_in_zip.toUtf8();
        ok = addFileToZip(ba_temp1.data(), ba_temp2.data(), h_zip, &paby_buffer, buffer_size);
        if (!ok)
            break;
    }

    CPLCloseZip(h_zip);
    CPLFree(paby_buffer);

    if (!ok)
        throw QObject::tr("Unable to write file into ZIP-arcive");

    return;
}


void ZipIo::readFile (QString zip_path, QString file_path, std::string &file_contents)
{
    zip_path.prepend("/vsizip/");
    zip_path.append("/");
    zip_path.append(file_path);

    QByteArray ba_zip_path = zip_path.toUtf8();
    VSILFILE *fp = VSIFOpenL(ba_zip_path.data(), "rb");
    if (fp == NULL)
        throw QObject::tr("Unable to open %1 in ZIP-archive via GDAL vsizip").arg(file_path);
    do
    {
        const char *str = CPLReadLineL(fp);
        if (str == NULL)
            break;
        file_contents += str;
    }
    while (true);

    VSIFCloseL(fp);
}


bool ZipIo::addFileToZip (const CPLString &file_path, const CPLString &zip_file_name,
                          void* h_zip, GByte **paby_buffer, size_t buffer_size)
{
    int ret = 0;
    size_t bytes_read;
    VSILFILE *fp;

    fp = VSIFOpenL(file_path, "rb");
    if (fp == NULL)
        return false;

    // Write to ZIP metadata when the file starts and how it is called.
    if (CPLCreateFileInZip(h_zip, zip_file_name, NULL) != CE_None)
    {
        VSIFCloseL(fp);
        return false;
    }

    // Write data by blocks. Data is compressed in IO time.
    do
    {
        bytes_read = VSIFReadL(*paby_buffer, 1, buffer_size, fp);
        if (long(bytes_read) < 0)
            ret = -1;
        if (ret == 0 && CPLWriteFileInZip(h_zip, *paby_buffer, bytes_read) != CE_None)
            ret = -1;
    }
    while (ret == 0 && bytes_read == buffer_size);

    CPLCloseFileInZip(h_zip);
    VSIFCloseL(fp);

    return true;
}



