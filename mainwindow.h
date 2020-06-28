#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPainter>     //画家
#include <QMouseEvent>  //鼠标事件
#include <Qtimer>       //定时器
#include "defensetowerpit.h"    //防御塔坑类
#include "selectionbox.h"       //选择框类
#include "defetowerparent.h"    //防御塔父类
#include "monster.h"    //怪物类
#include "magic.h"   //法术类
#include <QLabel>  //标签类
#include <QPushButton>  //按钮类
#include <QMediaPlayer> //音乐类

class MainWindow : public QWidget
{
private:
    void paintEvent(QPaintEvent *);         //绘图事件
    void mousePressEvent(QMouseEvent *);    //鼠标点击事件
    void mouseMoveEvent(QMouseEvent *);   //鼠标移动事件
    void DrawMapArr(QPainter&);             //用于画出地图函数
    void DrawSelectionBox(QPainter&);       //用于画出选择框
    void DrawDefenseTower(QPainter&);       //画出防御塔
    void DrawMonster(QPainter&);            //画出怪物
    void DrawRangeAndUpgrade(QPainter&);    //画出防御塔攻击范围，升级按钮，拆塔按钮
    void DrawMagicImg(QPainter&);     //画出法术图标
    void DrawMagicGIF(QPainter&);     //画出法术释放类似GIF

    void IrodMonsProgDefa(CoorStr**, CoorStr**, CoorStr*, int*, QLabel*); //预设两条路产生怪物方案函数
    void ControlMagicTime(void);   //法术刷新时间控制
    void RemindTime(int);                     //法术刷新提示函数
    void MediaPlay(int);                    //音效播放函数

    bool DeductionMoney(int);         //判断金钱是否足够并刷新标签函数

    QVector<DefenseTowerPit*> TowerPitVec;  //防御塔坑数组
    QVector<DefeTowerParent*> DefeTowerVec; //重要防御塔父类数组
    QVector<Monster*> MonsterVec;           //怪物数组

    QLabel *moneylable = new QLabel(this);          //显示金钱标签控件
    QLabel *remindlable = new QLabel(this);          //提示金钱不够控件
    QLabel *lifelable = new QLabel(this);                 //生命值标签
    QLabel *remindTimelable = new QLabel(this); //提示法术正在刷新控件
    QLabel *monsterWave = new QLabel(this);      //提示剩余怪物波数

    QPushButton* disranpush = new QPushButton(this);                //显示所有防御塔范围按钮
    QPushButton* stop_resume = new QPushButton(this);              //暂停继续按钮
    QPushButton* stop_resume_BGM = new QPushButton(this);    //暂停继续BGM按钮

    QMediaPlayer* player_BGM = new QMediaPlayer;  //BGM播放
    QMediaPlayer* player_money = new QMediaPlayer;  //金钱不够音效
    QMediaPlayer* player_up = new QMediaPlayer;     //升级音效
    QMediaPlayer* player_victory = new QMediaPlayer;  //胜利音效
    QMediaPlayer* player_lose = new QMediaPlayer;    //失败音效
    QMediaPlayer* player_dismantlement = new QMediaPlayer;  //拆塔音效
    QMediaPlayer* player_creation = new QMediaPlayer;  //建塔音效

    CoorStr *homecoor = new CoorStr(0, 0);         //记录家的坐标，从地图中自动获取
    CoorStr *moneycoor = new CoorStr(0, 0);       //记录金钱的坐标，从地图中自动获取
    CoorStr *lifecoor = new CoorStr(0, 0);            //记录生命值的坐标，从地图中自动获取
    CoorStr *thundercoor = new CoorStr(0, 0);   //记录闪电药剂的坐标，从地图中自动获取
    CoorStr *slowcoor = new CoorStr(0, 0);        //记录缓慢药剂的坐标，从地图中自动获取

    SelectionBox* SelBox = new SelectionBox(":/image/选择框.png"); //选择框类

    Magic* Thunder = new Magic(":/image/闪电药剂.png", ":/image/火焰特效.png", ":/image/闪电药剂法阵.png", 0);
    Magic* Slow = new Magic(":/image/缓慢药剂.png", ":/image/魔法冰封.png", ":/image/缓慢药剂法阵.png", 1);

    bool button_state1 = true;          //暂停继续按钮状态
    bool button_state2 = true;          //暂停继续BGM按钮状态
    bool DisplayAllRange = false;  //标识是否显示所有防御塔的攻击范围
    bool DisplayRange = false;              //用于显示防御塔攻击范围

    int DisplayRangeX, DisplayRangeY;       //用于记录显示范围的防御塔的坐标
    int money = 1500;                                 //记录金钱
    int life = 10;                                           //生命数量
    int Thundertime = 0;                               //记录刷新时间
    int Slowtime = 0;
    int counter = 0;                                    //用于产生怪物的计数器
    int wave = 6;                                         //用于记录剩余怪物波数

    const int RewardMoney = 50;     //每次击败怪物获得的金钱数量
    const int LevelNumber;      //标识关卡

public:
    MainWindow(int);            //构造
    ~MainWindow();
};

#endif  //MAINWINDOW_H
