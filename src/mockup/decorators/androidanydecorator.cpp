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

#include "androidanydecorator.h"

// Include here all elem views:
#include "elemviews/default_view.h"
#include "elemviews/box_view.h"
#include "elemviews/label_view.h"
#include "elemviews/textedit_view.h"
#include "elemviews/checkbox_view.h"
#include "elemviews/combobox_view.h"
#include "elemviews/depcomboboxes_view.h"
#include "elemviews/splitcombobox_view.h"
#include "elemviews/datetimepicker_view.h"
#include "elemviews/counter_view.h"
#include "elemviews/radiogroup_view.h"
#include "elemviews/tabs_view.h"
#include "elemviews/ngmform_view.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

using namespace Fb;
using namespace Mockup;


void AndroidAnyDecorator::decorate (ElemView *elemview)
{
    if (elemview == nullptr)
        return;

    QWidget *w_base = elemview->getBaseDecorWidget();
    QVBoxLayout *lay_base = elemview->getBaseDecorLayout();
    if (w_base == nullptr || lay_base == nullptr)
        return;

   const QString &elem_name = elemview->getKeyName();

    // LABEL
    if (elem_name == "label")
    {
//        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        elemview->setMaximumHeight(99999);
        w_base->setObjectName("w_label_base");

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_label_text");

        lab_text->setWordWrap(true);

        lay_base->addWidget(lab_text);
    }

    // VOIDSPACE
    if (elem_name == "voidspace")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_voidspace_base");
    }

    // TEXTEDIT
    else if (elem_name == "textedit")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_textedit_base");

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_textedit_text");
        lab_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QLabel *lab_cross = new QLabel(w_base);
        lab_cross->setObjectName("lab_textedit_cross");
        lab_cross->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QWidget *w_bottom_line = new QWidget(w_base);
        w_bottom_line->setObjectName("w_textedit_bottom_line");
        w_bottom_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout *lay_field = new QHBoxLayout();
        lay_field->setContentsMargins(0, 0, 0, 0);
        lay_field->setSpacing(0);

        lay_field->addWidget(lab_text);
        lay_field->addWidget(lab_cross);

        lay_base->addLayout(lay_field);
        lay_base->addWidget(w_bottom_line);
    }

    // CHECKBOX
    else if (elem_name == "checkbox")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_checkbox_base");

        QHBoxLayout *lay_all = new QHBoxLayout();
        lay_all->setContentsMargins(0, 0, 0, 0);
        lay_all->setSpacing(0);

        QLabel *lab_on = new QLabel(w_base);
        lab_on->setObjectName("lab_checkbox_on");

        QLabel *lab_off = new QLabel(w_base);
        lab_off->setObjectName("lab_checkbox_off");

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_checkbox_text");
        lab_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        lay_all->addWidget(lab_on);
        lay_all->addWidget(lab_off);
        lay_all->addWidget(lab_text);

        lay_base->addLayout(lay_all);
    }

    // COMBOBOX
    else if (elem_name == "combobox")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_combobox_base");

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_combo_text");
        lab_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QLabel *lab_arrow = new QLabel(w_base);
        lab_arrow->setObjectName("lab_combo_arrow");
        lab_arrow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QWidget *w_bottom_line = new QWidget(w_base);
        w_bottom_line->setObjectName("w_combo_bottom_line");
        w_bottom_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout *lay_field = new QHBoxLayout();
        lay_field->setContentsMargins(0, 0, 0, 0);
        lay_field->setSpacing(0);

        lay_field->addWidget(lab_text);
        lay_field->addWidget(lab_arrow);

        lay_base->addLayout(lay_field);
        lay_base->addWidget(w_bottom_line);
    }

    // DEPENDENTCOMBOBOXES || SPLITTEDCOMBOBOX
    else if (elem_name == "depcomboboxes" || elem_name == "splitcombobox")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLabel *lab_text1 = new QLabel(w_base);
        lab_text1->setObjectName("lab_combo_text1");
        lab_text1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QLabel *lab_arrow1 = new QLabel(w_base);
        lab_arrow1->setObjectName("lab_combo_arrow");
        lab_arrow1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QLabel *lab_text2 = new QLabel(w_base);
        lab_text2->setObjectName("lab_combo_text2");
        lab_text2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QLabel *lab_arrow2 = new QLabel(w_base);
        lab_arrow2->setObjectName("lab_combo_arrow");
        lab_arrow2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QHBoxLayout *lay_1 = new QHBoxLayout();
        lay_1->setContentsMargins(0, 0, 0, 0);
        lay_1->setSpacing(0);

        QHBoxLayout *lay_2 = new QHBoxLayout();
        lay_2->setContentsMargins(0, 0, 0, 0);
        lay_2->setSpacing(0);

        lay_1->addWidget(lab_text1);
        lay_1->addWidget(lab_arrow1);
        lay_2->addWidget(lab_text2);
        lay_2->addWidget(lab_arrow2);

        if (elem_name == "depcomboboxes")
        {
            w_base->setObjectName("w_depcomboboxes_base");

            lay_base->addLayout(lay_1);
            lay_base->addLayout(lay_2);
        }

        else // elem_name == "splitcombobox"
        {
            w_base->setObjectName("w_splitcombobox_base");

            QLabel *lab_cap1 = new QLabel(w_base);
            lab_cap1->setObjectName("lab_combo_caption1");
            lab_cap1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            QLabel *lab_cap2 = new QLabel(w_base);
            lab_cap2->setObjectName("lab_combo_caption2");
            lab_cap2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            QGridLayout *lay_grid = new QGridLayout();
            lay_grid->setContentsMargins(0, 0, 0, 0);
            lay_grid->setSpacing(5);

            lay_grid->addWidget(lab_cap1, 0, 0);
            lay_grid->addWidget(lab_cap2, 0, 1);
            lay_grid->addLayout(lay_1, 1, 0);
            lay_grid->addLayout(lay_2, 1, 1);

            lay_base->addLayout(lay_grid);
        }

    }

    // RADIOGROUP
    else if (elem_name == "radiogroup")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        w_base->setObjectName("w_radiogroup_base");
    }

    // DATETIME
    else if (elem_name == "datetimepicker")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_datetime_base");

        QHBoxLayout *lay_all = new QHBoxLayout();
        lay_all->setContentsMargins(0, 0, 0, 0);
        lay_all->setSpacing(0);

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_datetime_text");
        lab_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLabel *lab_calendar = new QLabel(w_base);
        lab_calendar->setObjectName("lab_datetime_calendar");

        lay_all->addWidget(lab_text);
        lay_all->addWidget(lab_calendar);

        lay_base->addLayout(lay_all);
    }

    // PHOTO
    else if (elem_name == "photo")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_photo_base");

        QHBoxLayout *lay_camera = new QHBoxLayout();
        lay_camera->setContentsMargins(0, 0, 0, 0);
        lay_camera->setSpacing(0);

        QLabel *lab_camera = new QLabel(w_base);
        lab_camera->setObjectName("lab_photo_camera");

        lay_camera->addWidget(lab_camera);
        lay_camera->addStretch();

        lay_base->addLayout(lay_camera);
    }

    // SIGNFIELD
    else if (elem_name == "signfield")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_signfield_base");

        QHBoxLayout *lay_top = new QHBoxLayout();
        lay_top->setContentsMargins(0, 0, 0, 0);
        lay_top->setSpacing(0);

        QLabel *lab_clear = new QLabel(w_base);
        lab_clear->setObjectName("lab_signfield_clear");
        lab_clear->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        lay_top ->addStretch();
        lay_top->addWidget(lab_clear);

        lay_base->addLayout(lay_top);
        lay_base->addStretch();
    }

    // COORDINATES
    else if (elem_name == "coords")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_coordinates_base");

        QLabel *lab_lat = new QLabel(w_base);
        lab_lat->setObjectName("lab_coords_lat");
        lab_lat->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        lab_lat->setText("55.000000");

        QLabel *lab_lon = new QLabel(w_base);
        lab_lon->setObjectName("lab_coords_lon");
        lab_lon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        lab_lon->setText("37.000000");

        QWidget *w_line_lat = new QWidget(w_base);
        w_line_lat->setObjectName("w_coords_line");
        w_line_lat->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QWidget *w_line_lon = new QWidget(w_base);
        w_line_lon->setObjectName("w_coords_line");
        w_line_lon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        lay_base->addWidget(lab_lat);
        lay_base->addWidget(w_line_lat);
        lay_base->addStretch();
        lay_base->addWidget(lab_lon);
        lay_base->addWidget(w_line_lon);
    }

    // COUNTER
    else if (elem_name == "counter")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_counter_base");

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_counter_text");
        lab_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QWidget *w_line = new QWidget(w_base);
        w_line->setObjectName("w_counter_line");
        w_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        lay_base->addWidget(lab_text);
        lay_base->addWidget(w_line);
    }

    // DISTANCEMETER
    else if (elem_name == "dmeter")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        w_base->setObjectName("w_distancemeter_base");

        QLabel *lab_text = new QLabel(w_base);
        lab_text->setObjectName("lab_distancemeter_text");
        lab_text->setText(tr("0 m"));

        lay_base->addWidget(lab_text);
    }

    // TABS
    else if (elem_name == "tabs")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        w_base->setObjectName("w_tabs_base");
    }

    // NGMFORM
    else if (elem_name == "ngmform")
    {
        elemview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        w_base->setObjectName("w_ngmform_base");

        QLabel *w_back = new QLabel(w_base);
        w_back->setObjectName("w_ngmform_back");
        w_back->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QLabel *lab_header = new QLabel(w_base);
        lab_header->setObjectName("lab_ngmform_header");
        lab_header->setAlignment(Qt::AlignCenter);
        //lab_header->setText(tr("Set attributes"));
        lab_header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLabel *w_ok = new QLabel(w_base);
        w_ok->setObjectName("w_ngmform_ok");
        w_ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QLabel *w_dots = new QLabel(w_base);
        w_dots->setObjectName("w_ngmform_dots");
        w_dots->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QHBoxLayout *lay_header = new QHBoxLayout();
        lay_header->setContentsMargins(0, 0, 0, 0);
        lay_header->setSpacing(0);

        lay_header->addWidget(w_back);
        lay_header->addWidget(lab_header);
        lay_header->addWidget(w_ok);
        lay_header->addWidget(w_dots);

        // Insert decorations above the scroll area with the container.
        lay_base->insertLayout(0, lay_header);
    }

    // Add decoration steps for different elements here.
    // ...
}


void AndroidAnyDecorator::update (ElemView *elemview)
{
    if (elemview == nullptr)
        return;

    QWidget *w_base = elemview->getBaseDecorWidget();
    QVBoxLayout *lay_base = elemview->getBaseDecorLayout();
    if (w_base == nullptr || lay_base == nullptr)
        return;

    const QString &elem_name = elemview->getKeyName();
    const QVariant &decor_data = elemview->getDataForDecor();

    // LABEL
    if (elem_name == "label")
    {
        const QString text = decor_data.toString();

        QLabel *lab_text = elemview->findChild<QLabel*>("lab_label_text");
        lab_text->setText(text);
    }

    // TEXTEDIT
     if (elem_name == "textedit")
    {
        const QString text = decor_data.toString();

        QLabel *lab_text = elemview->findChild<QLabel*>("lab_textedit_text");
        lab_text->setText(text);
    }

    // CHECKBOX
    else if (elem_name == "checkbox")
    {
        bool is_checked = decor_data.value<CheckBoxViewDecorData>().first;
        QString text = decor_data.value<CheckBoxViewDecorData>().second;

        QLabel *lab_on = elemview->findChild<QLabel*>("lab_checkbox_on");
        QLabel *lab_off = elemview->findChild<QLabel*>("lab_checkbox_off");
        if (is_checked)
        {
            lab_on->show();
            lab_off->hide();
        }
        else
        {
            lab_off->show();
            lab_on->hide();
        }

        QLabel *lab_text = elemview->findChild<QLabel*>("lab_checkbox_text");
        //lab_text->setText(QString(" %1").arg(text));
        lab_text->setText(text);
    }

    // COMBOBOX
    else if (elem_name == "combobox")
    {
        const QString text = decor_data.toString();

        QLabel *lab_text = elemview->findChild<QLabel*>("lab_combo_text");
        lab_text->setText(QString(" %1").arg(text));
    }

    // RADIOGROUP
    else if (elem_name == "radiogroup")
    {
        QStringList lines = decor_data.value<RadioGroupViewDecorData>().first;
        int line_index = decor_data.value<RadioGroupViewDecorData>().second;

        // Clear all previous decorations (horizontal layouts with widgets).
        QLayoutItem *child;
        while ((child = lay_base->takeAt(0)) != 0)
        {
            QLayout *layout = child->layout();
            QWidget *w;
            w = layout->takeAt(0)->widget();
            delete w;
            w = layout->takeAt(0)->widget();
            delete w;
        }

        // Add lines and mark a selected one.
        for (int i = 0; i < lines.size(); i++)
        {
            QLabel *lab_round = new QLabel(w_base);
            lab_round->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            if (i == line_index)
                lab_round->setObjectName("lab_radio_round_on");
            else
                lab_round->setObjectName("lab_radio_round_off");

            QLabel *lab_text = new QLabel(w_base);
            lab_text->setObjectName("lab_radio_text");
            lab_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            lab_text->setText(QString(" %1").arg(lines[i]));

            QHBoxLayout *lay_item = new QHBoxLayout();
            lay_item->setContentsMargins(0, 0, 0, 0);
            lay_item->setSpacing(0);

            lay_item->addWidget(lab_round);
            lay_item->addWidget(lab_text);

            lay_base->addLayout(lay_item);
        }
    }

    // DEPENDENTCOMBOBOXES
    else if (elem_name == "depcomboboxes")
    {
        auto pair = decor_data.value<DepComboBoxesViewDecorData>();

        QLabel *lab_text1 = elemview->findChild<QLabel*>("lab_combo_text1");
        lab_text1->setText(QString(" %1").arg(pair.first));

        QLabel *lab_text2 = elemview->findChild<QLabel*>("lab_combo_text2");
        lab_text2->setText(QString(" %1").arg(pair.second));
    }

    // SPLITTEDCOMBOBOX
    else if (elem_name == "splitcombobox")
    {
        auto tuple = decor_data.value<SplitComboBoxViewDecorData>();

        QLabel *lab_caption1 = elemview->findChild<QLabel*>("lab_combo_caption1");
        lab_caption1->setText(QString(" %1").arg(std::get<0>(tuple)));

        QLabel *lab_caption2 = elemview->findChild<QLabel*>("lab_combo_caption2");
        lab_caption2->setText(QString(" %1").arg(std::get<1>(tuple)));

        QLabel *lab_text1 = elemview->findChild<QLabel*>("lab_combo_text1");
        lab_text1->setText(QString(" %1").arg(std::get<2>(tuple)));

        QLabel *lab_text2 = elemview->findChild<QLabel*>("lab_combo_text2");
        lab_text2->setText(QString(" %1").arg(std::get<3>(tuple)));
    }

    // DATETIME
    else if (elem_name == "datetimepicker")
    {
        const QString text = decor_data.toString();

        QLabel *lab_text = elemview->findChild<QLabel*>("lab_datetime_text");
        lab_text->setText(text);
    }

    // COUNTER
    else if (elem_name == "counter")
    {
        const QString text = decor_data.toString();

        QLabel *lab_text = elemview->findChild<QLabel*>("lab_counter_text");
        lab_text->setText(text);
    }

    // TABS
    else if (elem_name == "tabs")
    {
        // Clear previous header.
        QWidget *w_headers = elemview->findChild<QWidget*>("w_headers");
        if (w_headers != nullptr)
        {
            lay_base->removeWidget(w_headers);
            delete w_headers;
        }

        // Create new header.
        w_headers = new QWidget(w_base);
        w_headers->setObjectName("w_headers");
        w_headers->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout *lay_headers = new QHBoxLayout(w_headers);
        lay_headers->setContentsMargins(0, 0, 0, 0);
        lay_headers->setSpacing(0);

        QList<QString> headers = decor_data.value<TabsViewDecorData>().first;
        int cur_index = decor_data.value<TabsViewDecorData>().second;

        // Add new headers.
        for (int i = 0; i < headers.size(); i++)
        {
            QLabel *lab_header = new QLabel(w_headers);
            lab_header->setText(headers[i]);
            lab_header->setAlignment(Qt::AlignCenter);
            lab_header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            // Mark current header as highlighted.
            if (i == cur_index)
                lab_header->setObjectName("lab_header_current");
            else
                lab_header->setObjectName("lab_header");

            lay_headers->addWidget(lab_header);
        }

        // Insert header decorations above the stacked widget with containers.
        lay_base->insertWidget(0, w_headers);
    }

    // Add update steps for different elements here.
    // ...
}



