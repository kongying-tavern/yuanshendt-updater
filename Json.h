#ifndef JSON_H
#define JSON_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>

#include "Start.h"

#include <string.h>
using namespace std;

class JSON
{

public:
    explicit JSON(QObject *parent = nullptr);
    ~JSON();

    Start *tp = nullptr;

public:
    bool jsonStr2QSL(QString QSL,QStringList &filePath,QStringList &fildMD5);

};

#endif // JSON_H
