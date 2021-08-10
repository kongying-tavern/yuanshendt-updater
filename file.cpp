#include "file.h"
#include "mainwindow.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QCoreApplication>
#include "Start.h"
/*遍历目录*/
void file_search(QString path,QStringList &fileList)
{
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
            //emit tworkProcess(i,clist.size());
            //qDebug()<<"文件:"<<fileInfo.filePath().replace("/","\\");
            fileList<<fileInfo.filePath().replace("/","\\");
        }
        //emit tworkProcess(i,clist.size());
        i++;
    }while(i < clist.size()); //遍历当前目录完毕
    return;
}

/*获取环境变量temp*/
string getTempPath(string path)
{
    QString tem;
    tem = QProcessEnvironment::systemEnvironment().value(QString::fromStdString(path));
    return tem.toStdString();
}
/*创建文件夹*/
bool createFolderSlot(QString path)
{
    bool res;
    QDir dir;
    if (!dir.exists(path))
    {
    res = dir.mkpath(path);
    qDebug() << "新建目录是否成功" << res;
    }else{
        qDebug() << "目录已存在" ;
        res = true;
    }
    return res;
}
/*释放资源文件到目录*/
void saveResourecFile(QString resProfiex,QString resFileName,QString destFullPathFileName)
{
    QString resFile;
    resFile=":/"+resProfiex+"/"+resFileName;
    QFileInfo info(destFullPathFileName);
    createFolderSlot(info.path());
    QFile::copy(resFile,destFullPathFileName);
}
/*读取文件并返回json字符串*/
QString readTXT(QString Path)
{
    qDebug()<<"读取文件"<<Path;
    QFile file(Path.replace("\\","/"));
    QString TXT;
   // qDebug()<<"已被打开"<<file.isOpen();
    if(!file.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Can't open the file!"<<endl;
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
QStringList getUptater(QStringList localFilePath,QStringList localFileMD5,QStringList newFilePath,QStringList newFileMD5)
{
    QStringList needupdater;
    int oldsize = localFileMD5.size();
    int newsize = newFileMD5.size();
    qDebug()<<"新文件"<<newsize<<"旧文件"<<oldsize;
    bool duplicateFile;
    for(int i=0;i<newsize;i++)
    {
        duplicateFile=false;
        qDebug()<<i+1<<"//"<<newsize<<"|"<<newFileMD5.at(i);
        for(int j=0;j<oldsize;j++)
        {
            if(newFileMD5.at(i)==localFileMD5.at(j))
            {
                duplicateFile=true;
                break;
            }
        }
        if(!duplicateFile)
        {
            needupdater<<newFilePath.at(i);
        }        

    }
    /*
    for(int z=0;z<needupdater.size();z++)
    {
        qDebug()<<"需要更新的文件:"<<z+1<<"//"<<newsize<<"|"<<needupdater.at(z);
    }
    */
    return needupdater;
}
bool moveFile(QString oldPath,QString newPath)
{
    oldPath.replace("\\","/");
    newPath.replace("\\","/");
    qDebug()<<"oldPath:"<<oldPath;
    qDebug()<<"newPath:"<<newPath;

    QFile nfile(newPath);

    if(!nfile.remove())
    {
        qDebug()<<"moveFile?.?";
    }
    QFile ofile(oldPath);
    if(ofile.isOpen())
    {
        qDebug()<<"文件占用?.?尝试复制文件";
        return(QFile::copy(oldPath,newPath));

    }else{
        qDebug()<<"尝试移动文件";
        return(QFile::rename(oldPath,newPath));
    }

}
QString dp0()
{
    return QCoreApplication::applicationDirPath();
}

