#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QString>
#include <QStringList>



namespace Ui {
class logViewer;
}

class logViewer : public QDialog
{
    Q_OBJECT

public:
    explicit logViewer(QWidget *parent = nullptr);
    ~logViewer();

public slots:
    void moveLogViewer(QPoint e,QPoint mainPoint);
    void log(int module,QString str,void* mod);
private:
    Ui::logViewer *ui;
    void logUpdate();

};

#endif // LOGVIEWER_H
