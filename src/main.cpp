#include "fb.h"
#include <QApplication>

int main (int argc, char *argv[])
{
    QApplication a(argc, argv);
    FB w;
    FBProject::init();
    FBFactory::initAll(&w);
    w.initGui();
    w.setFbStyle();
    w.show();
    int ret = a.exec();
    FBFactory::deinitAll();
    FBProject::deinit();
    return ret;
}
