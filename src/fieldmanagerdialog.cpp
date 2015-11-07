/******************************************************************************
 *
 * Name:     fieldmanagerdialog.cpp
 * Project:  NextGIS Form Builder
 * Purpose:  Dialog for editing layer fields.
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


FBFieldManagerDialog::FBFieldManagerDialog(QWidget *parent): QDialog(parent)
{
    this->setStyleSheet("QWidget { color: black }");

    this->setWindowTitle(tr("Поля слоя в проекте"));

    tree = new QTreeWidget(this);
    QStringList headers;
    headers<<tr("Имя")<<tr("Тип данных");//<<tr("Привязка к форме");
    tree->setIndentation(0);
    tree->setItemsExpandable(false);
    tree->setFont(QFont("Candara",11));
    tree->setColumnCount(2);//(3);
    tree->setHeaderLabels(headers);
    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onItemClicked(QTreeWidgetItem*,int)));

    // Выводим в список поля проекта.
    Json::Value jsonMeta = FB::CUR_PROJECT->getJsonMeta();
    Json::Value jsonArFields = jsonMeta[FB_JSON_META_FIELDS];
    for (int i=0; i<jsonArFields.size(); ++i)
    {
        Json::Value jsonField = jsonArFields[i];
        QStringList strs;
        strs.append(QString::fromUtf8(jsonField[FB_JSON_META_KEYNAME].asString().data()));
        strs.append(QString::fromUtf8(jsonField[FB_JSON_META_DATATYPE].asString().data()));
        //strs.append("-");
        QTreeWidgetItem *item = new QTreeWidgetItem(strs);
        tree->addTopLevelItem(item);
    }

    // Интерфейс работы с полями.
    butAdd = new QToolButton(this);
    butAdd->setIcon(QIcon(":/img/plus.png"));
    butAdd->setToolTip(tr("Добавить новый элемент"));
    connect(butAdd, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    butRemove = new QToolButton(this);
    butRemove->setIcon(QIcon(":/img/minus.png"));
    butRemove->setToolTip(tr("Удалить выделенный элемент"));
    butRemove->setEnabled(false);
    connect(butRemove, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));
    butChange = new QToolButton(this);
    butChange->setIcon(QIcon(":/img/sharp.png"));
    butChange->setToolTip(tr("Изменить выделенный элемент"));
    butChange->setEnabled(false);
    connect(butChange, SIGNAL(clicked()), this, SLOT(onChangeClicked()));
        butChange->hide();
    QGroupBox *groupBox = new QGroupBox(this);
    groupBox->setTitle(tr("Поле"));
    QLabel *labName = new QLabel(groupBox);
    labName->setText(tr("Имя: "));
    editName = new QLineEdit(groupBox);
    QLabel *labType = new QLabel(groupBox);
    labType->setText(tr("Тип:"));
    comboType = new QComboBox(groupBox);
    comboType->addItem("INTEGER",QString("INTEGER"));
    comboType->addItem("REAL",QString("REAL"));
    comboType->addItem("DATE",QString("DATE"));
    comboType->addItem("STRING",QString("STRING"));

    // ОК и отмена.
    butOk = new QPushButton(this);
    butOk->setText("OK");
    connect(butOk, SIGNAL(clicked()), this, SLOT(accept()));
    butCancel = new QPushButton(this);
    butCancel->setText(tr("Отмена"));
    connect(butCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->addWidget(butAdd);
    vl1->addWidget(butRemove);
    vl1->addWidget(butChange);
    QHBoxLayout *h1 = new QHBoxLayout();
    h1->addWidget(labName);
    h1->addWidget(editName);
    QHBoxLayout *h2 = new QHBoxLayout();
    h2->addWidget(labType);
    h2->addWidget(comboType);
    QVBoxLayout *vlgb = new QVBoxLayout(groupBox);
    vlgb->addLayout(h1);
    vlgb->addLayout(h2);
    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addLayout(vl1);
    hl1->addWidget(groupBox);
    QHBoxLayout *hlbuts = new QHBoxLayout();
    hlbuts->addWidget(butOk);
    hlbuts->addWidget(butCancel);
    QVBoxLayout *vlall = new QVBoxLayout(this);
    vlall->addWidget(tree);
    vlall->addLayout(hl1);
    vlall->addLayout(hlbuts);

    this->adjustSize();
}


void FBFieldManagerDialog::onItemClicked (QTreeWidgetItem *item, int itemCol)
{
    editName->setText(item->text(0));
    comboType->setCurrentText(item->text(1));

    butRemove->setEnabled(true);
    butChange->setEnabled(true);
}


void FBFieldManagerDialog::onAddClicked ()
{
    QString newName = editName->text();
    QString newType = comboType->currentText();

    if (newName == "")
    {
        showMsgBox(tr("Не введено имя поля"),
                   QMessageBox::Warning);
        return;
    }

    // Заменяем неподдерживаемые символы. Список взят отсюда:
    // https://github.com/nextgis/android_maplib/blob/master/src/main/java/com/nextgis/maplib/util/LayerUtil.java#L25
    // TODO: Так же добавлены/убраны свои символы. Изменить это, если не правильно.
    QStringList forbiddenChars;
    forbiddenChars<<"@"<<"#"<<"%"<<"^"<<"&"<<"*"<<"!"<<"$"<<"("<<")";
    //forbiddenChars<<":";
    forbiddenChars<<" ";
    bool wasRepl = false;
    for(int i=0; i<forbiddenChars.size(); i++)
    {
        if (newName.contains(forbiddenChars[i]))
            wasRepl = true;
        newName.replace(forbiddenChars[i],"_");
    }

    // Смотрим, есть ли уже такое имя поля.
    for (int i=0; i<tree->topLevelItemCount(); i++)
    {
        if (tree->topLevelItem(i)->text(0) == newName)
        {
            showMsgBox(tr("Поле с таким именем уже существует, выберите другое"),
                       QMessageBox::Critical);
            return;
        }
    }

    if (wasRepl)
    {
        showMsgBox(tr("В имени поля были заменены неподдерживаемые символы"),
                   QMessageBox::Warning);
    }

    QStringList strs;
    strs.append(newName);
    strs.append(newType);
    //strs.append("-");
    QTreeWidgetItem *item = new QTreeWidgetItem(strs);
    tree->addTopLevelItem(item);

    tree->clearSelection();
    butRemove->setEnabled(false);

}


void FBFieldManagerDialog::onRemoveClicked ()
{
    if (tree->currentItem() == NULL)
        return;

    if (tree->topLevelItemCount() == 1)
    {
        showMsgBox(tr("В слое должно остаться как минимум одно поле"),
                   QMessageBox::Critical);
        return;
    }

    int ret = showAlertBox(tr("Удаление поля приведёт к удалению всех его данных"
                              " (в момент сохранения проекта). Удалить поле?"),
                           QMessageBox::Warning);
    if (ret == QMessageBox::Ok)
    {
        QString oldName = tree->currentItem()->text(0);
        QTreeWidgetItem *curItem = tree->takeTopLevelItem(tree->indexOfTopLevelItem(tree->currentItem()));
        delete curItem;

        // Сохраняем имя удалённого поля в список удалённых полей, чтобы после завершения работы
        // диалога снять соответствия элементам формы.
        // Поля хранятся в множестве - т.е. каждое 1 раз, т.к. нам важно удалить только
        // те поля, которые были поставлены в соответствие элементам до открытия диалога
        // (вновь добавленные поля с теми же именами значения не имеют).
        fieldsDeleted.insert(oldName);

        tree->clearSelection();
        butRemove->setEnabled(false);
        butChange->setEnabled(false);
    }
}


void FBFieldManagerDialog::onChangeClicked ()
{
    // TODO: сделать изменение имени.
}


void FBFieldManagerDialog::showMsgBox (QString msg, QMessageBox::Icon icon)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Сообщение"));
    msgBox.setIcon(icon);
    msgBox.exec();
}
int FBFieldManagerDialog::showAlertBox (QString msg, QMessageBox::Icon icon)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setWindowTitle(tr("Предупреждение"));
    msgBox.setIcon(icon);
    return msgBox.exec();
}



