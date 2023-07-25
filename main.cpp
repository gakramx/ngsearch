#include <QCoreApplication>
#include "filehandler.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileHandler mainapp(&a);
    QObject::connect(&mainapp, SIGNAL(finished()), &a, SLOT(quit()), Qt::QueuedConnection);

    QStringList arguments = a.arguments();
   // arguments.takeFirst(); // Remove the application name from the arguments list

    mainapp.run(arguments);

    return a.exec();
}
