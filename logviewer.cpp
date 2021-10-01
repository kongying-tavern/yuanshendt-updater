#include "logviewer.h"
#include "ui_logviewer.h"

#include <windows.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Sandefine.h"


QJsonArray httpLog;
QJsonArray StartLog;
QJsonArray MD5Log;
QJsonArray fileLog;

logViewer::logViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logViewer)
{

    ui->setupUi(this);
    ui->viewer->adjustSize();
    ui->viewer->setStyleSheet("border:1px solid red;");
    ui->viewer->setWordWrap(true);
}

logViewer::~logViewer()
{
    delete ui;
}
void logViewer::moveLogViewer(QPoint e,QPoint mainPoint)
{
    move(e+mainPoint);
}
void logViewer::log(int module,QString str,void* mod=NULL)
{
    switch (module)
    {
    case moduleHTTP:
    {
        QJsonObject lh;
        lh.insert("tid",(int)HandleToLong(mod));
        lh.insert("log",str);
        httpLog.append(lh);
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

        break;
    default:
        break;
    }
    this->logUpdate();
}
void logViewer::logUpdate()
{
    QJsonDocument logstr;
    logstr.setArray(httpLog);
    ui->viewer->setText(QString::fromUtf8(logstr.toJson(QJsonDocument::Compact).constData()));
    ui->viewer->adjustSize();
}
