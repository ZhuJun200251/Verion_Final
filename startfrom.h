#ifndef STARTFROM_H
#define STARTFROM_H

#include <QWidget>
//开始界面
namespace Ui {
class StartFrom;
}

class StartFrom : public QWidget
{
public:
    explicit StartFrom(QWidget *parent = 0);
    ~StartFrom();
private:
    Ui::StartFrom *ui;
};

#endif // STARTFROM_H
