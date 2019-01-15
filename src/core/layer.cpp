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

#include "layer.h"

#include <QRegularExpression>

using namespace Fb;
using namespace Core;


Layer::Layer (QString name, GeomType geom_type, SrsType srs_type):
    name(name),
    geom_type(geom_type),
    srs_type(srs_type)
{
    ngw_con = {-1, "", "", ""};
}

Layer::~Layer ()
{
    for (auto field: fields)
        delete field;
}


/*!
 * @brief Find field by index in a list.
 */
Field *Layer::getField (int index)
{
    if (index < 0 || index >= fields.size())
        return nullptr;
    return fields[index];
}

/*!
 * @brief Find field by name case-insensitevely.
 */
Field *Layer::getField (QString field_name)
{
    for (auto field: fields)
        if (Layer::areNamesEqual(field->getName(), field_name))
            return field;
    return nullptr;
}

/*!
 * @brief Find field by bound elem and its field slot name.
 */
Field *Layer::getField (const Elem *elem, QString slot)
{
    for (auto field: fields)
        if (field->getElem() == elem && field->getSlot() == slot)
            return field;
    return nullptr;
}


/*!
 * @brief Check that names are equal case-insensitevely.
 */
bool Layer::areNamesEqual (QString field_name1, QString field_name2) // STATIC
{
    return QString::compare(field_name1, field_name2, Qt::CaseInsensitive) == 0;
}

/*!
 * @brief Return true if the given string is a correct layer name.
 */
bool Layer::isNameCorrect (QString name) // STATIC
{
    return true; // TEMP, because it seems that NGW accepts any names
}

/*!
 * @brief Return unique field name & alias for this layer.
 */
QPair<QString, QString> Layer::createUniqueFieldName ()
{
    QString new_name;
    QString new_alias;
    int i = 1;
    do
    {
        new_name = QString("field_%1").arg(i);
        new_alias = QString(QObject::tr("Field %1")).arg(i);
        i++;
    }
    while (this->getField(new_name) != nullptr);
    return {new_name, new_alias};
}


/*!
 * @brief Add field to the list of fileds. A field must have a unique case-insensiteve name.
 */
bool Layer::addField (QString field_name, QString field_alias, FieldType field_type)
{
    auto field = this->getField(field_name);
    if (field != nullptr && Layer::areNamesEqual(field->getName(), field_name))
    {
        //qDebug
        return false;
    }

    fields.append(new Field(this, field_name, field_alias, field_type));
    return true;
}

/*!
 * @brief Remove field from the list of fileds.
 */
void Layer::removeField (QString field_name)
{
    for (int i = 0; i < fields.size(); i++)
    {
        Field *field = fields[i];
        if (Layer::areNamesEqual(field->getName(), field_name))
        {
            fields.removeAt(i);
            delete field;
            return;
        }
    }
}


/*!
 * @brief Search for elem and unbind it for the passed field slot.
 */
void Layer::unbindElem (const Elem *elem, QString slot)
{
    Field *field = this->getField(elem, slot);
    if (field == nullptr)
        return;
    field->unbind();
}

/*!
 * @brief Clear field bindings for all slots of the passed elem (not including inner elems of elem).
 */
void Layer::unbindElem (const Elem *elem)
{
    for (int i = 0; i < fields.size(); i++)
        if (elem == fields[i]->getElem())
            fields[i]->unbind(); // can occur several times for several field slots of one elem
}

/*!
 * @brief For all fields: clear all bindings.
 */
void Layer::clearAllBindings ()
{
    for (int i = 0; i < fields.size(); i++)
        fields[i]->unbind();
}


/*!
 * @brief For all fields: check that the allowed field types of the bound elem are still valid for
 *  the field and clear binding if false.
 */
void Layer::updateAllBindings ()
{
    for (int i = 0; i < fields.size(); i++)
    {
        const Elem *elem = fields[i]->getElem();
        if (elem == nullptr)
            continue;
        auto allowed_field_types = elem->getFieldSlots().value(fields[i]->getSlot());
        if (allowed_field_types.find(fields[i]->getType()) == allowed_field_types.end())
            fields[i]->unbind();
    }
}

// Search for all slots of one Elem and set field types to strings if the field type of Elem slot
// is not suppoted more.
void Layer::resetBindingsForElem (const Elem *elem)
{
    for (int i = 0; i < fields.size(); i++)
    {
        const Elem *field_elem = fields[i]->getElem();
        if (field_elem == elem)
        {
            auto allowed_field_types = elem->getFieldSlots().value(fields[i]->getSlot());
            if (allowed_field_types.find(fields[i]->getType()) == allowed_field_types.end())
                fields[i]->setType(FieldType::String);
        }
    }

}


/*!
 * @brief Comon actions when field is selected for an elem.
 */
void Layer::selectField (int field_index, const Elem *elem, QString field_slot)
{
    if (elem == nullptr)
        return;

    // Check that this is a slot from this elem.
    auto field_slots = elem->getFieldSlots();
    if (!field_slots.contains(field_slot))
        return;

    // The field was deselected for the elem: clear that binding.
    if (field_index == -1)
    {
        this->unbindElem(elem, field_slot);
        return;
    }

    Field *field = this->getField(field_index);
    if (field == nullptr)
        return;

    // Find if the passed elem is already bound to some field of this layer and clear that binding.
    // TEMP. Possible taht in future one elem can be bound to several fields of different layers.
    this->unbindElem(elem, field_slot);

    // Bind field if there is a slot of an appropriate field type. If the field is already bound
    // to some elem we overwrite this binding.
    auto allowed_field_types = field_slots.value(field_slot);
    if (allowed_field_types.find(field->getType()) != allowed_field_types.end())
        field->bind(elem, field_slot);
}


/*!
 * @brief Modify the set of fields overriding it with the passed fields data but leaving field
 * bindings.
 */
void Layer::modifyFields (const QList<FieldInfo> &new_fields_info)
{
    QList<Field*> new_fields;
    for (auto &field_info: new_fields_info)
    {
        Field *new_field = new Field(this, field_info.name, field_info.alias, field_info.type);

        // Find the binding and bind if possible.
        Field *old_field = this->getField(field_info.name);
        if (old_field != nullptr)
            new_field->bind(old_field->getElem(), old_field->getSlot());

        new_fields.append(new_field);
    }

    fields = std::move(new_fields);
}



