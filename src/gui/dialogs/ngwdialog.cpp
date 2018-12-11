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

#include "ngwdialog.h"
#include "ui_ngwdialog.h"

#include "util/settings.h"
#include "gui/dialogs/layerdialog.h"
#include "gui/gui_common.h"

#include <QLabel>
#include <QDebug>

using namespace Fb;
using namespace Gui;

using namespace Util;

bool NgwDialog::was_success_connection = false;

NgwDialog::NgwDialog (QWidget *parent, std::set<NgwResourceType> allowed_types):
    CustomDialog(parent),
    ui(new Ui::NgwDialog),
    allowed_types(allowed_types),
    is_gui_blocked(false)
{
    ui->setupUi(this);

    all_allowed_types = allowed_types; // copy
    all_allowed_types.insert(NgwResourceType::ResourceGroup); // always show folders

    int w = g_getSettings()->value(FB_STS_NGWDLG_W, 0).toInt();
    int h = g_getSettings()->value(FB_STS_NGWDLG_H, 0).toInt();
    if (w != 0 && h != 0)
        this->resize({w, h});

    url_compl = new QCompleter(this);
    url_compl_model = new QStringListModel();
    url_compl->setCaseSensitivity(Qt::CaseInsensitive);
    url_compl->setModel(url_compl_model);

    QString old_url = g_getSettings()->value(FB_STS_NGW_URL, "").toString();
    ui->edit_url->setText(old_url);
    connect(ui->edit_url, &QLineEdit::textChanged, this, &NgwDialog::onUrlTextChanged);
    ui->edit_url->setCompleter(url_compl);

    bool was_guest = g_getSettings()->value(FB_STS_NGW_GUEST, false).toBool();
    connect(ui->checkb_guest, &QCheckBox::stateChanged, [this](int state)
    {
        ui->edit_login->setEnabled(!state);
        ui->edit_password->setEnabled(!state);
    });
    ui->checkb_guest->setChecked(was_guest);

    QString old_login = g_getSettings()->value(FB_STS_NGW_LOGIN, "").toString();
    QString old_password = g_getSettings()->value(FB_STS_NGW_PASSWORD, "").toString();
    ui->edit_login->setText(old_login);
    ui->edit_password->setText(old_password);

    connect(ui->but_connect, &QPushButton::clicked, this, &NgwDialog::onConnectClicked);
    ui->but_connect->setFocusPolicy(Qt::NoFocus); // so not to connect accidentally

    connect(ui->tree_reses, &QTreeWidget::itemExpanded, this, &NgwDialog::onTreeItemExpanded);
    connect(ui->tree_reses, &QTreeWidget::itemClicked, this, &NgwDialog::onTreeItemClicked);
    connect(ui->tree_reses, &QTreeWidget::itemDoubleClicked, this, &NgwDialog::onTreeItemDblClicked);
    connect(ui->tree_reses, &QTreeWidget::itemCollapsed, this, &NgwDialog::onTreeItemCollapsed);
    ui->tree_reses->setHeaderLabels({tr("Resource"), tr("Type")});
//    ui->tree_reses->setEnabled(false);
    lay_spinner = new QVBoxLayout(ui->tree_reses);
    movie_spinner = new QMovie(":/images/network_spin.gif");

    int col_w = g_getSettings()->value(FB_STS_NGWDLG_COL0_W, 0).toInt();
    if (col_w != 0)
        ui->tree_reses->header()->resizeSection(0, col_w);

    ui->but_select->setEnabled(false);
    ui->but_select->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(ui->but_select, &QPushButton::clicked, this, &NgwDialog::accept);

    ngw_io = new NgwDefaultIo(new NgwApi());

    this->show();
    if (was_success_connection)
        this->onConnectClicked();
}


NgwDialog::~NgwDialog ()
{
    g_getSettings()->setValue(FB_STS_NGW_URL, ui->edit_url->text());
    g_getSettings()->setValue(FB_STS_NGW_GUEST, ui->checkb_guest->isChecked());
    g_getSettings()->setValue(FB_STS_NGW_LOGIN, ui->edit_login->text());
    g_getSettings()->setValue(FB_STS_NGW_PASSWORD, ui->edit_password->text());
    g_getSettings()->setValue(FB_STS_NGWDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_NGWDLG_H, this->height());
    g_getSettings()->setValue(FB_STS_NGWDLG_COL0_W, ui->tree_reses->header()->sectionSize(0));

    delete ngw_io;
    delete movie_spinner;
    delete url_compl_model;
    delete ui;
}

//QPair<QString, int> NgwDialog::getResource ()
//{
//    int id;
//    QTreeWidgetItem *item = ui->tree_reses->currentItem();
//    if (item != nullptr)
//    {
//        auto data = item->data(0, Qt::UserRole).value<NgwDialogItemData>();
//        id = data.res_id;
//    }
//    else
//    {
//        id = 0;
//    }

//    return qMakePair(cur_url, id);
//}

//QPair<QString, QString> NgwDialog::getCreds ()
//{
//    return qMakePair(cur_login, cur_password);
//}


NgwDialogResInfo NgwDialog::getSelectedResourceInfo ()
{
    NgwDialogResInfo ret;

    QTreeWidgetItem *item = ui->tree_reses->currentItem();
    if (item != nullptr)
    {
        ret.resource_id = item->data(0, Qt::UserRole).value<NgwDialogItemData>().res_id;
        ret.name = item->text(0);
    }
    else
    {
        ret.resource_id = 0;
        ret.name = tr("&lt;root&gt;");
    }

    ret.base_url = cur_url;
    ret.login = cur_login;
    ret.password = cur_password;

    return ret;
}


/*!
 * @brief Anytime user clicks Connect button we perform initial authorization and refill the tree
 * with resources.
 */
void NgwDialog::onConnectClicked ()
{
    cur_url = ui->edit_url->text();
    if (cur_url == "")
        return;

    if (!cur_url.startsWith("http://", Qt::CaseInsensitive) &&
        !cur_url.startsWith("https://", Qt::CaseInsensitive))
        cur_url.prepend("http://");

    while (cur_url.endsWith("/"), Qt::CaseInsensitive)
        cur_url.chop(1);

    cur_login = "";
    cur_password = "";
    if (!ui->checkb_guest->isChecked())
    {
        cur_login = ui->edit_login->text();
        cur_password = ui->edit_password->text();
    }

//    this->hideTreeDecor();
//    this->showTreeSpinner();
//    ui->tree_reses->setEnabled(false);
//    ui->but_connect->setEnabled(false);
//    ui->but_add->setEnabled(false);
//    ui->but_select->setEnabled(false);
    ui->tree_reses->clear();

    ngw_io->reset(); // this should also clear old authentication data

    this->hideTreeDecor(); // hide last message if was
    this->blockGuiOnConnect();

    QList<NgwResourceData> resources;
    bool ok = ngw_io->getRootResources(resources, cur_url, cur_login, cur_password,
                                       all_allowed_types);
    this->unblockGuiOnConnect();

    if (!ok)
    {
        qDebug() << ngw_io->getLastError();
        this->showTreeMessage(tr("Connection failed"));
        was_success_connection = false;
    }
    else if (resources.isEmpty())
    {
        this->showTreeMessage(tr("No resources in this web GIS"));
        was_success_connection = true;
    }
    else
    {
        this->updateTree(resources);
        was_success_connection = true;
    }

//    this->hideTreeDecor();
//    ui->tree_reses->setEnabled(true);
//    ui->but_connect->setEnabled(true);
//    ui->but_add->setEnabled(true);
}


/*
void NgwDialog::onAddClicked ()
{

    if (cur_layer == nullptr)
        return;

    QTreeWidgetItem *item = nullptr;
    NgwDialogItemData item_data;
    QTreeWidgetItem *parent_item = nullptr;

    int selected_res_group_id = 0;
    QString res_group_name = tr("root");
    item = ui->tree_reses->currentItem();
    if (item != nullptr)
    {
        item_data = item->data(0, Qt::UserRole).value<NgwDialogItemData>();
        if (item_data.res_type == NgwResourceType::ResourceGroup)
        {
            selected_res_group_id = item_data.res_id;
            res_group_name = item->text(0);
        }
        else
        {
            parent_item = item->parent();
            if (parent_item != nullptr) // then this is definitely a resource group
            {
                auto parent_item_data = parent_item->data(0, Qt::UserRole).value<NgwDialogItemData>();
                selected_res_group_id = parent_item_data.res_id;
                res_group_name = parent_item->text(0);
            }
        }
    }

    QString res_group_url = ngw_io->getNgwApi()->requestResourceView(cur_url,selected_res_group_id);

    LayerDialog dialog(this, cur_layer, true, res_group_url, res_group_name);
    dialog.setWindowTitle(tr("Create new layer"));
    dialog.setOkButtonText(tr("Create"));

    if (!dialog.exec())
        return;

//    this->showTreeSpinner();
//    ui->tree_reses->setEnabled(false);
//    ui->but_connect->setEnabled(false);

    QString new_name;
    Core::GeomType new_geom_type;
    QList<QPair<QString, Core::FieldType>> new_fields_data;

    dialog.getLayerMeta(new_name, new_geom_type);
    dialog.getFields(new_fields_data);

    NgwLayerInfo layer_info;
    layer_info.name = new_name;
    layer_info.geom_type = Core::g_getGeomTypeNgwName(new_geom_type);
    for (auto &field_data: new_fields_data)
        layer_info.fields.append({field_data.first, Core::g_getFieldTypeNgwName(field_data.second)});

    this->blockGuiOnConnect();

    int new_layer_id;
    bool ok = ngw_io->createLayer(new_layer_id, layer_info, cur_url, selected_res_group_id);

    this->unblockGuiOnConnect();

    if (!ok)
    {
        g_showWarningDet(this, tr("Unable to create NextGIS Web layer"),
                         ngw_io->getLastError());
    }
    else
    {
        new_layer_ids.insert(new_layer_id);

        if (parent_item != nullptr)
        {
            ui->tree_reses->collapseItem(parent_item);
            ui->tree_reses->expandItem(parent_item);
        }
        else if (item != nullptr && item_data.res_type == NgwResourceType::ResourceGroup)
        {
            ui->tree_reses->collapseItem(item);
            ui->tree_reses->expandItem(item);
        }
        else
        {
            this->onConnectClicked();
        }
    }

//    this->hideTreeDecor();
//    ui->tree_reses->setEnabled(true);
//    ui->but_connect->setEnabled(true);
}
*/


/*!
 * @brief Load and add resources for the selected resource.
 */
void NgwDialog::onTreeItemExpanded (QTreeWidgetItem *item)
{
//    this->showTreeSpinner();
//    ui->tree_reses->setEnabled(false);
//    ui->but_connect->setEnabled(false);
    this->blockGuiOnConnect();

    auto data = item->data(0, Qt::UserRole).value<NgwDialogItemData>();

    QList<NgwResourceData> resources;
    bool ok = ngw_io->getResources(resources, cur_url, data.res_id, all_allowed_types);
    if (ok)
    {
        this->updateTree(resources, item);
    }
    else
    {
        qDebug() << ngw_io->getLastError();
    }

//    this->hideTreeDecor();
//    ui->tree_reses->setEnabled(true);
//    ui->but_connect->setEnabled(true);
    this->unblockGuiOnConnect();
}

/*!
 * @brief Determine if we can allow user to select this resource.
 */
void NgwDialog::onTreeItemClicked (QTreeWidgetItem *item)
{
    auto data = item->data(0, Qt::UserRole).value<NgwDialogItemData>();
    if (allowed_types.find(data.res_type) != allowed_types.end())
    {
        ui->but_select->setEnabled(true);
        return;
    }
    ui->but_select->setEnabled(false);
}

/*!
 * @brief Select resource by double click (only if folders are not the target of selection).
 */
void NgwDialog::onTreeItemDblClicked (QTreeWidgetItem *item)
{
    auto data = item->data(0, Qt::UserRole).value<NgwDialogItemData>();
    if (data.res_type != NgwResourceType::ResourceGroup &&
        allowed_types.find(data.res_type) != allowed_types.end())
        this->accept();
}

/*!
 * @brief Just cansel resource selection.
 */
void NgwDialog::onTreeItemCollapsed (QTreeWidgetItem *item)
{
    ui->but_select->setEnabled(false);
}


void NgwDialog::onUrlTextChanged (QString text)
{
    QStringList list;

    if (!text.contains("."))
        list.append(QString("%1.nextgis.com").arg(text));

    url_compl_model->setStringList(list);
}


void NgwDialog::blockGuiOnConnect ()
{
    if (is_gui_blocked)
        return;

    this->showTreeSpinner();
    ui->tree_reses->setEnabled(false);
    ui->but_connect->setEnabled(false);

    is_gui_blocked = true;
}

void NgwDialog::unblockGuiOnConnect ()
{
    if (!is_gui_blocked)
        return;

    this->hideTreeDecor();
    ui->tree_reses->setEnabled(true);
    ui->but_connect->setEnabled(true);

    is_gui_blocked = false;
}

void NgwDialog::showTreeSpinner ()
{
//    if (is_tree_decor_shown)
//        return;
//    is_tree_decor_shown = true;

    QLabel *lab_spinner = new QLabel(ui->tree_reses);
    lab_spinner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lab_spinner->setAlignment(Qt::AlignCenter);
    lab_spinner->setMovie(movie_spinner);

    lay_spinner->addWidget(lab_spinner);

    movie_spinner->start();
}

void NgwDialog::showTreeMessage (QString text)
{
//    if (is_tree_decor_shown)
//        return;
//    is_tree_decor_shown = true;

    QLabel *lab_msg = new QLabel(ui->tree_reses);
    lab_msg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lab_msg->setAlignment(Qt::AlignCenter);
    lab_msg->setText(text);
    lab_msg->setWordWrap(true);
    lab_msg->setAutoFillBackground(false);

    lay_spinner->addWidget(lab_msg);
}

void NgwDialog::hideTreeDecor ()
{
//    if (!is_tree_decor_shown)
//        return;
//    is_tree_decor_shown = false;

    movie_spinner->stop();

    auto item = lay_spinner->takeAt(0);
    if (item == nullptr)
        return;
    QWidget *lab = item->widget();
    if (lab != nullptr)
        delete lab;
}


void NgwDialog::updateTree (const QList<Util::NgwResourceData> &resources)
{
    ui->tree_reses->clear();

    QList<QTreeWidgetItem*> items;
    this->makeItems(resources, items);

    // Allow to select root resource group if folders could be selected: add root folder.
    if (allowed_types.find(NgwResourceType::ResourceGroup) != allowed_types.end())
    {
        Util::NgwResourceData root_data = {0, NgwResourceType::ResourceGroup, "<root>", true};
        QTreeWidgetItem* root_item = this->makeItem(root_data);

        ui->tree_reses->addTopLevelItem(root_item);
        root_item->insertChildren(0, items);
    }

    // Add items to the tree directly.
    else
    {
        ui->tree_reses->addTopLevelItems(items);
    }

    ui->tree_reses->sortByColumn(0, Qt::AscendingOrder); // sort by name
    ui->tree_reses->sortByColumn(1, Qt::AscendingOrder); // folders on top
}

void NgwDialog::updateTree (const QList<Util::NgwResourceData> &resources, QTreeWidgetItem *item)
{
    QList<QTreeWidgetItem*> items;
    this->makeItems(resources, items);

    auto old_items = item->takeChildren();
    for (auto old_item: old_items)
        delete old_item;

    item->insertChildren(0, items);
    item->sortChildren(0, Qt::AscendingOrder); // sort by name
    item->sortChildren(1, Qt::AscendingOrder); // folders on top
}

void NgwDialog::makeItems (const QList<Util::NgwResourceData> &resources,
                           QList<QTreeWidgetItem*> &items)
{
    items.clear();
    for (auto &res_data: resources)
    {
        QTreeWidgetItem *item = this->makeItem(res_data);
        items.append(item);
    }
}

QTreeWidgetItem *NgwDialog::makeItem (Util::NgwResourceData res_data)
{
    NgwDialogItemData data = {res_data.id, res_data.cls};

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, res_data.display_name);
    item->setText(1, g_getNgwResTypeDispName(res_data.cls));
    item->setTextColor(1, {150, 150, 150});
    item->setData(0, Qt::UserRole, QVariant::fromValue(data));
    item->setIcon(0, QIcon(g_getNgwResTypeIconPath(res_data.cls)));

    if (new_layer_ids.contains(res_data.id))
    {
        QFont font(item->font(0));
        font.setBold(true);
        item->setFont(0, font);
    }

    if (res_data.cls == NgwResourceType::ResourceGroup &&
        res_data.children == true)
    {
        item->addChild(new QTreeWidgetItem()); // add fake item to show expand arrow
    }

    return item;
}



