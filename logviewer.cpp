#include "logviewer.h"
#include "ui_logviewer.h"

#include <windows.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextDocument>
#include <QEvent>

#include "Sandefine.h"

rectangle rectangle;

logViewer::logViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logViewer)
{

    ui->setupUi(this);
    for(int i=0;i<=5;i++)//初始化
    {
        logLabel.push_back(new QLabel(this));
        logJson.push_back(new QJsonArray);
    }
    rectangle.w=(int)(this->width()/6);
    for(int i=0;i<logLabel.size();++i)
    {
        logLabel[i]->setStyleSheet("border:1px solid red;");//1像素红色边框
        logLabel[i]->setWordWrap(true);//自动折行
        logLabel[i]->setText(QString::number(i));
        logLabel[i]->setAlignment(Qt::AlignTop);
        logLabel[i]->installEventFilter(this);
        //动态响应初始化部分由showEvent完成
    }
}

logViewer::~logViewer()
{
    delete ui;
}
void logViewer::moveLogViewer(QPoint e,QPoint mainPoint)
{
    move(e+mainPoint);
}
void logViewer::log(int module,QString str,int mod=NULL)
{
    switch (module)
    {
    case moduleHTTP:
    {
        bool addnew=true;
        if(stlogHTTP.size()==0)
        {
            stlogHTTP.push_back(new logHTTP);
        }
        for(int i=0;i<stlogHTTP.size();++i)
        {
            if(stlogHTTP[i]->tid==0)stlogHTTP[i]->tid=mod;

            if(stlogHTTP[i]->tid==mod)
            {
                stlogHTTP[i]->log<<str;
                addnew = false;
                break;
            }
            qDebug()<<i<<stlogHTTP.size();
            if(i+1==stlogHTTP.size() && addnew)
            {
                stlogHTTP.push_back(new logHTTP);
                this->log(module,str,mod);
            }
        }
        break;
    }
    case moduleStart:

        break;
    case modulefile:

        break;
    case modulemainWindows:

        break;
    case moduleJson:

        break;
    case moduleMD5:
    {
    QJsonObject lh;
    lh.insert("log",str);
    logJson[moduleMD5]->append(lh);
    }
        break;
    default:
        break;
    }
    this->logUpdate();
}
void logViewer::logUpdate()
{
    QJsonDocument logstr;
    for(int i=0;i<logLabel.size();++i)
    {
        switch (i)
        {
        case moduleHTTP:
        {
            QString  tem;
            for(int y=0;y<stlogHTTP.size();++y)
            {
                for(int j=0;j<stlogHTTP[y]->log.size();++j)
                {
                    //qDebug()<<"tid"<<stlogHTTP[y]->tid<<QString::number(stlogHTTP[y]->tid,10)<<QString::number(stlogHTTP[y]->tid,16);
                    tem.append("tid:");
                    tem.append(QString::number(stlogHTTP[y]->tid));
                    tem.append(" log:");
                    tem.append(stlogHTTP[y]->log.at(j));
                    tem.append("\r\n");
                }
                tem.append("---\r\n");
            }
            logLabel[i]->setText(tem);
        }
            break;
        default:
            logstr.setArray(*logJson[i]);
            logLabel[i]->setText(QString::fromUtf8(logstr.toJson(QJsonDocument::Compact).constData()));
            logLabel[i]->adjustSize();
        }


    }
}
void logViewer::resizeEvent(QResizeEvent *event)
{
    rectangle.w=(int)(event->size().width()/6);
    this->chooseLabel(moduleHTTP);//logLabel动态响应
}
bool logViewer::eventFilter(QObject *obj, QEvent *event)
{
    for(int i=0;i<logLabel.size();++i)
    {
        if(obj==logLabel[i])
        {
            if(event->type()==QEvent::Enter)
            {
                //qDebug()<<i;
                this->chooseLabel(i);
                return true;
            }else{
                return false;
            }
        }
    }
    return logViewer::eventFilter(obj, event);
}
void logViewer::chooseLabel(int num)
{
    int sw;
    int sx;
    for(int i=0;i<logLabel.size();++i)
    {
        if(i==0)//防止数下标错误
        {
            sx=0;
        }else{
            sx=logLabel[i-1]->pos().x()+logLabel[i-1]->width();
        }
        if(i!=num)//设置动态响应的Label宽度
        {
            sw=(int)(this->width()-rectangle.w*5)/5;
        }else{
            sw=rectangle.w*5;
        }
        //应用
        logLabel[i]->setMinimumWidth(sw);
        logLabel[i]->setMaximumWidth(sw);
        logLabel[i]->setGeometry(sx
                                 ,0
                                 ,sw
                                 ,logLabel[i]->geometry().height()
                                 );
        logLabel[i]->adjustSize();
    }
}
void logViewer::showEvent(QShowEvent *event)
{
    this->chooseLabel(moduleHTTP);
}
