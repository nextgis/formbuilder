#include "fb.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Перевод стандартных строк через Qt.
    // Например в диалогах выбора/открытия/сохранения файлов (QFileDialog).
    // Приходится это делать, т.к. dialog.setLabelText() например для
    // QFileDialog::Reject не работает.
    // http://stackoverflow.com/questions/8906944/qfiledialog-localization
    //QTranslator qtTranslator;
    //qtTranslator.load("qt_" + QLocale::system().name(),
    //        QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    //a.installTranslator(&qtTranslator);

    // TODO: сделать определение и попытку использования системного языка.
    //QString sysLang = QLocale::system().name();

    QSettings settingsIn (QSettings::IniFormat, QSettings::UserScope, "NextGIS", "FormBuilder");
    QString finalLang;
    finalLang = settingsIn.value("last_language_selected","en_GB").toString();

    // TODO: заменить во всём приложении язык по умолчанию с русского на английский (заменить
    // все tr() ).
    QTranslator translator;
    translator.load("fb_" + finalLang);
    a.installTranslator(&translator);

    FB w; // Настройки будут ещё раз считаны в конструкторе, а записаны в деструкторе.
    w.show();

    return a.exec();
}
