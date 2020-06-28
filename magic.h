#ifndef MAGIC_H
#define MAGIC_H

#include "monster.h"
#include <QLabel>
#include <cmath>
#include <Qtimer>       //定时器

class Magic : QLabel
{
public:
    Magic(const QString, const QString, const QString, const int);

    QString GetMagicImgPath() const;              //获取图片路径
    QString GetMagicGIFPath() const;               //获取效果图片路径
    QString GetMagicCircleImgPath() const;    //获取药剂圆圈图片途径
    void SetMagicImgPath(const QString);   //改变效果图片的路径

    void SetPutX(const int);   //释放点位置改变
    void SetPutY(const int);
    void SetImgX(const int);  //圆圈位置改变
    void SetImgY(const int);
    int GetPutX() const;         //释放点位置获取
    int GetPutY() const;
    int GetImgX() const;        //圆圈位置点坐标获取
    int GetImgY() const;

    bool ReadyToPut = false;  //用于记录药剂是否被点击准备释放
    bool Time = true;              //用于记录药剂是否刷新完毕

    void Effect(const int, QVector<Monster*>);  //法术效果函数
private:
    QString MagicImgPath;              //图片路径
    QString MagicGIFPath;              //效果图片路径
    QString MagicCircleImgPath;   //圆圈图片路径
    int PutX, PutY;                          //释放点位置坐标
    int ImgX, ImgY;                         //圆圈位置点坐标
    int type;                                     //法术类别
};

#endif // MAGIC_H
