#ifndef SANDEFINE_H
#define SANDEFINE_H
#include <QString>
/*云端服务器***********************************************************************/
#define dlurl "http://download.yuanshen.site/"
#define dlurlMap "http://download.yuanshen.site/Map/"
/*测试版云端服务器******************************************************************/
//#define dlurl "http://download.yuanshen.site/test/"
//#define dlurlMap "http://download.yuanshen.site/test/Map/"
/*2.1BUG专版**********************************************************************/
//#define dlurl "http://download.yuanshen.site/Old/2.1/"
//#define dlurlMap "http://download.yuanshen.site/Old/2.1/Map/"
/*临时目录子目录********************************************************************/
#define updaterTempDir "/yuanshendt/Updater/"
/*窗口默认值***********************************************************************/
#define progressBarColor "QProgressBar{background:%1;color:rgba(255, 255,255, 0);border:1px solid rgb(235, 235, 235);border-radius:3px;} QProgressBar::chunk{border-radius:3px;background:%2;}"
/*长文本常量***********************************************************************/
//更新失败
#define uperr \
"请按照以下步骤反馈:\r\n\r\n"\
"打开我的电脑\r\n"\
"复制以下路径并打开:\r\n"\
"%temp%\\yuanshendt\\Updater\\log\\\r\n"\
"添加群228382171\r\n"\
"将文件发给SanMeb\r\n\r\n"\
"另 交流群群号参见\r\n"\
"https://yuanshen.site/docs/communication-group.html\r\n"\
"谢谢!\r\n\r\n"\
"之后还请使用安装包或压缩包安装\r\n"\
"↓下载地址↓若显示两行还请复制完整\r\n\r\n"\
"http://download.yuanshen.site/%e7%a9%ba%e8%8d%a7%e9%85%92%e9%a6%86%e5%9c%b0%e5%9b%be%e5%ae%a2%e6%88%b7%e7%ab%af"


static QString logPath;
static QString logfile;

QString errcode2str(int errcode);
QString curlerr2str(int errcode);

class Sandefine
{
public:
    Sandefine();
};

#endif // SANDEFINE_H
