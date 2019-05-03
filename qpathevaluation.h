#ifndef QPATHEVALUATION_H
#define QPATHEVALUATION_H

#include <QObject>
#include <QDebug>
#include "searchnode.h"
#include "pareto.h"
#define MAXTURNANGLE 60
#define MAXHEIGHTCHANGEANGLE 60
#define MAXTURNNUMBER 6 //转弯次数最大值
#define MAXHEIGHTCHANGENUMBER 3 //高度变化次数最大值
#define MAXROUTELENGTH 8000
enum ThreatType{THAAD, MIM_104, INVALID};
ThreatType ThreatTypeTransfer(QString type);
//威胁区参数
struct ThreatArea
{
    ThreatType threatType;
    double x;
    double y;
    double z;
    double detectRadius; //雷达探测半径
    double c1;
    double c2; //雷达探测参数
    double tDelay; //武器反应时间
    double speed; //武器飞行速度
    double destroyRadius; //武器拦截半径
    double destroyAltitude; //最高拦截高度
};
struct navigationNode
{
    double x;
    double y;
    double z;
};

class QPathEvaluation : public QObject
{
    Q_OBJECT
public:
    explicit QPathEvaluation(QObject * parent = 0);
    explicit QPathEvaluation(int routenumber, QObject * parent = 0);

public:
    int m_RouteNumber; //航迹序号
    RouteLine m_routeLine; //存储航迹的航迹链表
    double m_routeLength; //航迹长度(航程)
    double m_MobileConsumption; //机动损耗
    double m_survivability; //生存能力
    double m_matchingupLevel; //匹配等级
    double m_OverallEvalResult; //综合评价结果

    int m_turnNumber; //转弯点个数
    int m_heightNumber; //高度变化点个数
    Pareto m_pareto;
    //有关帕累托解集的一些参数


public:

    virtual ~QPathEvaluation();
    QPathEvaluation(QObject *parent, int routenumber);
    //计算航迹长度
    double CalcRouteLength();
    //计算激动消耗
    double CalcMobileConsumption();
    //计算生存概率
    double CalcSurvivability(QVector<ThreatArea> threatAreaList);
    //计算匹配等级
    double CalcMatchingupLevel();
    //计算综合评价
    double CalcOverallEvaluation();
    //设置转弯点,添加转弯起始点和转弯结束点
    bool SetTurnPoint();
    //等号运算符重载
    void operator = (const QPathEvaluation & temp);
};
typedef QVector<QPathEvaluation *> PathEvaluationList;

//威胁区类型转换

//正态分布函数
double normalDistribution(double x, double u, double det);
//由两向量计算角度
double calculateAngleByTwoVectors(double vector1, double vector2);
//计算被探测概率
int detectProbability(double x, double y, double z, QVector<ThreatArea> ThreatAreaList, QVector<int> & ThreatAreaNumber, QVector<double> & sampleNodeDetectList);
//计算被跟踪概率
double followProbability(double x, double y, double z,  double stepLength, SearchNode * node, double navigationAngle, double speed, double heightChangeAngle, QVector<ThreatArea> ThreatAreaList, QVector<int> ThreatAreaNumber, QVector<double> & sampleNodeFollowList);
//计算被跟踪概率
double followProbability(double x, double y, double z, SearchNode * node, double navigationAngle, double speed, double heightChangeAngle, QVector<ThreatArea> ThreatAreaList);
//计算被摧毁概率
double destroyProbability(double x, double y, double z, QVector<ThreatArea> ThreatAreaList, QVector<int> ThreatAreaNumberList, QVector<double> & sampleNodeDestroyList);
//数值积分
double NumericalIntegration(QVector<double> dBlockProbability);
//计算计算截面积
double calculateRCS(double x, double y, double z, double xx, double yy, double zz, double navigationAngle, double heightChangeHeight);
//设置转弯点

#endif // QPATHEVALUATION_H
