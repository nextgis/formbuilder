/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  basic form implementations
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "form.h"


FBElem *FBForm::SELECTED = NULL;
bool FBForm::IS_SELECTED_MOVING = false;

FBForm::~FBForm ()
{

}

FBForm::FBForm ()
{
    /*
            // Экран:
        wScrollArea = new QScrollArea(ui->centerWidget);
        layCenter->addWidget(wScrollArea);
        wScrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        connect(wScrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
                this, SLOT(moveScrollBarToBottom(int, int)));
        wScrollArea->setStyleSheet("QScrollArea{border: 0px;}");
        widScreen = new QWidget();
        widScreen->setStyleSheet("QWidget{background-color:rgba(0,0,0,0);}");
        wScrollArea->setWidget(widScreen);
        wScrollArea->setWidgetResizable(true);
        layScreen = new QVBoxLayout(widScreen);
        layScreen->setContentsMargins(0,0,0,0);
        layScreen->setSpacing(0);

        // В самом начале мы добавляем всего один виджет для вставки элементов.
        FBInsertWidget *insWidget = new FBInsertWidget(widScreen);
        layScreen->addWidget(insWidget);

        // Добавляем в конец раскладки растягивающий не-виджет элемент. Не забываем
        // об этом далее, т.к. будет просмотр в цикле всех элементов в раскладке
        // и преобразование к QWidget, тогда как этот элемент - не виджет!
        // Все последующие элементы-виджеты добавлять ПЕРЕД этим элементом.
        layScreen->addStretch();
        */
}
