#ifndef FILE_H
#define FILE_H
#include<iostream>
#include<string>
#include<io.h>
#include <QString>
#include <QStringList>
#include <QFile>
using namespace std;

class file
{


};

void file_search(QString path,
                 QStringList &fileList
                 );

QString getTempPath(QString path);
bool createFolderSlot(QString path);
void saveResourecFile(QString resProfiex,
                      QString resFileName,
                      QString destFullPathFileName
                      );
QString readTXT(QString Path);
QStringList getUptater(QStringList localFilePath,
                       QStringList localFileMD5,
                       QStringList newFilePath,
                       QStringList newFileMD5,
                       QString lworkPath
                       );
bool moveFile(QString oldPath,
              QString newPath
              );
bool fileIsOpen(QString filePath);

QString dp0();
#endif // FILE_H
