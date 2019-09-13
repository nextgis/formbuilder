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

#include "propstablewidget.h"

#include "core/attr_registrar.h"
#include "core/fieldslot_registrar.h"

#include "qglobal.h"
#include <QHeaderView>
#include <QStandardItemModel>

#define FB_PROPSTABLE_STUB_STR "field"

using namespace Fb;
using namespace Gui;


FieldComboBox::FieldComboBox ():
    QComboBox(),
    elem(nullptr),
    field_slot("")
{
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FieldComboBox::onFieldIndexChange);
}

FieldComboBox::~FieldComboBox ()
{
}


void FieldComboBox::onFieldIndexChange (int index)
{
    emit fieldSelected(index - 1, elem, field_slot);
}


PropsTableWidget::PropsTableWidget (QWidget *parent, MainWindow *window):
    QTableWidget(parent),
    window(window)
{
    if (window == nullptr)
        throw; // TODO: named exception

    this->setObjectName("table_pros");
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setFocusPolicy(Qt::NoFocus);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->setColumnCount(2);

    // TODO: we need a way to resize columns not only by header but by cells too.

    // TEMP:
    //this->horizontalHeader()->setStretchLastSection(true);
    this->stretchColumns();
}

PropsTableWidget::~PropsTableWidget ()
{
}


// Clear than fill self with properties for the given element.
void PropsTableWidget::fillSelf (const Core::Elem *elem, const QList<Core::Field*> &fields)
{
    this->clearSelf();

    // Add attributes and their widgets.
    const auto attrs = elem->getAttrs();
    for (const auto attr_key_name: attrs.keys())
    {
        auto attr = attrs.value(attr_key_name);

        QString attr_name;
        QString attr_description;
        auto attr_data = Core::AttrRegistrar::get(attr_key_name);
        if (attr_data == nullptr)
        {
            attr_name = attr_key_name;
            attr_description = attr_key_name;
            //qDebug
        }
        else
        {
            attr_name = attr_data->display_name;
            attr_description = attr_data->description;
        }

        auto attr_watcher_fct = AttrWatcherRegistrar::get(attr->getInputType());
        if (attr_watcher_fct == nullptr)
        {
            //qDebug
            continue;
        }

        AttrWatcher *watcher = attr_watcher_fct->create(attr);
        if (watcher == nullptr)
        {
            //qDebug
            continue;
        }

        QWidget *widget = this->createAttrWidget(watcher);
        if (widget == nullptr)
        {
            //qDebug
            continue;
        }

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(QString(" %1").arg(attr_name));
        item->setToolTip(QString("<p>%1</p>").arg(attr_description));
        item->setFlags(Qt::ItemIsEnabled);
        item->setData(Qt::UserRole, QVariant::fromValue(watcher));

        connect(watcher, &AttrWatcher::attrChangedByWidget,
                elem, &Core::Elem::onAttrChanged);

        this->setRowCount(this->rowCount() + 1);
        this->setItem(this->rowCount() - 1, 0, item);
        this->setCellWidget(this->rowCount() - 1, 1, widget);
    }

    // Add fields and comboboxes for them based on field slots of this elem. There can be 0 or more
    // field slots for the elem.

    //TEMP:
    const auto field_slots = elem->getFieldSlots();
    //QMap<QString, std::set<Core::FieldType>> field_slots; // TEMP: void map

    for (auto field_slot: field_slots.keys())
    {
        auto field_slot_data = Core::FieldSlotRegistrar::get(field_slot);
        if (field_slot_data == nullptr)
        {
            //qDebug
            continue;
        }

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(QString(" %1").arg(field_slot_data->display_name));
        item->setToolTip(QString("<p>%1</p>").arg(field_slot_data->description));
        item->setFlags(Qt::ItemIsEnabled);
        item->setData(Qt::UserRole, FB_PROPSTABLE_STUB_STR);

        FieldComboBox *combo = this->createFieldCombo(elem, field_slot, fields);

        this->setRowCount(this->rowCount() + 1);
        this->setItem(this->rowCount() - 1, 0, item);
        this->setCellWidget(this->rowCount() - 1, 1, combo);
    }

    this->sortItems(0, Qt::AscendingOrder);
    this->stretchColumns(); // otherwise sorting brakes stretch effect (why?)
}

void PropsTableWidget::clearSelf ()
{
    for (int row = 0; row < this->rowCount(); row++)
    {
        delete this->item(row, 0)->data(Qt::UserRole).value<AttrWatcher*>();
        this->removeCellWidget(row, 1);
    }
    this->clear();
    this->setRowCount(0);
}

void PropsTableWidget::updateSelf (const Core::Elem *elem, const QList<Core::Field*> &fields)
{
//    const auto attrs = elem->getAttrs();

    // See through all rows and update each cell widget.
    for (int row = 0; row < this->rowCount(); row++)
    {
        QVariant var = this->item(row, 0)->data(Qt::UserRole);

        // We meet field combobox: refill its contents.
        if (var.toString() == FB_PROPSTABLE_STUB_STR)
        {
            FieldComboBox *old_combo = static_cast<FieldComboBox*>(this->cellWidget(row, 1));

            FieldComboBox *combo = this->createFieldCombo(old_combo->elem, old_combo->field_slot,
                                                          fields);
            this->removeCellWidget(row, 1);
            this->setCellWidget(row, 1, combo);
        }

        // We meet attr widget: recreate a widget. The new widget will have all according changes
        // while the table will have its focus saved.
        else
        {
            bool has_focus = this->cellWidget(row, 1)->hasFocus();

            auto attr_watcher = var.value<AttrWatcher*>();
//            auto attr = attr_watcher->getAttr();
//            if (attr == nullptr)
//            {
//                //qDebug
//                throw; // TODO: named exception
//            }

            QWidget *widget = this->createAttrWidget(attr_watcher);

            this->removeCellWidget(row, 1);
            this->setCellWidget(row, 1, widget);

            if (has_focus)
                widget->setFocus();
        }
    }
}


/*
void PropsTableWidget::resizeEvent (QResizeEvent *event)
{
    QTableWidget::resizeEvent(event); // otherwise no resizes when resizing parent
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    int width0 = this->horizontalHeader()->sectionSize(0);
    int width1 = this->horizontalHeader()->sectionSize(1);
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    this->horizontalHeader()->resizeSection(0, width0);
    this->horizontalHeader()->resizeSection(1, width1);
}
*/


void PropsTableWidget::stretchColumns ()
{
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

QWidget *PropsTableWidget::createAttrWidget (const AttrWatcher *attr_watcher)
{
    QWidget *widget = attr_watcher->createWidget(window);
    if (widget == nullptr)
        return nullptr;

    if (attr_watcher->getAttr()->isEnabled())
        widget->setEnabled(true);
    else
        widget->setEnabled(false);

    return widget;
}

FieldComboBox *PropsTableWidget::createFieldCombo (const Core::Elem *elem, QString slot,
                                                   const QList<Core::Field*> &fields)
{
    if (elem == nullptr)
        return nullptr;

    FieldComboBox *combo = new FieldComboBox();
    combo->addItem("-"); // add first item which means the "unbound" state of elem.

    int selected_index = 0;

    // A hack to set item disabled:
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(combo->model());

    for (int i = 0; i < fields.size(); i++)
    {
        // Anyway add all fields to this combobox.
        combo->addItem(fields[i]->getAlias());

        // Make item disabled if an elem cannot be bound to a field OR the field is already bound
        // to some elem.
        if (!fields[i]->canBind(elem, slot) ||
            fields[i]->isBound())
        {
            QStandardItem *item = model->item(combo->count() - 1);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }

        if (fields[i]->getElem() == elem && fields[i]->getSlot() == slot)
            selected_index = i + 1; // + 1 because of the stub first value
    }

    // Select the bound field for this elem & slot.
    combo->setCurrentIndex(selected_index);

    combo->elem = elem;
    combo->field_slot = slot;

    connect(combo, &FieldComboBox::fieldSelected, this, &PropsTableWidget::fieldSelected);

    return combo;
}



