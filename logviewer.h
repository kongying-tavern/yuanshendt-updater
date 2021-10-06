#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QtWidgets>
#include <vector>
using namespace std;
struct rectangle
{
    int x;
    int y=0;
    int w;
    int h;
};
struct logHTTP
{
    int tid;
    QStringList log;
};
//QLabel *logLabel[5];


namespace Ui {
class logViewer;
}

class logViewer : public QDialog
{
    Q_OBJECT

public:
    explicit logViewer(QWidget *parent = nullptr);
    ~logViewer();

    vector<QTextEdit*> logTextEdit;
    vector<QJsonArray*> logJson;
    vector<logHTTP*> stlogHTTP;

public slots:
    void moveLogViewer(QPoint e,QPoint mainPoint);
    void log(int module,QString str,int mod);

private slots:
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void showEvent(QShowEvent *event);

private:
    Ui::logViewer *ui;
    void logUpdate(int c);

    /*显示控制*/
    int editNowChoose=1;
    void chooseEdit(int num);


};

#endif // LOGVIEWER_H
