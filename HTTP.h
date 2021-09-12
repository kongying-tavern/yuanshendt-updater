#ifndef HTTP_H
#define HTTP_H
#include <QString>
#include <QObject>
#include <QThread>
#include <QRunnable>

#include <QDebug>
//#define CURL_STATICLIB
#include "curllib\curl.h"

static QString crtPath;
class HTTP : public QObject, public QRunnable
{
Q_OBJECT


public:
    HTTP(QString URL,QString Path,QObject *parent);
    ~HTTP();

    int httpDownLoad(QString URL,QString Path);
    static size_t write_data(void *ptr,
                      size_t size,
                      size_t nmemb,
                      void *stream
                      );
    static int progress_callback(void *clientp,
                          curl_off_t dltotal,
                          curl_off_t dlnow, curl_off_t ultotal,
                          curl_off_t ulnow
                          );
protected:
    void run();
private:
    QString turl;
    QString tdlpath;
    void *tid;
    FILE *pagefile;
    CURL *handle;
private slots:

signals:
    void tworkMessageBox(int tag,QString title,QString txt);
    void dldone();
};
void httpcrt();


QString conver(LONG64 l);



#endif // HTTP_H
