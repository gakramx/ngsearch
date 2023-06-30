#include <QCoreApplication>
#include "filehandler.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileHandler mainapp;
    mainapp.run(a.arguments());
    return a.exec();
}
