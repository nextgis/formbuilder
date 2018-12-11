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

#pragma once

#include "gui/custom_dialog.h"
#include "core/layer.h"
#include "util/ngw_default_io.h"

#include <QCompleter>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QMovie>

namespace Ui { class NgwDialog; }

namespace Fb
{
namespace Gui
{


struct NgwDialogItemData
{
    int res_id;
    Util::NgwResourceType res_type;
};

struct NgwDialogResInfo
{
    QString base_url;
    int resource_id;
    QString name;
    QString login;
    QString password;
};


/*!
 * @brief This dialog allows to select resources of specific types from NextGIS Web web GIS.
 */
class NgwDialog: public CustomDialog
{
    Q_OBJECT

    public:

     explicit NgwDialog (QWidget *parent, std::set<Util::NgwResourceType> allowed_types);
     virtual ~NgwDialog ();

//     QPair<QString, int> getResource ();
//     QPair<QString, QString> getCreds ();
     NgwDialogResInfo getSelectedResourceInfo ();

     Util::NgwDefaultIo *getNgwIo () const { return ngw_io; }

    protected slots:

     void onConnectClicked ();

//     void onAddClicked ();

     void onTreeItemExpanded (QTreeWidgetItem *item);
     void onTreeItemClicked (QTreeWidgetItem *item);
     void onTreeItemDblClicked (QTreeWidgetItem *item);
     void onTreeItemCollapsed (QTreeWidgetItem *item);

    protected:

     void onUrlTextChanged (QString text);

     void blockGuiOnConnect ();
     void unblockGuiOnConnect ();
     void showTreeSpinner ();
     void showTreeMessage (QString text);
     void hideTreeDecor ();

     void updateTree (const QList<Util::NgwResourceData> &resources);
     void updateTree (const QList<Util::NgwResourceData> &resources, QTreeWidgetItem *item);
     void makeItems (const QList<Util::NgwResourceData> &resources,
                     QList<QTreeWidgetItem *> &items);
     QTreeWidgetItem *makeItem (Util::NgwResourceData res_data);

     std::set<Util::NgwResourceType> allowed_types;
     std::set<Util::NgwResourceType> all_allowed_types;

     QString cur_url;
     QString cur_login;
     QString cur_password;

     QCompleter *url_compl;
     QStringListModel *url_compl_model;

     QVBoxLayout *lay_spinner;
     QMovie *movie_spinner;
     bool is_gui_blocked;

     Util::NgwDefaultIo *ngw_io;

     QSet<int> new_layer_ids;

     static bool was_success_connection;

    private:

     Ui::NgwDialog *ui;
};


}
}

Q_DECLARE_METATYPE(Fb::Gui::NgwDialogItemData)



