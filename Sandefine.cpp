#include "Sandefine.h"
#include <QDebug>

#include <windows.h>
Sandefine::Sandefine()
{

}
QString errcode2str(int errcode)
{
    LPVOID   lpMsgBuf;
    FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER   |
                FORMAT_MESSAGE_FROM_SYSTEM   |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errcode,
                MAKELANGID(LANG_NEUTRAL,   SUBLANG_DEFAULT),   //   Default   language
                (LPTSTR)   &lpMsgBuf,
                0,
                NULL
                );
    //MessageBox(hWnd,(LPCTSTR)lpMsgBuf,"Error",MB_OK);
    //qDebug()<<QString::fromWCharArray((LPTSTR)lpMsgBuf);
    return QString::fromWCharArray((LPTSTR)lpMsgBuf);
}
QString curlerr2str(int errcode)
{
    switch(errcode)
    {
    case 0:
        return "成功";
    case 1:
        return "协议不受支持";
    case 2:
        return "libcurl模块错误";
    case 3:
        return "URL格式错误";
    case 4:
        return "libcurl模块功能缺失";
    case 5:
        return "无法解析代理";
    case 6:
        return "无法解析远程主机";
    case 7:
        return "无法链接至远程主机(或代理)";
    case 8:
        return "FTP返回意料外的答复";
    case 9:
        return "指定资源访问被拒绝";
    case 10:
        return "等待FTP回连时发送了错误代码";
    case 11:
        return "发送密码到FTP服务器后收到了意外的代码";
    case 12:
        return "等待FTP时CURLOPT_ACCEPTTIMOUT_MS或默认值超时";
    case 13:
        return "未能获得合理的结果(PASV或EPSV收到异常回复)";
    case 14:
        return "FTP 277";
    case 15:
        return "解析新链接内部错误";
    case 16:
        return "";
    case 17:
        return "无法将传输模式设置为二进制或ASCII";
    case 18:
        return "收到的文件大小不符合预期";
    case 19:
        return "RETR恢复错误或0字节传输完成";
    case 20:
        return "";
    case 21:
        return "QUOTE收到>400的回复";
    case 22:
        return "HTTP返回>400";
    case 23:
        return "写出到文件时错误";
    case 24:
        return "";
    case 25:
        return "无法启动上传(可查询错误缓冲区)";
    case 26:
        return "读取本地文件或读取回调错误";
    case 27:
        return "内存分配请求失败!";
    case 28:
        return "操作超时";
    case 29:
        return "";
    case 30:
        return "FTP端口错误(详见CURLPOT_FTPPORT)";
    case 31:
        return "FTP REST命令返回错误";
    case 32:
        return "";
    case 33:
        return "服务器不接受范围请求";
    case 34:
        return "libcurl表示?\r\n错误34";
    case 35:
        return "SSL/TLS握手错误\r\n详见错误缓冲区";
    case 36:
        return "指定的偏移超出文件大小";
    case 37:
        return "FILE提供的文件无法打开";
    case 38:
        return "LDAP绑定失败";
    case 39:
        return "LDAP搜索失败";
    case 40:
        return "";
    case 41:
        return "未找到需要的zlib函数";
    case 42:
        return "回调中止";
    case 43:
        return "内部错误,某个函数使用了一个错误的参数";
    case 44:
        return "";
    case 45:
        return "指定的出站界面错误,请检查CURLOPT_INTERFACE设置";
    case 46:
        return "";
    case 47:
        return "重定向过多,请检查CURLOPT_MAXREDIRS设置";
    case 48:
        return "选项不被识别,请查阅相关文档或查询错误缓冲区信息.";
    case 49:
        return "telnet选项字符被非法格式化";
    case 50:
        return "";
    case 51:
        return "远程服务器的SLL证书或SSH MD5被认为不正常";
    case 52:
        return "未收到任何返回";
    case 53:
        return "未找到指定的加密(SSL)引擎";
    case 54:
        return "默认设置所选择的SSL加密引擎出现错误";
    case 55:
        return "发送网络数据失败";
    case 56:
        return "接受网络数据失败";
    case 57:
        return "";
    case 58:
        return "本地客户端证书出现问题";
    case 59:
        return "无法使用指定的密码(SSL)";
    case 60:
        return "对等证书无法通过已知的CA证书进行身份验证";
    case 61:
        return "无法识别传输的编码";
    case 62:
        return "无效的LDAP地址";
    case 63:
        return "超过最大文件大小";
    case 64:
        return "请求的FTP SSL级别失败";
    case 65:
        return "发送的数据不得不倒带重发时,倒带失败";
    case 66:
        return "启动SSL引擎失败";
    case 67:
        return "远程服务器拒绝卷曲登录";
    case 68:
        return "TFTP服务器上找不到文件";
    case 69:
        return "TFTP服务器上权限问题";
    case 70:
        return "超出服务器空间限制";
    case 71:
        return "非法TFTP操作";
    case 72:
        return "未知的TFTP传输ID";
    case 73:
        return "文件已存在,不会被覆盖";
    case 74:
        return "TFTP服务器返回了不对劲的东西";
    case 75:
        return "字符转换失败";
    case 76:
        return "呼叫者必须注册转换回调";
    case 77:
        return "读取SSL CA证书出现问题";
    case 78:
        return "URL中引用的资源不存在";
    case 79:
        return "SSH会话期间出现未指定的错误";
    case 80:
        return "无法关闭SSL链接";
    case 81:
        return "Socket发送准备未就绪";
    case 82:
        return "无法加载CRL文件";
    case 83:
        return "发行人检查失败";
    case 84:
        return "FTP服务器根本不了解PRET命令，也不支持给定的参数。使用CURLOPT_CUSTOMREQUEST时要小心，在PASV之前也会使用自定义LIST命令与PRET CMD一起发送";//这tm是什么东西?
    case 85:
        return "RTSP CSeq号码不匹配";
    case 86:
        return "RTSP会话标识符不匹配";
    case 87:
        return "无法解析FTP文件列表(在FTP通配符下载期间)";
    case 88:
        return "块回调报告错误";
    case 89:
        return "不存在的错误(无连接可用,会话将排队)";
    case 90:
        return "无法匹配使用CURLOPT_PINNEDPUBLICKEY指定的固定密钥";
    case 91:
        return "使用CURLOPT_SSL_VERIFYSTATUS询问状态时返回失败";
    case 92:
        return "HTTP/2框架层中的流错误";
    default:
        return "哈?";
    }
    return "哈??";
}
