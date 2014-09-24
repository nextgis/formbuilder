#include "formbuilder.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FormBuilder w;
    w.show();

    return a.exec();
}
