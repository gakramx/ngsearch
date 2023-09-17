#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QCommandLineParser>
class FileHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileHandler(QObject *parent = nullptr);
    void run(const QStringList &arguments);
signals:
    void finished();
private:
    QStringList readSourceFile(const QString &filename);
    void findFilesRecursive(const QDir &dir, const QStringList &filters, QStringList &foundFiles);
    void searchFileNames(const QString &name, const QString &folder, bool overwrite, bool rename, bool partial);
    bool fileContainsName(const QString &filename, const QString &name);
    void copyFile(const QString &filePath, const QString &copyPath, bool overwrite, bool rename);
    void moveFile(const QString &filePath, const QString &movePath, bool overwrite, bool rename);

    QString m_copyPath;
    QString m_movePath;
};

#endif // FILEHANDLER_H
