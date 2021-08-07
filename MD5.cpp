#include "MD5.h"
#include <QFile>
#include <QDebug>
#include <QCryptographicHash>
#include <QString>
MD5::MD5()
{

}
QString getFlieMD5(QString file)
{
    QFile theFile(file);
    theFile.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
    theFile.close();
    qDebug() <<"MD5:"<<ba.toHex().constData()<<"|"<<file;
    return ba.toHex().constData();
}
