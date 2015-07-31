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

    // Свой перевод:
    QTranslator translator;
    translator.load("fb_tr_en");
    a.installTranslator(&translator);

    FB w;
    w.show();

    return a.exec();
}
