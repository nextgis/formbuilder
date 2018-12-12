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

#include "fb_window.h"

#include "fb_aboutdialog.h"
#include "gui/gui_common.h"
#include "util/common.h"
#include "gui/dialogs/newvoidprojectdialog.h"
#include "gui/dialogs/ngwdialog.h"
#include "gui/dialogs/layerdialog2.h"
#include "mockup/elemview_registrar.h"
#include "serializer/ngfp_reader.h"
#include "serializer/ngfp_writer.h"
#include "util/settings.h"
#include "gui/sizes.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QDockWidget>
#include <QStatusBar>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QTemporaryDir>

#define FB_NGTOOLB_TEXT QObject::tr("Authentication & Updates")

using namespace Fb;
using namespace Core;
using namespace Gui;
using namespace Mockup;
using namespace Util;
using namespace Serializer;


FbWindow::FbWindow (Language last_language):
    MainWindow(),
    need_to_save(false), // a void project does not need to be saved
    cur_device(nullptr),
    last_language(last_language)
{
    this->resize(1024, 768);

    this->setColorTheme(ColorTheme::White);

    this->setWindowIcon(QIcon(":/data/ngfb.ico"));

    // MAIN menu.

    menu_file = this->addMainMenuEntry(tr("File"));
    menu_edit = this->addMainMenuEntry(tr("Edit"));
    menu_view = this->addMainMenuEntry(tr("View"));
    menu_settings = this->addMainMenuEntry(tr("Settings"));
    menu_help = this->addMainMenuEntry(tr("Help"));

    toolb_project = this->addMainMenuToolBar(tr("Project"));
    toolb_ngw = this->addMainMenuToolBar(tr("NextGIS Web"));
    toolb_form = this->addMainMenuToolBar(tr("Form"));

    // FILE menu.

    act_new_void = this->addMainMenuAction(
                            menu_file, toolb_project,
                            &FbWindow::onNewVoidClicked,
                            ":/images/theme_white/new_from_void2.svg",
                            tr("New"),
                            tr("Create new project"));

    act_open = this->addMainMenuAction(
                            menu_file, toolb_project,
                            &FbWindow::onOpenClicked,
                            ":/images/theme_white/open2.svg",
                            tr("Open"),
                            tr("Open project"));

    act_save = this->addMainMenuAction(
                            menu_file, toolb_project,
                            &FbWindow::onSaveClicked,
                            ":/images/theme_white/save2.svg",
                            tr("Save"),
                            tr("Save project"));

    act_save_as = this->addMainMenuAction(
                            menu_file, nullptr,
                            &FbWindow::onSaveAsClicked,
                            "",
                            tr("Save As..."),
                            tr("Save project as..."));

    menu_file->addSeparator();

    act_exit = this->addMainMenuAction(
                            menu_file, nullptr,
                            &FbWindow::onExitClicked,
                            "",
                            tr("Exit"),
                            tr("Exit application"));

    // EDIT menu.

    act_create_ngw_form = this->addMainMenuAction(
                            menu_edit, toolb_ngw,
                            &FbWindow::onCreateNgwForm,
                            ":/images/theme_white/ngw_create2.svg",
                            tr("Upload to NextGIS Web"),
                            tr("Upload form to NextGIS Web with creating new layer"));

    menu_edit->addSeparator();

    act_form_props = this->addMainMenuAction(
                            menu_edit, toolb_form,
                            &FbWindow::onEditFormPropsClicked,
                            ":/images/theme_white/form_props.svg",
                            tr("Form Properties"),
                            tr("Edit general form properties"));

    act_clear_screen = this->addMainMenuAction(
                            menu_edit, toolb_form,
                            &FbWindow::onClearScreenClicked,
                            ":/images/theme_white/clear_screen2.svg",
                            tr("Clear Form"),
                            tr("Clear form from all elements"));

    // VIEW menu.

    // Note: "Toolbars" menu will be added later in constructor.

    // SETTINGS menu.

    QActionGroup *actgr_langs = new QActionGroup(this);
    QMenu *menu_language = menu_settings->addMenu(tr("Language"));
    for (auto &language: LANGUAGES.keys())
    {
        QAction *act_lang = new QAction(LANGUAGES.value(language).display_name, this);
        act_lang->setData(QVariant::fromValue(language));
        menu_language->addAction(act_lang);
        act_lang->setActionGroup(actgr_langs);
        act_lang->setCheckable(true);
        if (language == last_language)
            act_lang->setChecked(true);
    }
    connect(actgr_langs, &QActionGroup::triggered, this, &FbWindow::onLanguageClicked);

    // HELP menu.

    act_view_help = this->addMainMenuAction(
                            menu_help, nullptr,
                            &FbWindow::onViewHelpClicked,
                            "",
                            tr("View Help"),
                            tr("View online help"));

    act_comm_supp = this->addMainMenuAction(
                            menu_help, nullptr,
                            &FbWindow::onCommSupportClicked,
                            "",
                            tr("Need Commercial Support?"),
                            tr("Need commercial support?"));

    act_about = this->addMainMenuAction(
                            menu_help, nullptr,
                            &FbWindow::onAboutClicked,
                            "",
                            tr("About"),
                            tr("About program"));

    // Elements dock menu.
    dock_elems = this->addDockMenu(tr("Elements"), Qt::LeftDockWidgetArea);
    tree_elems = new ElemTreeWidget(dock_elems, g_getColorThemeIconsDir(cur_color_theme));
    connect(tree_elems, &ElemTreeWidget::elemItemPressed,
            this, &FbWindow::onElemTreeItemPressed);
    connect(tree_elems, &ElemTreeWidget::elemItemMoved,
            this, &FbWindow::onElemTreeItemMoved);
    connect(tree_elems, &ElemTreeWidget::elemItemReleased,
            this, &FbWindow::onElemTreeItemReleased);
    connect(tree_elems, &ElemTreeWidget::elemItemCtrlClicked,
            this, &FbWindow::onElemTreeItemCtrlClicked);
    dock_elems->setWidget(tree_elems);
    dock_elems->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable);

    // Fields dock menu.
    dock_fields = this->addDockMenu(tr("Data"), Qt::RightDockWidgetArea);
    table_fields = new FieldsTableWidget2(dock_fields);
    dock_fields->setWidget(table_fields);
    dock_fields->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable);
    connect(table_fields, &FieldsTableWidget2::fieldAliasChanged,
            this, &FbWindow::onFieldAliasChangedInTable);
    connect(table_fields, &FieldsTableWidget2::fieldTypeChanged,
            this, &FbWindow::onFieldTypeChangedInTable);

    // Properties dock menu.
    dock_props = this->addDockMenu(tr("Properties"), Qt::RightDockWidgetArea);
    table_props = new PropsTableWidget(dock_props, this);
    connect(table_props, &PropsTableWidget::fieldSelected,
            this, &FbWindow::onFieldSelected);
    dock_props->setWidget(table_props);
    dock_props->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable);

    // Working area.
    scr_working = new CustomScrollArea(this);
    scr_working->setAlignment(Qt::AlignCenter);
    scr_working->setAutoFillBackground(false);
    scr_working->setWidgetResizable(false); // the device will be resized by its screen widget
    connect(scr_working, &CustomScrollArea::pressed,
            this, &FbWindow::onWorkingAreaPressed);
    this->setCentralWidget(scr_working);

    // Add toolbars menu after the dock widgets so to include them into that menu.
    menu_toolbs = this->createPopupMenu();
    menu_toolbs->setTitle(tr("Toolbars"));
    menu_view->addMenu(menu_toolbs);

    // NextGIS Authentication button.
    but_ngauth = new NGSignInButton(QString("40ONLYJYYQFLBD6btOpQnJNO9DHfuejUt4FPSUJ3"));
    but_ngauth->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    but_ngauth->setCursor(Qt::PointingHandCursor);
    but_ngauth->setToolTip(tr("Sign in to your account"));
    connect(but_ngauth, &NGSignInButton::supportInfoUpdated, this, &FbWindow::onSupportInfoUpdated);

    // NextGIS Updates button.
    but_updates = new QToolButton();
    but_updates->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    but_updates->setCursor(Qt::PointingHandCursor);
    but_updates->setToolTip(tr("Updates available!"));
    but_updates->setIcon(QIcon(":/images/theme_white/updates_available.svg"));
    but_updates->hide();
    connect(but_updates, &QToolButton::clicked, this, &FbWindow::onUpdatesAvailClicked);

    // Unclosable toolbar.
    QWidget *w_stretch_unclose = new QWidget();
    w_stretch_unclose->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QToolBar *toolbar_unclose = this->addToolBar(FB_NGTOOLB_TEXT);
    toolbar_unclose->setIconSize({TOOLB_SIZE, TOOLB_SIZE});
    toolbar_unclose->setFloatable(false);
    toolbar_unclose->setMovable(false);
    toolbar_unclose->addWidget(w_stretch_unclose);
    toolbar_unclose->addWidget(but_updates);
    toolbar_unclose->addWidget(but_ngauth);
    this->addToolBar(toolbar_unclose);

    // Create updater.
    fb_updater = new FbUpdater(this);
    connect(fb_updater, &FbUpdater::checkUpdatesFinished, this, &FbWindow::onCheckUpdatesFinished);

    // Start checking for updates. Updates button will be shown if updates will be found.
    fb_updater->checkUpdates();

    // Status bar.
    this->setStatusBar(new QStatusBar(this));

    // Update icons.
    this->u_updateSupportedIcons(false);

    // Initially fill tree with elems.
    tree_elems->fillSelf();

    // Initialize project.
    this->u_resetProject();
}

FbWindow::~FbWindow ()
{
    g_getSettings()->setValue(FB_STS_LANGUAGE, LANGUAGES.value(last_language).code);

    NGAccess::instance().save();

    delete menu_toolbs;
}


void FbWindow::keyPressEvent (QKeyEvent *event)
{
    // DELETE KEY.
    if (event->key() == Qt::Key_Delete)
        this->u_removeElem();

    // CTRL KEY.
    if (event->key() == Qt::Key_Control)
        tree_elems->setCtrlPressed(true);

    // Other keys.
    QMainWindow::keyPressEvent(event);
}

void FbWindow::keyReleaseEvent (QKeyEvent *event)
{
    // CTRL KEY.
    if (event->key() == Qt::Key_Control)
        tree_elems->setCtrlPressed(false);

    // Other keys.
    QMainWindow::keyPressEvent(event);
}

void FbWindow::closeEvent (QCloseEvent *event)
{
    bool ok_exit = true;

    if (need_to_save)
    {
        int ret = g_showYesNoQuestion(this, tr("Do you want to save project to file before closing "
                                               "the application?"));
        if (ret == QMessageBox::Yes)
            ok_exit = this->u_saveNgfp(cur_project.data()->getFilePath());
        else if (ret == QMessageBox::No)
            ok_exit = true;
        else
            ok_exit = false;
    }

    if (ok_exit)
        event->accept();
    else
        event->ignore();
}


void FbWindow::onNewVoidClicked ()
{
    bool ok = false;
    if (need_to_save)
    {
        int ret = g_showYesNoQuestion(this, tr("Do you want to save current project before "
                                               "creating a new one?"));
        if (ret == QMessageBox::Yes)
        {
            this->u_saveNgfp(cur_project.data()->getFilePath());
            ok = true;
        }
        else if (ret == QMessageBox::No)
        {
            ok = true;
        }
    }

    if (!ok)
        return;

    this->u_resetProject();
}

void FbWindow::onOpenClicked ()
{
	/*
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix("ngfp");
    dialog.setNameFilter("*.ngfp");
    dialog.setWindowTitle(tr("Select project file"));

    if (!dialog.exec())
        return;

    QString ngfp_path = dialog.selectedFiles()[0];

    this->u_openNgfp(ngfp_path);
	*/
}

void FbWindow::onSaveClicked ()
{
    this->u_saveNgfp(cur_project.data()->getFilePath());
}

void FbWindow::onSaveAsClicked ()
{
    this->u_saveNgfp(""); // force void path
}

void FbWindow::onExitClicked ()
{
    this->close(); // see closeEvent()
}

void FbWindow::onEditFormPropsClicked ()
{
    this->u_showLayerMetaDialog(
                cur_project.data()->layer0_get()->getNgwUrl() == "" ? true : false,
                "", "", tr("OK"));
}


void FbWindow::onCreateNgwForm ()
{
    if (!this->u_canUseSupportedFeature())
        return;

    // Prepare: Collect information about new layer.

    std::set<NgwResourceType> allowed_types;
    allowed_types.insert(NgwResourceType::ResourceGroup); // allow to pick only "resource groups"

    NgwDialog dialog(this, allowed_types);
    dialog.setWindowTitle(tr("Select resource group"));

    if (!dialog.exec())
        return;

    auto res_info = dialog.getSelectedResourceInfo();

    NgwDefaultIo *ngw_io = dialog.getNgwIo();

    QString res_group_url = ngw_io->getNgwApi()->urlResourcePage(res_info.base_url, res_info.resource_id);
    QString res_group_name = res_info.name;

    if (!this->u_showLayerMetaDialog(true, res_group_url, res_group_name, tr("Create")))
        return;

    const Layer *layer = cur_project.data()->layer0_get();

    NgwLayerInfo layer_info;
    layer_info.name = layer->getName();
    layer_info.geom_type = g_getGeomTypeNgwName(layer->getGeomType());
    for (auto field: layer->getFields())
        layer_info.fields.append({field->getName(), field->getAlias(),
                                  g_getFieldTypeNgwName(field->getType())});

    // Step 1 of 3: Create new layer.

    this->setEnabled(false);

    int new_layer_id;
    bool ok = ngw_io->createLayer(new_layer_id, layer_info, res_info.base_url, res_info.resource_id);
    if (!ok)
    {
        g_showWarningDet(this, tr("Unable to create NextGIS Web layer"),
                         ngw_io->getLastError());
        this->setEnabled(true);
        return;
    }

    auto pair = qMakePair(res_info.base_url, new_layer_id);
    if (!this->u_getNgwLayer(pair, {res_info.login, res_info.password}, ngw_io))
    {
        g_showWarningDet(this, tr("Unable to read newly created NextGIS Web layer"),
                         ngw_io->getLastError());
        this->setEnabled(true);
        return;
    }

    // Step 2 of 3: Save form to file and read its binary contents.

    QTemporaryDir temp_dir;
    if (!temp_dir.isValid())
    {
        g_showWarning(this, tr("Unable to create temp directory for saving file"));
        this->setEnabled(true);
        return;
    }
    QString file_path = temp_dir.path() + "/form.ngfp";

    try
    {
        NgfpWriter::saveNgfp(file_path, cur_project.data()->layer0_get(),
                             (NgmFormView*)cur_device->getScreen()->getTopLevelElemView());
    }
    catch (QString &msg)
    {
        g_showMessageDet(this, tr("Failed to save project"), msg);
        this->setEnabled(true);
        return;
    }

    QFile *file = new QFile(file_path);
    if (!file->open(QIODevice::ReadOnly))
    {
        g_showMessage(this, tr("Failed to save project"));
        this->setEnabled(true);
        return;
    }

    QByteArray ba_file = file->readAll(); // load all contents into memory
    file->close();

    // Step 3 of 3: Create form for the new layer.

    int new_form_id;
    if (!ngw_io->createForm(new_form_id, ba_file, res_info.base_url, new_layer_id))
    {
        g_showWarningDet(this, tr("Unable to create form on NextGIS Web"),
                         ngw_io->getLastError());
        this->setEnabled(true);
        return;
    }

    // Show success.

    g_showMessage(this, tr("Form was successfully uploaded to NextGIS Web"));
    this->setEnabled(true);

    need_to_save = true;
    this->u_updateTitle();
}

void FbWindow::onClearScreenClicked ()
{
    if (cur_device->getScreen() == nullptr)
        return;
    if (!cur_device->getScreen()->hasElemsExceptTopLevel())
        return;

    int ret = g_showQuestion(this, tr("This will delete all elements on the form. Do you really "
                                      "want to do this?"));
    if (ret == QMessageBox::Ok)
    {
        //cur_project.data()->layer0_clearAllBindings();

        this->u_recreateDevice();

        table_props->clearSelf();
        table_fields->updateSelf(cur_project.data()->layer0_get()->getFields(), nullptr);

        need_to_save = true;
        this->u_updateTitle();
    }
}

void FbWindow::onLanguageClicked (QAction *action)
{
    Language new_language = action->data().value<Language>();
    if (last_language == new_language)
        return;

    last_language = new_language;
    action->setChecked(true);

    g_showMessage(this, LANGUAGES.value(last_language).restart_message);
}

void FbWindow::onViewHelpClicked ()
{
    QString url = LANGUAGES.value(last_language).help_link;
    if (url != "")
        QDesktopServices::openUrl(url);
}

void FbWindow::onCommSupportClicked ()
{
    QString url = LANGUAGES.value(last_language).subscr_link;
    if (url == "")
        LANGUAGES.value(Language::en_GB).subscr_link;
    QDesktopServices::openUrl(url);
}

void FbWindow::onAboutClicked ()
{
    NGSignDialog *ng_dialog = (NGSignDialog*)but_ngauth->getDialog();

    FbAboutDialog dialog(this, last_language);
    dialog.setNgAccountWidget(ng_dialog);

    dialog.exec();

    dialog.removeNgAccountWidget();
    dialog.setParent(but_ngauth); // set back initial parent
}


void FbWindow::onSupportInfoUpdated ()
{
    bool is_authorized = NGAccess::instance().isUserAuthorized();
    if (is_authorized)
    {
        but_ngauth->setToolTip(tr("Your account"));
    }
    else
    {
        but_ngauth->setToolTip(tr("Sign in to your account"));
    }

    bool is_supported = NGAccess::instance().isUserSupported();
    if (!is_supported != tree_elems->getBlockUnsupported())
    {
        // 1. Change menu icons.
        this->u_updateSupportedIcons(is_supported);

        // 2. Change element icons in elem's tree.
        tree_elems->setBlockUnsupported(!is_supported);
        tree_elems->fillSelf();
    }
}


void FbWindow::onUpdatesAvailClicked ()
{
    fb_updater->startUpdate(cur_project.data()->getFilePath());

    this->close();
}

void FbWindow::onCheckUpdatesFinished (bool updates_avail)
{
    // TEMP:
    but_updates->show();
    if (updates_avail)
        but_updates->setIcon(QIcon(":/images/theme_white/updates_available.svg"));
    else
        but_updates->setIcon(QIcon(":/images/theme_white/updates_unavailable.svg"));
}


void FbWindow::onElemTreeItemPressed ()
{
    if (cur_device->getScreen() == nullptr)
        return;

    cur_device->getScreen()->startMoveNewElemView();
}

void FbWindow::onElemTreeItemMoved (QPoint coords)
{
    if (cur_device->getScreen() == nullptr)
        return;

    cur_device->getScreen()->doMoveNewElemView(coords);
}

void FbWindow::onElemTreeItemReleased ()
{
    if (cur_device->getScreen() == nullptr)
        return;

//    this->u_addElemByName(tree_elems->getElemNamePressed());
    this->u_addElem(tree_elems->getElemNamePressed());
}


void FbWindow::onElemTreeItemCtrlClicked ()
{
    if (cur_device->getScreen() == nullptr)
        return;

//    this->u_addElemByNameToTheEnd(tree_elems->getElemNamePressed());
    this->u_addElemToTheEnd(tree_elems->getElemNamePressed());
}


void FbWindow::onAfterElemViewAdded (ElemView *elemview)
{
    // Set project unsaved and update window title.
    need_to_save = true;
    this->u_updateTitle();

    // Create field(fields) and connect it(them) with new element. Note: we allow to add new fields
    // only if the layer is editable.
    this->u_createFieldsForNewElem(elemview);
}

void FbWindow::onAfterElemViewSelected (ElemView *elemview)
{
    // Fill attributes table for the selected elemview.
    table_props->fillSelf(elemview->getElem(), cur_project.data()->layer0_getFields());

    // Highlight bound fields in the table.
    //table_fields->highlightElemFields(elemview->getElem());
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), elemview->getElem());

    // Change state.
    // ...
}

void FbWindow::onAfterElemViewChanged (ElemView *elemview)
{
    // Some parameters of an elemview could have been changed so we need to alter field bindings.
    //cur_project.data()->layer0_updateAllBindings();
    cur_project.data()->layer0_resetBindingsForElem(elemview->getElem());

    // Update attributes table for the selected elemview: even a change of one attribute can lead
    // to the changes (e.g. enebleness) for other attributes.
    table_props->updateSelf(elemview->getElem(), cur_project.data()->layer0_getFields());

    // Update fields table cause bindings might have been changed.
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), elemview->getElem());

    // Set project unsaved and update window title.
    need_to_save = true;
    this->u_updateTitle();
}

void FbWindow::onAfterElemViewMoved (ElemView *elemview)
{
    // Set project unsaved and update window title.
    need_to_save = true;
    this->u_updateTitle();
}

void FbWindow::onAfterElemViewDeleted ()
{
    // Clear attributes table.
    table_props->clearSelf();

    // Update fields table cause bindings might have been changed.
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), nullptr);
    //table_fields->dehighlightElemFields();

    // Set project unsaved and update window title.
    need_to_save = true;
    this->u_updateTitle();

    // Change state.
    // ...
}


void FbWindow::onWorkingAreaPressed ()
{
    // Deselect elemview on the screen.
    if (cur_device->getScreen() == nullptr)
        return;
    cur_device->getScreen()->deselectElemView();

    // Clear attributes table.
    table_props->clearSelf();

    //table_fields->dehighlightElemFields();
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), nullptr);

    // Change state.
    // ...
}


void FbWindow::onFieldSelected (int field_index, const Elem *elem, QString field_slot)
{
    cur_project.data()->layer0_selectField(field_index, elem, field_slot);

    // We need to update a table where field bindings are placed because there can be other
    // field bindings except this one which can be reseted due to selection of this field.
    table_props->updateSelf(elem, cur_project.data()->layer0_getFields());

    // Update fields table.
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), elem);
    //table_fields->highlightElemFields(elem);

    // Set project unsaved and update window title.
    need_to_save = true;
    this->u_updateTitle();
}


void FbWindow::onFieldAliasChangedInTable (const QString &text)
{
    need_to_save = true;
    this->u_updateTitle();
}

void FbWindow::onFieldTypeChangedInTable (int index)
{
    need_to_save = true;
    this->u_updateTitle();
}


QMenu *FbWindow::createPopupMenu()
{
    QMenu *menu = QMainWindow::createPopupMenu(); // default menu

    // TODO: think about how to correctly hide menu point for "NextGIS Authentication" entry.
    // Currently just check action text.
    const QList<QAction*> &actions = menu->actions();
    for (auto action: actions)
    {
        if (action->text() == FB_NGTOOLB_TEXT)
        {
            action->setVisible(false);
            break;
        }
    }

    return menu;
}


// TODO: this should be moved to the parent class. Think about how to link with template function
// being placed in a library.
template <typename Slot>
QAction *FbWindow::addMainMenuAction (QMenu *menu, QToolBar *toolb, Slot slot,
                                      QString icon, QString name, QString tip)
{
    QAction *act = new QAction(QIcon(icon), name, this);
    act->setStatusTip(tip);

    if (menu != nullptr)
        menu->addAction(act);
    if (toolb != nullptr)
        toolb->addAction(act);

    connect(act, &QAction::triggered, this, slot);

    return act;
}


void FbWindow::u_addElem (QString elem_key_name)
{
    Screen *cur_screen = cur_device->getScreen();
    auto elem_data = ElemRegistrar::get(elem_key_name);
    auto elemview_fct = ElemViewRegistrar::get(elem_key_name);
    Elem *new_elem = nullptr;
    ElemView *new_elemview = nullptr;

    bool can_create = true;
    if (elem_data->is_for_supported)
        can_create = this->u_canUseSupportedFeature();

    if (can_create && cur_screen != nullptr && elem_data != nullptr &&
        elem_data->fct != nullptr && elemview_fct != nullptr)
    {
        // Create elem and its elem view.
        new_elem= elem_data->fct->create();
        new_elemview = elemview_fct->create(new_elem);

        // Connect elemview with project in order all elems will be deleted automatically.
        connect(new_elemview, &ElemView::needToRemoveElem,
                cur_project.data(), &Core::Project::onNeedToRemoveElem);
    }

    // Finish adding elem even if it was not created.
    cur_project.data()->addElem(new_elem);
    cur_screen->endMoveNewElemView(new_elemview);

    // Note: FbWindow::onAfterElemViewAdded will be called next if the addition was successfull.
}

void FbWindow::u_addElemToTheEnd (QString elem_key_name)
{
    Screen *cur_screen = cur_device->getScreen();
    cur_screen->form_pickLastSpace();
    cur_screen->form_setCanScrollToBottom();
    this->u_addElem(elem_key_name);
}

/*
void FbWindow::addElemByNameToTheEnd (QString new_elem_key_name)
{
    Screen *cur_screen = cur_device->getScreen();
    cur_screen->form_pickLastSpace();
    cur_screen->form_setCanScrollToBottom();

    this->addElemByName(new_elem_key_name);
}

void FbWindow::addElemByName (QString new_elem_key_name)
{
    auto elem_data = ElemRegistrar::get(new_elem_key_name);
    Elem *new_elem = nullptr;
    if (elem_data != nullptr && elem_data->fct != nullptr)
        new_elem= elem_data->fct->create();

    this->addElemViewAndElem(new_elem);
}

void FbWindow::addElemByOther (Elem *new_elem, Elem *container_elem)
{
    Screen *cur_screen = cur_device->getScreen();
    cur_screen->form_pickLastSpaceInContainer(container_elem);

    this->addElemViewAndElem(new_elem);
}

void FbWindow::addElemViewAndElem (Elem *new_elem)
{
    Screen *cur_screen = cur_device->getScreen();

    if (cur_screen != nullptr && new_elem != nullptr)
    {
        auto elemview_fct = ElemViewRegistrar::get(new_elem->getKeyName());
        ElemView *new_elemview = nullptr;
        if (elemview_fct != nullptr)
        {
            new_elemview = elemview_fct->create(new_elem);
            connect(new_elemview, &ElemView::needToRemoveElem,
                    cur_project.data(), &Core::Project::onNeedToRemoveElem);
        }
    }

    cur_project.data()->addElem(new_elem); // nullptr can be passed
    cur_screen->endMoveNewElemView(new_elemview);  // nullptr can be passed
}
*/


void FbWindow::u_removeElem ()
{
    Screen *cur_screen = cur_device->getScreen();
    if (cur_screen == nullptr)
        return;

    if (!cur_screen->isElemViewSelected())
        return;
    if (cur_screen->isElemViewMoving())
        return;
    if (cur_screen->isTopLevelElemViewSelected())
        return;

    int ret = QMessageBox::Ok;
    if (cur_screen->isSelectedElemViewAContainer())
        ret = g_showQuestion(this, tr("This element may contain other elements. Do you really want "
                                      "to delete them too?"));
    if (ret != QMessageBox::Ok)
        return;

    const ElemView *sel_elemview = cur_screen->getSelectedElemView();
    const Elem *sel_elem = sel_elemview->getElem();

    // Remove elemview with all its inner elemviews (if there are some).
    cur_screen->removeSelectedElemView();

    // Note: all elems (this and inners) have just been removed automatically. See
    // ElemView::needToRemoveElem()

    // Note: all field bindings for this elem and for inner elems have just been cleared
    // automatially. See Project::deleteElem().

    // Note: FbWindow::onElemViewDeleted() will be called next if the deletion was successfull.
}


void FbWindow::u_openNgfp (QString file_path)
{
	/*
    NgfpLayerInfo new_layer_info;
    NgfpFormInfo new_form_info;

    try
    {
        NgfpReader::loadNgfp(file_path, new_layer_info, new_form_info);
    }
    catch (QString &s)
    {
        g_showMessageDet(this, tr("Unable to open project"), s);
        return;
    }

    if (NgfpReader::last_warnings.size() > 0)
    {
        g_showMessageDet(this, tr("Project is opened but there are some warnings"),
                         NgfpReader::last_warnings);
    }

    // Change current layer.


    // Change current form.

    for ()
    {

    }
	*/
}

bool FbWindow::u_saveNgfp (QString file_path)
{
    if (this->u_initialFieldStructureWasChanged())
    {
        g_showWarning(this, tr("Project was not saved. Your data structure now differs from the "
                               "remote. Please upload new layer to NextGIS Web before saving"));
        return false;
    }

    if (file_path == "")
    {
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        //dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setViewMode(QFileDialog::List);
        dialog.setDefaultSuffix("ngfp");
        dialog.setNameFilter("*.ngfp");
        dialog.setWindowTitle(tr("Save project"));

        QFileInfo file_info(g_getSettings()->value(FB_STS_NGFP_FILE).toString());
        dialog.setDirectory(file_info.absoluteDir());

        if (!dialog.exec())
            return false;

        file_path = dialog.selectedFiles()[0];
        g_getSettings()->setValue(FB_STS_NGFP_FILE, file_path);
    }

    try
    {
        NgfpWriter::saveNgfp(file_path, cur_project.data()->layer0_get(),
                             (NgmFormView*)cur_device->getScreen()->getTopLevelElemView());

        // Set new file path in project.
        cur_project.data()->setFilePath(file_path);

        // Set project saved and update window title.
        need_to_save = false;
        this->u_updateTitle();
    }
    catch (QString &msg)
    {
        g_showMessageDet(this, tr("Failed to save project"), msg);
    }

    return true;
}


bool FbWindow::u_getNgwLayer (QPair<QString, int> &res_data, QPair<QString, QString> creds,
                              NgwDefaultIo *ngw_io)
{
    if (ngw_io == nullptr)
        return false;

    NgwLayerInfo layer_info;
    bool ok = ngw_io->getLayerInfo(layer_info, res_data.first, res_data.second);
    if (!ok)
    {
        qDebug() << ngw_io->getLastError();
        return false;
    }

    NgwApi *api = ngw_io->getNgwApi();

    //const_cast<Layer*>(cur_project.data()->layer0_get())->setNgwConnection({res_data.second, creds.first, creds.second, res_data.first});
    //const_cast<Layer*>(cur_project.data()->layer0_get())->setNgwUrl(api->urlResourcePage(res_data.first, res_data.second));
    const_cast<Layer*>(cur_project.data()->layer0_get())->setName(layer_info.name);
    const_cast<Layer*>(cur_project.data()->layer0_get())->setGeomType(g_findGeomType(layer_info.geom_type));

    QList<FieldInfo> new_fields_info;
    for (auto &tup: layer_info.fields)
        new_fields_info.append({std::get<0>(tup),
                                std::get<1>(tup),
                                g_findFieldType(std::get<2>(tup))});

    this->u_updateLayerFields(new_fields_info);

    // Save current fields structure so to check later whether it was changed and warn user
    // about it.
//    initial_fields_info.clear();
//    for (auto field_info: new_fields_info)
//        initial_fields_info.insert(field_info);

    return true;
}

void FbWindow::u_updateLayerFields (const QList<FieldInfo> &new_fields_info)
{
    // Set fields to project.
    cur_project.data()->layer0_modifyFields(new_fields_info);

    // Update fields table.
    const Elem *selected_elem = nullptr;
    if (this->cur_device->getScreen()->isElemViewSelected())
        selected_elem = this->cur_device->getScreen()->getSelectedElemView()->getElem();
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), selected_elem);

    // Update properties table because the bindings might have been change.
    if (cur_device->getScreen()->isElemViewSelected())
        table_props->updateSelf(cur_device->getScreen()->getSelectedElemView()->getElem(),
                                cur_project.data()->layer0_getFields());
}

void FbWindow::u_recreateDevice ()
{
    Mockup::DeviceData device_data =
    {
        QObject::tr("Default device"),
        QObject::tr("Default device"),
        480, 800, 4.0f,
        true,
        "black_smartphone",
        "android_any"
    };

    if (cur_device != nullptr)
    {
        scr_working->takeWidget();
        delete cur_device;
    }

    cur_device = new Mockup::Device(scr_working, device_data);
    scr_working->setWidget(cur_device);

    Mockup::Screen *cur_screen = cur_device->getScreen();
    connect(cur_screen, &Mockup::Screen::afterElemViewAdded,
            this, &FbWindow::onAfterElemViewAdded);
    connect(cur_screen, &Mockup::Screen::afterElemViewSelected,
            this, &FbWindow::onAfterElemViewSelected);
    connect(cur_screen, &Mockup::Screen::afterElemViewChanged,
            this, &FbWindow::onAfterElemViewChanged);
    connect(cur_screen, &Mockup::Screen::afterElemViewMoved,
            this, &FbWindow::onAfterElemViewMoved);
    connect(cur_screen, &Mockup::Screen::afterElemViewDeleted,
            this, &FbWindow::onAfterElemViewDeleted);

    QString top_level_elem_name = "ngmform";

    Elem *top_elem = ElemRegistrar::get(top_level_elem_name)->fct->create();
    cur_project.data()->addElem(top_elem);

    ElemView *top_elemview = ElemViewRegistrar::get(top_level_elem_name)->create(top_elem);
    cur_screen->setTopLevelElemView(top_elemview);
}

void FbWindow::u_resetProject ()
{
    cur_project.reset(new FbProject());
    cur_project.data()->initialize(GeomType::Point);

    this->u_recreateDevice();

    table_fields->updateSelf(cur_project.data()->layer0_getFields(), nullptr);
    table_props->clearSelf();

    // Reset current fields structure: user can freely change current layer's field structure.
//    initial_fields_info.clear();

    need_to_save = false;
    this->u_updateTitle();
}

/*
bool FbWindow::u_isLayerEditable ()
{
    // For now we allow editing only of local-created layers (not NGW layers).
    return cur_project.data()->layer0_get()->getNgwUrl() == "" ? true : false;
}
*/

void FbWindow::u_createFieldsForNewElem (ElemView *elemview)
{
    if (elemview == nullptr)
        return;

    // Create field(fields) and connect it(them) with new element. Note: we allow to add new fields
    // only if the layer is editable.
//    if (!this->u_isLayerEditable())
//        return;

    Layer *layer = const_cast<Layer*>(cur_project.data()->layer0_get());
    const Elem *elem = elemview->getElem();
    auto field_slots = elem->getFieldSlots();

    // Do it for all field slots of the elem.
    for (auto field_slot_name: field_slots.keys())
    {
        const auto &pair = layer->createUniqueFieldName();
        bool ok = layer->addField(pair.first, pair.second, FieldType::String);
        if (ok)
        {
            Field *field = layer->getField(pair.first);
            field->bind(elem, field_slot_name);
        }
    }

    // Update field table because bindings have been changed. Note: we do not need to update props
    // table because it is either not shown for the newly created elem or is shown for another elem.
    const Elem *selected_elem = nullptr;
    if (this->cur_device->getScreen()->isElemViewSelected())
        selected_elem = this->cur_device->getScreen()->getSelectedElemView()->getElem();
    table_fields->updateSelf(cur_project.data()->layer0_getFields(), selected_elem);
}

int FbWindow::u_showLayerMetaDialog (bool is_editable, QString res_group_url, QString res_group_name, QString but_ok_text)
{
    LayerDialog2 dialog(this, cur_project.data()->layer0_get(), is_editable, res_group_url, res_group_name);
    dialog.setOkButtonText(but_ok_text);

    int ret = dialog.exec();
    if (!ret)
        return ret;

    if (is_editable)
    {
        QString new_name;
        GeomType new_geom_type;

        dialog.getLayerMeta(new_name, new_geom_type);

        const_cast<Layer*>(cur_project.data()->layer0_get())->setName(new_name);
        const_cast<Layer*>(cur_project.data()->layer0_get())->setGeomType(new_geom_type);

        need_to_save = true;
        this->u_updateTitle();
    }

    return QDialog::Accepted;
}

bool FbWindow::u_initialFieldStructureWasChanged ()
{
    auto fields = cur_project.data()->layer0_get()->getFields();

    // Form current fields structure.
    QSet<FieldInfo> cur_fields_info;
    for (auto field: fields)
        cur_fields_info.insert({field->getName(), field->getAlias(), field->getType()});

//    if (cur_fields_info != initial_fields_info)
//        return true;

    return false;
}


void FbWindow::u_updateSupportedIcons (bool is_supported)
{
    // Create form at NGW icon:
    QIcon icon;
    if (!is_supported)
    {
        icon = NGAccess::lockIcon(QIcon(":/images/theme_white/ngw_create2_dis.svg"),
                                  //this->iconSize()); // at this point no size yet
                                  {TOOLB_SIZE, TOOLB_SIZE},
                                  QIcon(":/images/lock.svg"));
    }
    else
    {
        icon = QIcon(":/images/theme_white/ngw_create2.svg");
    }
    act_create_ngw_form->setIcon(icon);
}

bool FbWindow::u_canUseSupportedFeature ()
{
    if (!NGAccess::instance().isUserAuthorized())
    {
        g_showMessage(this, tr("Please upgrade and sign in to use this feature.<br>"
                               "<a href=\"%1\">Learn more ...</a>")
                               .arg(LANGUAGES.value(last_language).subscr_link));
        return false;
    }

    if (!NGAccess::instance().isUserSupported())
    {
        g_showMessage(this, tr("Please upgrade to use this feature.<br>"
                               "<a href=\"%1\">Learn more ...</a>")
                               .arg(LANGUAGES.value(last_language).subscr_link));
        return false;
    }

    return true;
}

void FbWindow::u_updateTitle ()
{
    QString title = "%1 - NextGIS Formbuilder";

    QString file;
    if (!cur_project.isNull())
        file = cur_project.data()->getFilePath();

    if (file.isEmpty())
        file = tr("new");
    if (need_to_save)
        title.prepend("* ");

    this->setWindowTitle(title.arg(file));
}



