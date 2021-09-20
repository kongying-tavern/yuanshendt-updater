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
    qDebug()<<QString::fromWCharArray((LPTSTR)lpMsgBuf);
    return "";
}
