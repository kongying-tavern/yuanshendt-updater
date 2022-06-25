#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>
#include <qt_windows.h>
#include <QtWidgets/QMessageBox>
#include <QMouseEvent>
#include <QtMath>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QtConcurrent>
#include <QAbstractButton>
#include <Start.h>
QT_BEGIN_NAMESPACE
#include <iostream>

#include "logviewer.h"
#include "md5maker.h"

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
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *e);
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
private slots:
    void on_pushButton_Start_clicked();
    void on_pushButton_Exit_clicked();
    //多线程窗口更新
    void Work_Process(int a,int b);
    void Work_Finished(bool done);

signals:
    void stopTwork();
    void moveLogViewer(QPoint e,QPoint mainPoint);
public slots:
    /*窗口控制托管槽*/
    void changeMainPage(int page,bool done=false);/*修改MainPage,0:进度条 1:选择目录按钮*/
    void changeMainPage0label_Text(QString str);/*修改MainPage->Label_Text文本*/
    void changeProgressBarValue(int a,int b);/*修改进度条进度*/
    void changeProgressBarColor(QString rgba,QString rgbb);
    void changePBText(QString str);//修改按钮文本
    void updataDlingmag();//强制刷新进度文本
public:
    logViewer *logUI;
    md5maker *md5makerUI = nullptr;
    /*窗口托管*/
    static MainWindow *mutualUi;


    void Work_Dlnow(QString txt);
    void Work_MessageBox(int tag,QString title,QString txt);//信息框
    /*多线程*/
    void threadWork(QString path);
    void startThread(QString path);

public:
    //void msgBoxClosed(QAbstractButton*);//非模态消息框回调


private:

    int boundaryWidth;
    QPoint clickPos;
    Start *ttstart=nullptr;
    HTTP *http=nullptr;
    Ui::MainWindow *ui;
    /*timerEvent*/
    void timerEvent(QTimerEvent *event);
    int timer1;



};

bool exeIsRunning();
#endif // MAINWINDOW_H
