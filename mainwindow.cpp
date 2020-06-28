#include "mainwindow.h"
#include <QDebug>
#include "globalstruct.h"       //选择框按钮全局结构
#include <math.h>               //数学
#include "greenturret.h"        //绿色防御塔类
#include "fireturret.h"         //火防御塔类
#include "lightturret.h"        //光炮防御塔类
#include "bigturret.h"          //大炮防御塔类
#include "magic.h"              //法术类

//圆周率π
#define PI 3.14159

//鼠标点击区域宏
#define MouClickRegion(X, Width, Y, Height)     \
(ev->x() >= (X) && ev->x() <= (X) + (Width) &&  \
ev->y() >= (Y) && ev->y() <= (Y) + (Height))

//计算两点之间距离宏
#define DistBetPoints(X1, Y1, X2, Y2)           \
sqrt((((X1) - (X2)) * ((X1) - (X2))) + (((Y1) - (Y2)) * ((Y1) - (Y2))))

//用于方便通过格子确定路径点坐标
#define X40(num) ((num) - 1) * 40 + 10

//插入怪物  路径点数组名、怪物初始坐标、怪物编号
#define InsterMonster(PathNum, StaCoorNum, MonsterId)     \
MonsterVec.push_back(new Monster(pointarr[PathNum], PathLength[PathNum], X40(staco[StaCoorNum].x), X40(staco[StaCoorNum].y), MonsterId));

int Map[16][26];    //用于拷贝不同的关卡数组

//音效播放函数
void MainWindow::MediaPlay(int type)
{
    switch (type) {
    //BGM
    case 0:
        player_BGM->setMedia(QUrl("qrc:/music/sagar001_002.mp3"));
        player_BGM->setVolume(10);
        player_BGM->play();
        break;

    //菜单失败错误音效
    case 1:
        player_money->setMedia(QUrl("qrc:/music/菜单失败错误音效_2.mp3"));
        player_money->setVolume(80);
        player_money->play();
        break;

    //建塔成功音效
    case 2:
        player_creation->setMedia(QUrl("qrc:/music/建塔成功音效.mp3"));
        player_creation->setVolume(80);
        player_creation->play();
        break;

    //拆塔音效
    case 3:
        player_dismantlement->setMedia(QUrl("qrc:/music/拆塔音效.mp3"));
        player_dismantlement->setVolume(80);
        player_dismantlement->play();
        break;

    //游戏失败音效
    case 4:
        player_lose->setMedia(QUrl("qrc:/music/游戏失败音效.mp3"));
        player_lose->setVolume(80);
        player_lose->play();
        break;

    //游戏成功音效
    case 5:
        player_victory->setMedia(QUrl("qrc:/music/游戏成功音效.mp3"));
        player_victory->setVolume(80);
        player_victory->play();
        break;

    //升级音效
    case 6:
        player_up->setMedia(QUrl("qrc:/music/升级音效.mp3"));
        player_up->setVolume(80);
        player_up->play();
        break;

    default:
        break;
    }
}

//判断金钱是否足够并刷新标签
bool MainWindow::DeductionMoney(int money)
{
    if(this->money - money < 0)
    {
        remindlable->show();
        MediaPlay(1);  //金钱不够音效

        QTimer* timer3 = new QTimer(this);
        timer3->start(1000);
        connect(timer3,&QTimer::timeout,[=]()
        {
            remindlable->hide();

            timer3->stop();
            delete timer3;

            update();
        });
        return true; //判断金钱是否足够
    }

    this->money -= money; //扣除金钱
    moneylable->setText(QString("%1").arg(this->money)); //刷新标签文本
    return false;
}

void MainWindow::RemindTime(int type) //法术刷新提示标签函数
{
    if(!type)
        remindTimelable->setText(QString("法术正在刷新，还有%1秒").arg(Thundertime));    //提示内容
    else
        remindTimelable->setText(QString("法术正在刷新，还有%1秒").arg(Slowtime));    //提示内容

    remindTimelable->show();
    QTimer* timer5 = new QTimer(this);
    timer5->start(1000);

    connect(timer5,&QTimer::timeout,[=]()
    {
        remindTimelable->hide();

        timer5->stop();
        delete timer5;

        update();
    });
}

//构造
MainWindow::MainWindow(int LevelNumber) : LevelNumber(LevelNumber)
{
    //设置固定窗口大小
    setFixedSize(1040, 640);

    //设置标题
    setWindowTitle("游戏界面");
    
    //BGM播放
    MediaPlay(0);

    //提示胜利标签
    QLabel *victorylable = new QLabel(this);
    victorylable->move(176, 180);
    victorylable->setFont(QFont("楷体", 110));
    victorylable->setText(QString("游戏胜利"));
    victorylable->hide();   //先行隐藏，等到胜利的时候再显示出来

    //提示失败标签
    QLabel *loselabel = new QLabel(this);
    loselabel->move(176, 180);
    loselabel->setFont(QFont("楷体", 110));
    loselabel->setText(QString("游戏失败"));
    loselabel->hide();   //先行隐藏，等到失败的时候再显示出来

    QTimer* timer2 = new QTimer(this);      //用于插入怪物定时器
    timer2->start(2000);   //怪物等待2秒才出现

    connect(timer2,&QTimer::timeout,[=]()
    {
        switch (LevelNumber) {
        case 0:
        {
            //设置路径点
            CoorStr* Waypointarr1[] = {new CoorStr(X40(8), X40(6)), new CoorStr(X40(2), X40(6)),
                                       new CoorStr(X40(2), X40(13)), new CoorStr(X40(19), X40(13)), new CoorStr(X40(19), X40(9)), new CoorStr(homecoor->x, homecoor->y)};
            CoorStr* Waypointarr2[] = {new CoorStr(X40(20), X40(5)), new CoorStr(X40(14), X40(5)),
                                       new CoorStr(X40(14), X40(9)), new CoorStr(X40(8), X40(9)), new CoorStr(X40(8), X40(6)), new CoorStr(X40(2), X40(6)),
                                             new CoorStr(X40(2), X40(13)), new CoorStr(X40(19), X40(13)),
                                       new CoorStr(X40(19), X40(9)), new CoorStr(homecoor->x, homecoor->y)}; //最后的路径点设为家
            //怪物的四个起始点
            CoorStr staco[] = {CoorStr(8, 0), CoorStr(20, 0), CoorStr(8, -1), CoorStr(20, -1)};

            //每条路径的结点个数
            int PathLength[] = {sizeof(Waypointarr1)/sizeof(CoorStr*), sizeof(Waypointarr1)/sizeof(CoorStr*)};

            IrodMonsProgDefa(Waypointarr1, Waypointarr2, staco, PathLength, victorylable);   //使用预设的两条路产生怪物方案
            break;
        }

        case 1:
        {
            //设置路径点
            CoorStr* Waypointarr1[] = {new CoorStr(X40(4), X40(8)), new CoorStr(X40(20), X40(8)),
                                       new CoorStr(X40(20), X40(13)), new CoorStr(X40(6), X40(13)), new CoorStr(homecoor->x, homecoor->y)};
            CoorStr* Waypointarr2[] = {new CoorStr(X40(11), X40(8)), new CoorStr(X40(20), X40(8)),
                                       new CoorStr(X40(20), X40(13)), new CoorStr(X40(6), X40(13)), new CoorStr(homecoor->x, homecoor->y)};

                                      //怪物的四个起始点
            CoorStr staco[] = {CoorStr(4, 0), CoorStr(11, 0), CoorStr(4, -1), CoorStr(11, -1)};

            //每条路径的结点个数
            int PathLength[] = {sizeof(Waypointarr1)/sizeof(CoorStr*), sizeof(Waypointarr1)/sizeof(CoorStr*)};

            IrodMonsProgDefa(Waypointarr1, Waypointarr2, staco, PathLength, victorylable);   //使用预设的两条路产生怪物方案
            break;
        }

        case 2:
        {
            //设置路径点
            CoorStr* Waypointarr1[] = {new CoorStr(X40(12), X40(9)), new CoorStr(X40(8), X40(9)),
                                       new CoorStr(X40(8), X40(0)), new CoorStr(homecoor->x, homecoor->y)};
            CoorStr* Waypointarr2[] = {new CoorStr(X40(12), X40(9)), new CoorStr(X40(16), X40(9)),
                                       new CoorStr(X40(16), X40(0)), new CoorStr(homecoor->x, homecoor->y)};

            //怪物的四个起始点
            CoorStr staco[] = {CoorStr(12, 16), CoorStr(12, 16), CoorStr(12, 17), CoorStr(12, 18)};

            //每条路径的结点个数
            int PathLength[] = {sizeof(Waypointarr1)/sizeof(CoorStr*), sizeof(Waypointarr1)/sizeof(CoorStr*)};

            IrodMonsProgDefa(Waypointarr1, Waypointarr2, staco, PathLength, victorylable);   //使用预设的两条路的产生怪物方案
            break;
        }
        default:
            break;
        }
    });

    //金钱标签
    moneylable->move(80, 20);       //位置
    setStyleSheet("color:white");   //设置颜色
    moneylable->setFont(QFont("微软雅黑", 20));             //设置金钱标签属性
    moneylable->setText(QString("%1").arg(money));    //显示金钱信息

    //提示标签
    remindlable->move(340, 250);
    remindlable->setStyleSheet("color:red");    //设置颜色为红色
    remindlable->setFont(QFont("微软雅黑", 24));             //设置提示标签属性
    remindlable->setText(QString("您没有足够的金钱！"));    //提示内容
    remindlable->hide();                      //先隐藏
    
    //提示刷新标签
    remindTimelable->move(340, 250);
    remindTimelable->setStyleSheet("color:blue");    //设置颜色为蓝色
    remindTimelable->setFont(QFont("微软雅黑", 24));             //设置提示标签属性
    remindTimelable->hide();                      //先隐藏

    //生命值标签
    lifelable->setGeometry(90, 60, 220, 40);   //设置控件位置和大小
    lifelable->setFont(QFont("微软雅黑", 20));
    lifelable->setText(QString("%1").arg(life));

    //怪物剩余波数标签
    monsterWave->setGeometry(850, 50, 220, 40);  //设置控件位置和大小
    monsterWave->setFont(QFont("微软雅黑", 20));
    monsterWave->setStyleSheet("color:yellow");   //设置颜色为黄色
    monsterWave->setText(QString("剩余波数：%1").arg(wave));

    //定时器每200毫秒执行一次防御塔父类数组的活动函数
    QTimer* timer = new QTimer(this);
    timer->start(180);

    //timer发出信号时进行的函数
    connect(timer,&QTimer::timeout,[=]()
    {
        //防御塔寻找目标怪物的规律：找到最后一个怪物作为目标，目标丢失后找再继续找最后一个目标
        for (auto defei : DefeTowerVec)
        {
            if (!defei->GetAimsMonster())   //目标怪物为空时从后往前遍历怪物数组寻找目标怪物
            {
                for(int i = MonsterVec.size() - 1; i >= 0; i--)
                    //这里以防御塔中心点和怪物中心点判断
                    if (DistBetPoints(defei->GetUpLeftX() + 40, defei->GetUpLeftY() + 40,
                        MonsterVec.at(i)->GetX() + (MonsterVec.at(i)->GetWidth() >> 1),
                        MonsterVec.at(i)->GetY() + (MonsterVec.at(i)->GetHeight() >> 1)) <= defei->GetRange())
                    {
                        defei->SetAimsMonster(MonsterVec.at(i));    //设置防御塔的目标怪物
                        break;  //找到后跳出循环
                    }
            }
            else                //当前防御塔拥有目标
                if (DistBetPoints(defei->GetUpLeftX() + 40, defei->GetUpLeftY() + 40,
                    defei->GetAimsMonster()->GetX() + (defei->GetAimsMonster()->GetWidth() >> 1),
                    defei->GetAimsMonster()->GetY() + (defei->GetAimsMonster()->GetHeight() >> 1)) <= defei->GetRange())
                {
                    //根据每个防御塔的目标怪物计算旋转角度
                    defei->SetRotatAngle(
                        atan2
                        (
                            defei->GetAimsMonster()->GetY() - defei->GetUpLeftY() + 40,
                            defei->GetAimsMonster()->GetX() - defei->GetUpLeftX()
                        ) * 180 / PI );

                     defei->InterBullet();           //拥有目标时一直创建子弹
                }

            //每次循环都判断目标怪物距离防御塔的距离
            if (defei->GetAimsMonster())    //目标怪物不为空
                if (DistBetPoints(defei->GetUpLeftX() + 40, defei->GetUpLeftY() + 40,
                    defei->GetAimsMonster()->GetX() + (defei->GetAimsMonster()->GetWidth() >> 1),
                    defei->GetAimsMonster()->GetY() + (defei->GetAimsMonster()->GetHeight() >> 1)) > defei->GetRange())
                        defei->SetAimsMonster(NULL);     //超过距离将目标怪物设为空
        }

        //防御塔子弹移动
        for (auto defei : DefeTowerVec)
            defei->BulletMove();

        //怪物移动
        for (auto Moni = MonsterVec.begin(); Moni != MonsterVec.end(); Moni++)
        {
            (*Moni)->Move();
            if(DistBetPoints((*Moni)->GetX(), (*Moni)->GetY(), homecoor->x, homecoor->y) <= 5) //怪物走到终点
            {
                delete *Moni;
                MonsterVec.erase(Moni);         //怪物走到终点则删除这个怪物

                MediaPlay(1);               //提示怪物进洞

                life--;                         //我们的生命值-1
                if(life<=9 && life>=6)
                    lifelable->setStyleSheet("color:yellow");   //设置颜色为黄色
                else if(life>=3)
                    lifelable->setStyleSheet("color:orange");   //设置颜色为橘色
                else
                    lifelable->setStyleSheet("color:red");   //设置颜色为红色
                lifelable->setText(QString("%1").arg(life));

                if (life <= 0)
                {
                    loselabel->show();  //显示失败标签

                    timer->stop();          //画面暂停
                    delete timer;

                    player_BGM->stop();                 //停止播放
                    delete player_BGM;
                    MediaPlay(4);                           //播放游戏失败音效

                    QTimer::singleShot(2000, this, SLOT(close()));  //等待两秒后自动关闭界面
                }

                break;
            }
        }

        //判断子弹击中怪物
        for (auto defei : DefeTowerVec)  //防御塔
        {
            auto &tbullvec = defei->GetBulletVec();    //临时存储子弹
            for (auto bullit = tbullvec.begin(); bullit != tbullvec.end(); bullit++)    //子弹
                for (auto monit = MonsterVec.begin(); monit != MonsterVec.end(); monit++)//怪物
                {
                    if ((*bullit)->GetX() + (defei->GetBulletWidth() >> 1) >= (*monit)->GetX() && (*bullit)->GetX() <= (*monit)->GetX() + (*monit)->GetWidth() &&
                            (*bullit)->GetY() + (defei->GetBulletHeight() >> 1) >= (*monit)->GetY() && (*bullit)->GetY() <= (*monit)->GetY() + (*monit)->GetHeight())
                    {   //击中怪物时
                        tbullvec.erase(bullit);     //删除子弹

                        (*monit)->SetHealth((*monit)->GetHealth() - defei->GetAttack());      //敌人血量 = 本身血量减去当前炮塔攻击力

                        if ((*monit)->GetHealth() <= 0) //生命值为空时
                        {
                            //判断一下其他防御塔的目标怪物是否和当前防御塔消灭的怪物重复，如果重复，则将那一个防御塔的目标怪物也设为空
                            for (auto defei2 : DefeTowerVec)
                                if (defei2->GetAimsMonster() == *monit)
                                    defei2->SetAimsMonster(NULL);

                            MonsterVec.erase(monit);    //删除怪物
                            DeductionMoney(-RewardMoney);       //击败怪物增加金钱
                        }

                        //这里不能将防御塔目标怪物设为空，因为防御塔的子弹攻击到的怪物不一定是该防御塔的目标怪物
                        goto L1;
                    }
                }
            L1:;
        }

        setMouseTracking(true);

        update();   //绘图
    });

    //显示防御塔范围按钮
    disranpush->setStyleSheet("QPushButton{color:yellow; background:blue}");  //蓝底黄字
    disranpush->setGeometry(20,120, 150, 45);
    disranpush->setFont(QFont("微软雅黑", 14));
    disranpush->setText("显示全部范围");

    connect(disranpush,&QPushButton::clicked,[=]()
    {
        DisplayAllRange ? DisplayAllRange = false : DisplayAllRange = true;  //通过改变标识令防御塔攻击范围显示或关闭
        update();
    });

    //暂停继续按钮
    stop_resume->setStyleSheet("QPushButton{border-image: url(image/暂停按钮.png)}");
    stop_resume->setGeometry(960, 20, 30, 30);

    connect(stop_resume,&QPushButton::clicked,[=]()
    {
        if(button_state1)
        {
            button_state1 = false;
            timer->stop();
            player_BGM->pause();
            stop_resume->setStyleSheet("QPushButton{border-image: url(image/继续按钮.png)}");
        }
        else
        {
            button_state1 = true;
            timer->start();
            player_BGM->play();
            stop_resume->setStyleSheet("QPushButton{border-image: url(image/暂停按钮.png)}");
        }
    });

    //BGM暂停继续按钮
    stop_resume_BGM->setStyleSheet("QPushButton{border-image: url(image/暂停BGM按钮.png)}");
    stop_resume_BGM->setGeometry(900, 20, 30, 30);

    connect(stop_resume_BGM,&QPushButton::clicked,[=]()
    {
        if(button_state2)
        {
            button_state2 = false;
            player_BGM->pause();
            stop_resume_BGM->setStyleSheet("QPushButton{border-image: url(image/继续BGM按钮.png)}");
        }
        else
        {
            button_state2 = true;
            player_BGM->play();
            stop_resume_BGM->setStyleSheet("QPushButton{border-image: url(image/暂停BGM按钮.png)}");
        }
    });
}

//预设的两条路产生怪物方案
void MainWindow::IrodMonsProgDefa(CoorStr** Waypointarr1, CoorStr** Waypointarr2, CoorStr* staco, int* PathLength, QLabel* victorylable)
{
    /*两条路径*/
    CoorStr** pointarr[] = {Waypointarr1, Waypointarr2};

    /*插入怪物*/
    if(counter >= 1 && counter <= 14)
    {//插入绵羊
        wave = 5;         //剩余5波
        monsterWave->setText(QString("剩余波数：%1").arg(wave));
        InsterMonster(0, 0, 1); //第几条路径、第几个起始点、怪物编号
    }
    else if(counter > 14 && counter <= 34)
    {//在、两路插入绵羊和眼镜蛇
        wave = 4;         //剩余4波
        monsterWave->setText(QString("剩余波数：%1").arg(wave));
        InsterMonster(0, 0, 1);
        InsterMonster(1, 1, 2);
    }
    else if (counter > 34 && counter <= 35)
    {//两路插入老鹰
        wave = 3;         //剩余3波
        monsterWave->setText(QString("剩余波数：%1").arg(wave));
        InsterMonster(0, 0, 3);
        InsterMonster(1, 1, 3);
    }
    else if (counter > 35 && counter <= 52)
    {//两路插入犀牛、犀牛、绵羊
        wave = 2;         //剩余2波
        monsterWave->setText(QString("剩余波数：%1").arg(wave));
        InsterMonster(0, 2, 4);
        InsterMonster(0, 0, 4);
        InsterMonster(1, 1, 1);
    }
    if(counter > 52 && counter <= 56)
    {//插入老鹰
        wave = 1;         //剩余1波
        monsterWave->setText(QString("剩余波数：%1").arg(wave));
        InsterMonster(0, 0, 3);
        InsterMonster(1, 1, 3);
    }
    if (counter > 52 && counter <= 71)
    {//插入眼镜蛇，绵羊
        wave = 0;         //剩余0波
        monsterWave->setText(QString("剩余波数：%1").arg(wave));
        InsterMonster(0, 2, 2);
        InsterMonster(0, 0, 5);
        InsterMonster(1, 3, 1);
        InsterMonster(1, 1, 4);
    }

    if (counter > 71 && MonsterVec.empty())   //时间大于71且怪物数组为空时游戏胜利
    {
        victorylable->show();       //显示标签
        player_BGM->stop();                 //停止播放
        delete player_BGM;
        MediaPlay(5);                   //游戏成功音效
    }

    counter++;          //计数器+1
    update();
}

//根据数组画出地图函数
//由绘图函数调用
void MainWindow::DrawMapArr(QPainter& painter)
{
    //地图数组  第一关
    int Map_1[16][26] =
    {
        9, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
        8, 0, 0, 0, 0, 0, 0, 1, 1, 3, 6, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 6, 0, 0, 0, 0, 0, 0, 3, 6, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 3, 6, 1, 1, 3, 6, 0, 0, 0, 0, 0, 0, 6, 6, 1, 1, 3, 6, 0, 0, 0,
        0, 0, 0, 0, 0, 6, 6, 1, 1, 6, 6, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 3, 6, 0, 1, 1, 0, 0, 0, 0, 3, 6, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 3, 6, 0, 1, 1, 0, 6, 6, 0, 1, 1, 0, 3, 6, 0, 6, 6, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 6, 6, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 6, 6, 1, 1, 1, 1, 1, 1, 5, 1,
        0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 3, 6, 0, 0, 3, 6, 0, 0, 3, 6, 0, 0, 3, 6, 1, 1, 3, 6, 0, 0, 0, 0,
        0, 1, 1, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 1, 1, 6, 6, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 11, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    //第二关
    int Map_2[16][26] =
    {
        9, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        8, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 3, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 3, 6, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 6, 6, 1, 1, 0, 0, 3, 6, 0, 1, 1, 0, 0, 3, 6, 0, 0, 3, 6, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 6, 6, 0, 1, 1, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 3, 6, 0, 0, 0, 0, 3, 6, 0, 0, 0, 0, 3, 6, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 6, 6, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 11, 0, 12, 0, 5, 1, 3, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    //第三关
    int Map_3[16][26] =
    {
        9, 0, 0, 0, 0, 3, 6, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        8, 0, 0, 0, 0, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 6, 0, 0, 3, 6, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 6, 0, 0, 6, 6, 1, 1, 3, 6, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 3, 6, 0, 0, 1, 1, 6, 6, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 6, 6, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 6, 0, 0, 3, 6, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 6, 0, 0, 6, 6, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 3, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 11, 0, 12, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    //选择关卡并且拷贝相应关卡的地图数组
    switch (LevelNumber)
    {
    case 0:
        memcpy(Map, Map_1, sizeof(Map));
        break;
    case 1:
        memcpy(Map, Map_2, sizeof(Map));
        break;
    case 2:
        memcpy(Map, Map_3, sizeof(Map));
        break;
    default:
        break;
    }

    //画出地图
    for (int j = 0; j < 16; j++)
        for (int i = 0; i < 26; i++)
        {
            switch (Map[j][i])
            {
            case 0:     /*草地*/
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                    QPixmap(":/image/草地块.png"));
                break;
            case 1:     /*地面*/
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                    QPixmap(":/image/地面.png"));
                break;
            case 3:     /*防御塔坑*/
                painter.drawPixmap(i * 40, j * 40, 80, 80,
                    QPixmap(":/image/石砖块.png"));
                //防御塔坑坐标初始化塔坑坐标，插入到塔坑对象数组
                TowerPitVec.push_back(new DefenseTowerPit(i * 40, j * 40));
                break;
            case 5:     //家在循环中也输出地面
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                    QPixmap(":/image/地面.png"));
                homecoor->x = i * 40, homecoor->y = j * 40;
                break;
            case 9:   //金钱标签坐标
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                    QPixmap(":/image/草地块.png"));
                moneycoor->x = i * 40+10, moneycoor->y = j * 40;
                break;
            case 8:    //生命值标签坐标
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                    QPixmap(":/image/草地块.png"));
                lifecoor->x = i * 40+10, lifecoor->y = j * 40;
                break;
            case 11:   //闪电图片坐标
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                     QPixmap(":/image/草地块.png"));
                thundercoor->x = i * 40+10, thundercoor->y = j * 40;
                break;
            case 12:  //缓慢图片坐标
                painter.drawPixmap(i * 40, j * 40, 40, 40,
                     QPixmap(":/image/草地块.png"));
                slowcoor->x = i * 40 + 10, slowcoor->y = j * 40;
                break;
            }
        }

    painter.drawPixmap(homecoor->x, homecoor->y, 80, 80,
                       QPixmap(":/image/房子.png")); //画出房子
    painter.drawPixmap(moneycoor->x, moneycoor->y, 80, 80,
                       QPixmap(":/image/金钱.png"));  //画出金钱标签
    painter.drawPixmap(lifecoor->x, lifecoor->y, 80, 80,
                       QPixmap(":/image/生命.png"));  //画出生命标签
}

//画出选择框
void MainWindow::DrawSelectionBox(QPainter& painter)
{
    //显示选择框
    if (!SelBox->GetDisplay())
        return;

   //画出选择框
    painter.drawPixmap(SelBox->GetX(), SelBox->GetY(), SelBox->GetWidth(), SelBox->GetHeight(),
        QPixmap(SelBox->GetImgPath()));

    //画出子按钮
    SubbutStr *ASubBut = SelBox->GetSelSubBut();    //接收子按钮结构数组
    for (int i = 0; i < 4; i++)
        painter.drawPixmap(ASubBut[i].SubX, ASubBut[i].SubY, ASubBut[i].SubWidth, ASubBut[i].SubHeight,
            QPixmap(ASubBut[i].SubImgPath));

    painter.setPen(QPen(Qt::yellow, 6, Qt::SolidLine));     //设置画笔
    painter.drawRect(QRect(SelBox->GetX() + 95, SelBox->GetY() + 95, 80, 80));
}

//画出防御塔
void MainWindow::DrawDefenseTower(QPainter& painter)
{
    //画出防御塔
    for (auto defei : DefeTowerVec)  //遍历防御塔数组
    {
        //画出底座
        painter.drawPixmap(defei->GetUpLeftX(), defei->GetUpLeftY(), 80, 80, QPixmap(defei->GetBaseImgPath()));

        //画出所有防御塔的攻击范围
        if(DisplayAllRange)
            painter.drawEllipse(QPoint(defei->GetUpLeftX() + 40, defei->GetUpLeftY() + 40), defei->GetRange(), defei->GetRange());

        //画出所有防御塔子弹
        for (auto bulli : defei->GetBulletVec())
            painter.drawPixmap(bulli->coor.x, bulli->coor.y, defei->GetBulletWidth(), defei->GetBulletHeight(),QPixmap(defei->GetBulletPath()));

        //画出防御塔
        painter.translate(defei->GetUpLeftX() + 40, defei->GetUpLeftY() + 40);          //设置旋转中心
        painter.rotate(defei->GetRotatAngle());             //旋转角度
        painter.translate(-(defei->GetUpLeftX() + 40), -(defei->GetUpLeftY() + 40));    //原点复位
        painter.drawPixmap(defei->GetX(), defei->GetY(), defei->GetWidth(), defei->GetHeight(), QPixmap(defei->GetDefImgPath())/*图片路径*/);

        painter.resetTransform();   //重置调整
    }
}

//画出怪物
void MainWindow::DrawMonster(QPainter& painter)
{
    static const int Health_Bar_Width = 60;

    for (auto moni : MonsterVec)//画出怪物
    {
        painter.drawPixmap(moni->GetX(), moni->GetY(), moni->GetWidth(), moni->GetHeight(), QPixmap(moni->GetImgPath()));
        painter.save();  //保存画笔状态

        //画出怪物的血条
        QPoint healthBarPoint(moni->GetX() - Health_Bar_Width/4 + 5, moni->GetY() - moni->GetHeight()/4);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
        QRect healthBarBackRect(healthBarPoint, QSize(Health_Bar_Width, 4));
        painter.drawRect(healthBarBackRect);

        painter.setBrush(Qt::green);
        QRect healthBarRect(healthBarPoint, QSize((double)moni->GetHealth() / moni->GetHealth_Max() * Health_Bar_Width, 4));
        painter.drawRect(healthBarRect);

        painter.restore();  //恢复画笔原状态
    }
}

//画出防御塔和升级按钮
void MainWindow::DrawRangeAndUpgrade(QPainter& painter)
{
    //根据条件画出防御塔攻击范围和升级按钮
    for (auto defei : DefeTowerVec)
    {
        if(defei->GetUpLeftX() == DisplayRangeX && defei->GetUpLeftY() == DisplayRangeY && DisplayRange)
        {   //画出防御塔攻击范围
            painter.setPen(QPen(Qt::red));  //使用红色画出范围
            painter.drawEllipse(QPoint(DisplayRangeX + 40, DisplayRangeY + 40), defei->GetRange(), defei->GetRange());

            if(defei->GetGrade() == 0)
            {
                painter.drawPixmap(DisplayRangeX - 50, DisplayRangeY - 80, 60, 60, QPixmap(":/image/等级1.png"));
                painter.drawPixmap(DisplayRangeX + 70, DisplayRangeY - 80, 60, 60, QPixmap(":/image/拆塔按钮.png"));
            }
            if(defei->GetGrade() == 1)
            {
                painter.drawPixmap(DisplayRangeX - 50, DisplayRangeY - 80, 60, 60, QPixmap(":/image/等级2.png"));
                painter.drawPixmap(DisplayRangeX + 70, DisplayRangeY - 80, 60, 60, QPixmap(":/image/拆塔按钮.png"));
            }
            if(defei->GetGrade() == 2)
            {
                painter.drawPixmap(DisplayRangeX - 50, DisplayRangeY - 80, 60, 60, QPixmap(":/image/等级3.png"));
                painter.drawPixmap(DisplayRangeX + 70, DisplayRangeY - 80, 60, 60, QPixmap(":/image/拆塔按钮.png"));
            }
            if(defei->GetGrade() == 3)
                painter.drawPixmap(DisplayRangeX + 10, DisplayRangeY - 80, 60, 60, QPixmap(":/image/拆塔按钮.png"));
        }
    }
}

//画出法术
void MainWindow::DrawMagicImg(QPainter &painter)
{
    painter.drawPixmap(thundercoor->x, thundercoor->y, 60, 60,
                           QPixmap(Thunder->GetMagicImgPath())); //画出闪电药剂图片

    painter.drawPixmap(slowcoor->x, slowcoor->y, 60, 60,
                           QPixmap(Slow->GetMagicImgPath()));  //画出缓慢药剂图片

    if(Thunder->ReadyToPut && Thunder->GetImgX() != 0)                                           //画出闪电法阵图片
        painter.drawPixmap(Thunder->GetImgX(), Thunder->GetImgY(), 240, 240,
                           QPixmap(Thunder->GetMagicCircleImgPath()));

    if(Slow->ReadyToPut && Slow->GetImgX() != 0)                                              //画出缓慢法阵图片
        painter.drawPixmap(Slow->GetImgX(), Slow->GetImgY(), 240, 240,
                           QPixmap(Slow->GetMagicCircleImgPath()));

    if(Thunder->ReadyToPut && Thunder->GetPutX() != 0)
    {
        DeductionMoney(300);                    //花费300释放闪电药剂
        painter.drawPixmap(Thunder->GetPutX() + 30, Thunder->GetPutY() + 30, 240, 240,
                           QPixmap(Thunder->GetMagicGIFPath()));  //画出火焰特效
        //重置
        Thunder->SetPutX(0);
        Thunder->SetPutY(0);
        Thunder->ReadyToPut = false;
    }

    if(Slow->ReadyToPut && Slow->GetPutX() != 0)
    {
        DeductionMoney(400);                    //花费400释放缓慢药剂
        painter.drawPixmap(Slow->GetPutX() + 30, Slow->GetPutY() + 30, 240, 240,
                           QPixmap(Slow->GetMagicGIFPath()));       //画出冰封特效
        //重置
        Slow->SetPutX(0);
        Slow->SetPutY(0);
        Slow->ReadyToPut = false;
    }
}

//法术时间重置
void MainWindow::ControlMagicTime(void)
{
    if(!Thunder->Time)
    {
        QTimer* timer4 = new QTimer(this);
        timer4->start(1000);                                        //每一秒刷新一次

        Thundertime = 20;                                                 //总计30秒
        connect(timer4, &QTimer::timeout,[=]()
        {
            Thundertime--;
            if(Thundertime == 0)                                    //刷新完成
            {
                Thunder->Time = true;
                Thunder->SetMagicImgPath(":/image/闪电药剂.png");

                timer4->stop();
                delete timer4;
            }
        });
    }

    if(!Slow->Time)
    {
        QTimer* timer5 = new QTimer(this);
        timer5->start(1000);                                        //每一秒刷新一次

        Slowtime = 20;                                                 //总计30秒
        connect(timer5, &QTimer::timeout,[=]()
        {
            Slowtime--;
            if(Slowtime == 0)                                    //刷新完成
            {
                Slow->Time = true;
                Slow->SetMagicImgPath(":/image/缓慢药剂.png");

                timer5->stop();
                delete timer5;
            }
        });
    }

    update();
}

//绘图事件
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);     //创建画家类

    painter.setRenderHint(QPainter::Antialiasing);    //设置抗锯齿

    DrawMapArr(painter);        //画出地图

    DrawDefenseTower(painter);  //画出防御塔和子弹

    DrawMonster(painter);       //画出怪物

    DrawRangeAndUpgrade(painter);//画出攻击范围和升级按钮

    DrawSelectionBox(painter);  //画出选择框

    DrawMagicImg(painter);    //画出法术
}

//鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousepos = event->pos();

    //这里发生了灵异事件，如果加入y轴坐标则会将y轴坐标当成x轴，故权衡之下，只能增加游戏难度了（逃）
    if(Thunder->ReadyToPut)
        Thunder->SetImgX(mousepos.x() - 120);

    if(Slow->ReadyToPut)
        Slow->SetImgX(mousepos.x() - 120);
}

//鼠标点击事件
void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::RightButton) //右键取消
    {
        DisplayRange = false;           //取消显示防御塔攻击范围
        DisplayAllRange = false;      //取消显示防御塔所有攻击范围
        SelBox->SetDisplay(false);      //取消显示选择框
        Thunder->ReadyToPut = false;   //取消准备释放闪电药剂
        Slow->ReadyToPut = false;    //取消准备释放缓慢药剂
        update();
    }

    if (DisplayRange)
    {
        //设置防御塔宽高，攻击力，微调坐标，拆塔。
        for (auto defei = DefeTowerVec.begin(); defei != DefeTowerVec.end(); defei++)
            if ((*defei)->GetUpLeftX() == DisplayRangeX && (*defei)->GetUpLeftY() == DisplayRangeY && DisplayRange)
            {
                //光炮升级技术
                if(typeid(*((*defei)->GetTypeid())) == typeid(LightTurret))  //原价240
                {
                    if((*defei)->GetGrade() == 0)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(300)) return;        //升级防御塔需要花费300
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/二级光炮.png");
                            (*defei)->SetGrade(1);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-120);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 1)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(350)) return;        //升级防御塔需要花费350
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/三级光炮.png");
                            (*defei)->SetGrade(2);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-270);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 2)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(400)) return;        //升级防御塔需要花费400
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetWidthHeight((*defei)->GetWidth() + 12, (*defei)->GetHeight() + 6);   //防御塔宽高增加
                            (*defei)->SetXY((*defei)->GetX() - 6, (*defei)->GetY() - 3); //调整防御塔坐标
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetGrade(3);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-445);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 3)
                        if(MouClickRegion(DisplayRangeX + 10, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-645);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                }


                //绿瓶升级技术
                if(typeid(*((*defei)->GetTypeid())) == typeid(GreenTurret))  //原价100
                {
                    if((*defei)->GetGrade() == 0)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(150)) return;        //升级防御塔需要花费150
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/二级绿瓶.png");
                            (*defei)->SetGrade(1);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-50);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 1)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(200)) return;        //升级防御塔需要花费200
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/三级绿瓶.png");
                            (*defei)->SetGrade(2);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-125);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 2)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(300)) return;        //升级防御塔需要花费300
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetWidthHeight((*defei)->GetWidth() + 12, (*defei)->GetHeight() + 6);   //防御塔宽高增加
                            (*defei)->SetXY((*defei)->GetX() - 6, (*defei)->GetY() - 3); //调整防御塔坐标
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetGrade(3);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-225);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 3)
                        if(MouClickRegion(DisplayRangeX + 10, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-375);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                }


                //火瓶升级技术
                if(typeid(*((*defei)->GetTypeid())) == typeid(FireTurret))  //原价150
                {
                    if((*defei)->GetGrade() == 0)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(200)) return;        //升级防御塔需要花费200
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/二级火瓶.png");
                            (*defei)->SetGrade(1);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-75);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 1)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(250)) return;        //升级防御塔需要花费250
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/三级火瓶.png");
                            (*defei)->SetGrade(2);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-175);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 2)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(300)) return;        //升级防御塔需要花费300
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetWidthHeight((*defei)->GetWidth() + 12, (*defei)->GetHeight() + 6);   //防御塔宽高增加
                            (*defei)->SetXY((*defei)->GetX() - 6, (*defei)->GetY() - 3); //调整防御塔坐标
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetGrade(3);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-300);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 3)
                        if(MouClickRegion(DisplayRangeX + 10, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-450);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                }


                //大炮升级技术
                if(typeid(*((*defei)->GetTypeid())) == typeid(BigTurret))  //原价400
                {
                    if((*defei)->GetGrade() == 0)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(450)) return;        //升级防御塔需要花费450
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/二级大炮.png");
                            (*defei)->SetGrade(1);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-200);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 1)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(500)) return;        //升级防御塔需要花费500
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetDefImgPath(":/image/三级大炮.png");
                            (*defei)->SetGrade(2);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-425);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 2)
                    {
                        if (MouClickRegion(DisplayRangeX - 50, 60 , DisplayRangeY - 80, 60))  //点击升级按钮
                        {
                            if (DeductionMoney(600)) return;        //升级防御塔需要花费600
                            MediaPlay(6);                                           //升级音效
                            (*defei)->SetAttack((*defei)->GetAttack() + 20);          //每次升级防御塔攻击力+20
                            (*defei)->SetWidthHeight((*defei)->GetWidth() + 12, (*defei)->GetHeight() + 6);   //防御塔宽高增加
                            (*defei)->SetXY((*defei)->GetX() - 6, (*defei)->GetY() - 3); //调整防御塔坐标
                            (*defei)->SetAimsMonster(NULL);                        //将防御塔目标设为空
                            (*defei)->SetRange() += 14;                            //设置防御塔的攻击范围
                            (*defei)->SetGrade(3);
                            break;
                        }
                        if(MouClickRegion(DisplayRangeX + 70, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-675);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                    }

                    if((*defei)->GetGrade() == 3)
                        if(MouClickRegion(DisplayRangeX + 10, 60, DisplayRangeY - 80, 60))  //点击拆塔按钮
                        {
                            MediaPlay(3);                       //拆塔音效
                            DeductionMoney(-975);
                            DefeTowerVec.erase(defei);
                            break;
                        }
                }
            }

            SelBox->SetDisplay(false);      //取消显示新建防御塔框
            DisplayRange = false;           //取消显示自己
            Thunder->ReadyToPut = false;   //取消准备释放闪电药剂
            Slow->ReadyToPut = false;    //取消准备释放缓慢药剂

            update();
            return;
    }

    //判断闪电药剂的点击
    if (MouClickRegion(thundercoor->x, 60 , thundercoor->y, 60))
    {
        Slow->ReadyToPut = false;                   //取消缓慢药剂的释放，防止冲突
        if(Thunder->Time && money>=300)  //闪电药剂刷新完毕并且金钱不少于300
        {
            this->setCursor(Qt::PointingHandCursor);
            Thunder->ReadyToPut = true;       //准备释放闪电药剂
        }

        if(!Thunder->Time)
            RemindTime(0);

        if(money < 300 && Thunder->Time)
            DeductionMoney(300);
        
        return;
    }

    //判断缓慢药剂的点击
    if (MouClickRegion(slowcoor->x, 60 , slowcoor->y, 60))
    {
        Thunder->ReadyToPut = false;          //同样是为了防止冲突
        if(Slow->Time && money>=400)  //缓慢药剂刷新完毕并且金钱不少于400
        {
            this->setCursor(Qt::PointingHandCursor);
            Slow->ReadyToPut = true;                //准备释放缓慢药剂
        }

        if(!Slow->Time)
            RemindTime(1);

        if(money < 400 && Slow->Time)
            DeductionMoney(400);

        return;
    }

    //释放法术
    for(int i=0; i<16; i++)
        for(int j=0; j<26; j++)
        {
            if(Map[i][j] == 1 || Map[i][j] == 0)   //只允许释放在草地和石块上
            {
                if(MouClickRegion(i * 40, 60, j * 40, 60))
                {
                    if(Thunder->ReadyToPut && !Slow->ReadyToPut) //释放闪电药剂
                    {
                        Thunder->SetPutX(i*40 - 120);          //获取释放位置，使得可以释放
                        Thunder->SetPutY(j*40 - 120);

                        Thunder->Time = false;                      //开始冷却
                        Thunder->SetMagicImgPath(":/image/闪电药剂准备状态.png");   //变化图片路径

                        ControlMagicTime();                         //时间冷却函数
                        Thunder->Effect(0, MonsterVec);

                        update();
                    }

                    if(Slow->ReadyToPut && !Thunder->ReadyToPut)     //释放缓慢药剂
                    {
                        Slow->SetPutX(i*40 - 120);
                        Slow->SetPutY(j*40 - 120);

                        Slow->Time = false;
                        Slow->SetMagicImgPath(":/image/缓慢药剂准备状态.png");

                        ControlMagicTime();                         //时间冷却函数
                        Slow->Effect(1, MonsterVec);

                        update();
                    }
                    this->setCursor(Qt::ArrowCursor);
                }
            }
        }

    //判断选择框四个子按钮的点击
    SubbutStr *ASubBut = SelBox->GetSelSubBut();
    for (int i = 0; i < 4; i++)
        if (MouClickRegion(ASubBut[i].SubX, ASubBut[i].SubWidth, ASubBut[i].SubY, ASubBut[i].SubHeight) && SelBox->GetDisplay())
        {
            SelBox->SetDisplay(false);      //取消显示选择框
            MediaPlay(2);                           //建塔成功音效

            //根据点击的不同的按钮，将防御塔子类插入到防御塔父类数组
            switch (i)
            {
                case 0: //绿瓶
                    if (DeductionMoney(100)) return;    //扣除金钱100
                    DefeTowerVec.push_back(new GreenTurret(SelBox->GetX() + 110, SelBox->GetY() + 112, SelBox->GetX() + 95, SelBox->GetY() + 95, 72, 46));
                    break;
                case 1: //火瓶
                    if (DeductionMoney(150)) return;    //扣除金钱150
                    DefeTowerVec.push_back(new FireTurret(SelBox->GetX() + 110, SelBox->GetY() + 112, SelBox->GetX() + 95, SelBox->GetY() + 95, 72, 46));
                    break;
                case 2: //光炮
                    if (DeductionMoney(250)) return;    //扣除金钱250
                    DefeTowerVec.push_back(new LightTurret(SelBox->GetX() + 110, SelBox->GetY() + 112, SelBox->GetX() + 95, SelBox->GetY() + 95, 76, 50));
                    break;
                case 3: //大炮
                    if (DeductionMoney(400)) return;    //扣除金钱400
                    DefeTowerVec.push_back(new BigTurret(SelBox->GetX() + 110, SelBox->GetY() + 104, SelBox->GetX() + 95, SelBox->GetY() + 95, 80, 70));
                    break;
                default:
                    break;
            }

            update();
            return;
        }

    //遍历所有塔坑
    for (auto APit : TowerPitVec)
        //判断点击塔坑
        if (MouClickRegion(APit->GetX(), APit->GetWidth(), APit->GetY(), APit->GetHeight()))
        {
            DisplayRange = false;                     //将防御塔的升级选择显示关闭
            for (auto defei : DefeTowerVec)      //遍历数组判断防御塔坐标和点击坑坐标重合则返回
                if(defei->GetUpLeftX() == APit->GetX() && defei->GetUpLeftY() == APit->GetY())
                {
                    DisplayRangeX = defei->GetUpLeftX(), DisplayRangeY = defei->GetUpLeftY();   //记录要显示攻击范围的防御塔的坐标
                    DisplayRange = true;        //显示防御塔攻击范围

                    return;
                }

            SelBox->CheckTower(APit->GetX(), APit->GetY());  //选中防御塔，选择框显示
            update();

            return;
        }

    DisplayRange = false;           //取消显示防御塔攻击范围
    SelBox->SetDisplay(false);      //取消显示选择框

    update();
}

//析构释放内存
MainWindow::~MainWindow()
{
    //释放防御塔坑指针数组TowerPitVec
    for (auto it = TowerPitVec.begin(); it != TowerPitVec.end(); it++)
    {
        delete *it;
        *it = NULL;
    }

    //释放选择框类SelBox
    delete SelBox;
    SelBox = NULL;

    //释放防御塔父类指针数组DefeTowerVec
    for (auto it = DefeTowerVec.begin(); it != DefeTowerVec.end(); it++)
    {
        delete *it;
        *it = NULL;
    }

    //释放怪物数组MonsterVec
    for (auto it = MonsterVec.begin(); it != MonsterVec.end(); it++)
    {
        delete *it;
        *it = NULL;
    }

    //释放位置coor指针
    delete homecoor;
    delete moneycoor;
    delete lifecoor;
    delete thundercoor;
    delete slowcoor;
}
