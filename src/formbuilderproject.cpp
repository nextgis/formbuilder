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
//                         НОВЫЙ ПУСТОЙ ПРОЕКТ                           //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionNewVoid ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("При создании нового проекта, все "
                     "несохранённые изменения в текущем проекте будут "
                     "потеряны. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    // Просто создаём новый проект, удалив старый.
    delete CUR_PRJ;
    CUR_PRJ = new FBProject();

    ui->label_2->setText(QString::fromUtf8("Открытый проект: <без имени>. "
                                "Источник данных не используется"));

    // Очищаем весь интерфейс и создаём новую форму.
    ClearAll();
}


//-----------------------------------------------------------------------//
//                 НОВЫЙ ПРОЕКТ ИЗ ИСТОЧНИКА ДАННЫХ GDAL                 //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionNewDataset ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("При создании нового проекта, все "
                                     "несохранённые изменения в текущем проекте будут "
                                     "потеряны. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    QFileDialog dialogNewDataset (this);
    dialogNewDataset.setAcceptMode(QFileDialog::AcceptOpen);
    // Выбрать можно только один существующий файл:
    dialogNewDataset.setFileMode(QFileDialog::ExistingFile);
    dialogNewDataset.setViewMode(QFileDialog::List);
    //dialogNewDataset->setDefaultSuffix("shp");
    //dialogNewDataset->setNameFilter("*.shp");
    dialogNewDataset.setLabelText(QFileDialog::LookIn,QString::fromUtf8("Путь:"));
    dialogNewDataset.setLabelText(QFileDialog::FileName,QString::fromUtf8("Имя файла"));
    dialogNewDataset.setLabelText(QFileDialog::FileType,QString::fromUtf8("Тип файла"));
    dialogNewDataset.setLabelText(QFileDialog::Accept,QString::fromUtf8("Выбрать"));
    dialogNewDataset.setLabelText(QFileDialog::Reject,QString::fromUtf8("Отмена"));
    dialogNewDataset.setWindowTitle(QString::fromUtf8("Создание нового проекта: "
                       "выберите файловый источник данных, поддерживаемый GDAL ..."));
    dialogNewDataset.setDirectory(QDir()); //ставим текущую директорию

    if (dialogNewDataset.exec())
    {
        QStringList dsPaths = dialogNewDataset.selectedFiles();
        // TODO: сделать выдачу сообщения, что нужно выбрать только 1 файл.
        QByteArray dsPathBa = dsPaths[0].toUtf8(); // всегда будет выбран только один файл

        // Создаём новый проект.
        NEW_PRJ = new FBProject();

        // Пытаемся его инициализировать.
        if (!_initGdalDataset(dsPathBa.data()))
        {
            // Если была какая-то ошибка - отменяем создание.
            delete NEW_PRJ;
            return;
        }

        // Иначе удаляем текущий, и новый становится текущим.
        delete CUR_PRJ;
        CUR_PRJ = NEW_PRJ;

        // Очищаем весь интерфейс и создаём новую форму.
        ClearAll();
    }
}


//-----------------------------------------------------------------------//
//                 НОВЫЙ ПРОЕКТ ИЗ ПОДКЛЮЧЕНИЯ К NEXTGISWEB              //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionNewNGW ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("При создании нового проекта, все "
                                     "несохранённые изменения в текущем проекте будут "
                                     "потеряны. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

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
    lineEdit1->setText(QString::fromUtf8("http://bishop.gis.to"));
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

        // IMPORTANT TODO: проверять на наличие '/' в конце строки.

        QString finalUrl = lineEdit1->text() + "/resource/"
                + treeWidget->currentItem()->data(0,Qt::UserRole).toString()
                + "/geojson/";
        QByteArray finalUrlBa = finalUrl.toUtf8();

        // Создаём новый проект.
        NEW_PRJ = new FBProject();

        // Пытаемся его инициализировать.
        if (!_initGdalDataset(finalUrlBa.data()))
        {
            // Если была какая-то ошибка - отменяем создание.
            delete NEW_PRJ;
            return;
        }

        // Иначе удаляем текущий, и новый становится текущим.
        delete CUR_PRJ;
        CUR_PRJ = NEW_PRJ;

        // Формируем json-строку подключения к NGW. При сохранении будет создан файл
        // json с этими параметрами.
        Json::Value root;
        const Json::Value vUrl = finalUrlBa.data();
        root["url"] = vUrl;
        QByteArray logBa = lineEdit2->text().toUtf8();
        const Json::Value vLogin = logBa.data();
        root["login"] = vLogin;
        QByteArray pasBa = lineEdit3->text().toUtf8();
        const Json::Value vPassword = pasBa.data();
        root["password"] = vPassword;
        Json::StyledWriter writer;
        CUR_PRJ->NGWConnection = writer.write(root);

        // Очищаем весь интерфейс и создаём новую форму.
        ClearAll();
    }
}


// Инициализация источника данных GDAL.
bool FormBuilder::_initGdalDataset (char *datasetName)
{
    // Не удаляем старый источник данных и не стираем его доступные поля,
    // пока не убедимся, что всё новое загрузилось верно.
    //GDALDataset *poNewDS;
//    OGRDataSource* poNewDS;
    //poNewDS = (GDALDataset*) GDALOpenEx(datasetName, GDAL_OF_VECTOR, NULL, NULL, NULL);
//    poNewDS = OGRSFDriverRegistrar::Open(datasetName);

    if (NEW_PRJ == NULL)
        return false;

    NEW_PRJ->poDS = OGRSFDriverRegistrar::Open(datasetName);
    if(NEW_PRJ->poDS == NULL)
    {
        ShowMsgBox(QString::fromUtf8("Ошибка при попытке открыть выбранный"
                                     "источник данных!"));
        return false;
    }

    // Проверяем кол-во слоёв.
    if (NEW_PRJ->poDS->GetLayerCount() == 0 || NEW_PRJ->poDS->GetLayerCount() > 1)
    {
        ShowMsgBox(QString::fromUtf8("Ошибка: требуется, чтобы источник данных "
                                     "содержал только 1 слой! Выбранный источник "
                                     "данных содержит 0 или больше чем 1 слой."));
        //GDALClose(poNewDS);
//        OGRDataSource::DestroyDataSource(poNewDS);
        return false;
    }

    OGRLayer *poLayer =  NEW_PRJ->poDS->GetLayer(0);
    if (poLayer == NULL)
    {
        ShowMsgBox(QString::fromUtf8("Ошибка: невозможно считать слой в выбранном "
                                     "источнике данных "));
        //GDALClose(poNewDS);
//        OGRDataSource::DestroyDataSource(poNewDS);
        return false;
    }

    // Проверяем правильность системы координат.
    OGRSpatialReference SpaRef1(SRS_WKT_WGS84);
    OGRSpatialReference SpaRef2;
    SpaRef2.SetFromUserInput("EPSG:3857");
    if ((poLayer->GetSpatialRef()->IsSame(&SpaRef1) == FALSE)
            && (poLayer->GetSpatialRef()->IsSame(&SpaRef2) == FALSE))
    {
        ShowMsgBox(QString::fromUtf8("Ошибка: слой выбранного источника данных"
                                     " имеет неподдерживаемую систему координат "
                                     "(поддерживаются только EPSG:4326 и EPSG:3857)"));
        //GDALClose(poNewDS);
//        OGRDataSource::DestroyDataSource(poNewDS);
        return false;
    }

    // Стираем и удаляем всё старое - создаём новый проект.
//    if (poDS != NULL)
        //GDALClose(poDS);
//        OGRDataSource::DestroyDataSource(poDS);
//    poDS = poNewDS;

    // Считываем набор доступных целевых полей.
    NEW_PRJ->availableFields.clear();
    OGRFeatureDefn *poLayerDefn = poLayer->GetLayerDefn();
    for (int i=0; i<poLayerDefn->GetFieldCount(); i++)
    {
        OGRFieldDefn *poFieldDefn = poLayerDefn->GetFieldDefn(i);
        NEW_PRJ->availableFields.append(QString(poFieldDefn->GetNameRef()));
    }

    //ClearAll();

    ui->label_2->setText(QString::fromUtf8("Открытый проект: <без имени>."
                         " Используется источник данных: ") + QString(datasetName));

    // Гео-данные будут сохранены в сопутствующий json.
    //wasDataChanged = true;

    return true;
}


//-----------------------------------------------------------------------//
//                          ОТКРЫТЬ ПРОЕКТ                               //
//-----------------------------------------------------------------------//
void FormBuilder::OnActionOpen ()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("При открытии нового проекта, все "
                                     "несохранённые изменения в текущем проекте будут "
                                     "потеряны. Продолжить?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

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
    dialogOpen.setWindowTitle(QString::fromUtf8("Открыть проект ..."));

    QString pathName;
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

// ************************* 1. Считываем JSON-форму *********************************

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

        Json::Value jsonRoot;
        Json::Reader jsonReader;
        Json::Value jsonValue;

        if (!jsonReader.parse(jsonStr, jsonRoot, false)
                || jsonRoot.isNull())
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не удалось прочесть JSON-файл формы"));
            return;
        }

// ************************ 2. Считываем GEOJSON данные ******************************

        jsonValue = jsonRoot[FBJSONKEY_dataset];
        if (jsonValue.isNull())
        {
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не удалось прочесть JSON-файл формы"));
            return;
        }

        FBProject *OPEN_PRJ = new FBProject();

        if (jsonValue.asBool() == true)
        {
            filePath = pathName;
            filePath.prepend("/vsizip/");
            filePath.append("/data.geojson");
            pathBa = filePath.toUtf8();

            OGRRegisterAll();
            OPEN_PRJ->poDS = OGRSFDriverRegistrar::Open(pathBa.data());
            if(OPEN_PRJ->poDS != NULL)
            {
                OGRLayer *poJsonLayer = OPEN_PRJ->poDS->GetLayer(0);
                if(poJsonLayer != NULL)
                {
                    OGRFeatureDefn *poJsonDefn = poJsonLayer->GetLayerDefn();
                    if (poJsonDefn != NULL)
                    {
                        for(int y=0; y<poJsonDefn->GetFieldCount(); y++)
                        {
                            OGRFieldDefn *poFieldDefn = poJsonDefn->GetFieldDefn(y);
                            OPEN_PRJ->availableFields
                                    .append(QString(poFieldDefn->GetNameRef()));
                        }
                    }
                    else
                    {
                        delete OPEN_PRJ;
                        ShowMsgBox(QString::fromUtf8("Ошибка при открытии проекта: "
                          "не удалось считать описание полей в источнике данных"
                                                     "формата GeoJSON"));
                        return;
                    }
                }
                else
                {
                    delete OPEN_PRJ;
                    ShowMsgBox(QString::fromUtf8("Ошибка при открытии проекта: "
                       "не удалось считать слой в источнике данных формата GeoJSON"));
                    return;
                }
            }
            else
            {
                delete OPEN_PRJ;
                ShowMsgBox(QString::fromUtf8("Ошибка при открытии проекта: "
                 "не удалось открыть GDAL источник данных формата GeoJSON"));
                return;
            }
        }

// ******************** 3. Считываем JSON с соединением к NGW ************************

        jsonValue = jsonRoot[FBJSONKEY_ngw_connection];
        if (jsonValue.isNull())
        {
            delete OPEN_PRJ;
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не удалось прочесть JSON-файл формы"));
            return;
        }

        if (jsonValue.asBool() == true)
        {
            filePath = pathName;
            filePath.prepend("/vsizip/");
            filePath.append("/connection.json");
            pathBa = filePath.toUtf8();
            fp = VSIFOpenL(pathBa.data(), "rb");
            if (fp == NULL)
            {
                delete OPEN_PRJ;
                ShowMsgBox(QString::fromUtf8("Ошибка при открытии проекта: "
             "не удалось открыть JSON-файл с информацией о соединении с NextGIS Web"));
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

            // TODO: сделать проверку на правильность строки (м.б. даже
            // сохранять не строку, а json-объект)

            OPEN_PRJ->NGWConnection = jsonConStr;
        }

// ************************ 4. Устанавливаем открытый проект ***********************

        // Сначала считываем вкладки из файла формы.
        Json::Value jsonTabs;
        jsonTabs = jsonRoot[FBJSONKEY_tabs];
        if (jsonTabs.isNull())
        {
            delete OPEN_PRJ;
            ShowMsgBox(QString::fromUtf8("Ошибка при открытие проекта! "
              "Не удалось прочесть JSON-файл формы"));
            return;
        }

        pathBa = pathName.toUtf8();
        OPEN_PRJ->path = QString(CPLGetPath(pathBa.data())) + "/";
        OPEN_PRJ->name_base = QString(CPLGetFilename(pathBa.data()));
        OPEN_PRJ->name_base.chop(5);
        delete CUR_PRJ;
        CUR_PRJ = OPEN_PRJ;

// ************************ 5. Отображаем новую форму ******************************

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

        if (CUR_PRJ->poDS == NULL)
        {
            ui->label_2->setText(QString::fromUtf8("Открытый проект: ")
                       + CUR_PRJ->path + CUR_PRJ->name_base + ".ngfp" +
                QString::fromUtf8(". Источник данных не используется"));
        }
        else
        {
            ui->label_2->setText(QString::fromUtf8("Открытый проект: ")
                       + CUR_PRJ->path + CUR_PRJ->name_base + ".ngfp" +
                QString::fromUtf8(". Используется источник данных: внутренний GeoJSON"));
        }
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
    dialogSave.selectFile(CUR_PRJ->name_base+".ngfp");

    if (dialogSave.exec())
    {
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

        QString xmlFilePath;
        QString geojsonFilePath;
        QString jsonFilePath;

// ********************* 1. СОЗДАЁМ ВРЕМЕННЫЙ JSON ФАЙЛ ФОРМЫ ************************

        QTemporaryFile file_xml (prj_path + "XXXXXXform.json");
        if (file_xml.open())
        {
            xmlFilePath = file_xml.fileName();
            QTextStream xml_out(&file_xml);
            xml_out.setCodec("UTF-8"); // Важно поставить кодек, иначе будет неправильная кодировка.

            Json::Value jsonRoot;
            Json::Value jsonValue;
            QByteArray baValue;

            jsonValue = FB_current_version;
            jsonRoot[FBJSONKEY_version] = jsonValue;
            if (CUR_PRJ->poDS != NULL)
            {
                jsonValue = true;
            }
            else
            {
                jsonValue = false;
            }
            jsonRoot[FBJSONKEY_dataset] = jsonValue;

            if (CUR_PRJ->NGWConnection != "")
            {
                jsonValue = true;
            }
            else
            {
                jsonValue = false;
            }
            jsonRoot[FBJSONKEY_ngw_connection] = jsonValue;

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
//                        QList<QPair<QString,QString> > attrs
                        QList<QPair<QString,Json::Value> > attrs
                                = parentLabel->elements[k]->attributes;
                        for (int l=0; l<attrs.size(); l++)
                        {
                            QByteArray attrBaValue;

                            // Узнаём имя атрибута.
                            attrBaValue = attrs[l].first.toUtf8();

                            // Присваиваем определённое JSON-значение.
                            jsonAttrs[attrBaValue.data()] =
                               //FBElemType::GetAttributeValueJson(attrs[l].first,
                               //                                  attrs[l].second);
                                    attrs[l].second;
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
            // TODO: найти и вызывать метод по получению уникального имени.
            QTemporaryFile *file_geojson;
            file_geojson = new QTemporaryFile(prj_path + "XXXXXXdata.geojson");
            if (file_geojson->open())
            {
                // Удаляем файл, но сохраням имя, чтобы создать ИД методами GDAL.
                QByteArray geojsonBa = file_geojson->fileName().toUtf8();
                geojsonFilePath = file_geojson->fileName();
                const char *geojson_unique_name = geojsonBa.data();
                file_geojson->close();
                delete file_geojson;

                // Если всё успешно, то файл с уникальным именем будет создан
                // GDAL-ом, а удалён в конце метода, как простой файл.
                OGRRegisterAll();
                OGRSFDriver *poJsonDriver = OGRSFDriverRegistrar::GetRegistrar()
                        ->GetDriverByName("GeoJSON");
                if(poJsonDriver != NULL)
                {
                    OGRDataSource *poJsonDS = poJsonDriver
                            ->CreateDataSource(geojson_unique_name);
                    if(poJsonDS != NULL)
                    {
                        if (CUR_PRJ->poDS != NULL)
                        {
                            OGRLayer *poSourceLayer = CUR_PRJ->poDS->GetLayer(0);
                            OGRLayer *poJsonLayer = poJsonDS->CopyLayer(poSourceLayer,
                                                                    poSourceLayer->GetName());
                            if(poJsonLayer == NULL)
                            {
                                progress_ok = false;
                                ShowMsgBox(QString::fromUtf8("Ошибка при создании временного GeoJSON-файла "
                               "данных: не удалось создать слой в источнике данных GeoJSON"));
                            }
                        }

                        OGRDataSource::DestroyDataSource(poJsonDS);
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
            else
            {
                progress_ok = false;
                ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось создать временный GeoJSON-файл "
                       "данных во время сохранения проекта."));
            }
        }

// ***************** 3. СОЗДАЁМ ВРЕМЕННЫЙ JSON ФАЙЛ ПОДКЛЮЧЕНИЯ К NGW ******************

        QTemporaryFile file_json (prj_path + "XXXXXXconnection.json");
        if (progress_ok)
        {
            if (file_json.open())
            {
                jsonFilePath = file_json.fileName();
                QTextStream json_out(&file_json);
                json_out.setCodec("UTF-8");
                json_out << QString::fromStdString(CUR_PRJ->NGWConnection);
            }
            else
            {
                progress_ok = false;
                ShowMsgBox(QString::fromUtf8("Ошибка! Не удалось создать временный JSON-файл "
                       "соединения с NextGIS Web во время сохранения проекта."));
            }
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

                fullPathBa = xmlFilePath.toUtf8();
                if (!AddFileToZip(fullPathBa.data(), "form.json",//CPLGetFilename(fullPathBa.data()),
                                 hZip, &pabyBuffer, nBufferSize))
                {
                    ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! Не "
                           "удалось записать временный JSON-файл формы в ZIP-архив."));
                }
                else
                {
                    fullPathBa = geojsonFilePath.toUtf8();
                    if (!AddFileToZip(fullPathBa.data(), "data.geojson",//CPLGetFilename(fullPathBa.data()),
                                     hZip, &pabyBuffer, nBufferSize))
                    {
                        ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! Не "
                           "удалось записать временный GeoJSON-файл данных в ZIP-архив."));
                    }
                    else
                    {
                        fullPathBa = jsonFilePath.toUtf8();
                        if (!AddFileToZip(fullPathBa.data(), "connection.json",//CPLGetFilename(fullPathBa.data()),
                                         hZip, &pabyBuffer, nBufferSize))
                        {
                            ShowMsgBox(QString::fromUtf8("Ошибка при сохранении проекта! "
                              "Не удалось записать временный JSON-файл соединения с "
                                                         "NextGIS Web в ZIP-архив."));
                        }
                        else
                        {
                            CUR_PRJ->path = prj_path;
                            CUR_PRJ->name_base = prj_name_base;

                            // TODO: по идее здесь должен так же поменяться poDS.
                            // availableFields в любом случае остаётся прежним.

                            if (CUR_PRJ->poDS == NULL)
                            {
                                ui->label_2->setText(QString::fromUtf8("Открытый проект: ")
                                      + prj_full_pathname + QString::fromUtf8("."
                                     " Источник данных не используется"));
                            }
                            else
                            {
                                ui->label_2->setText(QString::fromUtf8("Открытый проект: ")
                                      + prj_full_pathname + QString::fromUtf8("."
                                     " Используется источник данных: внутренний GeoJSON"));
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
        QFile::remove(geojsonFilePath);
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
