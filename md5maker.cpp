#include "md5maker.h"
#include "ui_md5maker.h"
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>

#include "file.h"
#include "MD5.h"
#include "QFile"
#include "Sandefine.h"
QString md5makerWorkPath;

md5maker::md5maker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::md5maker)
{
    ui->setupUi(this);
}

md5maker::~md5maker()
{
    delete ui;
}

void md5maker::on_pushButton_clicked()
{
    QString dir;
    string tem;
    dir = QFileDialog::getExistingDirectory
            (
                this,
                "空荧酒馆地图安装目录",
                "./",
                QFileDialog::ShowDirsOnly
                );
    tem =dir.toStdString();
    qDebug()<<"目标路径"<<dir;

    if(string(tem)=="")
    {
        qDebug()<<"空路径";
    }else{
        md5makerWorkPath=dir;
    };
    QStringList fileList;
    file_search(md5makerWorkPath,fileList);
    //    qDebug()<<fileList;
    QJsonArray allJson;
    QJsonObject job;
    for(int i=0;i<fileList.size();++i)
    {
        QFile file(fileList.at(i));
        QJsonObject job2;
        job2.insert("Length",file.size());
        job.insert("data",job2);
        job.insert("filePath",QString(fileList.at(i)).replace(md5makerWorkPath+"/",""));
        job.insert("MD5",getFlieMD5(fileList.at(i)));
        job.insert("version",MD5jsonVersion);
        //qDebug()<<job;
        allJson.push_back(job);
    }
    qDebug()<<allJson.size();
//    for (int i = 0; i < allJson.size(); ++i)
//    {
//        qDebug()<<allJson.at(i);
//    }

    for (int a = 0; a < allJson.size(); ++a)
    {
        for(int b = 0; b < allJson.size()-1; ++b)
        {
            if(allJson.at(b).toObject()["data"].toObject()["Length"].toInt()
                    <
                    allJson.at(b+1).toObject()["data"].toObject()["Length"].toInt()
                    )
            {
                QJsonValue jovTemp;
                jovTemp=allJson[b];
                allJson[b]=allJson[b+1];
                allJson[b+1]=jovTemp;
            }
        }
    }
    QJsonDocument doc;
    doc.setArray(allJson);
    QString str=QString::fromUtf8(doc.toJson(QJsonDocument::Compact).constData());
    writeTXT(&str,
             getTempPath("temp")+updaterTempDir+"MD5/",
             "md5.json"
             );
    //qDebug()<<allJson;
    ui->textBrowser->setText(str.replace("},{","}\r\n,\r\n{"));
    //QDesktopServices::openUrl(QUrl(getTempPath("temp")+updaterTempDir+"MD5/md5.json"));

}

