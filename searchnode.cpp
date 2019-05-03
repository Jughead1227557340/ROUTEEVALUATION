#include "searchnode.h"
#pragma execution_character_set("utf-8")

SearchNode::SearchNode()
{
    x = 0.0;
    y = 0.0;
    z = -1.0;
    NodeKind = Invalidation;
    angle = 0.0;
    r = -1.0;
    routelen = 0.0;
    speed = 0;
    MatchNum = 0;
    matchLevel = -1;
    PressureFlag = 0;
    ErrLevel = 0;
    turnAngle = 0.0;
    heightChangeAngle = 0.0;
}
SearchNode::SearchNode(double xx, double yy, double zz, WayPointType type, double angle, double r, double turnAngle, double routeLength, double speed)
{
    x = xx;
    y = yy;
    z = zz;
    NodeKind = type;
    this->angle = angle;
    this->r = r;
    this->routelen = routeLength;
    this->speed = speed;
    MatchNum = 0;
    matchLevel = -1;
    PressureFlag = 0;
    ErrLevel = 0;
    this->turnAngle = turnAngle;
    heightChangeAngle = 0.0;
}
WayPointType TypeTranslate(QString strType)
{
    WayPointType tempType;
    if(strType == QString("目标点"))
        tempType = Target;
    else if(strType == QString("景象匹配控制点"))
        tempType = Match;
    else if(strType == QString("高度修正点"))
        tempType = AltituteAment;
    else if(strType == QString("转弯控制点"))
        tempType = TurnPoint;
    else if(strType == QString("转弯开始点"))
        tempType = TurnStart;
    else if(strType == QString("转弯结束点"))
        tempType = TurnEnd;
    else if(strType == QString("圆心"))
        tempType = CirCentre;
    else if(strType == QString("高度变化点"))
        tempType = HeightChange;
    else if(strType == QString("进入点"))
        tempType = EnterPoint;
    else if(strType == QString("GPS导航点"))
        tempType = GpsPoint;
    else if(strType == QString("无定义点"))
        tempType = Invalidation;
    else if(strType == QString("起始点"))
        tempType = Start;
    else
    {
        qDebug() << "*****没有匹配类型*****";
        tempType = Invalidation;
    }
    return tempType;
}
//两点之间的距离
double DistanceBetweenTwoPoints(double x1, double y1, double z1, double x2, double y2, double z2)
{
    double distance =  (x2 - x1) * (x2 - x1);
    distance += (y2 - y1) * (y2 - y1);
    distance += (z2 - z1) * (z2 - z1);
    distance = sqrt(distance);
    return distance;
}


