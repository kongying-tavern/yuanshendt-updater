#include "file.h"
#include "mainwindow.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QCoreApplication>



#include "Start.h"
#include "Sandefine.h"
using namespace std;

void file_search(QString path,QStringList &fileList)//已弃用
{
    /*已弃用*/
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
QString getTempPath(char* path)
{
    return QProcessEnvironment::systemEnvironment().value(QString(path)).replace("\\","/");
}
/*创建文件夹*/
bool createFolderSlot(QString path)
{
    bool res;
    QDir dir;
    if (!dir.exists(path))
    {
        res = dir.mkpath(path);
        qDebug()<<"新建目录"<<path<<res;
    }else{
        qDebug()<<"目录已存在"<<path;
        res = true;
    }
    return res;
}
/*释放资源文件到目录*/
void saveResourecFile(QString resProfiex,QString resFileName,QString destFullPathFileName)
{
    //:/CRT/Resource/crt/curl-ca-bundle.crt
    QString resFile=":/"+resProfiex+"/"+resFileName;
    QFileInfo info(destFullPathFileName);
    createFolderSlot(info.path());
    QFile::copy(resFile,destFullPathFileName);
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
    oldPath.replace("\\","/");
    newPath.replace("\\","/");
    qDebug()<<"oldPath:"<<oldPath;
    qDebug()<<"newPath:"<<newPath;
    bool re;

    QFile nfile(newPath);
    if(nfile.exists())//删除目标文件
    {
        qDebug()<<"删除目标文件";
        int err;
        SetLastError(err);

        if(!nfile.remove())
        {
            err=GetLastError();
            qDebug()<<"QFile::remove fail"<<errcode2str(err);
            SetLastError(err);
            if(remove(newPath.replace("/","\\").toLocal8Bit().constData())==-1)
            {
                err=GetLastError();
                qDebug()<<"std::remove fail"<<errcode2str(err);
                SetLastError(err);
                if(DeleteFileW((LPCWSTR)newPath.replace("/","\\").unicode())==0)
                {
                    err=GetLastError();
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
        qDebug()<<"文件占用?.?";
        return false;

    }else{

        qDebug()<<"尝试移动文件";
        re = QFile::rename(oldPath,newPath);
        if(!re)
        {
            qDebug()<<"?";
            int reint;
            int err=GetLastError();
            reint = rename(
                        oldPath.replace("/","\\").toLocal8Bit().constData(),
                        newPath.replace("/","\\").toLocal8Bit().constData()
                        );
            qDebug()<<"std::rename"<<reint<<errcode2str(err);
            if(reint==0)
            {
                re=true;
            }
        }
    }
    return re;
}
QString dp0()
{
    return QCoreApplication::applicationDirPath();
}

