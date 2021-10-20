#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QProcess>
#include <QPropertyAnimation>


#include <shlwapi.h>

#include "Sandefine.h"
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
    mutualUi = this;//托管初始化，非常重要



    this->setWindowFlag(Qt::FramelessWindowHint); // 无边框窗口
    setAttribute(Qt::WA_TranslucentBackground, true);  // 背景透明

    //仙哥版窗口阴影
    //设置窗口四周阴影 - label_Shadow实现
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setOffset(0, 0);//设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
    shadow->setColor(QColor(0, 0, 0, 50));//设置阴影颜色，也可以setColor(QColor(220,220,220))
    shadow->setBlurRadius(15);//设定阴影的模糊半径，数值越大越模糊
    ui->label_Shadow->setGraphicsEffect(shadow);

    /*初始化装逼窗口*/
    //logViewer *logUI(new logViewer);
    logUI = new logViewer(this);
    logUI->setModal(false);
    logUI->setWindowTitle(_version);
    //logUI->show();
    connect(this, &MainWindow::moveLogViewer
            ,logUI, &logViewer::moveLogViewer);
    logUI->log(modulemainWindows,"窗口初始化√",NULL);
    if(pathStr == nullptr)
    {
        logUI->log(modulemainWindows,"无传参",NULL);
        MainWindow::mutualUi->changeMainPage(1);//无路径传参,显示选择路径按钮

    }else{
        QFileInfo info(pathStr);
        workPath=info.path();
        qDebug()<<"workPath:"<<workPath;
        logUI->log(modulemainWindows,"传参路径："+workPath,NULL);
        threadWork(workPath);
    }

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::changePBText(QString str)
{
    ui->pushButton_Start->setText(str);
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
        //qDebug()<<e->pos();
        windowsMoveOn=true;
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)  // 移动窗口(根据windowsMoveOn)
{        
    if(windowsMoveOn)
    {
        move(this->pos()+e->pos()-clickPos);
        emit moveLogViewer(this->pos(),QPoint(this->width(),0));
    }
}
void MainWindow::showEvent(QShowEvent *event)
{

    qDebug()<<"窗口句柄"<<this->winId();
    emit moveLogViewer(this->pos(),QPoint(this->width(),0));
}
void MainWindow::mouseReleaseEvent(QMouseEvent *e)//鼠标任何按键松开
{
    if(windowsMoveOn){windowsMoveOn=false;}
}

void MainWindow::on_pushButton_Exit_clicked()//关闭按钮被单击
{
    updaterIsRunning = false;
    qDebug()<<"关闭按钮被单击";
    if(ttstart)
    {
        ttstart->stopWork();
    }
    if(logUI)logUI->close();
    QApplication* app;
    app->exit(0);
}
void MainWindow::keyPressEvent(QKeyEvent *e)
{

    if(e->key()==Qt::Key_L && e->modifiers()==Qt::ShiftModifier)
    {
        if(logUI)logUI->show();
    }
    if(e->key()==Qt::Key_M && e->modifiers()==Qt::ShiftModifier)
    {
        if(md5makerUI==nullptr)
        {
            md5makerUI=new md5maker(this);
            md5makerUI->setModal(false);
            md5makerUI->setWindowTitle(_version);
        }
        md5makerUI->show();
    }
}
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG")//WINDOWS环境
    {
        Q_UNUSED(eventType);
        MSG *msg = static_cast<MSG*>(message);  //类型转换
        qDebug()<<msg->message<<msg->lParam<<msg->wParam;
        /*此处的结构也可用switch来代替*/
        if(msg->message==WM_COPYDATA)
        {
            qDebug()<<"妈的终于跑通了";
            COPYDATASTRUCT *data = reinterpret_cast<COPYDATASTRUCT*>(msg->lParam);

            qDebug()<<"接收到的消息类型"<<data->dwData;
            switch (data->dwData)
            {
            case 0:
                //wchar
            {
                data->lpData;
                data->cbData;
                QString str;
                str=QString((char*)data->lpData);
                qDebug()<<"接收到的文本消息"<<str;
            }
                break;
            case 1:
                //int
                break;
            default:
                break;
            }

            *result=233;
            return true;  //返回值为false表示该事件还会继续向上传递，被其他捕获
        }
    }
    if (eventType == "NSEvent")//MacOS环境
    {}
    if (eventType == "xcb_generic_event_t")//Linux环境
    {}
    return false;
}
void MainWindow::on_pushButton_Start_clicked() /*选择安装文件夹后file_search(多线程已完成)*/
{
    //getExistingDirectory(父对象一般是this,对话框标题,对话框开始目录  一般是"./",默认是只显示目录-https://doc.qt.io/qt-5/qfiledialog.html#Option-enum);
    //返回空对象则没选
    //选择了目录则返回QString
    logUI->log(modulemainWindows,"on_pushButton_Start_clicked()",NULL);
    if(alldone)
    {
        logUI->log(modulemainWindows,"尝试打开空荧地图",NULL);
        logUI->log(modulemainWindows,"start "+workPath+"/Map.exe",NULL);
        qDebug()<<QString("start "+workPath+"/Map.exe");
        QProcess process(this);
        QStringList argument;
        /*
        process.setProgram("cmd");
        argument <<"/c"
                 <<"start"
                 <<""""
                 <<workPath+"/Map.exe"
                   ;
        */
        QString e=workPath+"/Map.exe";
        process.setProgram(e);
        process.setArguments(argument);
        argument <<"/c";
        if(process.startDetached())
        {
            qDebug()<<"start √";
            logUI->log(modulemainWindows,"这句日志看不到了",NULL);
            QApplication* app;
            app->exit(0);
        }else{
            qDebug()<<"start ×";
            logUI->log(modulemainWindows,"start ×",NULL);
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
        tem =dir.toStdString();
        qDebug()<<"目标路径"<<dir;
        logUI->log(modulemainWindows,"目标路径"+dir,NULL);
        if(string(tem)==""){
            logUI->log(modulemainWindows,"为空路径",NULL);
            qDebug()<<"空路径";
            MainWindow::changeMainPage0label_Text("?刚才发生了什么?");
        }else{
            workPath=dir;
            threadWork(dir);
        };
    }


}
void MainWindow::threadWork(QString path)
{
    logUI->log(modulemainWindows,"初始化",NULL);
    updaterIsRunning = true;
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
    logUI->log(modulemainWindows,"新建线程",NULL);
    ttstart = new Start(path, NULL);

    //连接工作触发信号
    //connect(Start, &Start::workError, this, &MainWindow::Work_Error);
    connect(ttstart, &Start::tworkProcess
            ,this, &MainWindow::Work_Process);
    connect(ttstart, &Start::tworkFinished
            ,this, &MainWindow::Work_Finished);
    connect(ttstart, &Start::tworkMessageBox
            ,this, &MainWindow::Work_MessageBox);
    //connect(this,&MainWindow::stopTwork,ttstart,&Start::stopWork,Qt::DirectConnection);
    //日志窗口信号
    connect(ttstart,&Start::log
            ,logUI,&logViewer::log);
    //开始工作
    logUI->log(modulemainWindows,"新建定时器",NULL);
    timer1 = startTimer(500);//0.5s定时器
    logUI->log(modulemainWindows,"激活任务线程",NULL);
    emit ttstart->tstart();
}
void MainWindow::updataDlingmag()
{
    MainWindow::changeMainPage0label_Text(tNowWork());
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    //qDebug()<<"timerShot:"<<event->timerId();
    if(event->timerId()==timer1)
    {
        QString tem =tNowWork();
        //if(tem!="")ui->label_Dlnow->setText(tem);
        if(tem!="")MainWindow::changeMainPage0label_Text(tem);
    }
}
void MainWindow::Work_Finished(bool done)
{
    logUI->log(modulemainWindows,"任务线程主动上报完成信号",NULL);
    alldone=done;
    logUI->log(modulemainWindows,"删除计时器",NULL);
    killTimer(timer1);
}
void MainWindow::Work_MessageBox(int tag,QString title,QString txt)
{
    logUI->log(modulemainWindows,"非模态对话框 "+QString::number(tag),NULL);
    QMessageBox *box=new QMessageBox(this);
    switch (tag) {
    case 0:
        //信息
        box->setIcon(QMessageBox::Information);
        break;
    case 1:
        //错误
        box->setIcon(QMessageBox::Warning);
        break;
    default:
        printf("----------?--------\n");
    }
    box->setWindowTitle(title);
    box->setText(txt);
    box->setStandardButtons(QMessageBox::Ok);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setModal(false);//非模态
    box->setTextInteractionFlags(Qt::TextSelectableByMouse);
    box->show();
}



