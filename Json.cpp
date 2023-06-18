#include "Json.h"
#include <QDebug>


#include "Sandefine.h"
#include <Start.h>
#include <iostream>

JSON::JSON(QObject *parent)
{

}
JSON::~JSON()
{

}
bool JSON::jsonStr2QSL(QString QS,QStringList &filePath,QStringList &fileMD5)
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
   if(jdc.isNull() || error->error != QJsonParseError::NoError)
   {
       tp->stlog(moduleJson,"json基础类型:Null",0);
       tp->stlog(moduleJson,error->errorString(),0);//待验证
   }
   if(jdc.isObject()){
       tp->stlog(moduleJson,"json基础类型:Object",0);
   }
   if(jdc.isArray()){
       tp->stlog(moduleJson,"json基础类型:Array",0);
   }
   if(jdc.isEmpty()){
       tp->stlog(moduleJson,QString("json基础类型:Empty"),0);
       return false;
   }
   jay = jdc.array();
   qDebug()<<"jaysize:"<<jay.size();
   for(int i=0;i<jay.size();i++)
   {
       tp->stlog(moduleJson,
                    QString::number(i+1)+"/"+QString::number(jay.size())+"\t"+
                    jay.at(i).toObject()["MD5"].toString()+"\t"+
                    jay.at(i).toObject()["filePath"].toString(),
                    0);
       filePath<<jay.at(i).toObject()["filePath"].toString();
       fileMD5<<jay.at(i).toObject()["MD5"].toString();
       emit tp->changeMainPage0label_Text("正在解析云端MD5\t"+QString::number(i)+"|"+QString::number(jay.size()));
       emit tp->tworkProcess(i,jay.size());
   }
   return true;
}

