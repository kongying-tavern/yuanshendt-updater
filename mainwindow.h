#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>
#include <windowsx.h>
#include <QMouseEvent>
#include <QtMath>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QtConcurrent>
#include <start.h>
QT_BEGIN_NAMESPACE
#include <iostream>
using namespace std;

namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QString startPath = "");
    ~MainWindow();

protected:
    /*bool nativeEvent(const QByteArray &eventType, void *message, long *result);*/
    /*窗口控制*/
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private slots:

    void on_pushButton_Start_clicked();
    void on_pushButton_Exit_clicked();
    //多线程窗口更新
    void Work_Process(int a,int b);
    void Work_Finished(bool done);






public:
    static MainWindow *mutualUi;
    /*窗口控制托管sub*/
    void changeMainPage(int page,bool done=false);/*修改MainPage,0:进度条 1:选择目录按钮*/
    void changeMainPage0label_Text(QString str);/*修改MainPage->Label_Text文本*/
    void changeProgressBarValue(int a,int b);/*修改进度条进度*/
    void changeProgressBarColor(QString rgba,QString rgbb);

    /*多线程*/

    void threadWork(QString path);
    void startThread(QString path);



private:
    int boundaryWidth;
        QPoint clickPos;
        Start *ttstart=nullptr;
    Ui::MainWindow *ui;

    /*多线程传参*/

};

bool exeIsRunning();
static void threadChangeMainPage0label_Text(QString str);/*修改MainPage->Label_Text文本*/
#endif // MAINWINDOW_H
