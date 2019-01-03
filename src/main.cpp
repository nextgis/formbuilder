#include "fb.h"

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>

#include <QtDebug>

#ifdef Q_OS_MACOS
static void translationPath(const QString &basePath,
                               QList<QString> &localePaths)
{
    QDir baseDir(basePath);
    QStringList filters;
    filters << QStringLiteral("ngstd_*.framework");
    QStringList list = baseDir.entryList(filters);
    foreach (QString subPath, list) {
        const QString &libTrPath = basePath + "/" + subPath +
                "/Resources/translations";
        localePaths.append(libTrPath);
    }
}
#endif // Q_OS_MACOS

int main (int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("FormBuilder");
    a.setOrganizationName("NextGIS");
    a.setOrganizationDomain("com.nextgis");

    // TODO: implement the recognition and setting of the system language firstly.
    //QString langSys = QLocale::system().name();

    // TODO: move this and reading of other settings somewhere else.
#ifdef Q_OS_MACOS
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "NextGIS", "FormBuilder");
#else
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "NextGIS", "FormBuilder");
#endif
    QString langSet;
    langSet = settings.value("language","en_GB").toString();

    QString langSetSys = langSet; // get qt translation file suffix
    langSetSys.chop(3);

    QList<QString> localePaths;
#ifdef Q_OS_MACOS
    const QString &libTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String("/Contents/Resources/translations/");
    localePaths.append(libTrPath);
    localePaths.append(QCoreApplication::applicationDirPath()
            + QLatin1String("/../../Contents/Resources/translations/"));
    translationPath(QCoreApplication::applicationDirPath() +
                       "/Contents/Frameworks/", localePaths);
    translationPath(QCoreApplication::applicationDirPath() +
                       "/../../Contents/Frameworks/", localePaths);
    translationPath(QCoreApplication::applicationDirPath() +
                       "/../Library/Frameworks/", localePaths);
    translationPath(QCoreApplication::applicationDirPath() +
                       "/../../../../Library/Frameworks/", localePaths);
#else
    const QString &libTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String("/../share/translations");
    localePaths.append(libTrPath);
#endif

    localePaths.append(QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    // get qm files list in libTrPath
    QStringList filters;
    filters << QStringLiteral("ngstd_*%1*").arg(langSetSys);
    filters << QStringLiteral("qt_%1*").arg(langSetSys);
    filters << QStringLiteral("qtbase_%1*").arg(langSetSys);
    filters << QStringLiteral("fb_%1*").arg(langSet);
    foreach(QString localePath, localePaths) {
        QDir localeDir(localePath);
        QStringList libTrList = localeDir.entryList(filters);
        foreach (QString trFileName, libTrList) {
            QString loadFile = localeDir.absoluteFilePath(trFileName);
            QTranslator *translator = new QTranslator;
            if (translator->load(loadFile)) {
                qDebug() << "Loaded file " << loadFile; // trFileName << " and folder " << localePath;
                a.installTranslator(translator);
            }
            else {
                delete translator;
            }
        }
    }

    FB w;

    FBForm::initDateTimeFormats();
    FBProject::initEnv();
    w.registerElements();

    w.initGui();
    w.setFbStyle();
    w.show();

    int ret = a.exec();

    w.deregisterElements();
    FBProject::deinitEnv();
    FBForm::deinitDateTimeFormats();

    return ret;
}
