#ifndef SEARCHNODE_H
#define SEARCHNODE_H
#include <QVector>
#include <QString>
#include <QDebug>
#pragma execution_character_set("utf-8")
#define PI 3.1415926
enum WayPointType{Target, //目标点
                  Match, //匹配点
                  AltituteAment, //高度修正点
                  TurnPoint, //转弯点
                  TurnStart, //转弯开始点
                  TurnEnd, //转完结束点
                  CirCentre, // 圆心
                  HeightChange, //高度变化点
                  EnterPoint, //进入点
                  GpsPoint, //GPS导航点
                  Start, //起始点
                  Invalidation //无定义点
                  };
//还差了一个转弯角度
class SearchNode
{
public:
    SearchNode();
    SearchNode::SearchNode(double xx, double yy, double zz, WayPointType type, double angle, double r, double turnAngle, double routeLength, double speed);
public:
    double x;
    double y;
    double z;
    WayPointType NodeKind; //节点属性
    double angle; //航行方向
    double r; //转弯半径
    double heightChangeAngle; //高度变化角度
    double turnAngle; //转弯角度
    double routelen; //航迹长度
    double speed; //航行速度
    int MatchNum; //匹配区序号
    int PressureFlag; //雷达测高高度修正去标记，1表示航迹上已有雷达测高高度修正区，0表示没有
    int matchLevel; //雷达测高修正区的等级
    int ErrLevel;  //散布大小
    SearchNode * pFather; //父节点指针
};

#endif // SEARCHNODE_H
typedef QVector<SearchNode *> RouteLine;

//导航点类型转换
WayPointType TypeTranslate(QString strType);
//由三点确定圆的半径，转弯起始点 和 转弯结束点
//bool FindCircleCentreByThreePoint(double& Radius,double &x1,double &y1,double &x2,double &y2,double &swervex,double &swervey);
//两点之间的距离
//double DistanceBetweenTwoPoints(double x1, double y1, double x2, double y2);
//三点之间的距离
double DistanceBetweenTwoPoints(double x1, double y1, double z1, double x2, double y2, double z2);
//确定圆弧的开始角度，扫过的角度
//double dCircleArc(double& centerx,double centery,double Radius, double& point1x,double& point1y,double& point2x,double& point2y,double& dStartAngle,double& dSweepAngle,int& iMode);
//三点是否共线
//int  ThreePointInLine(double &x1,double &y1,double &x2,double &y2,double &x3,double &y3);

