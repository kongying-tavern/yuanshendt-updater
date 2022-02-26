#include "logviewer.h"
#include "ui_logviewer.h"

#include <qt_windows.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEvent>
#include <QTextDocument>
#include "Sandefine.h"
#define editw 4.5
rectangle rectangle;

logViewer::logViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logViewer)
{

    ui->setupUi(this);
    for(int i=0;i<=5;i++)//初始化
    {
        logTextEdit.push_back(new QPlainTextEdit(this));
        logJson.push_back(new QJsonArray);
    }
    rectangle.w=(int)(this->width()/6);
    for(int i=0;i<logTextEdit.size();++i)
    {
        logTextEdit[i]->setReadOnly(true);
        logTextEdit[i]->installEventFilter(this);//事件钩子
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

        if(stlogHTTP.size()==0)
        {
            stlogHTTP.push_back(new logHTTP);
        }
        for(int i=0;i<stlogHTTP.size();++i)
        {
            if(stlogHTTP[i]->tid==0)
            {
                stlogHTTP[i]->tid=mod;
            }
            if(stlogHTTP[i]->tid==mod)
            {
                stlogHTTP[i]->log<<str;
                break;
            }
            if(i+1==stlogHTTP.size())
            {
                stlogHTTP.push_back(new logHTTP);
                //this->log(module,str,mod);//日
                //push_back以后size变了会多循环一轮了日志重复添加了
            }
        }
    }
        break;
    default:
    {
        QJsonObject lh;
        lh.insert("log",str);
        logJson[module]->append(lh);
    }
        break;
    }
    this->logUpdate(module);

}
void logViewer::logUpdate(int c)
{
    QString tem;
    switch (c)
    {
    case moduleHTTP:
    {

        for(int y=0;y<stlogHTTP.size();++y)
        {
            for(int j=0;j<stlogHTTP[y]->log.size();++j)
            {
                tem.append("tid:");
                tem.append(QString::number(stlogHTTP[y]->tid));
                tem.append(" log:");
                tem.append(stlogHTTP[y]->log.at(j));
                tem.append("\r\n");
            }
            tem.append("---\r\n");
        }
        logTextEdit[moduleHTTP]->setPlainText(tem);
    }
        break;

    default:
        logTextEdit[c]->appendPlainText(
                    logJson[c]->at(logJson[c]->size()-1)
                    .toObject()["log"]
                    .toString()
                    );

    }
    logTextEdit[c]->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
}

void logViewer::resizeEvent(QResizeEvent *event)
{
    rectangle.w=(int)(event->size().width()/6);
    rectangle.h=this->height();
    this->chooseEdit(editNowChoose);//logTextEdit动态响应
}
bool logViewer::eventFilter(QObject *obj, QEvent *event)
{

    //qDebug()<<obj<<event->type();
    for(int i=0;i<logTextEdit.size();++i)
    {
        if(obj==logTextEdit[i])
        {
            if(event->type()==QEvent::Enter)
            {
                editNowChoose=i;
                this->chooseEdit(i);
                return true;
            }else{
                return false;
            }
        }
    }
    return logViewer::eventFilter(obj, event);
}
void logViewer::chooseEdit(int num)
{
    int sw;
    int sx;
    for(int i=0;i<logTextEdit.size();++i)
    {
        if(i==0)//防止数下标错误
        {
            sx=0;
        }else{
            sx=logTextEdit[i-1]->pos().x()+logTextEdit[i-1]->width();
        }
        if(i!=num)//设置动态响应的Label宽度
        {
            sw=(int)(this->width()-(int)(rectangle.w*editw))/5;
        }else{
            sw=(int)(rectangle.w*editw);
        }
        //响应
        logTextEdit[i]->setMinimumWidth(sw);
        logTextEdit[i]->setMaximumWidth(sw);
        logTextEdit[i]->setGeometry(sx
                                    ,0
                                    ,sw
                                    ,rectangle.h
                                    );
    }
}
void logViewer::showEvent(QShowEvent *event)
{
    this->chooseEdit(moduleHTTP);
}
