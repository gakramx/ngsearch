#include <QCoreApplication>
#include "filehandler.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileHandler mainapp(&a);
    QObject::connect(&mainapp, SIGNAL(finished()), &a, SLOT(quit()), Qt::QueuedConnection);
    mainapp.run(a.arguments());

    return a.exec();
}
