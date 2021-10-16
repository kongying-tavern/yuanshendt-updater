#include "file.h"
#include "mainwindow.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QCoreApplication>

#include <RestartManager.h>
#include <winbase.h>
#include <psapi.h>
#include <stdio.h>

#include "Start.h"
#include "Sandefine.h"
using namespace std;

void file_search(QString path,QStringList &fileList)
{
    /*遍历目录*/
    //path=path.replace("/","\\")+"\\";
    //path =path.replace(" ","%20")+"/";
    //qDebug()<<"开始遍历目标目录:"<<path;

    QDir dir(path);
    if (!dir.exists())
    {
        qDebug()<<dir.exists();
        qDebug()<<dir.isRelative();
        return;
    }

    //取到所有的文件和文件名，但是去掉.和..的文件夹
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    //文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList clist = dir.entryInfoList();
    if(clist.size()< 1 ) {
        return;
    }

    //递归
    int i=0;

    do{
        //此时list带当前目录所有文件的信息,包括文件夹和文件
        QFileInfo fileInfo = clist.at(i);

        if(fileInfo.isDir())
        {
            //文件夹,递归
            //qDebug()<<"文件夹:"<<fileInfo.filePath();
            file_search(fileInfo.filePath(),fileList);
        }else{
            //文件,添加成员

            fileList<<fileInfo.filePath();
        }
        //emit tworkProcess(i,clist.size());
        i++;
    }while(i < clist.size()); //遍历当前目录完毕
    return;
}
/*获取环境变量temp*/
QString getTempPath(QString path)
{
    Start::stlog(modulefile,
                 "获取环境变量"+path,
                 0);
    return QProcessEnvironment::systemEnvironment().value(path).replace("\\","/");
}
/*创建文件夹*/
bool createFolderSlot(QString path)
{
    Start::stlog(modulefile,
                 "尝试新建目录 "+path,
                 0);
    bool res;
    QDir dir;
    if (!dir.exists(path))
    {
        res = dir.mkpath(path);
        qDebug()<<"新建目录"<<path<<res;
        if(res){
            Start::stlog(modulefile,
                         "成功",
                         0);
        }else{
            Start::stlog(modulefile,
                         "失败",
                         0);
        }

    }else{
        qDebug()<<"目录已存在"<<path;
        Start::stlog(modulefile,
                     "目录已存在",
                     0);
        res = true;
    }
    return res;
}
/*释放资源文件到目录*/
void saveResourecFile(QString resProfiex,QString resFileName,QString destFullPathFileName)
{
    //     :/CRT/Resource/crt/curl-ca-bundle.crt

    QString resFile=":/"+resProfiex+"/"+resFileName;
    QFileInfo info(destFullPathFileName);    
    createFolderSlot(info.path());
    Start::stlog(modulefile,
                 "尝试释放资源文件\r\n"+
                 resFile+
                 destFullPathFileName,
                 0);
    if(QFile::copy(resFile,destFullPathFileName))
    {
        Start::stlog(modulefile,
                     "成功",
                     0);
    }else{
        Start::stlog(modulefile,
                     "失败",
                     0);
    }
}
/*读取文本文件并返回字符串*/
QString readTXT(QString Path)
{
    qDebug()<<"读取文件"<<Path;
    QFile file(Path.replace("\\","/"));
    QString TXT;
   // qDebug()<<"已被打开"<<file.isOpen();
    if(!file.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Can't open the file!";
        }
        while(!file.atEnd())
        {
            QByteArray line = file.readLine();
            QString str(line);
            //删除换行符 str.trimmed();
            //qDebug()<<"正在读文件"<< str.trimmed();
            TXT.append(str.trimmed());
        }
    file.close();
    return TXT;
}
/*对比获取需要更新的文件*/
QStringList getUptater(QStringList localFilePath
                       ,QStringList localFileMD5
                       ,QStringList newFilePath
                       ,QStringList newFileMD5
                       ,QString lworkPath=""
                       )//已弃用
{

    /*已弃用*/
    QStringList needupdater;
    int oldsize = localFileMD5.size();
    int newsize = newFileMD5.size();
    qDebug()<<"新文件"<<newsize<<"旧文件"<<oldsize;
    bool duplicateFile;
    for(int i=0;i<newsize;i++)
    {
        duplicateFile=false;
        qDebug()<<i+1<<"/"<<newsize<<"|"<<newFileMD5.at(i)<<newFilePath.at(i);
        for(int j=0;j<oldsize;j++)
        {

            if(newFileMD5.at(i)==localFileMD5.at(j)
                    &&
                    newFilePath.at(i)==QString(localFilePath.at(j)).replace(lworkPath+"/","")
                    )
            {
//                qDebug()<<newFilePath.at(i);
//                qDebug()<<lworkPath;
//                qDebug()<<QString(localFilePath.at(j)).replace(lworkPath+"/","");
                duplicateFile=true;
                break;
            }
        }
        if(!duplicateFile)
        {
            needupdater<<newFilePath.at(i);
        }        

    }

    for(int z=0;z<needupdater.size();z++)
    {
        qDebug()<<"需要更新的文件:"<<z+1<<"/"<<newsize<<"|"<<needupdater.at(z);
    }

    return needupdater;
}
bool moveFile(QString oldPath,QString newPath)
{
    int err;
    Start::stlog(modulefile,
                 "更新文件\t"+oldPath,
                 0);
    Start::stlog(modulefile,
                 "\t"+newPath,
                 0);
    oldPath.replace("\\","/");
    newPath.replace("\\","/");
    qDebug()<<"oldPath:"<<oldPath;
    qDebug()<<"newPath:"<<newPath;
    bool re;

    QFile nfile(newPath);
    if(nfile.exists())//删除目标文件
    {
        Start::stlog(modulefile,
                     "检查文件属性\t",
                     0);
        qDebug()<<"文件为只读";
        SetLastError(err);
        DWORD FileAttributes=GetFileAttributesW((LPCWSTR)newPath.replace("/","\\").unicode());
        if(FileAttributes)
        {
            if (FileAttributes & FILE_ATTRIBUTE_READONLY)
            {
                Start::stlog(modulefile,
                             "文件为只读\tFILE_ATTRIBUTE_READONLY",
                             0);
                qDebug()<<"文件为只读";
                SetLastError(err);
                SetFileAttributesW(
                            (LPCWSTR)newPath.replace("/","\\").unicode()
                            ,FILE_ATTRIBUTE_NORMAL
                            );
                err=GetLastError();
                Start::stlog(modulefile,
                                     "改写文件属性(下只读) "+errcode2str(err),
                                     0);
                qDebug()<<"SetFileAttributesW"<<errcode2str(err);
            }
        }else{
            Start::stlog(modulefile,
                                 "获取文件属性失败"+errcode2str(err),
                                 0);
            qDebug()<<"获取文件属性失败"+errcode2str(err);
        }
        Start::stlog(modulefile,
                             "删除目标文件",
                             0);
        qDebug()<<"删除目标文件";
        SetLastError(err);
        if(!nfile.remove())
        {
            err=GetLastError();
            Start::stlog(modulefile,
                                 "QFile::remove fail "+errcode2str(err),
                                 0);
            qDebug()<<"QFile::remove fail "<<errcode2str(err);
            SetLastError(err);
            if(remove(newPath.replace("/","\\").toLocal8Bit().constData())==-1)
            {
                err=GetLastError();
                Start::stlog(modulefile,
                                     "std::remove fail "+errcode2str(err),
                                     0);
                qDebug()<<"std::remove fail"<<errcode2str(err);
                SetLastError(err);
                if(DeleteFileW((LPCWSTR)newPath.replace("/","\\").unicode())==0)
                {
                    err=GetLastError();
                    Start::stlog(modulefile,
                                         "winAPI remove fail "+errcode2str(err),
                                         0);
                    qDebug()<<"winAPI remove fail"<<errcode2str(err);
                    SetLastError(err);
                    return false;
                }
            }
        }
    }
    QFile ofile(oldPath);
    if(ofile.isOpen())
    {
        Start::stlog(modulefile,
                  "文件占用?.?",
                  0);
        qDebug()<<"文件占用?.?";
        re = false;

    }else{
        SetLastError(err);
        Start::stlog(modulefile,
                  "尝试移动文件",
                  0);
        qDebug()<<"尝试移动文件";
        re = QFile::rename(oldPath,newPath);
        if(!re)
        {
            err=GetLastError();
            Start::stlog(modulefile,
                      "QFile::rename file "+errcode2str(err),
                      0);
            qDebug()<<"宽字符方法";
            int reint;
            SetLastError(err);
            reint = rename(
                        oldPath.replace("/","\\").toLocal8Bit().constData(),
                        newPath.replace("/","\\").toLocal8Bit().constData()
                        );
            err=GetLastError();
            qDebug()<<"std::rename "<<reint<<errcode2str(err);
            if(reint==0)
            {
                re=true;
            }else{
                Start::stlog(modulefile,
                          "std::rename fail "+errcode2str(err),
                          0);
            }
        }
    }
    if(re)
    {
        Start::stlog(modulefile,
                     "成功--------------\r\n",
                     0);
    }else{
        Start::stlog(modulefile,
                     "失败--------------\r\n",
                     0);
    }

    return re;
}
bool fileIsOpen(QString filePath)
{

}

QString dp0()
{
    return QCoreApplication::applicationDirPath();
}

