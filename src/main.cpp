#include "fb.h"

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>

int main (int argc, char *argv[])
{
    QApplication a(argc, argv);

    // TODO: implement the recognition and setting of the system language firstly.
    //QString langSys = QLocale::system().name();

    // TODO: move this and reading of other settings somewhere else.
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                          "NextGIS", "FormBuilder");
    QString langSet;
    langSet = settings.value("language","en_GB").toString();

    // TODO: how else to get qt translation file suffix?
    QString langSetSys = langSet;
    langSetSys.chop(3);

    QTranslator translatorSys;
    translatorSys.load("qt_" + langSetSys,
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translatorSys);

    QTranslator translator;
    translator.load("fb_" + langSet);
    a.installTranslator(&translator);

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
