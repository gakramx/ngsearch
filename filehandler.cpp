#include "filehandler.h"
#include <QCoreApplication>
#include <QRegularExpression>
#include <QFileInfo>
FileHandler::FileHandler(QObject *parent)
    : QObject(parent)
{

}

void FileHandler::run(const QStringList &arguments)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("File Handler Application");
    parser.addHelpOption();

    // Add options
    // Add options with the correct names
    QCommandLineOption sourceOption("txt", "Specify the text file", "sourceFile");
    parser.addOption(sourceOption);

    QCommandLineOption searchOption("source", "Specify the source folder", "sourceFolder");
    parser.addOption(searchOption);

    QCommandLineOption copyOption("cp", "Copy files to the specified path", "copyPath");
    parser.addOption(copyOption);

    QCommandLineOption moveOption("mv", "Move files to the specified path", "movePath");
    parser.addOption(moveOption);

    QCommandLineOption overwriteOption("ow", "Overwrite if the file already exists2");
    parser.addOption(overwriteOption);

    QCommandLineOption renameOption("re", "Rename if the file already exists");
    parser.addOption(renameOption);
    if (arguments.isEmpty()) {

        qDebug() << "No arguments provided.";
        parser.showHelp(1);
        emit finished();
        return;
    }

    // Process the command-line arguments
    if (!parser.parse(arguments)) {
        qDebug() << "Failed to parse arguments:" << parser.errorText();
        parser.showHelp(1);
        emit finished();
        return;
    }
    // Add debug outputs to check the parsed values
    qDebug() << "Parsed arguments:" << arguments;
    qDebug() << "Source option:" << parser.value("txt");
    qDebug() << "Search option:" << parser.value("source");
    qDebug() << "Copy option:" << parser.value("cp");
    qDebug() << "Move option:" << parser.value("mv");
    qDebug() << "Overwrite option:" << parser.isSet("ow");
    qDebug() << "Rename option:" << parser.isSet("re");


    QString sourceFile = parser.value(sourceOption);
    QString distanceFolder = parser.value(searchOption);
    m_copyPath = parser.value(copyOption);
    m_movePath = parser.value(moveOption);

    bool overwrite = parser.isSet(overwriteOption);
    bool rename = parser.isSet(renameOption);
    if(overwrite==false&&rename==false){
        parser.showHelp(1);
        emit finished();
        return;
    }

    // Check if required options are provided
    if (sourceFile.isEmpty() || distanceFolder.isEmpty()) {
        parser.showHelp(1);
        emit finished();
        return;
    }

    // Set default copy and move paths if necessary
    if (m_copyPath.isEmpty() && m_movePath.isEmpty()) {
        parser.showHelp(1);
        emit finished();
        return;
    }

    QStringList names = readSourceFile(sourceFile);

    for (const QString &name : names) {
        searchFileNames(name, distanceFolder, overwrite, rename);
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

void FileHandler::searchFiles(const QString &name, const QString &folder, const QString &copyPath, const QString &movePath, const QString &sourceFile , bool overwrite, bool rename)
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
                copyFile(file, copyPath, overwrite, rename);
            }

            if (!movePath.isEmpty()) {
                moveFile(file, movePath, overwrite, rename);
            }
        }
    }
}
void FileHandler::searchFileNames(const QString &name, const QString &folder, bool overwrite, bool rename){

    QDir dir(folder);

    if (!dir.exists()) {
        qWarning() << "Distance folder does not exist:" << folder;
        return;
    }

    QStringList fileFilters;
    fileFilters << "*";

    QStringList foundFiles;
    findFilesRecursive(dir, fileFilters, foundFiles);

    QStringList patterns;
    QStringList normalWords;

    if (name.contains(',')) {
        patterns = name.split(',', Qt::SkipEmptyParts);
    } else {
        normalWords.append(name);
    }

    for (const QString &pattern : patterns) {
        QStringList patternWords = pattern.split(',', Qt::SkipEmptyParts);
        QStringList matchingFiles = foundFiles; // Initialize a new list for each pattern

        for (const QString &word : patternWords) {
            QString cleanedWord = word.trimmed();
            cleanedWord.remove(QRegularExpression("[\"'\\(\\)\\[\\]\\{\\}]"));
            QRegularExpression re(QString("\\b%1\\b").arg(QRegularExpression::escape(cleanedWord)), QRegularExpression::CaseInsensitiveOption);

            for (const QString &file : foundFiles) {
                QString fileName = QFileInfo(file).fileName();
                QRegularExpressionMatch match = re.match(fileName);

                if (!match.hasMatch()) {
                    matchingFiles.removeAll(file);
                }
            }
        }

        for (const QString &file : matchingFiles) {
            QString fileName = QFileInfo(file).fileName();
            qInfo() << "Found file name" << "\033[32m" << fileName << "\033[0m" << "in:" << file;

            if (!m_copyPath.isEmpty()) {
                copyFile(file, m_copyPath, overwrite, rename);
            }

            if (!m_movePath.isEmpty()) {
                moveFile(file, m_movePath, overwrite, rename);
            }
        }
    }

    // Process normal words separately
    for (const QString &word : normalWords) {
        QString cleanedWord = word.trimmed();
        cleanedWord.remove(QRegularExpression("[\"'\\(\\)\\[\\]\\{\\}]"));
        QRegularExpression re(QString("\\b%1\\b").arg(QRegularExpression::escape(cleanedWord)), QRegularExpression::CaseInsensitiveOption);

        QStringList matchingFiles;

        for (const QString &file : foundFiles) {
            QString fileName = QFileInfo(file).fileName();
            QRegularExpressionMatch match = re.match(fileName);

            if (match.hasMatch()) {
                matchingFiles.append(file);
            }
        }

        for (const QString &file : matchingFiles) {
            QString fileName = QFileInfo(file).fileName();
            qInfo() << "Found file name" << "\033[32m" << fileName << "\033[0m" << "in:" << file;

            if (!m_copyPath.isEmpty()) {
                copyFile(file, m_copyPath, overwrite, rename);
            }

            if (!m_movePath.isEmpty()) {
                moveFile(file, m_movePath, overwrite, rename);
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

void FileHandler::copyFile(const QString &filePath, const QString &copyPath, bool overwrite, bool rename)
{
    QFileInfo fileInfo(filePath);
    QString destinationPath;
    qDebug()<<"Works";
    if (copyPath.endsWith('/') || copyPath.endsWith('\\')) {
        destinationPath = copyPath + fileInfo.fileName();
    } else {
        QDir dir(copyPath);
        destinationPath = dir.filePath(fileInfo.fileName());
    }

    if (!overwrite && !rename) {
        qDebug()<<"Noo";
        // Don't perform any additional actions, simply copy the file
        QFile::copy(filePath, destinationPath);
        return;
    }

    if (overwrite && QFile::exists(destinationPath)) {
        qDebug()<<"overwrite";
        // Delete the existing file before copying
        QFile::remove(destinationPath);
    }

    if (rename) {
        qDebug()<<"rename";
        // Check if the destination file already exists
        int suffix = 1;
        QString baseName = fileInfo.baseName();
        QString suffixStr;

        while (QFile::exists(destinationPath)) {
            suffixStr = QString("_%1").arg(suffix);
            destinationPath = copyPath + baseName + suffixStr + "." + fileInfo.suffix();
            suffix++;
        }
    }

    // Copy the file to the destination
    QFile::copy(filePath, destinationPath);
}

void FileHandler::moveFile(const QString &filePath, const QString &movePath, bool overwrite, bool rename)
{
    QFileInfo fileInfo(filePath);
    QString destinationPath;

    if (movePath.endsWith('/') || movePath.endsWith('\\')) {
        destinationPath = movePath + fileInfo.fileName();
    } else {
        QDir dir(movePath);
        destinationPath = dir.filePath(fileInfo.fileName());
    }

    if (!overwrite && !rename) {
        // Don't perform any additional actions, simply move the file
        QFile::rename(filePath, destinationPath);
        return;
    }

    if (overwrite && QFile::exists(destinationPath)) {
        // Delete the existing file before moving
        QFile::remove(destinationPath);
    }

    if (rename) {
        // Check if the destination file already exists
        int suffix = 1;
        QString baseName = fileInfo.baseName();
        QString suffixStr;

        while (QFile::exists(destinationPath)) {
            suffixStr = QString("_%1").arg(suffix);
            destinationPath = movePath + baseName + suffixStr + "." + fileInfo.suffix();
            suffix++;
        }
    }

    // Move the file to the destination
    QFile::rename(filePath, destinationPath);
}

void FileHandler::printUsage()
{
    qDebug() << "Usage: ngsearch --source-file source.txt --search-folder /folder/distance/ [--copy [copyPath]] [--move [movePath]] [-o] [-r]";
    qDebug() << "--copy: Copy found files. Optional: copyPath specifies the destination path";
    qDebug() << "--move: Move found files. Optional: movePath specifies the destination path";
    qDebug() << "-o: Overwrite existing files when copying/moving";
    qDebug() << "-r: Rename files with a suffix if destination file already exists";
    emit finished();
}
