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

#include "tabs_view.h"

using namespace Fb;
using namespace Mockup;


TabsView::TabsView (const Core::Elem *elem):
    ElemView(elem)
{
    stack_main = new QStackedWidget(w_base);
    stack_main->setObjectName("stack_main");

    lay_base->addWidget(stack_main);

    Core::Attr *attr_hdrs = elem->getAttrs().value("page_hdrs");
    auto hdrs = attr_hdrs->getValueAsVar().toStringList();
    for (auto hdr: hdrs)
    {
        headers.append(hdr);
        this->appendPage();
    }

    // Ensure that we will have at least 1 page with container.
    if (headers.size() == 0)
    {
        //qDebug
        headers.append("");
    }

    Core::Attr *attr_cur_page = elem->getAttrs().value("cur_page");
    cur_container_index = this->getPageIndex(attr_cur_page);
}

TabsView::~TabsView ()
{
}


void TabsView::appendAllInnerElemviews (QList<ElemView*> &list)
{
    for (int j = 0; j < containers.size(); j++)
    {
        QLayout *lay = containers[j]->layout();
        for (int i = 0; i < lay->count(); i++)
        {
            QLayoutItem *item = lay->itemAt(i);
            if (item == nullptr)
                continue;
            ElemView *elemview = qobject_cast<ElemView*>(item->widget());
            if (elemview == nullptr)
                continue;

            list.append(elemview);
            elemview->appendAllInnerElemviews(list);
        }
    }
}


const Container *TabsView::getContainer () const
{
    return containers[cur_container_index]; // we see and operate only one container at a time
}

const QVariant TabsView::getDataForDecor () const
{
    return QVariant::fromValue(qMakePair(headers, cur_container_index));
}


void TabsView::atElemChange (Core::Attr *attr)
{
    // Devide the reaction on attribute changes so not to load a list of items each time user
    // clicks the "current page" attribute's spinbox in GUI:

    // a) "Current page" attr changed: just show the current page.
    if (attr != nullptr && attr->getKeyName() == "cur_page")
    {
        cur_container_index = this->getPageIndex(attr);
        stack_main->setCurrentIndex(cur_container_index);
    }

    // b) "Tabs headers" attr changed: check if the count of new headers more (then add new void
    // pages) or less (then remove deleted pages) the count of existed headers. Otherwise just do
    // not touch existed pages. An according warning message should be already shown to the user.
    else
    {
        Core::Attr *attr_hdrs = elem->getAttrs().value("page_hdrs");
        Core::Attr *attr_cur_page = elem->getAttrs().value("cur_page");

        QList<QString> new_headers = attr_hdrs->getValueAsVar().toStringList();

        int old_size = headers.size();
        int new_size = new_headers.size();
        int diff = new_size - old_size;

        if (diff > 0)
            for (int i = 0; i < abs(diff); i++)
                this->appendPage();
        else if (diff < 0)
            for (int i = 0; i < abs(diff); i++)
                this->removeLastPage();

        // Current index and its new range has been already changed in Attr.
        cur_container_index = this->getPageIndex(attr_cur_page);
        stack_main->setCurrentIndex(cur_container_index);

        headers = new_headers;
    }
}


void TabsView::appendPage ()
{
    QWidget *widget = new QWidget();
    widget->setObjectName("stack_page");

    // Warning. Do not set min-height for this widget or for the whole stack-widget in style sheets.
    // Otherwise pages of the stack widget will not be correctly extended by its contents.
    // TODO: think about a way how to fix this.

    QVBoxLayout *lay_widget = new QVBoxLayout(widget);
    lay_widget->setContentsMargins(0, 0, 0, 0);
    lay_widget->setSpacing(0);

    Container *container = new Container(widget, ContainerType::Column, this);
    lay_widget->addWidget(container);

    stack_main->addWidget(widget);

    containers.append(container);
}

void TabsView::removeLastPage ()
{
    if (containers.isEmpty())
        return;
    containers.removeLast();

    QWidget *widget = stack_main->widget(stack_main->count() - 1);
    stack_main->removeWidget(widget);
    widget->setParent(nullptr);
    delete widget;
}

int TabsView::getPageIndex (Core::Attr *attr_cur_page)
{
    int index = attr_cur_page->getValueAsVar().toInt();
    if (index < 1 || index > headers.size())
    {
        //qDebug
        index = 1;
    }
    return index - 1; // - 1 because "cur_page" starts from 1
}



