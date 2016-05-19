#include "fb.h"
#include <QApplication>

int main (int argc, char *argv[])
{
    QApplication a(argc, argv);
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
