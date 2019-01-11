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

#include "depdoubleitemsdialog.h"

#include "util/settings.h"

#include <QHeaderView>

using namespace Fb;
using namespace Gui;


DepDoubleItemsDialog::DepDoubleItemsDialog (QWidget *parent):
    CustomDialog(parent)
{
    this->setWindowTitle(tr("Modify items"));

    int w = g_getSettings()->value(FB_STS_DEPITMDLG_W).toInt();
    int h = g_getSettings()->value(FB_STS_DEPITMDLG_H).toInt();
    int width_l = g_getSettings()->value(FB_STS_DEPITMDLG_COL0_W_L).toInt();
    int width_r = g_getSettings()->value(FB_STS_DEPITMDLG_COL0_W_R).toInt();

    if (w != 0 && h != 0)
        this->resize({w, h});

    lay_this = new QGridLayout(this);
    lay_this->setContentsMargins(4, 4, 4, 4);
    lay_this->setSpacing(6);

    lay_right = new QVBoxLayout();
    lay_right->setContentsMargins(0, 0, 0, 0);
    lay_right->setSpacing(0);

    lay_this->addLayout(lay_right, 0, 1);

    // Create left table.
    left_widget = new ItemsWidget(this, tr("Items (main list):"),
                                  {tr("Value"), tr("Display text")},
                                  0, CMNTBL_MAX_ROWS, false);
    lay_this->addWidget(left_widget, 0, 0);
    if (width_l != 0)
        left_widget->getTable()->horizontalHeader()->resizeSection(0, width_l);

    connect(left_widget->getTable(), &CommonTableWidget::rowSelected,
            this, &DepDoubleItemsDialog::onLeftWidgetRowSelected);
    connect(left_widget->getTable(), &CommonTableWidget::rowAdded,
            this, &DepDoubleItemsDialog::onLeftWidgetRowAdded);
    connect(left_widget->getTable(), &CommonTableWidget::rowRemoved,
            this, &DepDoubleItemsDialog::onLeftWidgetRowRemoved);
    connect(left_widget->getTable(), &CommonTableWidget::cleared,
            this, &DepDoubleItemsDialog::onLeftWidgetCleared);

//    left_widget->getTable()->clearSelection(); // so not to show dotted frame at the beginnning

    // Create last right table which will always be disabled and which will be shown when nothing or
    // the last row in the left table is selected.
    cur_right_widget = new ItemsWidget(this, tr("Items (dependent list):"),
                                       {tr("Value"), tr("Display text")},
                                       0, CMNTBL_MAX_ROWS, false);
    cur_right_widget->setEnabled(false);
    lay_right->addWidget(cur_right_widget);
    if (width_r != 0)
        cur_right_widget->getTable()->horizontalHeader()->resizeSection(1, width_r);

    right_widgets.append(cur_right_widget);

    QPushButton *but_ok = new QPushButton(this);
    but_ok->setText(tr("OK"));
    but_ok->setFocusPolicy(Qt::NoFocus); // so not to exit accidentally
    connect(but_ok, &QPushButton::clicked, this, &DepDoubleItemsDialog::onOkClicked);

    lay_this->addWidget(but_ok, 1, 0, 1, 2);

    // TEMP:
    left_widget->hideCsvButton();
    cur_right_widget->hideCsvButton();
}

DepDoubleItemsDialog::~DepDoubleItemsDialog ()
{
    g_getSettings()->setValue(FB_STS_DEPITMDLG_W, this->width());
    g_getSettings()->setValue(FB_STS_DEPITMDLG_H, this->height());
    g_getSettings()->setValue(FB_STS_DEPITMDLG_COL0_W_L,
                              left_widget->getTable()->horizontalHeader()->sectionSize(0));
    g_getSettings()->setValue(FB_STS_DEPITMDLG_COL0_W_R,
                              right_widgets.last()->getTable()->horizontalHeader()->sectionSize(0));
}


void DepDoubleItemsDialog::putItems (const Core::DepDoubleItemsValue &value)
{
    // Fill left table.
    left_widget->getTable()->putItems(
                        {value.main.inners, value.main.outers}, value.main.def_index);

    // Delete all right tables but do not touch last disabled right table.
    this->removeAllRightWidgets();

    // Create and fill right tables.
    for (int i = 0; i < value.deps.size(); i++)
    {
        auto right_widget = this->appendRightWidget();
        right_widget->getTable()->putItems(
                            {value.deps[i].inners, value.deps[i].outers}, value.deps[i].def_index);
    }

    left_widget->getTable()->clearFocus();
}

void DepDoubleItemsDialog::getItems (Core::DepDoubleItemsValue &value) const
{
    // Get items from the left table.
    QList<QStringList> items;
    left_widget->getTable()->getItems(items, value.main.def_index);
    value.main.inners = items[0];
    value.main.outers = items[1];

    // Get items from all right tables (except the last disabled one).
    value.deps.clear();
    for (int i = 0; i < right_widgets.size() - 1; i++)
    {
        value.deps.append({{}, {}, -1});

        QList<QStringList> items;
        right_widgets[i]->getTable()->getItems(items, value.deps.last().def_index);
        value.deps.last().inners = items[0];
        value.deps.last().outers = items[1];
    }
}


void DepDoubleItemsDialog::onOkClicked ()
{
    // Check that the last input row in left table is not void - we must insert a (void) right table
    // for it.
    left_widget->getTable()->addRowFromInputRow();
    if (left_widget->getTable()->rowCount() > right_widgets.size())
    {
        this->appendRightWidget();
    }

    this->accept();
}


// Show the according right table. If the last "input" row is selected we show the last disabled
// right table.
void DepDoubleItemsDialog::onLeftWidgetRowSelected ()
{
    int index = left_widget->getTable()->currentRow();
    this->setRightWidget(index);
}

// Add according right table. We know that a row is always appended to the end of the table (but
// before the "input" row).
void DepDoubleItemsDialog::onLeftWidgetRowAdded ()
{
    this->appendRightWidget();
}

// Delete according right table and show a right table for the newly selected row.
void DepDoubleItemsDialog::onLeftWidgetRowRemoved ()
{
    this->removeCurRightWidget();
    int index = left_widget->getTable()->currentRow();
    if (index == -1) // the first row was removed
        index = left_widget->getTable()->rowCount() - 1;
    this->setRightWidget(index);
}

// Delete all right tables except the last one. We know that there are no restrictions such as
// min or max rows (see constructor) so we do not need to care abount the count of rows.
void DepDoubleItemsDialog::onLeftWidgetCleared ()
{
    this->removeAllRightWidgets();
    cur_right_widget = nullptr;
    this->setRightWidget(0);
}

// Resize the last right table on resize of any other table in order to save the size of columns
// later. We know that the last right table will never be deleted.
void DepDoubleItemsDialog::onRightWidgetResize (QSize new_size)
{
    right_widgets.last()->getTable()->horizontalHeader()->resizeSection(0, new_size.width());
}


ItemsWidget *DepDoubleItemsDialog::appendRightWidget ()
{
    ItemsWidget *right_widget = new ItemsWidget(this,
                                tr("Items (dependent list):"),
                                {tr("Value"), tr("Display text")},
                                0, CMNTBL_MAX_ROWS, false);
    right_widget->getTable()->horizontalHeader()->resizeSection(0,
                              right_widgets.last()->getTable()->horizontalHeader()->sectionSize(0));
    right_widget->hide();
    lay_right->addWidget(right_widget);
    right_widgets.insert(right_widgets.size() - 1, right_widget);

    // TEMP:
    right_widget->hideCsvButton();

    return right_widget;
}

void DepDoubleItemsDialog::removeCurRightWidget ()
{
    for (int i = 0; i < right_widgets.size(); i++)
    {
        if (right_widgets[i] == cur_right_widget)
        {
            right_widgets.removeAt(i);
            break;
        }
    }
    lay_right->removeWidget(cur_right_widget);
    delete cur_right_widget;
    cur_right_widget = nullptr;
}

void DepDoubleItemsDialog::removeAllRightWidgets ()
{
    while (right_widgets.size() > 1) // do not touch last table
    {
        lay_right->removeWidget(right_widgets.first());
        delete right_widgets.first();
        right_widgets.removeFirst();
    }
}

void DepDoubleItemsDialog::setRightWidget (int index)
{
    if (index < 0 || index >= right_widgets.size())
        return;
    if (cur_right_widget != nullptr)
        cur_right_widget->hide();
    right_widgets[index]->show();
    cur_right_widget = right_widgets[index];
}



