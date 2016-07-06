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

    // It seems that it is ok to pass this filename+path in order to load system
    // translations in Windows from the current dir and in Linux from the system dir.
    // TODO: check this.
    QString sysTransName = "qt_" + langSetSys;
    QString sysTransPath = "";
    #ifdef FB_SYSTRANSLATIONS_IN_SHARE
    sysTransPath = FB_PATH_TRANSLATIONS + QString("/");
    #endif
    QTranslator translatorSys;
    if (!translatorSys.load(sysTransName,sysTransPath))
        translatorSys.load(sysTransName,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translatorSys);

    // For program translations we always have the same path for Linux and for Windows
    // if the program installed correctly.
    QTranslator translator;
	translator.load(FB_PATH_TRANSLATIONS + QString("/fb_") + langSet);
    a.installTranslator(&translator);

    FB w;

    FBForm::initDateTimeFormats();
    FBProject::initEnv();
    w.registerElements();
    //w.registerScreens();

    w.initGui();
    w.setFbStyle();
    w.show();

    int ret = a.exec();

    //w.deregisterScreens();
    w.deregisterElements();
    FBProject::deinitEnv();
    FBForm::deinitDateTimeFormats();

    return ret;
}
