/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  interface for working with NextGIS Web
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

#ifndef NGW_H
#define NGW_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QScrollArea>
#include <QToolButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "json/json.h"

typedef QPair<int,QString> FBNgwResData; // first = id, second = resource type
Q_DECLARE_METATYPE(FBNgwResData);

// NGW API.
#define FB_NGW_WEBGIS_SUFFIX ".nextgis.com"
#define FB_NGW_ITEMTYPE_UNDEFINED "fb_undefined"
#define FB_NGW_ITEMTYPE_RESOURCEGROUP "resource_group"
#define FB_NGW_ITEMTYPE_VECTORLAYER "vector_layer"
#define FB_NGW_ITEMTYPE_POSTGISLAYER "postgis_layer"
#define FB_NGW_ITEMTYPE_LOOKUPTABLE "lookup_table"

// TODO: create special class or set of C methods for using NextGIS Web API.


class FBDialogNgw: public QDialog
{
    Q_OBJECT
    public:
     FBDialogNgw (QWidget *parent);
     virtual ~FBDialogNgw () { }
    protected slots:
     void onConnectClicked ();
     void onSelectClicked ();
//void onCheckboxClick (bool pressed);
     void httpOnItemExpended (QTreeWidgetItem *treeItem);
     void httpOnItemCollapsed (QTreeWidgetItem *treeItem);
     void httpOnItemClicked (QTreeWidgetItem *treeItem, int treeItemColumn);
     void httpReadyAuthRead ();
     void httpReadyRead ();
     void httpReadyResourceRead ();
     void httpReadySelectedRead ();
     void httpAuthFinished ();
     void httpFinished ();
     void httpResourceFinished ();
     virtual void httpSelectedFinished () = 0;
    protected:
     QList<QTreeWidgetItem*> parseJsonReply (QNetworkReply *reply);
     QString getResTypeDispPrefix (QString resType);
    protected:
     // selected params
     QString strUrl;
     QString strUrlName;
     QString strLogin;
     QString strPass;
     QString strId;
     Json::Value jsonLayerMeta;
     // temp variables
     std::string receivedJson;
     QTreeWidgetItem *itemToExpand;
     // http
     QNetworkAccessManager httpManager;
     QNetworkReply *httpAuthReply;
     QNetworkReply *httpReply;
     QNetworkReply *httpResourceReply;
     QNetworkReply *httpSelectedReply;
     // gui
     QVBoxLayout *lMain;
     QLineEdit *wEditUrl;
     QLineEdit *wEditLogin;
     QLineEdit *wEditPass;
     QPushButton *wButConnect;
     QPushButton *wButSelect;
     QTreeWidget *wTree;
     QProgressBar *wProgBar;
     QLabel *wLabelStatus;
     //QCheckBox *chbGuest;
     // other
     QStringList availResTypes;
};


#endif //NGW_H
