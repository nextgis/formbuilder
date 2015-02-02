/******************************************************************************
 *
 * Name:     formbuilderproject.h
 * Project:  NextGIS Form Builder
 * Purpose:  New, open and save methods
 * Author:   NextGIS
 *
 ******************************************************************************
 * Copyright (c) 2014, NextGIS
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

#include "formbuilder.h"
#include "ui_formbuilder.h"


//-----------------------------------------------------------------------//
//                       НОВЫЙ ПРОЕКТ ИЗ ШЕЙПФАЙЛА                       //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionNewDataset ()
{
    if (CUR_PRJ != NULL)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("При создании нового проекта, все "
                                         "несохранённые изменения в текущем проекте будут "
                                         "потеряны. Продолжить?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret != QMessageBox::Ok)
            return;
    }

    QFileDialog dialogNewDataset (this);
    dialogNewDataset.setAcceptMode(QFileDialog::AcceptOpen);
    // Выбрать можно только один существующий файл:
    dialogNewDataset.setFileMode(QFileDialog::ExistingFile);
    dialogNewDataset.setViewMode(QFileDialog::List);
    dialogNewDataset.setDefaultSuffix("shp");
    dialogNewDataset.setNameFilter("*.shp");
    dialogNewDataset.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogNewDataset.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogNewDataset.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogNewDataset.setLabelText(QFileDialog::Accept,QString::fromUtf8("Выбрать"));
    dialogNewDataset.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogNewDataset.setWindowTitle(QString::fromUtf8("Создание нового проекта:"
                       " выберите Shapefile ..."));
    dialogNewDataset.setDirectory(QDir()); //ставим текущую директорию

    if (dialogNewDataset.exec())
    {
        QStringList dsPaths = dialogNewDataset.selectedFiles();
        // TODO: сделать выдачу сообщения, что нужно выбрать только 1 файл.
        QByteArray dsPathBa = dsPaths[0].toUtf8(); // всегда будет выбран только один файл

        // Создаём новый проект.
        FBProject *NEW_PRJ = new FBProject();

        // Не удаляем старый источник данных и не стираем его доступные поля,
        // пока не убедимся, что всё новое загрузилось верно.
        // Пытаемся его инициализировать.
        if (!_initNewProject(dsPathBa.data(),NEW_PRJ))
        {
            // Если была какая-то ошибка - отменяем создание.
            delete NEW_PRJ;
            return;
        }

        // Иначе удаляем текущий, и новый становится текущим.
        if (CUR_PRJ != NULL) delete CUR_PRJ;
        CUR_PRJ = NEW_PRJ;

        // Очищаем экран телефона и весь интерфейс. Создайм новую форму.
        ClearAll();

        // Выводим информацию об открытом проекте.
        QString output_path = QString::fromUtf8(dsPathBa.data());
        if (output_path.size() > 60)
        {
            output_path.remove(0,output_path.size()-60);
            output_path.prepend("...");
        }
        ui->label_2->setText(QString::fromUtf8("Доступ к Shapefile-у установлен "
         "успешно. Сохраните проект, чтобы импортировать данные "
         "из ") + output_path
         + QString::fromUtf8(" и зафиксировать изменения на форме."));

        // Разблокируем весь интерфейс, если это создание проекта при отсутствии
        // текущего проекта.
        EnableInterface();
    }
}


//-----------------------------------------------------------------------//
//                 НОВЫЙ ПРОЕКТ ИЗ ПОДКЛЮЧЕНИЯ К NEXTGISWEB              //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionNewNGW ()
{
    if (CUR_PRJ != NULL)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("При создании нового проекта, все "
                                         "несохранённые изменения в текущем проекте будут "
                                         "потеряны. Продолжить?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret != QMessageBox::Ok)
            return;
    }

    QDialog dialogNgw(this);
    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialogNgw);
    dialogNgw.setWindowTitle(QString::fromUtf8("Установите соединение с NextGIS Web"));

    QLabel* label;
    QHBoxLayout *hLayout;

    hLayout = new QHBoxLayout();
    label = new QLabel(&dialogNgw);
    label->setText(QString::fromUtf8("URL: "));
    hLayout->addWidget(label);
    QLineEdit* lineEdit1 = new QLineEdit(&dialogNgw);
// TEST -----------------------------------------------------------------------
    lineEdit1->setText(QString::fromUtf8("http://176.9.38.120/wwf"));
// ----------------------------------------------------------------------------
    hLayout->addWidget(lineEdit1);
    dialogLayout->addLayout(hLayout);

     hLayout = new QHBoxLayout();
     label = new QLabel(&dialogNgw);
     label->setText(QString::fromUtf8("Логин: "));
     hLayout->addWidget(label);
     QLineEdit* lineEdit2 = new QLineEdit(&dialogNgw);
// TEST -----------------------------------------------------------------------
     lineEdit2->setText(QString::fromUtf8("administrator"));
// ----------------------------------------------------------------------------
     hLayout->addWidget(lineEdit2);
     dialogLayout->addLayout(hLayout);

      hLayout = new QHBoxLayout();
      label = new QLabel(&dialogNgw);
      label->setText(QString::fromUtf8("Пароль: "));
      hLayout->addWidget(label);
      QLineEdit* lineEdit3 = new QLineEdit(&dialogNgw);
// TEST -----------------------------------------------------------------------
      lineEdit3->setText(QString::fromUtf8("***************"));
// ----------------------------------------------------------------------------
      lineEdit3->setEchoMode(QLineEdit::Password);
      hLayout->addWidget(lineEdit3);
      dialogLayout->addLayout(hLayout);

    QHBoxLayout *cancelLayout = new QHBoxLayout();
    QLabel *statusLabel = new QLabel(&dialogNgw);
    statusLabel->setText("                                        ");
    QProgressBar *progBar = new QProgressBar(&dialogNgw);
    progBar->setValue(0);
    progBar->setMaximum(100);
    progBar->setTextVisible(false);
    QPushButton *button3 = new QPushButton(&dialogNgw);
    button3->setText(QString::fromUtf8("Отменить"));
    button3->setEnabled(false);
    cancelLayout->addWidget(statusLabel);
    cancelLayout->addWidget(progBar);
    cancelLayout->addWidget(button3);

    QTreeWidget *treeWidget = new QTreeWidget(&dialogNgw);
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderLabel(QString::fromUtf8("Доступные ресурсы:"));
    QPushButton *button2 = new QPushButton(&dialogNgw);
    button2->setText(QString::fromUtf8("Выбрать"));
    connect(button2, SIGNAL(clicked()), &dialogNgw, SLOT(accept()));
    button2->setEnabled(false);
    FBConnectButton *button1 = new FBConnectButton(&dialogNgw,lineEdit1,lineEdit2,
                                                   lineEdit3,treeWidget,button3,
                                                   progBar,statusLabel,button2);
    button1->setText(QString::fromUtf8("Соединить"));
    connect(button1, SIGNAL(clicked()), button1, SLOT(OnClicked()));

    dialogLayout->addWidget(button1);
    dialogLayout->addLayout(cancelLayout);
    dialogLayout->addWidget(treeWidget);
    dialogLayout->addWidget(button2);

    if (dialogNgw.exec())
    {
        // Диалог завершился (по нажатию кнопки "Выбрать"), но не удалился,
        // поэтому мы можем считать данные из его полей.
        QString logPas = lineEdit2->text() + ":" + lineEdit3->text();
        QByteArray logPasBa = logPas.toUtf8();
        CPLSetConfigOption("GDAL_HTTP_USERPWD", logPasBa.data());

        // Убираем '/' с конца строки, если пользователь его зачем-то поставил.
        QString lineEdit1Text = lineEdit1->text();
        //while(lineEdit1Text.endsWith("/"))
        //{
        //    lineEdit1Text.chop(1);
        //}

        QString finalUrl = lineEdit1Text + "/resource/"
                + treeWidget->currentItem()->data(0,Qt::UserRole).toString()
                + "/geojson/";
        QByteArray finalUrlBa = finalUrl.toUtf8();

        // Создаём новый проект.
        FBProject *NEW_PRJ = new FBProject();

        // Не удаляем старый источник данных и не стираем его доступные поля,
        // пока не убедимся, что всё новое загрузилось верно.
        // Пытаемся его инициализировать.
        if (!_initNewProject(finalUrlBa.data(),NEW_PRJ))
        {
            // Если была какая-то ошибка - отменяем создание.
            delete NEW_PRJ;
            return;
        }

        // Из-за особенностей формата GeoJSON, а именно из-за того, что если в нём
        // не определены данные - в возвращаемом с сервера NGW JSON-е может не
        // содержаться ни список полей, ни другая информация об ИД. Поэтому
        // необходимо попробовать достать всё это другим способом.
        if (NEW_PRJ->datasetToConvert->GetLayer(0)->GetLayerDefn()->GetFieldCount() == 0)
        {
            QString ngw_api_url;
            ngw_api_url = lineEdit1Text + "/api/resource/"
             + treeWidget->currentItem()->data(0,Qt::UserRole).toString();

            // Из-за асинхронной работы классов по работе с http необходимо
            // считывать json, только после того, как он полностью и без
            // ошибок передался. Это можно сделать при помощи потоков, а можно
            // и дополнительным диалогом со строкой-прогрессом, чтобы ещё
            // и показывать ход "загрузки".
            // Диалог не завершит работу, пока не будет полностью получен http
            // ответ или пользователь его сам не закроет.
            std::string receivedJson = "";
            FBWaitNgwApiDialog dlg(&button1->httpManager,ngw_api_url,&receivedJson);
            dlg.exec();

            // Парсим полученный json.
            if (!receivedJson.empty())
            {
                Json::Reader json_reader;
                Json::Value json_root;
                if (json_reader.parse(receivedJson, json_root, false))
                {
                    Json::Value json_fields;
                    Json::Value json_new_fields;
                    json_fields = json_root["feature_layer"]["fields"];
                    for (int i=0; i<json_fields.size(); ++i)
                    {
                        std::string str_keyname
                                = json_fields[i]["keyname"].asString();
                        std::string str_datatype
                                = json_fields[i]["datatype"].asString();
                        std::string str_display_name
                                = json_fields[i]["display_name"].asString();

                        Json::Value json_new_field;
                        json_new_field["keyname"] = str_keyname;
                        json_new_field["datatype"] = str_datatype;
                        json_new_field["display_name"] = str_display_name;
                        json_new_fields.append(json_new_field);
                    }
                    NEW_PRJ->metaData[FBJSONKEY_meta_fields] = json_new_fields;

                    NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type]
                            = json_root["vector_layer"]["geometry_type"];

                    NEW_PRJ->metaData[FBJSONKEY_meta_srs]
                            = json_root["vector_layer"]["srs"];
                }
                else
                {
                    delete NEW_PRJ;
                    ShowMsgBox(QString::fromUtf8("Не удалось считать описание "
                     "полей из источника данных ") + ngw_api_url);
                    return;
                }
            }
            else
            {
                delete NEW_PRJ;
                ShowMsgBox(QString::fromUtf8("Не удалось считать описание "
                 "полей из источника данных ") + ngw_api_url);
                return;
            }
        }

        // Иначе удаляем текущий, и новый становится текущим.
        if (CUR_PRJ != NULL) delete CUR_PRJ;
        CUR_PRJ = NEW_PRJ;

        // Формируем json-строку подключения к NGW.
        Json::Value val;
        const Json::Value vUrl = finalUrlBa.data();
        val["url"] = vUrl;
        QByteArray logBa = lineEdit2->text().toUtf8();
        const Json::Value vLogin = logBa.data();
        val["login"] = vLogin;
        QByteArray pasBa = lineEdit3->text().toUtf8();
        const Json::Value vPassword = pasBa.data();
        val["password"] = vPassword;
        CUR_PRJ->metaData[FBJSONKEY_meta_ngw_connection] = val;

        // Очищаем экран телефона и весь интерфейс. Создайм новую форму.
        ClearAll();

        // Выводим информацию об открытом проекте.
        QString output_path = QString::fromUtf8(finalUrlBa.data());
        if (output_path.size() > 60)
        {
            output_path.remove(0,output_path.size()-60);
            output_path.prepend("...");
        }
        ui->label_2->setText(QString::fromUtf8("Соединение с сервером NextGIS Web "
         "установлено успешно. Сохраните проект, чтобы импортировать данные "
         "из ") +output_path+ QString::fromUtf8(" и зафиксировать изменения на форме."));

        // Разблокируем весь интерфейс, если это создание проекта при отсутствии
        // текущего проекта.
        EnableInterface();

        // TODO: Убрать заданные Config Option для GDAL.
    }
}


void FormBuilder::EnableInterface()
{
    for (int i=0; i<vElemTypes.size(); i++)
    {
        vElemTypes[i]->setEnabled(true);
    }
    ui->groupBox_4->setEnabled(true);
    ui->groupBox_5->setEnabled(true);
    actionSave->setEnabled(true);
    tabWidget->setEnabled(true);
    tabHorWidget->setEnabled(true);
    menuScreen->setEnabled(true);
}


// Инициализация источника данных GDAL.
bool FormBuilder::_initNewProject (char *datasetName, FBProject *NEW_PRJ)
{
    //NEW_PRJ->poDS = OGRSFDriverRegistrar::Open(datasetName);
    NEW_PRJ->datasetToConvert = (GDALDataset*) GDALOpenEx(datasetName,
                                    GDAL_OF_VECTOR, NULL, NULL, NULL);
    //if(NEW_PRJ->poDS == NULL)
    if(NEW_PRJ->datasetToConvert == NULL)
    {
        ShowMsgBox(QString::fromUtf8("Ошибка при попытке открыть "
                       "источник данных ") + QString::fromUtf8(datasetName));
        return false;
    }

    // Проверяем кол-во слоёв.
    //if (NEW_PRJ->poDS->GetLayerCount() == 0 || NEW_PRJ->poDS->GetLayerCount() > 1)
    if (NEW_PRJ->datasetToConvert->GetLayerCount() == 0
            || NEW_PRJ->datasetToConvert->GetLayerCount() > 1)
    {
        ShowMsgBox(QString::fromUtf8("Ошибка: требуется, чтобы источник данных "
                                     "содержал только 1 слой! Выбранный источник "
                                     "данных содержит 0 или больше чем 1 слой."));
        return false;
    }

    //OGRLayer *poLayer =  NEW_PRJ->poDS->GetLayer(0);
    OGRLayer *poLayer =  NEW_PRJ->datasetToConvert->GetLayer(0);
    if (poLayer == NULL)
    {
        ShowMsgBox(QString::fromUtf8("Ошибка: невозможно считать слой в выбранном "
                                     "источнике данных "));
        return false;
    }

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
        ShowMsgBox(QString::fromUtf8("Ошибка: слой выбранного источника данных"
                                     " имеет неподдерживаемую систему координат "
                                     "(поддерживаются только EPSG:4326 и EPSG:3857)"));
        return false;
    }

    // Считываем метаданные слоя, чтобы при сохранении проекта не считывать второй раз.
    Json::Value nullVal;

    // 0. Берём текущую версию приложения (и формата тоже).
    NEW_PRJ->metaData[FBJSONKEY_meta_version] = FB_current_version;

    // 1. Считываем все доступные поля слоя.
    OGRFeatureDefn *poLayerDefn = poLayer->GetLayerDefn();
    Json::Value json_mas;
    for (int i=0; i<poLayerDefn->GetFieldCount(); i++)
    {
        OGRFieldDefn *poFieldDefn = poLayerDefn->GetFieldDefn(i);

        Json::Value json_mas_elem;
        json_mas_elem["keyname"] = poFieldDefn->GetNameRef();
        OGRFieldType ft = poFieldDefn->GetType();
        switch (ft)
        {
            case OFTInteger:
                json_mas_elem["datatype"] = "INTEGER";
            break;
            case OFTReal:
                json_mas_elem["datatype"] = "REAL";
            break;
            case OFTDate:
                json_mas_elem["datatype"] = "DATE";
            break;
            // TODO: расширить список.
            default://case OFTString:
                json_mas_elem["data_type"] = "STRING";
            break;
            //default:
            //    json_mas_elem["data_type"] = nullVal;
            //break;
        }
        json_mas_elem["display_name"] = poFieldDefn->GetNameRef();
        json_mas.append(json_mas_elem);
    }
    NEW_PRJ->metaData[FBJSONKEY_meta_fields] = json_mas;

    // 2. Считываем тип геометрии.
    OGRwkbGeometryType tt = poLayerDefn->GetGeomType();
    switch (tt)
    {
        case wkbLineString:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = "LINESTRING";
        break;
        case wkbPoint:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = "POINT";
        break;
        case wkbPolygon:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = "POLYGON";
        break;
        case wkbMultiPoint:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = "MULTIPOINT";
        break;
        case wkbMultiLineString:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = "MULTILINESTRING";
        break;
        case wkbMultiPolygon:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = "MULTIPOLYGON";
        break;
        default:
            NEW_PRJ->metaData[FBJSONKEY_meta_geometry_type] = nullVal;
        break;
    }

    // 3. Считываем систему координат.
    Json::Value jsonSrs;
    if (srsNumber == "null")
    {
        jsonSrs = nullVal;
    }
    else
    {
        QByteArray baSrs;
        baSrs = srsNumber.toUtf8();
        jsonSrs["id"] = baSrs.data();
    }
    NEW_PRJ->metaData[FBJSONKEY_meta_srs] = jsonSrs;

    // 4. Заполняем пустую связь с НГВ. Если этот метод вызван при создании проекта
    // с НГВ то этот параметр получит значение в другом месте.
    NEW_PRJ->metaData[FBJSONKEY_meta_ngw_connection] = nullVal;

    // 5. Создаём счётчик для внутренних имён изображений.
    NEW_PRJ->metaData[FBJSONKEY_meta_image_counter] = 0;

    return true;
}


//-----------------------------------------------------------------------//
//                          ОТКРЫТЬ ПРОЕКТ                               //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionOpen ()
{
    if (CUR_PRJ != NULL)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("При открытии нового проекта, все "
                                         "несохранённые изменения в текущем проекте будут "
                                         "потеряны. Продолжить?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret != QMessageBox::Ok)
            return;
    }

    // Настраиваем диалог открытия проекта.
    QFileDialog dialogOpen(this);
    dialogOpen.setAcceptMode(QFileDialog::AcceptOpen);
    dialogOpen.setFileMode(QFileDialog::ExistingFile);
    dialogOpen.setViewMode(QFileDialog::List);
    dialogOpen.setDefaultSuffix("ngfp");
    dialogOpen.setNameFilter("*.ngfp");
    dialogOpen.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogOpen.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogOpen.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogOpen.setLabelText(QFileDialog::Accept,QString::fromUtf8("Открыть"));
    dialogOpen.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogOpen.setWindowTitle(QString::fromUtf8("Открыть проект. Выберите файл "
                    "с расширением .ngfp ..."));

// ======================= Начало работы диалога =============================

    QString pathName;
    if (CUR_PRJ != NULL)
        dialogOpen.setDirectory(CUR_PRJ->path);

    if (dialogOpen.exec())
    {
        pathName = dialogOpen.selectedFiles().first();
        if (!pathName.endsWith(".ngfp",Qt::CaseInsensitive))
        {
            ShowMsgBox(QString::fromUtf8("Ошибка! Выбран неверный "
                         "файл проекта. Требуется файл с расширением .ngfp"));
            return;
        }

        QString filePath;
        QByteArray pathBa;

// ======================= Считываем JSON-форму =============================

        CPLSetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS","ngfp");

        //CPLSetConfigOption("CPL_ZIP_ENCODING", "cp866");

        filePath = pathName;
        filePath.prepend("/vsizip/");
        filePath.append("/form.json");
        pathBa = filePath.toUtf8();

        VSILFILE *fp;
        fp = VSIFOpenL(pathBa.data(), "rb");
        if (fp == NULL)
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие "
                     "проекта! Не удалось прочитать JSON-файл формы"));
            return;
        }

        std::string jsonStr = "";
        do
        {
            const char *str = CPLReadLineL(fp);
            if (str == NULL)
                break;
            jsonStr += str;
        }
        while (true);

        VSIFCloseL(fp);

        Json::Reader jsonReader;
        Json::Value jsonValue;

        Json::Value jsonFormRoot;

        if (!jsonReader.parse(jsonStr, jsonFormRoot, false)
                || jsonFormRoot.isNull())
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не удалось прочесть JSON-файл формы"));
            return;
        }

// =============== Считываем метаданные =========================================

        // Создаём новый проект, пока что не трогая старый.
        FBProject *OPEN_PRJ = new FBProject();

        // Считываем всю json-структуру метаданных.
        filePath = pathName;
        filePath.prepend("/vsizip/");
        filePath.append("/meta.json");
        pathBa = filePath.toUtf8();
        fp = VSIFOpenL(pathBa.data(), "rb");
        if (fp == NULL)
        {
            delete OPEN_PRJ;
            ShowMsgBox(QString::fromUtf8("Ошибка при открытии проекта: "
             "не удалось открыть JSON-файл метаданных"));
            return;
        }
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

        Json::Value jsonMetaRoot;

        if (!jsonReader.parse(jsonConStr, jsonMetaRoot, false)
                || jsonMetaRoot.isNull())
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не удалось прочесть JSON-файл метаданных"));
            delete OPEN_PRJ;
            return;
        }

        // Проверяем версию проекта.
        Json::Value versVal = jsonMetaRoot[FBJSONKEY_meta_version];
        if (QString::fromUtf8(versVal.asString().data())
                != QString::fromUtf8(FB_current_version))
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не совпадают версии приложения и открываемого проекта"));
            delete OPEN_PRJ;
            return;
        }

        // TODO: по-хорошему надо проверять на правильность всю JSON-структуру
        // метаданных, потому что от этого зависит корректная работа с проектом.
        // Но пока подразумевается, что внутрь файла проекта никто не полезет.

// ====================== Меняем старый проект на новый ==============================

        pathBa = pathName.toUtf8();
        OPEN_PRJ->path = QString::fromUtf8(CPLGetPath(pathBa.data())) + "/";
        OPEN_PRJ->nameBase = QString::fromUtf8(CPLGetFilename(pathBa.data()));
        OPEN_PRJ->nameBase.chop(5);
        OPEN_PRJ->metaData = jsonMetaRoot;
        // Не забываем: dataset при любом открытии может быть только NULL.
        if (CUR_PRJ != NULL) delete CUR_PRJ;
        CUR_PRJ = OPEN_PRJ;

// ====================== Отображаем новую форму ==================================

        // Очищаем всё старое.
        // Удаляем даже самую первую вкладку в обоих табах, т.к.
        // она будет загружена из файла.
        ClearAll();
        QWidget *tabToDel = tabWidget->widget(0);
        tabWidget->clear();
        delete tabToDel;
        tabToDel = tabHorWidget->widget(0);
        tabHorWidget->clear();
        delete tabToDel;
        // Не забываем полностью очистить массив соответствий:
        correspondence.clear();

        // Просматриваем вкладки в JSON-файле.
        Json::Value jsonTabs;
        jsonTabs = jsonFormRoot[FBJSONKEY_tabs];
        if (!jsonTabs.isNull())
        {
            for (int i=0; i<jsonTabs.size(); ++i)
            {
                // Создаём вкладку: горизонтальную и вертикальную.
                CreatePage();
                // По сути тут не важна какая ориентация активная, но задать надо.
                isCurrentVertical = true;
                // Считываем имя вкладки.
                Json::Value jsonTab = jsonTabs[i];
                jsonValue = jsonTab[FBJSONKEY_name];
                // Делаем текущую вкладку для обоих табов активной. Это важно,
                // т.к. именно на неё сейчас будут добавляться элементы.
                tabWidget->setCurrentIndex(tabWidget->count()-1);
                // Переименовываем созданную вкладку.
                ChangeTabName(QString::fromUtf8(jsonValue.asString().data()));

                // Просматриваем ориентации для текущей вкладки в JSON-файле.
                Json::Value jsonOrtns [2];
                jsonOrtns[0] = jsonTab[FBJSONKEY_portrait_elements];
                jsonOrtns[1] = jsonTab[FBJSONKEY_album_elements];
                for (int j=0; j<2; j++) // этот цикл по сути вместо функции
                {
                    if (jsonOrtns[j].isNull())
                        continue;

                    QWidget *curWidget;

                    if (j == 0)
                    {
                        isCurrentVertical = true;
                        curWidget = tabWidget->currentWidget();
                    }
                    else
                    {
                        isCurrentVertical = false;
                        curWidget = tabHorWidget->currentWidget();
                    }

                    // Получаем родительский лейбл для текущего
                    // виджета - понадобится в дальнейшем.
                    // curParentLabel не должен быть нулём, т.к. родительский лейбл
                    // был только что добавлен.
                    FBParentLabel *parentLabel = correspondence[curWidget].second;

                    // Просматриваем элементы текущей вкладки для текущей ориентации
                    // в JSON-файле.
                    Json::Value jsonElems = jsonOrtns[j];
                    for (int k=jsonElems.size()-1; k>=0; --k) // просматриваем с конца, т.к. добавление будет происходить с верху
                    {
                        Json::Value jsonElem = jsonElems[k];

                        Json::Value jsonType = jsonElem[FBJSONKEY_type];
                        if (jsonType.isNull())
                            continue;

                        // Задаём текщий тип элемента, как будто выбираем из
                        // правой колонки.
                        FBElemType::CURRENT = NULL;
                        if (pElemTypeGroupStart->name
                                == QString::fromUtf8(jsonType.asString().data()))
                        {
                            FBElemType::CURRENT = pElemTypeGroupStart;
                        }
                        else if (pElemTypeGroupEnd->name
                                 == QString::fromUtf8(jsonType.asString().data()))
                        {
                            FBElemType::CURRENT = pElemTypeGroupEnd;
                        }
                        else
                        {
                            for (int kk=0; kk<vElemTypes.size(); kk++)
                            {
                                if (vElemTypes[kk]->name
                                        == QString::fromUtf8(jsonType.asString().data()))
                                {
                                    FBElemType::CURRENT = vElemTypes[kk];
                                    break;
                                }
                            }
                        }
                        // На вскяий случай, если файл был изменён вручную: если
                        // не нашли тип элемента - просто не добавляем его на сцену:
                        if (FBElemType::CURRENT == NULL)
                            continue;

                        FBElem *newElem = FBElemType::CURRENT->CreateElem();
                        if (parentLabel->InsertElem(newElem,0))
                        {
                            connect(newElem, SIGNAL(elemPressed()),
                                    this, SLOT(OnElemPressed()));
                        }
                        else
                        {
                            delete newElem;
                            continue;
                        }

                        // Очищаем атрибуты элемента, т.к. они сейчас будут загружены
                        // из файла.
                        newElem->attributes.clear();

                        // Загружаем атрибуты и проверяем на нуль. У таких элементов
                        // как пробел или конец группы атрибутов может не быть, поэтому
                        // элемент мы создали, но дальше не идём.
                        Json::Value jsonAttrs = jsonElem[FBJSONKEY_attributes];
                        if (jsonAttrs.isNull())
                            continue;

                        // Загружаем атрибуты элемента (в JSON-файле это не массив,
                        // поэтому считываем как кортеж).
                        std::vector<std::string> members = jsonAttrs.getMemberNames();
                        for (int l=0; l<members.size(); l++)
                        {
                            //Json::Value jsonAttrName = jsonAttrs[l][FBJSONKEY_name];
                            //Json::Value jsonAttrAlias = jsonAttrs[l][FBJSONKEY_alias];
                            //Json::Value jsonAttrValue = jsonAttrs[l][FBJSONKEY_value];

                            jsonValue = jsonAttrs[members[l].data()];

                            //newElem->attributes.append(QPair<QString,QString>(
                            newElem->attributes.append(QPair<QString,Json::Value>(
                                members[l].data(),
                                //FBElemType::GetAttributeValueString(members[l].data(),
                                //                                    jsonValue)));
                                                           jsonValue));
                        }
                    }
                }
            }

            // Пользователь начинает с 1-ой открытой вертикальной вкладки:
            isCurrentVertical = false; // чтоб с гарантией сменилось при смене ориентации + т.к. последняя была всё=таки горизонтальной
            OnActionOrtnPrt(); // вызываем смену ориентации
            actionOrtnPrt->setChecked(true); // Добавляем галку которая либо есть либо нету
            tabWidget->setCurrentIndex(0);
            ui->lineEdit->setText(tabWidget->tabText(0));
            ui->groupBox_5->setTitle(QString::fromUtf8("Редактирование вкладки: ") +
                                     ui->lineEdit->text());
        }

        EnableInterface();

        ui->label_2->setText(QString::fromUtf8("Открытый проект: ")
                   + pathName);
    }
}


//-----------------------------------------------------------------------//
//                         СОХРАНИТЬ ПРОЕКТ                              //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionSave ()
{
    // Настраиваем диалог сохранения проекта.
    QFileDialog dialogSave(this);
    dialogSave.setAcceptMode(QFileDialog::AcceptSave);
    dialogSave.setFileMode(QFileDialog::AnyFile);
    dialogSave.setViewMode(QFileDialog::List);
    dialogSave.setDefaultSuffix("ngfp");
    dialogSave.setNameFilter("*.ngfp");
    dialogSave.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogSave.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogSave.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogSave.setLabelText(QFileDialog::Accept,QString::fromUtf8("Сохранить"));
    dialogSave.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogSave.setWindowTitle(QString::fromUtf8("Сохранить проект как ..."));

    // Задаём выбор по умолчанию, но выбранное значение может быть совершенно другим.
    dialogSave.setDirectory(CUR_PRJ->path);
    dialogSave.selectFile(CUR_PRJ->nameBase+".ngfp");

    if (dialogSave.exec())
    {
        // TODO: Подумать, можно ли сделать временную директорию с уникальным
        // именем, чтобы не создавать уникальные имена для временных файлов.

        // Определяем zip-файл проекта, который будем записывать на диск.
        QString prj_full_pathname = dialogSave.selectedFiles().first();
        if (!prj_full_pathname.endsWith(".ngfp",Qt::CaseInsensitive))
            prj_full_pathname = prj_full_pathname + ".ngfp";

        // Получаем базовое имя для создания всех сопутствующих временных файлов
        // проекта.
        QString prj_name_base;
        QString str = prj_full_pathname; // копируем чтобы не запортить
        str.chop(5); // убираем расширение
        while (str[str.length()-1] != '/')
        {
            prj_name_base.prepend(str[str.length()-1]);
            str.chop(1);
        }

        // Получаем выбранный путь к zip файлу, чтобы в этой же директории создать
        // временные файлы.
        QString prj_path = str;

        // По ходу создания и работы с временными файлами символизирует о том,
        // что работу можно продолжать.
        bool progress_ok = true;

        QString formFilePath;
        QString dataFilePath;
        QString metaFilePath;

// ********************* 1. СОЗДАЁМ ВРЕМЕННЫЙ JSON ФАЙЛ ФОРМЫ ************************

        QTemporaryFile file_xml (prj_path + "XXXXXXform.json");
        QList<QPair<QString,QString> > images_to_save;
        if (file_xml.open())
        {
            formFilePath = file_xml.fileName();
            QTextStream xml_out(&file_xml);
            xml_out.setCodec("UTF-8"); // Важно поставить кодек, иначе будет неправильная кодировка.

            Json::Value jsonRoot;
            Json::Value jsonValue;
            QByteArray baValue;

            // Для каждой вкладки:
            // Для каждой ориентации (если имеется):
            // Для каждого элемента:
            // Сохраняем его состояние и параметры в файл.
            Json::Value jsonTabs;
            for (int i=0; i<tabWidget->count(); i++)
            {
                Json::Value jsonTab;

                baValue = tabWidget->tabText(i).toUtf8();
                jsonValue = baValue.data();
                jsonTab[FBJSONKEY_name] = jsonValue;

                Json::Value jsonPortraitElems;
                Json::Value jsonAlbumElems;

                // по очереди для обоих ориентаций:
                for (int ornt=0; ornt<=1; ornt++)
                {
                    FBTabWidget *curTabWidget;
                    Json::Value *curJsonElems;

                    if (ornt == 0)
                    {
                        curTabWidget = tabWidget;
                        curJsonElems = &jsonPortraitElems;
                    }
                    else
                    {
                        curTabWidget = tabHorWidget;
                        curJsonElems = &jsonAlbumElems;
                    }

                    // Получаем родительский лейбл для данной вкладки, для того чтобы взять
                    // все его виджеты - элементы интерфейса.
                    FBParentLabel *parentLabel = correspondence[curTabWidget->widget(i)].second;

                    for (int k=0; k<parentLabel->elements.size(); k++)
                    {
                        Json::Value jsonElem;
                        baValue = parentLabel->elements[k]->elemType->name.toUtf8();
                        jsonValue = baValue.data();
                        jsonElem[FBJSONKEY_type] = jsonValue;

                        Json::Value jsonAttrs;
                        QList<QPair<QString,Json::Value> > attrs
                                = parentLabel->elements[k]->attributes;
                        for (int l=0; l<attrs.size(); l++)
                        {
                            // Отдельная логика для атрибута path (для картинки),
                            // если был задан новый путь к новой картинке.
                            // Изменяем значение атрибута - указываем, имя
                            // картинки в архиве, вместо полного имени на
                            // диске.
                          /*  if (attrs[l].first == FBATTR_path
                                && !attrs[l].second.isNull()
                                && !QString::fromUtf8(
                                   attrs[l].second.asString().data())
                                   .startsWith("archive:"))
                            {
                                // Запоминаем полный путь к картинке.
                                QString str_real_path = QString::fromUtf8(
                                            attrs[l].second.asString().data());

                                // Ищем новое уникальное имя для картинки при помощи счётчика
                                // в метаданных и изменяем само значение этого счётчика.
                                int counter = CUR_PRJ->metaData[FBJSONKEY_meta_image_counter].asInt();
                                QString str_archive_name = QString::fromUtf8("archive:image"+QString::number(counter)+".png");
                                counter++;
                                CUR_PRJ->metaData[FBJSONKEY_meta_image_counter] = counter;

                                // Сохраняем оба определённых имени в
                                // массив, для того чтобы чуть позже записать
                                // изображение в итоговый архив.
                                images_to_save.append(QPair<QString,QString>
                                               (str_real_path,str_archive_name));

                                // Сохраняем в метаданных имя картинки в архиве.
                                QByteArray archive_name_ba;
                                archive_name_ba = str_archive_name.toUtf8();
                                attrs[l].second = archive_name_ba.data();
                            }
                            */

                            // Готовим атрибут для записи в файл.
                            QByteArray attr_ba_Name;
                            attr_ba_Name = attrs[l].first.toUtf8();
                            jsonAttrs[attr_ba_Name.data()] = attrs[l].second;
                        }

                        jsonElem[FBJSONKEY_attributes] = jsonAttrs;

                        curJsonElems->append(jsonElem);
                    }

                }

                jsonTab[FBJSONKEY_portrait_elements] = jsonPortraitElems;
                jsonTab[FBJSONKEY_album_elements] = jsonAlbumElems;

                jsonTabs.append(jsonTab);
            }

            jsonRoot[FBJSONKEY_tabs] = jsonTabs;

            Json::StyledWriter jsonWriter;
            //Json::FastWriter jsonWriter;
            std::string finalJsonString = jsonWriter.write(jsonRoot);
            // Обязательная трансформация из Utf8.
            QString xml_result = QString::fromUtf8(finalJsonString.data());
            xml_out<<xml_result;
        }
        else
        {
            progress_ok = false;
            ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось создать временный JSON-файл формы "
                                         "во время сохранения проекта."));
        }

// ******************** 2. СОЗДАЁМ ВРЕМЕННЫЙ GEOJSON ФАЙЛ ДАННЫХ ***********************

        // Если текущий ИД - это уже трансформированный JSON открытого проекта,
        // то это не проблема, т.к. он находится внутри архива и конфликтов с
        // перезаписью не будет. 
        if (progress_ok)
        {
            // Создаём сначала пустой файл GeoJSON методами Qt, чтобы удостовериться,
            // что его имя будет уникально. По сути эти дейсвтия нужны только чтобы
            // получить уникальное имя.

            // TODO: делать это всё во временной директории - тогда не нужны
            // уникальные имена файлов. Использовать QTemporaryDir но в Qt5.

            QTemporaryFile *file_geojson;
            file_geojson = new QTemporaryFile(prj_path + "XXXXXXdata.geojson");
            if (file_geojson->open())
            {
                // Удаляем файл, но сохраням имя, чтобы создать ИД методами GDAL.
                // Если всё успешно, то файл с уникальным именем будет затем создан
                // GDAL-ом, а удалён в конце метода, как простой файл.
                QByteArray geojsonBa = file_geojson->fileName().toUtf8();
                dataFilePath = file_geojson->fileName();
                const char *geojson_unique_name = geojsonBa.data();
                file_geojson->close();
                delete file_geojson;

                // Мы должны сохранить данные (фичи) слоя только в том случае, если
                // текущий проект - это только что созданный из другого ИД проект.
                if (CUR_PRJ->datasetToConvert != NULL)
                {
                    //OGRRegisterAll();
                    //GDALAllRegister();
                    //OGRSFDriver *poJsonDriver = OGRSFDriverRegistrar::GetRegistrar()
                    //        ->GetDriverByName("GeoJSON");
                    GDALDriver *poJsonDriver = GetGDALDriverManager()
                            ->GetDriverByName("GeoJSON");
                    if(poJsonDriver != NULL)
                    {
                        //OGRDataSource *poJsonDS = poJsonDriver
                        //        ->CreateDataSource(geojson_unique_name);
                        GDALDataset *poJsonDS = poJsonDriver
                                ->Create(geojson_unique_name, 0, 0, 0, GDT_Unknown, NULL);
                        if(poJsonDS != NULL)
                        {
                            //if (CUR_PRJ->poDS != NULL)
                            //OGRLayer *poSourceLayer = CUR_PRJ->poDS->GetLayer(0);
                            OGRLayer *poSourceLayer = CUR_PRJ->
                                    datasetToConvert->GetLayer(0);
                            OGRLayer *poJsonLayer = poJsonDS->
                                 CopyLayer(poSourceLayer,poSourceLayer->GetName());
                            if(poJsonLayer != NULL)
                            {
                                //poJsonLayer->
                            }
                            else
                            {
                                progress_ok = false;
                                ShowMsgBox(QString::fromUtf8("Ошибка при создании временного GeoJSON-файла "
                               "данных: не удалось создать слой в источнике данных GeoJSON"));
                            }

                            //OGRDataSource::DestroyDataSource(poJsonDS);
                            GDALClose(poJsonDS);
                        }
                        else
                        {
                            progress_ok = false;
                            ShowMsgBox(QString::fromUtf8("Ошибка при создании временного GeoJSON-файла "
                           "данных: не удалось создать источник данных GeoJSON"));
                        }
                    }
                    else
                    {
                        progress_ok = false;
                        ShowMsgBox(QString::fromUtf8("Ошибка при создании временного GeoJSON-файла "
                       "данных: не удалось инициализировать драйвер формата GeoJSON"));
                    }
                }

                // Иначе ничего не делаем, т.к. файл уже существует в сохранённой
                // на данный момент версии проекта.
                else
                {
                    // Тут на самом деле мы копируем старый GeoJSON ИД, т.к.
                    // так приходится делать из-за текущего способоа создания итогового
                    // ЗИП-архива - он создаётся каждый раз с нуля.

                    // TODO: не создавать каждый раз новый, а модифицировать старый
                    // файл проекта, нет трогая при этом файл данных и метаданных.

                    CPLSetConfigOption("CPL_VSIL_ZIP_ALLOWED_EXTENSIONS","ngfp");
                    QString __filePath = CUR_PRJ->path + CUR_PRJ->nameBase
                     + QString::fromUtf8(".ngfp");
                    __filePath.prepend("/vsizip/");
                    __filePath.append("/data.geojson");
                    QByteArray __pathBa;
                    __pathBa = __filePath.toUtf8();
                    char **__allowedDrivers = NULL;
                    __allowedDrivers = CSLAddString(__allowedDrivers,"GeoJSON");
                    GDALDataset *__srcJsonDs = (GDALDataset*) GDALOpenEx(__pathBa.data(),
                     GDAL_OF_VECTOR | GDAL_OF_READONLY, __allowedDrivers, NULL, NULL);
                    CSLDestroy(__allowedDrivers);
                    if(__srcJsonDs == NULL)
                    {
                        ShowMsgBox(QString::fromUtf8("Ошибка при пересохранении файла данных!"));
                        progress_ok = false;
                    }
                    else
                    {
                        OGRLayer * __srcJsonLayer = __srcJsonDs->GetLayer(0);
                        if (__srcJsonLayer == NULL)
                        {
                            ShowMsgBox(QString::fromUtf8("Ошибка при пересохранении файла данных!"));
                            progress_ok = false;
                        }
                        else
                        {
                            GDALDriver *__jsonDriver = GetGDALDriverManager()
                                    ->GetDriverByName("GeoJSON");
                            if(__jsonDriver == NULL)
                            {
                                ShowMsgBox(QString::fromUtf8("Ошибка при пересохранении файла данных!"));
                                progress_ok = false;
                            }
                            else
                            {
                                GDALDataset *__tgtJsonDs = __jsonDriver
                                 ->Create(geojson_unique_name, 0, 0, 0, GDT_Unknown, NULL);
                                if (__tgtJsonDs == NULL)
                                {
                                    ShowMsgBox(QString::fromUtf8("Ошибка при пересохранении файла данных!"));
                                    progress_ok = false;
                                }
                                else
                                {
                                    OGRLayer *__tgtJsonLayer = __tgtJsonDs
                                     ->CopyLayer(__srcJsonLayer,__srcJsonLayer->GetName());
                                    if (__tgtJsonLayer == NULL)
                                    {
                                        ShowMsgBox(QString::fromUtf8("Ошибка при пересохранении файла данных!"));
                                        progress_ok = false;
                                    }
                                }
                                GDALClose(__tgtJsonDs);
                            }
                        }
                    }
                    GDALClose(__srcJsonDs);
                }
            }
            else
            {
                progress_ok = false;
                ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось создать временный GeoJSON-файл "
                       "данных во время сохранения проекта."));
            }
        }

// ***************** 3. СОЗДАЁМ ВРЕМЕННЫЙ JSON ФАЙЛ МЕТАДАННЫХ ******************

        // Создаём файл вне условий, чтобы он удалился не при завершении ветвления,
        // а в конце метода.
        QTemporaryFile file_meta (prj_path + "XXXXXXmeta.json");
        if (progress_ok)
        {
            // TODO: сохранять метаданные только, если это проект, созданный впервые,
            // иначе нужный файл уже существует. Все метаданные сохраняются только
            // 1 раз и не могут быть обновлены.
            // Сделать по аналогии с файлом данных.

            // На данный момент сохраняем файл в любом случае.
            //if (CUR_PRJ->datasetToConvert != NULL)
            //{
                if (file_meta.open())
                {
                    metaFilePath = file_meta.fileName();
                    QTextStream json_out(&file_meta);
                    json_out.setCodec("UTF-8");

                    // Записываем сформированную json-строку в meta-файл.
                    Json::StyledWriter jsonWriter;
                    std::string final_json_string = jsonWriter.write(CUR_PRJ->metaData);
                    QString res_json_str = QString::fromUtf8(final_json_string.data());
                    json_out<<res_json_str;
                }
                else
                {
                    progress_ok = false;
                    ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось создать временный JSON-файл "
                           "метаданных во время сохранения проекта."));
                }
            //}
        }

// ********************* 4. СОЗДАЁМ ИТОГОВЫЙ ZIP ФАЙЛ ПРОЕКТА *************************

        if (progress_ok)
        {
            QByteArray fullNameBa = prj_full_pathname.toUtf8();
            void *hZip = CPLCreateZip(fullNameBa.data(),NULL);

            if (hZip)
            {
                size_t nBufferSize = 1024 * 1024;
                GByte *pabyBuffer = (GByte *)CPLMalloc(nBufferSize);
                QByteArray fullPathBa;

                fullPathBa = formFilePath.toUtf8();
                if (!AddFileToZip(fullPathBa.data(), "form.json",//CPLGetFilename(fullPathBa.data()),
                                 hZip, &pabyBuffer, nBufferSize))
                {
                    ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! Не "
                           "удалось записать временный JSON-файл формы в файл проекта."));
                }
                else
                {
                    bool okk = true;
                    //if (CUR_PRJ->datasetToConvert != NULL)
                    //{
                        fullPathBa = dataFilePath.toUtf8();
                        okk = AddFileToZip(fullPathBa.data(), "data.geojson",//CPLGetFilename(fullPathBa.data()),
                                           hZip, &pabyBuffer, nBufferSize);
                    //}

                    if (!okk)
                    {
                        ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! Не "
                           "удалось записать временный GeoJSON-файл данных в файл проекта."));
                    }
                    else
                    {
                        okk = true;
                        //if (CUR_PRJ->datasetToConvert != NULL)
                        //{
                            fullPathBa = metaFilePath.toUtf8();
                            okk = AddFileToZip(fullPathBa.data(), "meta.json",//CPLGetFilename(fullPathBa.data()),
                                               hZip, &pabyBuffer, nBufferSize);
                        //}

                        if (!okk)
                        {
                            ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! "
                              "Не удалось записать временный JSON-файл метаданных в файл проекта."));
                        }

                        else
                        {
                            okk = true;
/*
                            Выше сформирован список для копирования - и что важно
                            картинки, контролы которых удалились, должны быть тоже
                            удалены (не скопированы).

                            Использовать МЕМ файл
*/
                            for (int i=0; i<images_to_save.size(); i++)
                            {
                                QByteArray path_ba = images_to_save[i].first.toUtf8();
                                images_to_save[i].second.remove(0,8); // удаляем 'archive:'
                                QByteArray archive_name_ba = images_to_save[i].second.toUtf8();
                                okk = AddFileToZip(path_ba.data(), archive_name_ba.data(),
                                                   hZip, &pabyBuffer, nBufferSize);
                                if (!okk) break;
                            }

                            if (!okk)
                            {
                                ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! "
                                  "Не удалось записать один из PNG файлов изображений в файл проекта."));
                            }

                            // Нужные файлы были записаны. Итоговый архив сформирован
                            // и сохранён.
                            // Делаем текущий проект таким, как будто он был открыт.
                            else
                            {

                                // Теперь путь к проекту - тот, что сохранён.
                                CUR_PRJ->path = prj_path;
                                CUR_PRJ->nameBase = prj_name_base;

                                // ИД больше не нужен.
                                if (CUR_PRJ->datasetToConvert != NULL)
                                {
                                    //OGRDataSource::DestroyDataSource(CUR_PRJ->datasetToConvert);
                                    GDALClose(CUR_PRJ->datasetToConvert);
                                    CUR_PRJ->datasetToConvert = NULL;
                                }

                                 ui->label_2->setText(QString::fromUtf8("Открытый проект: ")
                                   + prj_full_pathname);
                            }
                        }
                    }
                }

                CPLCloseZip(hZip);
                CPLFree(pabyBuffer);
            }
            else
            {
                ShowMsgBox(QString::fromUtf8("Ошибка при сохранении "
                                         "проекта! Не удалось создать ZIP-архив."));
            }
        }

        // Временные файлы удалятся сами как только метод закончит выполнение.
        // Файл с данными не удалится, т.к. он не временный, а создан GDAL-ом,
        // поэтому удаляем вручную.
        QFile::remove(dataFilePath);
    }
}



bool FormBuilder::AddFileToZip(const CPLString &szFilePathName,
                               const CPLString &szFileName,
                  void* hZIP, GByte **pabyBuffer, size_t nBufferSize)
{
    int nRet = 0;
    size_t nBytesRead;
    VSILFILE *fp;

    fp = VSIFOpenL(szFilePathName, "rb");
    if (fp == NULL)
    {
        return false;
    }

    if (CPLCreateFileInZip(hZIP, szFileName, NULL) != CE_None)
    {
        VSIFCloseL(fp);
        return false;
    }

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
