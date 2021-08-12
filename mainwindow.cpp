#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include "qstring.h"
#include <shlwapi.h>
#include "Sandefine.h"
#include <QString>
#include <QFileDialog>
#include <QProcess>
#include <QPropertyAnimation>
#include <HTTP.h>


bool windowsMoveOn;
bool updaterIsRunning;
bool alldone=false;
QString workPath;
MainWindow *MainWindow::mutualUi = nullptr;/*托管初始化，非常重要*/
MainWindow::MainWindow(QWidget *parent, QString pathStr)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    //qDebug()<<(pathStr);
    ui->setupUi(this);
    mutualUi = this;/*托管初始化，非常重要*/
    this->setWindowFlag(Qt::FramelessWindowHint); // 无边框窗口
    setAttribute(Qt::WA_TranslucentBackground, true);  // 背景透明

    //仙哥版窗口阴影
    //设置窗口四周阴影 - label_Shadow实现
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        shadow->setOffset(0, 0);//设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
        shadow->setColor(QColor(0, 0, 0, 50));//设置阴影颜色，也可以setColor(QColor(220,220,220))
        shadow->setBlurRadius(15);//设定阴影的模糊半径，数值越大越模糊
        ui->label_Shadow->setGraphicsEffect(shadow);


    if(pathStr == nullptr)
    {
        MainWindow::mutualUi->changeMainPage(1);//无路径传参,显示选择路径按钮

    }else{
        QFileInfo info(pathStr);
        workPath=info.path();
        qDebug()<<"workPath:"<<workPath;
        threadWork(workPath);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeMainPage(int mod,bool done)/*托管修改MainPage*/
{
    //cout<<mod<<endl;
    ui->MainPage->setCurrentIndex(mod);
    if(done)
    {
        alldone = true;
        ui->pushButton_Start->setText("打开地图");
    }
    //Sleep(10);
}
void MainWindow::Work_Process(int a,int b)
{
    //MainWindow::changeProgressBarValue(a,b);
    int c=(int)(100*(float(a)/float(b)));
    QPropertyAnimation * animation =new QPropertyAnimation(ui->progressBar, "value");
    animation->setDuration(300);
    animation->setStartValue(ui->progressBar->value());
    animation->setEndValue(c);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
void MainWindow::Work_Dlnow(QString txt)
{
    ui->label_Dlnow->setText(txt);
}

void MainWindow::changeProgressBarValue(int a,int b)//修改进度条进度//极容易崩溃!
{

    int c=(int)(100*(float(a)/float(b)));
    qDebug()<<"进度条"<<ui->progressBar->value()<<"|"<<c;
    ui->progressBar->setValue(c);
    Sleep(10);
}

void MainWindow::changeProgressBarColor(QString rgba, QString rgbb)
{
    QString tem=QString(progressBarColor)
            .arg(rgba)
            .arg(rgbb)
            ;

    ui->progressBar->setStyleSheet(tem);
    //Sleep(10);
}
void MainWindow::changeMainPage0label_Text(QString str="?好像啥都没有发生?")/*托管修改MainPageTXT*/
{

    ui->label_Text->setText(str);
    //Sleep(10);
}
void MainWindow::mousePressEvent(QMouseEvent *e)  // 鼠标按下触发记录当前鼠标位置（全局）
{

    if(e->button()==Qt::LeftButton)
    {
        clickPos=e->pos();
        //qDebug()<<this->pos();
        //qDebug()<<e->pos();
        windowsMoveOn=true;
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)  // 移动窗口(根据windowsMoveOn)
{    
    if(windowsMoveOn){
        //qDebug()<<e;
        move(this->pos()+e->pos()-clickPos);
        //clickPos=this->pos();

    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *e)//鼠标任何按键松开
{
    if(windowsMoveOn){windowsMoveOn=false;}
}
void MainWindow::on_pushButton_Exit_clicked()//关闭按钮被单击
{
    updaterIsRunning = false;
    QApplication* app;
    app->exit(0);
}



void MainWindow::on_pushButton_Start_clicked() /*选择安装文件夹后file_search(多线程已完成)*/
{
    //getExistingDirectory(父对象一般是this,对话框标题,对话框开始目录  一般是"./",默认是只显示目录-https://doc.qt.io/qt-5/qfiledialog.html#Option-enum);
    //返回空对象则没选
    //选择了目录则返回QString
    if(alldone)
    {

        qDebug()<<QString(workPath+"/Map.exe");
        QProcess process(this);
        process.setProgram("cmd");
        QStringList argument;
        argument << "/c" <<"start"<<""""<<workPath+"/Map.exe";
        process.setArguments(argument);
        if(process.startDetached())
        {
            QApplication* app;
            app->exit(0);
        }else{
            QMessageBox::information(this,
                                     "出了点问题",
                                     "请手动打开空荧地图/n能搞出这个提示San表示San也不知道怎么回事"
                                     );
        }


    }else{
        QString dir;
        string tem;
        dir = QFileDialog::getExistingDirectory
                (
                    this,
                    "空荧酒馆地图安装目录(不会创建子文件夹!)",
                    "./",
                    QFileDialog::ShowDirsOnly
                );
        //dir.replace("/","\\");
        tem =dir.toStdString();
        qDebug()<<dir;
        if(string(tem)==""){
            MainWindow::changeMainPage0label_Text("?刚才发生了什么?");
        }else{
            workPath=dir;
            threadWork(dir);
        };
    }


}
void MainWindow::threadWork(QString path)
{
    updaterIsRunning = true;
    //QtConcurrent::run(this,&MainWindow::startThread,path);
    MainWindow::startThread(path);
}
void MainWindow::startThread(QString path)
{
    ui->MainPage->setCurrentIndex(0);
    //startUpdate(path);
    //正经多线程
    if (ttstart != nullptr)
        {
            delete ttstart;
        }
    //创建多线程工作对象
    ttstart = new Start(path, NULL);
    //连接工作触发信号
    //connect(Start, &Start::workError, this, &MainWindow::Work_Error);
    connect(ttstart, &Start::tworkProcess, this, &MainWindow::Work_Process);
    connect(ttstart, &Start::tworkFinished, this, &MainWindow::Work_Finished);


    //开始工作
    timer1 = startTimer(500);//0.5s定时器
    emit ttstart->tstart();
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    qDebug()<<"timerShot:"<<event->timerId();
    if(event->timerId()==timer1)
    {
        qDebug()<<"0.5s计时器";
        ui->label_Dlnow->setText(tNowWork());
    }

}
void MainWindow::Work_Finished(bool done)
{
    alldone=done;
}



