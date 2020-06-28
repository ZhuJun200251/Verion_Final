#include "magic.h"

//构造函数
Magic::Magic(const QString MagicImgPath, const QString MagicGIFPath, const QString MagicCircleImgPath, const int type)
{
    this->MagicImgPath = MagicImgPath;
    this->MagicGIFPath = MagicGIFPath;
    this->MagicCircleImgPath = MagicCircleImgPath;
    this->PutX = 0;
    this->PutY = 0;
    this->ImgX = 0;
    this->ImgY = 0;
    this->type = type;
}

QString Magic::GetMagicImgPath() const  //获取法术图标地址
{
    return this->MagicImgPath;
}

QString Magic::GetMagicGIFPath() const  //获取法术效果地址
{
    return this->MagicGIFPath;
}

QString Magic::GetMagicCircleImgPath() const //获取法阵地址
{
    return this->MagicCircleImgPath;
}

void Magic::SetMagicImgPath(const QString path)  //修改法术图标地址
{
    this->MagicImgPath = path;
}

int Magic::GetPutX() const  //获取法术释放点X坐标
{
    return PutX;
}

int Magic::GetPutY() const  //获取法术释放点Y坐标
{
    return PutY;
}

int Magic::GetImgX() const //获取圆圈X坐标
{
    return ImgX;
}

int Magic::GetImgY() const //获取圆圈Y坐标
{
    return ImgY;
}

void Magic::SetPutX(const int x)  //修改法术释放点X坐标
{
    this->PutX = x;
}

void Magic::SetPutY(const int y)  //修改法术释放点Y坐标
{
    this->PutY = y;
}

void Magic::SetImgX(const int x = 0) //修改法阵位置点X坐标
{
    this->ImgX = x;
}

void Magic::SetImgY(const int y = 0) //修改法阵位置点Y坐标
{
    this->ImgX = y;
}

int distance(const int x1, const int y1, const int x2, const int y2)
{
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void Magic::Effect(const int type, QVector<Monster *> MonsterVec) //法术效果函数
{
    for(Monster** iter=MonsterVec.begin(); iter != MonsterVec.end(); iter++)
        if(distance(ImgX, ImgY, (*iter)->GetX(), (*iter)->GetY()) <= 240)
        {
            if(!type)
                (*iter)->SetHealth((1 > (*iter)->GetHealth() - 300) ? 1 : (*iter)->GetHealth() - 300);
            else
                (*iter)->SetSpeed((*iter)->GetSpeed_Max() / 2);
        }
}
