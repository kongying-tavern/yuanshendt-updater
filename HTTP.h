#ifndef HTTP_H
#define HTTP_H
#include <QString>

class HTTP
{
public:
    HTTP();

};
void httpcrt();
int httpDownLoad(QString URL,QString Path);
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

static QString crtPath;

#endif // HTTP_H
