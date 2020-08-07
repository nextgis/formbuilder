/**************************************************************************************************
 *                                                                                                *
 *    Project:  NextGIS Formbuilder                                                               *
 *    Authors:  Mikhail Gusev, gusevmihs@gmail.com                                                *
 *              Copyright (C) 2014-2019 NextGIS                                                   *
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

#include "fb_project.h"
#include "fb_updater.h"
#include "languages.h"
#include "gui/mainwindow.h"
#include "gui/elemtreewidget.h"
#include "gui/fieldstablewidget2.h"
#include "gui/propstablewidget.h"
#include "gui/custom_scrollarea.h"
#include "mockup/device.h"
#include "util/ngw_gdal_io.h"

#include "framework/access/access.h"
#include "framework/access/signbutton.h"

using namespace Fb;
using namespace Gui;


/*!
 * \brief A window for NextGIS Formbuilder 2.X. A Screen's top-level ElemView is always NgmFormView.
 */
class FbWindow final: public MainWindow
{
    Q_OBJECT

    public:

     explicit FbWindow (Language last_language);
     ~FbWindow ();

     void keyPressEvent (QKeyEvent *event) override;
     void keyReleaseEvent (QKeyEvent *event) override;
     void closeEvent (QCloseEvent *event) override;

    private slots:

     void onNewVoidClicked ();
     void onOpenClicked ();
     void onSaveClicked ();
     void onSaveAsClicked ();
     void onExitClicked ();
     void onDownloadFromNgw ();
     void onUploadToNgw ();
     void onEditFormPropsClicked ();
     void onClearScreenClicked ();
     void onEditPrefixesClicked ();
     void onEditTranslationsClicked ();
     void onChainElemsClicked ();
     void onLanguageClicked (QAction *action);
     void onViewHelpClicked ();
     void onCommSupportClicked ();
     void onAboutClicked ();

     void onSupportInfoUpdated ();
     void onUpdatesAvailClicked ();
     void onCheckUpdatesFinished (bool updates_avail);

     void onElemTreeItemPressed ();
     void onElemTreeItemMoved (QPoint coords);
     void onElemTreeItemReleased ();

     void onElemTreeItemCtrlClicked ();

     void onAfterElemViewAdded (Mockup::ElemView *elemview);
     void onAfterElemViewSelected (Mockup::ElemView *elemview);
     void onAfterElemViewChanged (Mockup::ElemView *elemview);
     void onAfterElemViewMoved (Mockup::ElemView *elemview);
     void onAfterElemViewDeleted ();

     void onWorkingAreaPressed ();

     void onFieldSelected (int field_index, const Core::Elem *elem, QString field_slot);

     void onFieldAliasChangedInTable (const QString &text);
     void onFieldTypeChangedInTable (int index);

    private:

      QMenu *createPopupMenu () override;

      template <typename Slot>
      QAction *addMainMenuAction (QMenu *menu, QToolBar *toolb, Slot slot,
                                  QString icon, QString name, QString tip);

      void u_addElem (QString elem_key_name);
      void u_addElemToTheEnd (QString elem_key_name);
      void u_removeElem ();
      void u_openNgfp (QString file_path);
      bool u_saveNgfp (QString file_path);
      bool u_getNgwLayer (QPair<QString, int> &res_data, QPair<QString, QString> creds,
                          Fb::Util::NgwGdalIo *ngw_io);
      void u_updateLayerFields (const QList<FieldInfo> &new_fields_info);
      void u_recreateDevice ();
      void u_recreateForm ();
      void u_resetProject ();
//      bool u_isLayerEditable ();
      void u_createFieldsForNewElem (Mockup::ElemView *elemview);
      int u_showLayerMetaDialog (bool is_editable, QString res_group_url, QString res_group_name, QString but_ok_text);
      bool u_initialFieldStructureWasChanged ();
      void u_updateSupportedIcons (bool is_supported);
      bool u_canUseSupportedFeature ();
      bool u_okToReset ();
      void u_updateTitle ();
      void u_updateCounterLists (const QList<QStringList> &lists);
      QStringList u_getStringsToTranslate ();

      void writeToLog (QString str);
      void sendToSentry (QString str, NGAccess::LogLevel log_level);

     QMenu *menu_file;
     QMenu *menu_edit;
     QMenu *menu_view;
     QMenu *menu_settings;
     QMenu *menu_help;

     QMenu *menu_toolbs;

     QToolBar *toolb_project;
     QToolBar *toolb_ngw;
     QToolBar *toolb_form;
     QToolBar *toolb_elems;

     NGSignInButton *but_ngauth;
     //QToolButton *but_updates;
     QAction *act_updates;

     QAction *act_new_void;
     QAction *act_open;
     QAction *act_save;
     QAction *act_save_as;
     QAction *act_exit;
     QAction *act_download_from_ngw;
     QAction *act_upload_to_ngw;
     QAction *act_form_props;
     QAction *act_clear_screen;
     QAction *act_edit_prefixes;
     QAction *act_edit_translations;
     QAction *act_chain_elems;
     QAction *act_view_help;
     QAction *act_comm_supp;
     QAction *act_about;

     QDockWidget *dock_elems;
     QDockWidget *dock_fields;
     QDockWidget *dock_props;

     ElemTreeWidget *tree_elems;
     FieldsTableWidget2 *table_fields;
     PropsTableWidget *table_props;
     CustomScrollArea *scr_working;

     Mockup::Device *cur_device;
     QScopedPointer<FbProject> cur_project;

     bool need_to_save;

     bool chain_elems;

     Language last_language;

//     QSet<FieldInfo> initial_fields_info;

     FbUpdater *fb_updater;

     QStringList tr_language_keys;
     QMap<QString, QStringList> tr_values;
};



