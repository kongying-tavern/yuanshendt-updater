#ifndef MD5MAKER_H
#define MD5MAKER_H

#include <QDialog>

namespace Ui {
class md5maker;
}

class md5maker : public QDialog
{
    Q_OBJECT

public:
    explicit md5maker(QWidget *parent = nullptr);
    ~md5maker();

private slots:
    void on_pushButton_clicked();

private:
    Ui::md5maker *ui;
};

#endif // MD5MAKER_H
