#include "fb.h"

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>

int main (int argc, char *argv[])
{
    QApplication a(argc, argv);

    // TODO: do the recognition and setting of the system language firstly.
    //QString langSys = QLocale::system().name();
/*    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                          "NextGIS", "FormBuilder");
    QString langSet;
    langSet = settings.value(,).toString();
    QTranslator translator;
    translator.load("fb_" + langSet);
    a.installTranslator(&translator); */

    FB w;

    FBForm::initEnv();
    FBProject::initEnv();
    FBFactory::initAll(&w);

    w.initGui();
    w.setFbStyle();

    w.show();
    int ret = a.exec();

    FBFactory::deinitAll();
    FBProject::deinitEnv();
    FBForm::deinitEnv();

    return ret;
}
