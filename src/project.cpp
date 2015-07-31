/******************************************************************************
 *
 * Name:     project.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Various project manipulation methods implementation (FBProject class).
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, 2015 NextGIS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "fb.h"


FBProject::FBProject (FB *fbPtr)
{
    this->fbPtr = fbPtr;
    strPath = "";
    strNameBase = FB_INIT_PROJECT_NAMEBASE;
    strFirstTimeDataset = "";
}

FBProject::~FBProject ()
{
}


/* --------------------------------------------------------------------------------- */
/*                                  Инициализация                                    */
/* --------------------------------------------------------------------------------- */
bool FBProject::init (char *datasetName)
{
    Json::Value nullVal;
    bool progress = true;

    #ifndef FB_GDAL_2_X
    OGRDataSource *dataset;
    dataset = OGRSFDriverRegistrar::Open(datasetName,FALSE);
    #else
    GDALDataset *dataset;
    dataset = (GDALDataset*) GDALOpenEx(datasetName,
                                    GDAL_OF_VECTOR, NULL, NULL, NULL);
    #endif

    if (dataset == NULL)
    {
        emit sendMsg(tr("Ошибка при попытке открыть источник данных ")
                     + QString(datasetName));
        progress = false;
    }
    else
    {
        // Проверяем кол-во слоёв.
        if (dataset->GetLayerCount() == 0 || dataset->GetLayerCount() > 1)
        {
            emit sendMsg(tr("Ошибка: требуется, чтобы источник данных"
                            " содержал только 1 слой! Выбранный источник"
                            " данных содержит 0 или больше чем 1 слой."));
            progress = false;
        }
        else
        {
            OGRLayer *poLayer = dataset->GetLayer(0);
            if (poLayer == NULL)
            {
                emit sendMsg(tr("Ошибка: невозможно считать слой в выбранном"
                                " источнике данных ") + QString(datasetName));
                progress = false;
            }
            else
            {
                // Проверяем правильность системы координат.
                OGRSpatialReference SpaRef1(SRS_WKT_WGS84);
                OGRSpatialReference SpaRef2;
                SpaRef2.SetFromUserInput("EPSG:3857");
                //OGRSpatialReference SpaRef22;
                //SpaRef22.SetFromUserInput("EPSG:3857");
                //SpaRef22.morphToESRI(); // У Shape-файлов свой формат записи СК.
                OGRSpatialReference *layerSpaRef = poLayer->GetSpatialRef();
                QString srsNumber;
                if (layerSpaRef == NULL) // Допускаем пустую систему координат.
                {
                    srsNumber = "null";
                }
                else if (layerSpaRef->IsSame(&SpaRef1) == TRUE)
                {
                    srsNumber = "4326";
                }
                else if (layerSpaRef->IsSame(&SpaRef2) == TRUE)
                {
                    srsNumber = "3857";
                }
                else
                {
                    emit sendMsg(tr("Ошибка: слой выбранного источника данных"
                                   " имеет неподдерживаемую систему координат"
                                   " (поддерживаются только EPSG:4326 и EPSG:3857)"));
                    progress = false;
                }

                if (progress != false)
                {
                    // Текущая версия приложения.
                    QByteArray baVers;
                    baVers = QString::number(FB_VERSION,'f',1).toUtf8();
                    jsonMeta[FB_JSON_META_VERSION] = baVers.data();

                    // Доступные поля слоя.
                    OGRFeatureDefn *poLayerDefn = poLayer->GetLayerDefn();
                    Json::Value json_mas;
                    for (int i=0; i<poLayerDefn->GetFieldCount(); i++)
                    {
                        OGRFieldDefn *poFieldDefn = poLayerDefn->GetFieldDefn(i);

                        Json::Value json_mas_elem;
                        json_mas_elem[FB_JSON_META_KEYNAME] = poFieldDefn->GetNameRef();
                        OGRFieldType ft = poFieldDefn->GetType();
                        switch (ft)
                        {
                            case OFTInteger:
                                json_mas_elem[FB_JSON_META_DATATYPE] = "INTEGER";
                            break;
                            case OFTInteger64:
                                json_mas_elem[FB_JSON_META_DATATYPE] = "INTEGER";
                            break;
                            case OFTReal:
                                json_mas_elem[FB_JSON_META_DATATYPE] = "REAL";
                            break;
                            case OFTDate:
                                json_mas_elem[FB_JSON_META_DATATYPE] = "DATE";
                            break;

                            // TODO: расширить список

                            default://case OFTString:
                                json_mas_elem[FB_JSON_META_DATATYPE] = "STRING";
                            break;
                            //default:
                            //    json_mas_elem[FB_JSON_DATATYPE] = nullVal;
                            //break;
                        }
                        json_mas_elem[FB_JSON_META_DISPLAY_NAME] = poFieldDefn->GetNameRef();
                        json_mas.append(json_mas_elem);
                    }
                    jsonMeta[FB_JSON_META_FIELDS] = json_mas;

                    // Тип геометрии.
                    OGRwkbGeometryType tt = poLayerDefn->GetGeomType();
                    switch (tt)
                    {
                        case wkbLineString:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = "LINESTRING";
                        break;
                        case wkbPoint:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = "POINT";
                        break;
                        case wkbPolygon:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = "POLYGON";
                        break;
                        case wkbMultiPoint:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = "MULTIPOINT";
                        break;
                        case wkbMultiLineString:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = "MULTILINESTRING";
                        break;
                        case wkbMultiPolygon:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = "MULTIPOLYGON";
                        break;
                        default:
                            jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = nullVal;
                        break;
                    }

                    // Система координат.
                    Json::Value jsonSrs;
                    if (srsNumber == "null")
                    {
                        jsonSrs = nullVal;
                    }
                    else
                    {
                        QByteArray baSrs;
                        baSrs = srsNumber.toUtf8();
                        jsonSrs[FB_JSON_META_ID] = baSrs.data();
                    }
                    jsonMeta[FB_JSON_META_SRS] = jsonSrs;

                    // Заполняем пустую связь с НГВ. Если этот метод вызван при создании проекта
                    // с НГВ то этот параметр получит значение в другом месте.
                    jsonMeta[FB_JSON_META_NGW_CONNECTION] = nullVal;

                    // Запоминаем изначальный путь+имя источника данных.
//                    jsonMeta[FB_JSON_INITIAL_DATASET] = datasetName;

                    strFirstTimeDataset = datasetName;
                }
            }
        }

        #ifndef FB_GDAL_2_X
        OGRDataSource::DestroyDataSource(dataset);
        #else
        GDALClose(dataset);
        #endif
    }

    return progress;
}


/* --------------------------------------------------------------------------------- */
/*                            Инициализация из NextGIS Web                           */
/* --------------------------------------------------------------------------------- */
bool FBProject::initFromNgw (char *datasetName, QString strUrl, QString strLogin,
                             QString strPass, QString strId, Json::Value jsonMeta)
{
    bool progress = true;

    // TODO: проверить это для более старых версий GDAL!
    QString logPas = strLogin + ":" + strPass;
    QByteArray logPasBa = logPas.toUtf8();
    CPLSetConfigOption("GDAL_HTTP_USERPWD", logPasBa.data());

    // Заполняем поля проекта общим способом.
    if (!init(datasetName))
    {
        progress = false;
    }
    else
    {
        // Заменяем метаданные, которые не удалось достать обычным способом.

        // TODO: чтобы не делать лишних действий, можно полностью переписать
        // изначальный init() или просто всегда проверять на количество объектов
        // в слое, и если оно равно 0, то переприсваивать здесь (а иначе эти поля
        // уже корректно заполнены).
        this->jsonMeta[FB_JSON_META_FIELDS] = jsonMeta[FB_JSON_META_FIELDS];
        this->jsonMeta[FB_JSON_META_GEOMETRY_TYPE] = jsonMeta[FB_JSON_META_GEOMETRY_TYPE];
        this->jsonMeta[FB_JSON_META_SRS] = jsonMeta[FB_JSON_META_SRS];

        // Так же заменяем незаполненный параметр с адресом ресурса NGW.
        QByteArray ngwBa;
        Json::Value val;
        ngwBa = strUrl.toUtf8();
        const Json::Value vUrl = ngwBa.data();
        const Json::Value vId = strId.toInt();
        val[FB_JSON_META_URL] = vUrl;
        val[FB_JSON_META_ID] = vId;
        ngwBa = strLogin.toUtf8();
        const Json::Value vLogin = ngwBa.data();
        val[FB_JSON_META_LOGIN] = vLogin;
        ngwBa = strPass.toUtf8();
        const Json::Value vPassword = ngwBa.data();
        val[FB_JSON_META_PASSWORD] = vPassword;
        this->jsonMeta[FB_JSON_META_NGW_CONNECTION] = val;
    }

    // TODO: Убрать заданные для пароля Config Option в GDAL, если это нужно.

    return progress;
}


/* --------------------------------------------------------------------------------- */
/*                                     Открыть                                       */
/* --------------------------------------------------------------------------------- */
// В параметре retForm возвращается форма, которую потом надо вывести на экран.
bool FBProject::open(QString strFullPath, Json::Value &retForm)
{
    bool progress = true;

    QByteArray ba;
    VSILFILE *fp;
    Json::Reader jsonReader;

    Json::Value jsonMeta;
    Json::Value jsonForm;

    // Считываем метаданные.
    QString strMetaPath = strFullPath;
    strMetaPath.prepend("/vsizip/");
    strMetaPath.append("/");
    strMetaPath.append(FB_PROJECT_META_FILENAME);
    ba = strMetaPath.toUtf8();
    fp = VSIFOpenL(ba.data(), "rb");
    if (fp == NULL)
    {
        emit sendMsg(tr("Ошибка при открытии проекта. Не удалось открыть"
                        " JSON-файл метаданных"));
        progress = false;
    }
    else
    {
        std::string jsonConStr = "";
        do
        {
            const char *str = CPLReadLineL(fp);
            if (str == NULL)
                break;
            jsonConStr += str;
        }
        while (true);
        VSIFCloseL(fp);
        if (!jsonReader.parse(jsonConStr, jsonMeta, false)
                || jsonMeta.isNull())
        {
            emit sendMsg(tr("Ошибка при открытии проекта. Не удалось считать"
                            " JSON-файл метаданных"));
            progress = false;
        }
        else
        {
            // Проверяем метаданные.
            // Наличие и правильность: список полей, система координат, строка подключения
            // к НГВ, тип геометрии.
            // Так же проверяем на соответствие версию приложения и файла проекта.

            // TODO: доделать здесь.
        }
    }

    // Считываем форму аналогичным способом.
    if (progress)
    {
        QString strFormPath = strFullPath;
        strFormPath.prepend("/vsizip/");
        strFormPath.append("/");
        strFormPath.append(FB_PROJECT_FORM_FILENAME);
        ba = strFormPath.toUtf8();
        fp = VSIFOpenL(ba.data(), "rb");
        if (fp == NULL)
        {
            emit sendMsg(tr("Ошибка при открытии проекта. Не удалось открыть"
                            " JSON-файл формы"));
            progress = false;
        }
        else
        {
            std::string jsonConStr = "";
            do
            {
                const char *str = CPLReadLineL(fp);
                if (str == NULL)
                    break;
                jsonConStr += str;
            }
            while (true);
            VSIFCloseL(fp);
            if (!jsonReader.parse(jsonConStr, jsonForm, false)
                    || jsonForm.isNull())
            {
                emit sendMsg(tr("Ошибка при открытии проекта. Не удалось считать"
                                " JSON-файл формы"));
                progress = false;
            }
            else
            {
                // Проверяем правильность формы.
                // Проверка нужна именно тут, т.к. в главном классе приложения
                // будет вывод формы на экран без всяких проверок.

                // TODO: доделать здесь.
            }
        }
    }

    // Если всё правильно, то запоняем поля проекта.
    if (progress)
    {
        this->jsonMeta = jsonMeta;
        retForm = jsonForm;
        getPathComponents(strFullPath,strPath,strNameBase);
    }

    return progress;
}


/* --------------------------------------------------------------------------------- */
/*                                  Сохранить как                                    */
/* --------------------------------------------------------------------------------- */
// В этом методе сохраняются заного ВСЕ внутренние файлы проекта.
// Приходится вызывать этот метод так же и для кнопки Сохранить, поскольку пересохранение
// всех файлов потребуется в любом случае, т.к. в зип-архиве нельзя перезаписать один
// файл, не трогая остальные.
bool FBProject::saveAs (QString strFullPath)
{
    bool progress = true;
    // Далее используем сигналы чтобы менять значение прогресс бара, т.к. виджет
    // создан в другом потоке - GUI-thread.
    emit changeProgress(0);

    QString strNameBase;
    QString strPath;
    getPathComponents(strFullPath,strPath,strNameBase);

    QString strTempPath;
    QTemporaryDir dir(strPath+"formXXXXXX"); // Только для Qt >= 5
    if (!dir.isValid())
    {
        emit sendMsg(tr("Ошибка при сохранении проекта. Невозможно создать "
                      "временную директорию. Попробуйте выбрать другой путь "
                      "для сохранения."));
        progress = false;
    }
    else
    {
        // Создаём внутренние файлы во временной директории.
        strTempPath = dir.path() + "/";

        // Сохраняем файл формы.
        QString strFormPath;
        if (progress)
        {
            emit changeProgress(10);
            QFile fileForm (strTempPath + FB_PROJECT_FORM_FILENAME);
            if (!fileForm.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось создать "
                              "временный JSON-файл формы."));
                progress = false;
            }
            else
            {
                // Считываем форму в json.
                Json::Value jsonRootArray = fbPtr->formToJson();
                if (jsonRootArray.isNull())
                {
                    emit sendMsg(tr("Внимание! Проект сохранён не будет, т.к. форма пуста"
                                    " (не содержит элементов)"));
                    progress = false;
                }
                else
                {
                    // Записываем считанную форму в файл.
                    Json::StyledWriter writer;
                    std::string finalStdString = writer.write(jsonRootArray);
                    QString finalString = QString::fromUtf8(finalStdString.data());
                    strFormPath = fileForm.fileName();
                    QTextStream out(&fileForm);
                    out.setCodec("UTF-8");
                    out<<finalString;
                }
            }
        }

        // Сохраняем файл данных.
        QString strDataPath = "";
        if (progress)
        {
            emit changeProgress(30);

            #ifndef FB_GDAL_2_X
            OGRDataSource *datasetOld;
            #else
            GDALDataset *datasetOld;
            #endif

            // Если этот проект - только что созданный проект (ни разу не
            // сохранявшийся), то берём данные из источника данных через
            // GDAL.
            if (strFirstTimeDataset != "")
            {
                QByteArray baDataPathOld = strFirstTimeDataset.toUtf8();
                #ifndef FB_GDAL_2_X
                datasetOld = OGRSFDriverRegistrar::Open(baDataPathOld.data(),FALSE);
                #else
                datasetOld = (GDALDataset*) GDALOpenEx(baDataPathOld.data(),
                                                    GDAL_OF_VECTOR, NULL, NULL, NULL);
                #endif
            }

            // Иначе просто копируем старый GeoJSON источник данных из старого файла-архива
            // в новый, т.к. согласно концепции - связь с исходным слоем, который
            // пользователь выбрал в самом начале, уже утеряна.
            else
            {
                // Предыдущий путь и имя файла как раз сохранены в проекте.
                QString strZipDataPath = getFullPath();
                strZipDataPath.prepend("/vsizip/");
                strZipDataPath.append("/");
                strZipDataPath.append(FB_PROJECT_DATA_FILENAME);
                QByteArray baZipDataPath;
                baZipDataPath = strZipDataPath.toUtf8();
                char **allowedDrivers = NULL;
                allowedDrivers = CSLAddString(allowedDrivers,"GeoJSON");
                #ifndef FB_GDAL_2_X
                datasetOld = OGRSFDriverRegistrar::Open(baZipDataPath.data(),FALSE);
                #else
                datasetOld = (GDALDataset*) GDALOpenEx(baZipDataPath.data(),
                        GDAL_OF_VECTOR | GDAL_OF_READONLY, allowedDrivers, NULL, NULL);
                #endif
                CSLDestroy(allowedDrivers);
            }

            if(datasetOld == NULL)
            {
                emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось открыть"
                                " исходный источник данных слоя"));
                progress = false;
            }
            else
            {
                #ifndef FB_GDAL_2_X
                OGRSFDriver *driverNew = OGRSFDriverRegistrar::GetRegistrar()
                     ->GetDriverByName("GeoJSON");
                #else
                GDALDriver *driverNew = GetGDALDriverManager()
                     ->GetDriverByName("GeoJSON");
                #endif
                if(driverNew == NULL)
                {
                    emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось инициализировать"
                                    " GeoJSON драйвер GDAL для сохранения данных слоя"));
                    progress = false;
                }
                else
                {
                    strDataPath = strTempPath + FB_PROJECT_DATA_FILENAME;
                    QByteArray baDataPathNew = strDataPath.toUtf8();

                    #ifndef FB_GDAL_2_X
                    OGRDataSource *datasetNew = driverNew
                        ->CreateDataSource(baDataPathNew.data());
                    #else
                    GDALDataset *datasetNew = driverNew
                        ->Create(baDataPathNew.data(), 0, 0, 0, GDT_Unknown, NULL);
                    #endif
                    if (datasetNew == NULL)
                    {
                        emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось создать"
                                    " GeoJSON источник данных для сохранения данных слоя"));
                        progress = false;
                    }
                    else
                    {
                        // TODO: подумать с изменением кодировки, если она неправильно
                        // считана.

                        OGRLayer *layerOld = datasetOld->GetLayer(0);
                        OGRLayer *layerNew = datasetNew->CopyLayer(layerOld,layerOld->GetName());
                        if(layerNew == NULL)
                        {
                            emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось скопировать"
                                        " данные слоя в GeoJSON файл"));
                            progress = false;
                        }

                        #ifndef FB_GDAL_2_X
                        OGRDataSource::DestroyDataSource(datasetNew);
                        #else
                        GDALClose(datasetNew);
                        #endif
                    }
                }

                #ifndef FB_GDAL_2_X
                OGRDataSource::DestroyDataSource(datasetOld);
                #else
                GDALClose(datasetOld);
                #endif
            }
        }

        // Сохраняем файл метаданных.
        QString strMetaPath;
        if (progress)
        {
            emit changeProgress(50);
            QFile fileMeta(strTempPath + FB_PROJECT_META_FILENAME);
            if (!fileMeta.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось создать "
                              "временный JSON-файл метаданных."));
                progress = false;
            }
            else
            {
                strMetaPath = fileMeta.fileName();
                QTextStream out(&fileMeta);
                out.setCodec("UTF-8");
                Json::StyledWriter writer;
                std::string finalStdString = writer.write(jsonMeta);
                QString finalString = QString::fromUtf8(finalStdString.data());
                out<<finalString;
            }
        }

        // Сохраняем файлы изображений - кладём их пока во временную директорию.
        QStringList strsImgs;
        if (progress)
        {
            emit changeProgress(60);
            // Сначала добавляем новые изображения.
            if (!strsNewImgsToSave.isEmpty())
            {
                //...
            }

            // Далее копируем все уже существующие изображения из старого файла-архива,
            // кроме тех, что были удалены.
            //...

            // В итоге сформирован список путей к изображениям во временной директории,
            // оторые будут записаны в файл-архив.
        }

        // Записываем созданные во временной директории файлы в итоговый файл формы.
        // Этот файл создаём в той же самой временной директории, а затем копируем его
        // с заменой. Предупреждение о перезаписи уже было выдано пользователю.
        QString strTempFullPath = strTempPath + strNameBase + "." + FB_PROJECT_EXTENSION;
        if (progress)
        {
            emit changeProgress(80);
            QByteArray baTemp;
            QByteArray baTempFullPath;
            baTempFullPath = strTempFullPath.toUtf8();
            void *hZip = CPLCreateZip(baTempFullPath.data(),NULL);
            if (hZip)
            {
                size_t nBufferSize = 1024 * 1024;
                GByte *pabyBuffer = (GByte *)CPLMalloc(nBufferSize);

                // TODO: сделать следующее:
                // Сохраняем все файлы временной директории в цикле - по массиву пар:
                // их путь и новое имя в архиве.

                baTemp = strFormPath.toUtf8();
                progress = addFileToZip(baTemp.data(), FB_PROJECT_FORM_FILENAME, hZip,
                                     &pabyBuffer, nBufferSize);
                if (!progress)
                {
                    emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось записать "
                                    "временный JSON-файл формы в файл проекта."));
                    progress = false;
                }

                else
                {
                    baTemp = strDataPath.toUtf8();
                    progress = addFileToZip(baTemp.data(), FB_PROJECT_DATA_FILENAME, hZip,
                                         &pabyBuffer, nBufferSize);
                    if (!progress)
                    {
                        emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось записать "
                                        "временный GeoJSON-файл данных в файл проекта."));
                        progress = false;
                    }

                    else
                    {
                        baTemp = strMetaPath.toUtf8();
                        progress = addFileToZip(baTemp.data(), FB_PROJECT_META_FILENAME, hZip,
                                             &pabyBuffer, nBufferSize);
                        if (!progress)
                        {
                            emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось записать "
                                            "временный JSON-файл метаданных в файл проекта."));
                            progress = false;
                        }

                        else
                        {
                            // ... изображения ...
                        }
                    }
                }

                CPLCloseZip(hZip);
                CPLFree(pabyBuffer);
            }

            else
            {
                emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось "
                                "создать итоговый файл проекта (ZIP-архив)."));
                progress = false;
            }
        }

        // Если сохранение было успешно - то копируем с заменой полученный файл
        // из временной директории в ту, что указал пользователь. Копируем,
        // даже не смотря на то, что GDAL сам может создать зип поверх существующего,
        // на всякий случай - чтобы не затереть уже существующий файл если в процессе
        // сохрания зипа что-то пошло не так.
        if (progress)
        {
            if (QFile::exists(strFullPath)
                 && !QFile::remove(strFullPath))
            {
                emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось "
                                "сохранить итоговый файл проекта (ZIP-архив)."));
                progress = false;
            }
            if (!QFile::copy(strTempFullPath, strFullPath))
            {
                emit sendMsg(tr("Ошибка при сохранении проекта. Не удалось "
                                "сохранить итоговый файл проекта (ZIP-архив)."));
                progress = false;
            }

            // И изменяем параметры этого проекта.
            emit changeProgress(99);
            this->strPath = strPath;
            this->strNameBase = strNameBase;
            this->strFirstTimeDataset = ""; // сохранять данные больше не надо
            this->strsNewImgsToSave.clear(); // изображения синхронизировать тоже
            this->strsNewImgsToDelete.clear();
        }
    }

    return progress;
}


bool FBProject::addFileToZip (const CPLString &szFilePathName, const CPLString &szFileName,
                  void* hZIP, GByte **pabyBuffer, size_t nBufferSize)
{
    // TODO: проверить работает ли следующий код для GDAL < 2.0.

    int nRet = 0;
    size_t nBytesRead;
    VSILFILE *fp;

    fp = VSIFOpenL(szFilePathName, "rb");
    if (fp == NULL)
    {
        return false;
    }

    // Записывает в метаданные зипа разметку где начинается и как называется новый файл.
    if (CPLCreateFileInZip(hZIP, szFileName, NULL) != CE_None)
    {
        VSIFCloseL(fp);
        return false;
    }

    // Записывает поблоково сами данные, которые при IO сжимаются.
    do
    {
        nBytesRead = VSIFReadL(*pabyBuffer, 1, nBufferSize, fp);
        if (long(nBytesRead) < 0)
            nRet = -1;
        if (nRet == 0 && CPLWriteFileInZip(hZIP, *pabyBuffer, nBytesRead) != CE_None)
            nRet = -1;
    }
    while (nRet == 0 && nBytesRead == nBufferSize);

    CPLCloseFileInZip(hZIP);
    VSIFCloseL(fp);

    return true;
}


QString FBProject::getFullPath ()
{
    return strPath + strNameBase + "." + FB_PROJECT_EXTENSION;
}


// Кладёт в переданные параметры компоненты полного имени.
// Поное имя должно иметь правильное расширение!
void FBProject::getPathComponents (QString strFullPath, QString &strPath, QString &strNameBase)
{
    strNameBase = "";
    QString str = strFullPath;
    str.chop(QString(FB_PROJECT_EXTENSION).size()+1); // убираем расширение с точкой
    // TODO: удостовериться, что на всех ОС в Qt символ '/' - разделитель.
    while (str[str.size()-1] != '/')
    {
        strNameBase.prepend(str[str.length()-1]);
        str.chop(1);
    }
    strPath = str;
}



