#include "fb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FBProject::init();

    FB w; 
    w.show();

    return a.exec();
}
