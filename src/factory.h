/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  factories for virtual creation
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

#ifndef FACTORY_H
#define FACTORY_H

#include <QTreeWidget>
#include <QToolButton>

#include "project.h"
#include "form.h"
#include "tool.h"


class FBRegistrar
{
    public:
     static void registerAllFactories ();
     void registerFactory (FBFactory fct);
    private:
     QList<FBFactoryProject*> fctsProj;
     QMap<QString,FBFactoryElem*> fctsElem; // key = elem's keyname
     QList<FBFactoryTool*> fctsTool;
};


class FBFactory
{
    public:

};

class FBFactoryButton: public FBFactory
{
    public:
     virtual QToolButton *createButton ();
};

class FBFactoryProject: public FBFactoryButton
{
    public:
     FBFactoryProject (QString imgPath, QString name);
     virtual FBProject *create () = 0;

    protected:

};

class FBFactoryElem: public FBFactory
{
    public:
     FBFactoryElem (bool isFull, QString keyName, QString keyGroupName,
                    QString imgPath, QString name);
     virtual FBElem *create () = 0;
     virtual QTreeWidgetItem *createTreeItem ();

    protected:

};

class FBFactoryTool: public FBFactoryButton
{
    public:
     FBFactoryTool (QString imgPath, QString name);
     virtual QToolButton *createButton (); // returns FBToolButton

    protected:

};



#endif //FACTORY_H
