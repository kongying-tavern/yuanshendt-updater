#include <QApplication>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "HTTP.cpp"
#include "MD5.cpp"
#include "file.cpp"
#include "Sandefine.h"

#include <qt_windows.h>



void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    qDebug().noquote()<<msg;//调试窗输出
    QJsonObject joc;
    QJsonArray jay;
    switch(type)
    {
        case QtDebugMsg:
            joc.insert("tag","Debug");
        break;
        case QtInfoMsg:
            joc.insert("tag","Info");
        break;
        case QtWarningMsg:
            joc.insert("tag","Warning");
        break;
        case QtCriticalMsg:
            joc.insert("tag","Critical");
        break;
        case QtFatalMsg:
            joc.insert("tag","FatalMsg");
        break;
    }
    // 设置输出json格式
    joc.insert("Time",
               QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch())
               );
    joc.insert("File",context.file);
    joc.insert("Line",context.line);
    joc.insert("Function",context.function);
    joc.insert("Message",msg);

    static QMutex mutex;
    // 加锁
    mutex.lock();
    // 写日志

    //logstream->setEncoding(QStringConverter::Utf8);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    logstream->setCodec(QTextCodec::codecForName("utf-8"));
#else
    logstream->setEncoding(QStringConverter::Utf8);
#endif
    QString jsonstr= QJsonDocument(joc).toJson().replace("\\\\","\\").replace("\\\"","\"");
    //stream->
    *logstream <<jsonstr.toUtf8()+"\n";
    logstream->flush();//强制写出
    //logfile.close();
    // 解锁
    mutex.unlock();
}
void cleanLog()
{
    QStringList fList;
    file_search(logPath,fList);
    for(int i=0;i<fList.size()-3;i++)//保留3个//暂且能按时间来
    {
        qDebug()<<"删除旧日志:"<<fList.at(i);
        QFile(fList.at(i)).remove();
    }
}
//读注册表
QString regRead(HKEY rootKey,QString mainKey,QString key)
{
    QString re="-";
    HKEY hkey;
    qDebug().noquote()<<rootKey;
    qDebug().noquote()<<mainKey;
    QByteArray sk=mainKey.toLocal8Bit();
    LPCSTR k=(LPCSTR)sk.data();
    DWORD err = 0;
    /*
LSTATUS RegOpenKeyEx(                如果函数成功，则返回值ERROR_SUCCESS。
  [in]           HKEY    hKey,        rootPath
  [in, optional] LPCWSTR lpSubKey,    要打开的注册表子项的名称。
  [in]           DWORD   ulOptions,   指定打开密钥时要应用的选项。将此参数设置为零或以下值：REG_OPTION_OPEN_LINK
  [in]           REGSAM  samDesired,  一个掩码
  [out]          PHKEY   phkResult    指向变量的指针，该变量接收打开的键的句柄。
);
*/
    SetLastError(err);
    err = ::RegOpenKeyExA(rootKey,
                          k,
                          0,
                          KEY_READ,
                          &hkey
                          );
    qDebug()<<err<<errcode2str(err);//2:nofound
    if(err==ERROR_SUCCESS)
    {
        SetLastError(err);
        DWORD dwType = REG_SZ;
        QByteArray vsk=key.toLatin1();
        qDebug().noquote()<<vsk;
        LPCSTR vk = (LPCSTR)vsk.data();
        char wre[MAX_PATH]={"\0"};
        DWORD size = sizeof(wre);
        err = RegQueryValueExA(
                  hkey,          // 一个已打开的注册表句柄
                  vk,            // 要查询注册表键值的名字字符串，注册表键的名字，以空字符结束。
                  0,             // reserved
                  &dwType,       // 装载指定值的一个缓冲区
                  (LPBYTE)&wre,  // 装载取回数据类型的一个变量
                  &size          // 装载lpData缓冲区长度的一个变量。返回设置为被操作的字节数
                  );
        qDebug()<<err<<errcode2str(err);//2:nofound
        qDebug()<<wre<<size;
        if(err == ERROR_SUCCESS)
        {
            if(dwType == REG_SZ)  //字符串类型
            {
                re=QString(wre);
            }
        }else{
            qDebug()<<"open regKey error 0x1";
        }
    }else{
        qDebug()<<"open regKey error 0x0";
    }
    SetLastError(err);
    ::RegCloseKey(hkey);
    return re;
}
//写注册表
bool regWrite(HKEY rootKey,QString mainKey,QString key,QString value)
{
    bool re = false;
    HKEY hkey;
    qDebug().noquote()<<rootKey;
    qDebug().noquote()<<mainKey;
    QByteArray sk=mainKey.toLocal8Bit();
    LPCSTR k=(LPCSTR)sk.data();
    DWORD err = 0;
    SetLastError(err);
    err = ::RegOpenKeyExA(rootKey,
                          k,
                          0,
                          KEY_SET_VALUE,
                          &hkey
                          );
    qDebug()<<err<<errcode2str(err);//2:nofound
    if(err==ERROR_SUCCESS)
    {
        QByteArray vsk=key.toLatin1();
        qDebug().noquote()<<vsk;
        LPCSTR vk = (LPCSTR)vsk.data();
        QByteArray vsv=value.toLatin1();
        qDebug().noquote()<<vsv;
        LPCSTR vv = (LPCSTR)vsv.data();
        DWORD dwType = REG_SZ;
        SetLastError(err);
        /*
LSTATUS RegSetValueExA(
  [in]           HKEY       hKey,
  [in, optional] LPCSTR     lpValueName, //要设置的值的名称。如果键中尚不存在具有此名称的值，则该函数会将其添加到键中。
                 DWORD      Reserved,    //0
  [in]           DWORD      dwType,      //数据类型
  [in]           const BYTE *lpData,     //要存储的数据。REG_SZ需要0结尾
  [in]           DWORD      cbData       //信息的大小，以字节为单位。//包括终止符
);
*/
        err = RegSetValueExA(
                  hkey,
                  vk,
                  0,
                  dwType,
                  (CONST BYTE*)vv,
                  sizeof(&vv)
                  );
        qDebug()<<err<<errcode2str(err);//2:nofound
        if(err == ERROR_SUCCESS){
            re=1;
        }else{

        }
    }else{
        qDebug()<<"open regKey error 0x0";
    }

    ::RegCloseKey(hkey);
    return re;
}
//转换为长路径
QString short2longPath(QString p)
{
    int    len = MultiByteToWideChar(CP_ACP, 0, p.toLocal8Bit(), -1, NULL, 0);
      LPWSTR buf = (LPWSTR)malloc(2*len);
      if (buf) {
        MultiByteToWideChar(CP_ACP, 0, p.toLocal8Bit(), -1, buf, len);
        int plen = GetLongPathNameW(buf, NULL, 0);
        LPWSTR buf2 = (LPWSTR)malloc(plen*2+2);
        if (buf2) {
          GetLongPathNameW(buf, buf2, plen);
          //LPWSTR buf3 = wcsrchr((wchar_t*)buf2, L'\\')+1;
          int ulen = WideCharToMultiByte(CP_UTF8, 0, buf2, -1, NULL, 0, NULL, NULL);
          //int ulen = WideCharToMultiByte(CP_UTF8, 0, buf3, -1, NULL, 0, NULL, NULL);
          if (ulen) {
            std::string tmp;
            tmp.resize(ulen-1);
            WideCharToMultiByte(CP_UTF8, 0, buf2, -1, (LPSTR)tmp.c_str(), ulen-1, NULL, NULL);
            //WideCharToMultiByte(CP_UTF8, 0, buf3, -1, (LPSTR)tmp.c_str(), ulen-1, NULL, NULL);
            free(buf2);
            free(buf);
            return QString::fromStdString(tmp);
          }
          free(buf2);
        }
        free(buf);
      }
      return "";
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    logPath=getTempPath("temp")
            +updaterTempDir
            +"log/";
    createFolderSlot(logPath);
    logfilePath=logPath+QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());
    logfile=new QFile(logfilePath);
    logfile->open(QIODevice::WriteOnly | QIODevice::Append);
    logstream=new QTextStream(logfile);
    qInstallMessageHandler(MessageOutput);//启动日志
    cleanLog();//清理旧日志
    qDebug()<<"version"<<_version;
    //处理短路径
    dpnx0 = short2longPath(QCoreApplication::applicationFilePath());
    qDebug()<<"运行路径"<<dpnx0;
    //return 0;
    QString tem="";
    QStringList argument;
    //传参时路径最后必须加反斜杠,头尾加双引号//很重要！
    for(int i = 1; i< argc;++i)
    {
        qDebug()<<i<<argv[i];
        if(i<2)
        {
            tem =QString::fromLocal8Bit(argv[i]);
        }else{
            tem = tem+" "+QString::fromLocal8Bit(argv[i]);
        }
        argument<<argv[i];
    }
    if(tem.indexOf("?")>0)//唉
    {
        argument.clear();
        qDebug()<<"宽字符传参";
        LPWSTR s=GetCommandLineW();
        LPWSTR *cmdLineAry=CommandLineToArgvW(s,&argc);
        for(int i = 1; i< argc;++i)
        {
            if(i<2)
            {

                tem =QString::fromStdWString(cmdLineAry[i]);
            }else{
                tem = tem+" "+QString::fromStdWString(cmdLineAry[i]);
            }
            argument<<QString::fromStdWString(cmdLineAry[i]);
        }

    }
    qDebug().noquote()<<argument;
    //return 0;
    qDebug()<<QSysInfo::productType()<<QSysInfo::productVersion()<<QSysInfo::productVersion().toInt();//"windows" "10"
    if(QSysInfo::productVersion().toInt()<10)//只做win7 不做win7sp1 还有XP给爷死  //不会还有人用win8吧不会吧不会吧不会吧
    {
        QString rootPath = "hkcu";
        QString mainKey="Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers";
        QString regv=regRead(HKEY_CURRENT_USER,
                             mainKey,
                             dpnx0.replace("/","\\")
                             );
        qDebug().noquote()<<"读取到的value"<<regv;
        if(regv!="WIN7RTM")
        {
            qDebug()<<"run in Windows8 or earlier";
            QString value="WIN7RTM";
            bool regb=regWrite(HKEY_CURRENT_USER,
                               mainKey,
                               dpnx0.replace("/","\\"),
                               value
                               );
            if(regb)
            {
                QProcess process;
                process.setProgram(argv[0]);
                process.setArguments(argument);
                if(process.startDetached())
                {return 0;}else{}
            }else{
                //弹窗警告
            }
        }
    }

    qDebug()<<"传参路径："<<tem;
    MainWindow w(NULL,tem);
    w.show();
    return a.exec();
}


