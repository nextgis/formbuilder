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

#include <QWidget>
#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>

namespace Fb
{
namespace Gui
{


inline void g_showMsg (QWidget *parent, QString caption, QString text, QMessageBox::Icon icon_type)
{
    QMessageBox box(parent);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.setWindowTitle(caption);
    box.setIcon(icon_type);
    box.exec();
}

inline void g_showMsgDet (QWidget *parent, QString caption, QString text, QString detailed_text,
                          QMessageBox::Icon icon_type)
{
    QMessageBox box(parent);
    box.setText(text);
    box.setDetailedText(detailed_text);
    box.setWindowTitle(caption);
    box.setIcon(icon_type);

    box.setStandardButtons(QMessageBox::Ok);
    box.setDefaultButton(QMessageBox::Ok);
    box.setEscapeButton(QMessageBox::Ok); // otherwise close button is disabled (why?)

    box.exec();
}

inline int g_showQuestion (QWidget *parent, QString message)
{
    QMessageBox box(parent);
    box.setText(message);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setWindowTitle(QObject::tr("Question"));
    box.setIcon(QMessageBox::Question);
    return box.exec();
}

inline int g_showYesNoQuestion (QWidget *parent, QString message)
{
    QMessageBox box(parent);
    box.setText(message);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    box.setWindowTitle(QObject::tr("Question"));
    box.setIcon(QMessageBox::Question);
    return box.exec();
}


inline void g_showWarning (QWidget *parent, QString text)
{
    g_showMsg(parent, QObject::tr("Warning"), text, QMessageBox::Warning);
}

inline void g_showWarningDet (QWidget *parent, QString text, QString detailed_text)
{
    g_showMsgDet(parent, QObject::tr("Warning"), text, detailed_text, QMessageBox::Warning);
}

inline void g_showMessage (QWidget *parent, QString text)
{
    g_showMsg(parent, QObject::tr("Information"), text, QMessageBox::Information);
}

inline void g_showMessageDet (QWidget *parent, QString text, QString detailed_text)
{
    g_showMsgDet(parent, QObject::tr("Information"), text, detailed_text, QMessageBox::Information);
}


}
}



