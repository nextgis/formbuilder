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
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "NextGIS", "FormBuilder");
    QString langSet;
    langSet = settings.value("language","en_GB").toString();

    QString langSetSys = langSet; // get qt translation file suffix
    langSetSys.chop(3);

    // TODO: maybe we need to load more files with system translations? See e.g.
    // C:\Qt\5.8\msvc2013\translations for Windows.
    QString sysTransName = "qtbase_" + langSetSys;
    QTranslator translatorSys;
    translatorSys.load(sysTransName,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translatorSys); // will use qt.conf for file paths if it exists

    // For program translations we always have the same path for Linux and for Windows
    // if the program installed correctly.
    QTranslator translator;
	translator.load(FB_PATH_TRANSLATIONS + QString("/fb_") + langSet);
    a.installTranslator(&translator);

    FB w;

    FBForm::initDateTimeFormats();
    FBProject::initEnv();
    w.registerElements();

    w.initGui();
    w.setFbStyle();
    w.show();

    w.startInitialAuthentication();

    int ret = a.exec();

    w.deregisterElements();
    FBProject::deinitEnv();
    FBForm::deinitDateTimeFormats();

    return ret;
}
