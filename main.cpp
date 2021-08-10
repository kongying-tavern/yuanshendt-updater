#include "mainwindow.h"
#include <QApplication>
#include "HTTP.cpp"
#include "MD5.cpp"
#include "file.cpp"
#include <QDebug>
#include <QString>
#include "Sandefine.h"

int main(int argc, char *argv[])
{

    //qDebug()<<"工作路径"<<QString(plwordPath);
    QString tem="";
    //qDebug()<<argc;
    for(int i = 1; i< argc;++i)
    {
        if(i<2)
        {
            tem =QString::fromLocal8Bit(argv[i]);
        }else{
            tem = tem+" "+QString::fromLocal8Bit(argv[i]);
        }

        //qDebug()<<tem;
    }
    qDebug()<<"传参路径："<<tem;

    QApplication a(argc, argv);

    MainWindow w(NULL,tem);
    w.show();
    return a.exec();
}


