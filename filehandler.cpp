#include "filehandler.h"
#include <QCoreApplication>
#include <QRegularExpression>
FileHandler::FileHandler(QObject *parent)
    : QObject(parent)
{

}

void FileHandler::run(const QStringList &arguments)
{

    if (arguments.size() < 4) {
        printUsage();
        return;
    }

    QString sourceFile;
    QString distanceFolder;

    // Parse the command line arguments
    for (int i = 1; i < arguments.size(); i += 2) {
        QString arg = arguments.at(i);
        QString value = arguments.value(i + 1);

        if (arg == "-s") {
            sourceFile = value;
        } else if (arg == "-d") {
            distanceFolder = value;
        } else if (arg == "-c") {
            if (value != "-m") {
                if (i + 1 < arguments.size() && !arguments.at(i + 1).startsWith("-")) {
                    m_copyPath = arguments.at(i + 1);
                    ++i;
                } else {
                    m_copyPath = QFileInfo(sourceFile).absolutePath();
                }
            } else {
                m_movePath.clear(); // Clear move path if -m is provided
            }
        } else if (arg == "-m") {
            if (value != "-c") {
                if (i + 1 < arguments.size() && !arguments.at(i + 1).startsWith("-")) {
                    m_movePath = arguments.at(i + 1);
                    ++i;
                } else {
                    m_movePath = QFileInfo(sourceFile).absolutePath();
                }
            } else {
                m_copyPath.clear(); // Clear copy path if -c is provided
            }
        }
    }

    if (sourceFile.isEmpty() || distanceFolder.isEmpty()) {
        printUsage();


        return;
    }

    QStringList names = readSourceFile(sourceFile);

    for (const QString &name : names) {
        searchFiles(name, distanceFolder, m_copyPath, m_movePath, sourceFile);
    }

    emit finished();
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

void FileHandler::searchFiles(const QString &name, const QString &folder, const QString &copyPath, const QString &movePath, const QString &sourceFile)
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
    QFile source(sourceFile);
    if (!source.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open source file:" << sourceFile;
        return;
    }

    QTextStream sourceIn(&source);
    QString sourceText = sourceIn.readAll();
    source.close();

    int count = sourceText.count(name, Qt::CaseInsensitive);

    for (const QString &file : foundFiles) {
        if (fileContainsName(file, name)) {
            qInfo() << "Found " << count << "\033[32m" <<name<< "\033[0m"<< "In :" << file;

            if (!copyPath.isEmpty()) {
                copyFile(file, copyPath);
            }

            if (!movePath.isEmpty()) {
                moveFile(file, movePath);
            }
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
        QRegularExpression re(QString("\\b%1\\b").arg(QRegularExpression::escape(name)), QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            file.close();
            return true;
        }
    }


    file.close();
    return false;
}

void FileHandler::copyFile(const QString &filePath, const QString &copyPath)
{
    QFileInfo fileInfo(filePath);
    QString destinationPath;

    if (copyPath.endsWith('/') || copyPath.endsWith('\\')) {
        destinationPath = copyPath + fileInfo.fileName();
    } else {
        QDir dir(copyPath);
        destinationPath = dir.filePath(fileInfo.fileName());
    }

    QFile::copy(filePath, destinationPath);
}

void FileHandler::moveFile(const QString &filePath, const QString &movePath)
{
    QFileInfo fileInfo(filePath);
    QString destinationPath;

    if (movePath.endsWith('/') || movePath.endsWith('\\')) {
        destinationPath = movePath + fileInfo.fileName();
    } else {
        QDir dir(movePath);
        destinationPath = dir.filePath(fileInfo.fileName());
    }

    QFile::rename(filePath, destinationPath);
}

void FileHandler::printUsage()
{
    qDebug() << "Usage: ngsearch -s source.txt -d /folder/distance/ [-c [copyPath]] [-m [movePath]]";
    qDebug() << "-c: Copy found files. Optional: copyPath specifies the destination path";
    qDebug() << "-m: Move found files. Optional: movePath specifies the destination path";
    emit finished();
}
