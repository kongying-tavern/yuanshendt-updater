#include "Json.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

void jsonStr2QSL(QString QS,QStringList &filePath,QStringList &fileMD5)
{

   QByteArray qa;
   QJsonArray jay;

   qa = QS.toUtf8();
   QJsonParseError *error=new QJsonParseError;
   QJsonDocument jdc=QJsonDocument::fromJson(qa,error);
   //判断文件是否完整
   qDebug()<<"isNull:"<<jdc.isNull();
   qDebug()<<"isObject:"<<jdc.isObject();
   qDebug()<<"isArray:"<<jdc.isArray();
   qDebug()<<"isEmpty:"<<jdc.isEmpty();

   jay = jdc.array();
   qDebug()<<"jaysize:"<<jay.size();
   for(int i=0;i<jay.size();i++) {       
       filePath<<jay.at(i).toObject()["filePath"].toString();       
       fileMD5<<jay.at(i).toObject()["MD5"].toString();
   }
}

