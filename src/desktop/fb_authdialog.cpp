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

#include "fb_authdialog.h"
#include "ui_fb_authdialog.h"

#include "util/settings.h"
#include "gui/gui_common.h"

#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>

using namespace Fb;
using namespace Gui;


FbAuthDialog::FbAuthDialog (QWidget *parent):
    CustomDialog(parent),
    ui(new Ui::FbAuthDialog),
    ng_dialog(nullptr)
{
    ui->setupUi(this);

    ui->cmbType->addItem(tr("NextGIS ID"));

    bool alter_auth = g_getSettings()->value(FB_STS_ALTER_AUTH, false).toBool();
    QString endpoint = g_getSettings()->value(FB_STS_ALTER_AUTH_ENDPOINT, "").toString();

    ui->grSettings->setChecked(alter_auth);
    ui->editEndpoint->setText(endpoint);

    connect(ui->editEndpoint, &QLineEdit::editingFinished,
            this, &FbAuthDialog::onEndpointEditFinished);
    connect(ui->grSettings, &QGroupBox::toggled,
            this, &FbAuthDialog::onGroupSettingsToggled);

    setMinimumHeight(350); // temporary for full showing of the sign-in dialog
}

FbAuthDialog::~FbAuthDialog ()
{
    g_getSettings()->setValue(FB_STS_ALTER_AUTH, ui->grSettings->isChecked());
    g_getSettings()->setValue(FB_STS_ALTER_AUTH_ENDPOINT, getEndpoint());

    delete ui;
}


void FbAuthDialog::setNgAccountWidget (NGSignDialog *new_ng_dialog)
{
    if (new_ng_dialog == nullptr)
        return;
    ng_dialog = new_ng_dialog;

    old_ng_dialog_size = ng_dialog->size();
    old_ng_dialog_w_flags = ng_dialog->windowFlags();

    ng_dialog->setParent(ui->grUser);
    //static_cast<QVBoxLayout*>(ui->grUser->layout())
    //        ->addWidget(static_cast<QWidget*>(ng_dialog));
    auto layout = ui->grUser->layout();
    if (layout != nullptr)
        delete layout;
    QVBoxLayout *vlayout = new QVBoxLayout();
    ui->grUser->setLayout(vlayout);
    vlayout->addWidget(ng_dialog);
    ng_dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->onNeedToUpdateAccountInfo();

    QPushButton *ng_button = ng_dialog->getSignButton(); // needed for Exit action
    connect(ng_button, &QPushButton::clicked,
            this, &FbAuthDialog::onSignButtonClick);

    connect(&NGAccess::instance(), SIGNAL(userInfoUpdated()),
            this, SLOT(onNeedToUpdateAccountInfo()));
    connect(&NGAccess::instance(), SIGNAL(supportInfoUpdated()),
            this, SLOT(onNeedToUpdateAccountInfo()));
}

void FbAuthDialog::removeNgAccountWidget ()
{
    static_cast<QVBoxLayout*>(ui->grUser->layout())->takeAt(0);
    ng_dialog->setParent(nullptr);

    ng_dialog->setWindowFlags(old_ng_dialog_w_flags);
    ng_dialog->resize(old_ng_dialog_size);
}


void FbAuthDialog::onSignButtonClick ()
{

    onNeedToUpdateAccountInfo();
}


void FbAuthDialog::onNeedToUpdateAccountInfo ()
{
    if (ng_dialog == nullptr)
        return;
    ng_dialog->updateContent();
    ng_dialog->show();

    if (NGAccess::instance().isUserAuthorized())
        ui->grSettings->setEnabled(false);
    else
        ui->grSettings->setEnabled(true);
}


void FbAuthDialog::onEndpointEditFinished ()
{
    QString enpoint = getEndpoint();
    NGAccess::instance().setEndPoint(enpoint);

    qDebug() << "onEndpointEditFinished()";
}

void FbAuthDialog::onGroupSettingsToggled (bool checked)
{
    if (checked)
        onEndpointEditFinished();
    else
        NGAccess::instance().setEndPoint(FB_DEFAULT_AUTH_ENDPOINT);

    qDebug() << "onGroupSettingsToggled()";
}


QString FbAuthDialog::getEndpoint ()
{
    QString endpoint = ui->editEndpoint->text();

    // Some adjustments of given url.
    while (endpoint.endsWith("/"))
        endpoint.chop(1);

    if (endpoint == "")
        endpoint = " "; // otherwise lib_ngstd uses my.nextgis.com

    return endpoint;
}



