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

#include "textedit.h"

#include "attr_registrar.h"

using namespace Fb;
using namespace Core;


TextEdit::TextEdit (QString key_name):
    Elem(key_name)
{
    attr_keep_last  = (Boolean*) this->addAttr(new Boolean("keep_last", false));
    attr_init_text  = (String*)  this->addAttr(new String("init_text", ""));
    attr_only_nums  = (Boolean*) this->addAttr(new Boolean("only_nums", false));
    attr_max_lines  = (Number*)  this->addAttr(new Number("max_lines", 1, 1, 256));
    attr_ngw_login  = (Boolean*) this->addAttr(new Boolean("ngw_login", false));
    attr_ngid_login = (Boolean*) this->addAttr(new Boolean("ngid_login", false));

    fslot_common = this->addFieldSlot("field_common");

    this->behave(nullptr);
}

TextEdit::~TextEdit ()
{
}


void TextEdit::behave (Attr *attr)
{
    // No integer/real fields support at the beginning.
    this->clearTypesForFieldSlot(fslot_common);

    // The "ngw login" and "ng id login" are opposit to each other.
    if (attr != nullptr) // if we see that one of these attrs just has been changed
    {
        if (attr == attr_ngw_login && attr_ngw_login->getValue() == true)
            attr_ngid_login->setValue(false);
        else if (attr == attr_ngid_login && attr_ngid_login->getValue() == true)
            attr_ngw_login->setValue(false);
    }
    else // if no attrs have been changed: some default logic
    {
        if (attr_ngw_login->getValue() == true)
            attr_ngid_login->setValue(false);
    }

    // The "ngw login" and "ng id login" disable other attrs.
    if (attr_ngw_login->getValue() || attr_ngid_login->getValue())
    {
        attr_only_nums->setValue(false);
        attr_keep_last->setValue(false);

        attr_init_text->setEnabled(false);
        attr_only_nums->setEnabled(false);
        attr_max_lines->setEnabled(false);
        attr_keep_last->setEnabled(false);
    }
    else
    {
        attr_init_text->setEnabled(true);
        attr_only_nums->setEnabled(true);
        attr_max_lines->setEnabled(true);
        attr_keep_last->setEnabled(true);

        // The "only numbers" resets "initial text" if set to true.
        if (attr_only_nums->getValue() == true)
        {
            QString s_init_text = attr_init_text->getValue();

            // TODO: maybe check not a result of number conversion but the appropriate symbols here
            // (including point).
            long long number = s_init_text.toLongLong();
            if (number == 0ll && s_init_text != "0")
                attr_init_text->setValue("");

            // The "only numbers" attr allows to write to numeric fields.
            this->addTypeToFieldSlot(fslot_common, {FieldType::Integer});
            this->addTypeToFieldSlot(fslot_common, {FieldType::BigInteger});
            this->addTypeToFieldSlot(fslot_common, {FieldType::Real});
        }
    }

}



