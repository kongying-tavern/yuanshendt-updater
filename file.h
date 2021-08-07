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

void file_search(QString path,QStringList &fileList);
string getTempPath(string path);
bool createFolderSlot(QString path);
void saveResourecFile(QString resProfiex,QString resFileName,QString destFullPathFileName);
QString readTXT(QString Path);
QStringList getUptater(QStringList localFilePath,QStringList localFileMD5,QStringList newFilePath,QStringList newFileMD5);
bool moveFile(QString oldPath,QString newPath);
QString dp0();
#endif // FILE_H
