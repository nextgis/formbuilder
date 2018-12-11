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

#include "project.h"

using namespace Fb;
using namespace Core;


Project::Project ():
    QObject(),
    is_initialized(false)
{

}

Project::~Project ()
{
    for (auto elem: elems)
        delete elem;
    for (auto layer: layers)
        delete layer;
}


/*!
 * \brief Add elem to the inner set of elems. Project will take ownership on passed elem.
 */
void Project::addElem (Elem *elem)
{
    if (elem == nullptr)
        return;
    elems.insert(elem);
}

/*!
 * \brief Remove elem from the inner set of elems and delete it. Also remove bound fields.
 */
void Project::deleteElem (Elem *elem)
{
    if (elem == nullptr)
        return;

    for (auto &layer: layers)
    {
        QStringList fields_to_delete;

        for (auto field: layer->fields)
            if (field->getElem() == elem)
                //field->unbind();
                fields_to_delete.append(field->getName());

        for (auto &field_name: fields_to_delete)
            layer->removeField(field_name);
    }

    elems.remove(elem);
    delete elem;
}


void Project::onNeedToRemoveElem (const Elem *elem)
{
    this->deleteElem(const_cast<Elem*>(elem));
}
