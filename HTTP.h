#ifndef HTTP_H
#define HTTP_H

#include <QString>
//#define CURL_STATICLIB
#include "curllib\curl.h"

static QString crtPath;
class HTTP
{

public:
    HTTP();
    ~HTTP();

};
void httpcrt();
int httpDownLoad(QString URL,QString Path);
size_t write_data(void *ptr,
                  size_t size,
                  size_t nmemb,
                  void *stream
                  );
int progress_callback(void *clientp,
                      curl_off_t dltotal,
                      curl_off_t dlnow, curl_off_t ultotal,
                      curl_off_t ulnow
                      );
QString conver(LONG64 l);



#endif // HTTP_H
