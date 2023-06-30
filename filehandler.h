#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QTextStream>
#include <QDebug>
class FileHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileHandler(QObject *parent = nullptr);
    void run(const QStringList &arguments);

signals:

private:
    QStringList readSourceFile(const QString &filename);
    void searchFiles(const QString &name, const QString &folder);
    void findFilesRecursive(const QDir &dir, const QStringList &filters, QStringList &foundFiles);
    bool fileContainsName(const QString &filename, const QString &name);
    void printUsage();



};

#endif // FILEHANDLER_H
