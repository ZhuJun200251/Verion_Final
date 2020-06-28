#include "monster.h"
#include <QDebug>

//怪物类函数实现
Monster::Monster(CoorStr **pointarr, int arrlength, int x, int y, int fid) :
    mx(x), my(y), id(fid)
{
    for(int i = 0; i < arrlength; i++)      //将传进来的数组插入到Waypoint动态数组
        Waypoint.push_back(pointarr[i]);

    //确定不同怪物的生命值
    switch (id)
    {
    case 1: //绵羊
        health_max = 200;             //初始生命值
        speed_max = 10;                  //初始速度
        mwidth = 64, mheight = 64;  //宽高
        ImgPath = ":/image/怪物1.png";
        break;
    case 2: //眼镜蛇
        health_max = 250;
        speed_max = 10;
        mwidth = 64, mheight = 64;
        ImgPath = ":/image/怪物2.png";
        break;
    case 3: //老鹰
        health_max = 500;
        speed_max = 10;
        mwidth = 72, mheight = 90;
        ImgPath = ":/image/怪物3.png";
        break;
    case 4: //犀牛
        health_max = 310;
        speed_max = 10;
        mwidth = 98, mheight = 70;
        ImgPath = ":/image/怪物4.png";
        break;
    case 5: //螺蟹
        health_max = 800;
        speed_max = 5;
        mwidth = 90, mheight = 76;
        ImgPath = ":/image/怪物5.png";
        break;
    default:
        break;
    }

    health = health_max;   //生命值
    mspeed = speed_max;  //移动速度
}

//怪物按设定路径点移动
bool Monster::Move()
{
    if(Waypoint.empty())
        return true;

    //如果第一个路径点的y小于怪物原本的路径点，则怪物向下走
    if (Waypoint.at(0)->y > my) //下
    {
        my += mspeed;
        return false;
    }

    if (Waypoint.at(0)->x < mx) //左
    {
        mx -= mspeed;
        return false;
    }

    if (Waypoint.at(0)->x > mx) //右
    {
        mx += mspeed;
        return false;
    }

    if (Waypoint.at(0)->y < my) //上
    {
        my -= mspeed;
        return false;
    }

    //如果怪物的坐标和路径点坐标几乎重合，则删除这个路径点
    if (Waypoint.at(0)->y == my && Waypoint.at(0)->x == mx)
    {
        delete Waypoint.begin();                //释放坐标点内存
        Waypoint.erase(Waypoint.begin());       //从数组中删除
    }

    return false;
}

int Monster::GetX() const       //获取横坐标
{
    return mx;
}

int Monster::GetY() const       //获取横坐标
{
    return my;
}

int Monster::GetWidth() const   //获取宽
{
    return mwidth;
}

int Monster::GetHeight() const  //获取高
{
    return mheight;
}

QString Monster::GetImgPath() const //获取图片路径
{
    return ImgPath;
}

int Monster::GetId() const      //获取编号
{
    return id;
}

int Monster::GetHealth() const  //获取生命值
{
    return health;
}

int Monster::GetHealth_Max() const  //获取初始生命值
{
    return health_max;
}

int Monster::GetSpeed() const  //获取移动速度
{
    return mspeed;
}

int Monster::GetSpeed_Max() const  //获取初始移动速度
{
    return speed_max;
}

void Monster::SetHealth(int fhealth)//设置生命值
{
    health = fhealth;
}

void Monster::SetSpeed(int fspeed)//设置移动速度
{
    mspeed = fspeed;
}
