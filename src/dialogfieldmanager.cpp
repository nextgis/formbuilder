/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  fields manager dialog
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

#include <QGroupBox>

#include "fb.h"

FBDialogFieldsManager::~FBDialogFieldsManager()
{
}
 

FBDialogFieldsManager::FBDialogFieldsManager(QWidget *parent):
    QDialog(parent)
{
    this->setWindowTitle(tr("Fields in project"));

    tree = new QTreeWidget(this);
    QStringList headers;
    headers<<tr("Keyname")<<tr("Data type");
    tree->setHeaderLabels(headers);
    tree->setIndentation(0);
    tree->setItemsExpandable(false);
    tree->setColumnCount(2);
    QObject::connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(onItemClick(QTreeWidgetItem*,int)));

    butAdd = new QToolButton(this);
    butAdd->setIcon(QIcon(":/img/plus.png"));
    butAdd->setToolTip(tr("Add new field"));
    QObject::connect(butAdd, SIGNAL(clicked()),
                     this, SLOT(onAddClick()));
    butRemove = new QToolButton(this);
    butRemove->setIcon(QIcon(":/img/minus.png"));
    butRemove->setToolTip(tr("Delete selected field"));
    butRemove->setEnabled(false);
    QObject::connect(butRemove, SIGNAL(clicked()),
                     this, SLOT(onRemoveClick()));
    QGroupBox *groupBox = new QGroupBox(this);
    groupBox->setTitle(tr("Field"));
    QLabel *labName = new QLabel(groupBox);
    labName->setText(tr("Keyname: "));
    editName = new QLineEdit(groupBox);
    QLabel *labType = new QLabel(groupBox);
    labType->setText(tr("Type:        "));
    comboType = new QComboBox(groupBox);
    comboType->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    for (int i=0; i<FBProject::DATA_TYPES.size(); i++)
    {
        comboType->addItem(FBProject::DATA_TYPES[i]->aliasNgw);
    }
    butOk = new QPushButton(this);
    butOk->setText(tr("OK"));
    QObject::connect(butOk, SIGNAL(clicked()),
                     this, SLOT(accept()));
    butCancel = new QPushButton(this);
    butCancel->setText(tr("Cancel"));
    QObject::connect(butCancel, SIGNAL(clicked()),
                     this, SLOT(reject()));

    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->addWidget(butAdd);
    vl1->addWidget(butRemove);
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
    hl1->addWidget(groupBox);
    hl1->addLayout(vl1);
    QHBoxLayout *hlbuts = new QHBoxLayout();
    hlbuts->addWidget(butOk);
    hlbuts->addWidget(butCancel);
    QVBoxLayout *vlall = new QVBoxLayout(this);
    vlall->addWidget(tree);
    vlall->addLayout(hl1);
    vlall->addLayout(hlbuts);

    this->adjustSize();
}


void FBDialogFieldsManager::loadFields (QMap<QString,FBField> fields)
{
    tree->clear();
    QMap<QString,FBField>::const_iterator it = fields.constBegin();
    while (it != fields.constEnd())
    {
        QStringList strs;
        strs.append(it.key());
        strs.append(it.value().datataype->aliasNgw);
        QTreeWidgetItem *item = new QTreeWidgetItem(strs);
        item->setData(0,Qt::UserRole,it.value().display_name); // display name can vary
                                                       // from the keyname, but in this
                                                       // program we can't change it
        tree->addTopLevelItem(item);
        ++it;
    }
}


QMap<QString,FBField> FBDialogFieldsManager::getFields ()
{
    QMap<QString,FBField> map;
    for (int i=0; i<tree->topLevelItemCount(); i++)
    {
        FBField field;
        field.datataype = FBProject::findDataTypeByNgw(
                    tree->topLevelItem(i)->text(1));
        field.display_name = tree->topLevelItem(i)->data(0,Qt::UserRole).toString();
        map.insert(tree->topLevelItem(i)->text(0),field);
    }
    return map;
}


void FBDialogFieldsManager::onItemClick (QTreeWidgetItem *item, int itemCol)
{
    editName->setText(item->text(0));
    comboType->setCurrentText(item->text(1));
    butRemove->setEnabled(true);
}


void FBDialogFieldsManager::onAddClick ()
{
    QString newName = editName->text();
    QString newType = comboType->currentText();

    if (newName == "")
    {
        this->onShowMsgBox(tr("Enter field's keyname!"), QMessageBox::Warning);
        return;
    }

    //

    // Change unsupported symbols. The list is from here:
    // https://github.com/nextgis/android_maplib/blob/master/src/main/java/com/
    // nextgis/maplib/util/LayerUtil.java#L25
    QStringList forbiddenChars;
    forbiddenChars<<"@"<<"#"<<"%"<<"^"<<"&"<<"*"<<"!"<<"$"<<"("<<")";
    // Also added/removed own unsupported symbols.
    // TODO: Change this if unneeded.
    forbiddenChars<<" ";
    bool wasRepl = false;
    for(int i=0; i<forbiddenChars.size(); i++)
    {
        if (newName.contains(forbiddenChars[i]))
            wasRepl = true;
        newName.replace(forbiddenChars[i],"_");
    }

    //

    // IMPORTANT TODO: we need to support only these symbols in fields names:
    //"abcdefghijklmnopqrstuvwxyz1234567890_"

/*
    QRegExp expr("[A-Z][a-z][0-9]\_]");
    QRegExpValidator v(expr);
    for (int i=0; i<newName.size(); i++)
    {
        int n;
        if (v.validate(QString(newName[i]),n) != QValidator::Acceptable)
        {
            this->onShowMsgBox(tr("Entered keyname contains unsupported symbols!"),
                               QMessageBox::Critical);
            return;
        }
    }
*/

    // See if we already have such keyname.
    for (int i=0; i<tree->topLevelItemCount(); i++)
    {
        // For NextGIS Mobile all letters in the field names will be converted
        // to low-case. We check names case-insensitively here so there will be
        // no several equal keynames (e.g. "ID" and "Id" will be converted to "id"
        // both).
        if (QString::compare(tree->topLevelItem(i)->text(0), newName,
                             Qt::CaseInsensitive) == 0)
        {
            this->onShowMsgBox(tr("There is already a field with this keyname. Select"
                                  " another keyname!"), QMessageBox::Critical);
            return;
        }
    }

    if (wasRepl)
    {
        this->onShowMsgBox(tr("Unsupported symbols in the field's keyname have"
                " been replaced with the symbol \'_\'"), QMessageBox::Information);
    }

    QStringList strs;
    strs.append(newName);
    strs.append(newType);
    QTreeWidgetItem *item = new QTreeWidgetItem(strs);
    item->setData(0,Qt::UserRole,newName); // set display_name = keyname
    tree->addTopLevelItem(item);

    tree->clearSelection();
    butRemove->setEnabled(false);
}


void FBDialogFieldsManager::onRemoveClick ()
{
    if (tree->currentItem() == NULL)
        return;

    if (tree->topLevelItemCount() == 1)
    {
        this->onShowMsgBox(tr("There must be at least one field in the layer!"),
                   QMessageBox::Critical);
        return;
    }

    int ret = this->onShowAlertBox(tr("Deletion of field will cause deletion of all"
            " its according data (at the moment of saving project). Delete field?"),
            QMessageBox::Warning);
    if (ret == QMessageBox::Ok)
    {
        QString oldName = tree->currentItem()->text(0);
        QTreeWidgetItem *curItem = tree->takeTopLevelItem(
                    tree->indexOfTopLevelItem(tree->currentItem()));
        delete curItem;

        // Save keyname of deleted field.
        // Note: keynames of fields are unique.
        // If we create the new field with the samekey name after this deletion, it will
        // not cause any problems: we will anyway delete old field with data, and the
        // new field will be created automatically as others.
        fieldsDeleted.insert(oldName);

        tree->clearSelection();
        butRemove->setEnabled(false);
    }
}


void FBDialogFieldsManager::onShowMsgBox (QString msg, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Info"));
    msgBox.setIcon(icon);
    msgBox.exec();
}
int FBDialogFieldsManager::onShowAlertBox (QString msg, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setIcon(icon);
    return msgBox.exec();
}
