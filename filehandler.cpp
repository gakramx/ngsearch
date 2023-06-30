#include "filehandler.h"

FileHandler::FileHandler(QObject *parent)
    : QObject{parent}
{

}
void FileHandler::run(const QStringList &arguments)
{
    if (arguments.size() < 5) {
        printUsage();
        return;
    }

    QString sourceFile;
    QString distanceFolder;

    // Parse the command line arguments
    for (int i = 1; i < arguments.size(); i += 2) {
        QString arg = arguments.at(i);
        QString value = arguments.at(i + 1);

        if (arg == "-s") {
            sourceFile = value;
        } else if (arg == "-d") {
            distanceFolder = value;
        }
    }

    if (sourceFile.isEmpty() || distanceFolder.isEmpty()) {
        printUsage();
        return;
    }

    QStringList names = readSourceFile(sourceFile);

    for (const QString &name : names) {
        searchFiles(name, distanceFolder);
    }
}
QStringList FileHandler::readSourceFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open source file:" << filename;
        return QStringList();
    }

    QTextStream in(&file);
    QStringList names;

    while (!in.atEnd()) {
        QString name = in.readLine().trimmed();
        if (!name.isEmpty()) {
            names.append(name);
        }
    }

    file.close();
    return names;
}
void FileHandler::searchFiles(const QString &name, const QString &folder)
{
    QDir dir(folder);

    if (!dir.exists()) {
        qWarning() << "Distance folder does not exist:" << folder;
        return;
    }

    QStringList fileFilters;
    fileFilters << "*";

    QStringList foundFiles;
    findFilesRecursive(dir, fileFilters, foundFiles);

    for (const QString &file : foundFiles) {
        if (fileContainsName(file, name)) {
            qInfo() << "Found" <<"\033[32m"<<name<<"\033[0m"<<"in :"<< file;
        }
    }
}
void FileHandler::findFilesRecursive(const QDir &dir, const QStringList &filters, QStringList &foundFiles)
{
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

    for (const QFileInfo &fileInfo : fileList) {
        foundFiles.append(fileInfo.filePath());
    }
    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &dirInfo : dirList) {
        QDir subDir(dirInfo.filePath());
        findFilesRecursive(subDir, filters, foundFiles);
    }
}
bool FileHandler::fileContainsName(const QString &filename, const QString &name)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filename;
        return false;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains(name, Qt::CaseInsensitive)) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}
void FileHandler::printUsage()
{
    qDebug() << "Usage: myapp -s source.txt -d /folder/distance/";
}


