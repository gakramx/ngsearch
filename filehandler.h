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
     void finished();
private:
    QStringList readSourceFile(const QString &filename);
    void searchFiles(const QString &name, const QString &folder, const QString &copyPath, const QString &movePath, const QString &sourceFile);
    void findFilesRecursive(const QDir &dir, const QStringList &filters, QStringList &foundFiles);
    bool fileContainsName(const QString &filename, const QString &name);
    void copyFile(const QString &filePath, const QString &copyPath);
    void moveFile(const QString &filePath, const QString &movePath);
    void printUsage();

    QString m_copyPath;
    QString m_movePath;
};

#endif // FILEHANDLER_H
