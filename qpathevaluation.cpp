#include "qpathevaluation.h"


QPathEvaluation::QPathEvaluation(QObject *parent) : QObject(parent)
{
    //各项指标赋初值
    m_routeLength = 0.0; //航迹长度（直飞能量消耗）
    m_MobileConsumption = 0.0; //激动能量消耗
    m_survivability = 0.0; //生存能力
    m_matchingupLevel = 0.0; //匹配等级
    m_OverallEvalResult = 0.0; //综合评价

    m_turnNumber = 0; //转弯点个数
    m_heightNumber = 0; //高度变化点个数
    m_pareto.DominatedTimesCount = 0;
    m_pareto.TotalAnglePenaltyScore = 0.0;
}
QPathEvaluation::QPathEvaluation(int routenumber, QObject * parent) : QObject(parent)
{
    m_RouteNumber = routenumber; //航迹序号
    m_routeLength = 0.0; //航迹长度（直飞能量消耗）
    m_MobileConsumption = 0.0; //激动能量消耗
    m_survivability = 0.0; //生存能力
    m_matchingupLevel = 0.0; //匹配等级
    m_OverallEvalResult = 0.0; //综合评价

    m_turnNumber = 0; //转弯点个数
    m_heightNumber = 0; //高度变化点个数

    m_pareto.DominatedTimesCount = 0;
    m_pareto.TotalAnglePenaltyScore = 0.0;
}

//析构函数，释放航迹指针
QPathEvaluation::~QPathEvaluation()
{

}
void QPathEvaluation::operator=(const QPathEvaluation & temp)
{
    this->m_RouteNumber = temp.m_RouteNumber;
    this->m_routeLength = temp.m_routeLength;
    this->m_MobileConsumption = temp.m_MobileConsumption;
    this->m_survivability = temp.m_survivability;
    this->m_matchingupLevel = temp.m_matchingupLevel;
    this->m_pareto.DominatedTimesCount = temp.m_pareto.DominatedTimesCount;
    this->m_pareto.TotalAnglePenaltyScore = temp.m_pareto.TotalAnglePenaltyScore;
    this->m_pareto.score = temp.m_pareto.score;
}

//计算整个航迹的长度
double QPathEvaluation::CalcRouteLength()
{
    return m_routeLine.at(m_routeLine.size() - 1)->routelen / 1000;
}

//计算真实的机动消耗
double QPathEvaluation::CalcMobileConsumption()
{
    m_turnNumber = 0; //转弯点个数
    m_heightNumber = 0; //高度变化点个数
    double tempTurnAngle = 0.0; //转弯角度和
    double tempHeightChangeAngle = 0.0; //高度变化角度和
    int i;
    for (i = 0; i < m_routeLine.size(); i++)
    {
        //qDebug() << "第" << i + 1 << "个节点的类型为：" << m_routeLine.at(i)->NodeKind;
        if ((m_routeLine.at(i) != nullptr) && (m_routeLine.at(i)->NodeKind == TurnStart))
        {
            qDebug() << "该点为转弯起始点，且转弯角度为：" << m_routeLine.at(i)->turnAngle;
            tempTurnAngle += abs(m_routeLine.at(i)->turnAngle);
            m_turnNumber++;
        }
        else if ((m_routeLine.at(i) != nullptr) && (m_routeLine.at(i)->NodeKind == HeightChange))
        {
            tempHeightChangeAngle += abs(m_routeLine.at(i)->heightChangeAngle);
            m_heightNumber++;
        }
        else
        { 
            continue;
        }
    }
    //qDebug() << "机动消耗计算完毕！！！";
    return (tempTurnAngle + tempHeightChangeAngle);
}

//计算突防概率
double QPathEvaluation::CalcSurvivability(QVector<ThreatArea> threatAreaList)
{

    //1.读取每一点的威胁数据
    //2.通过数值积分计算总体被捕捉的概率
    //3.根据公式计算出突防概率
    double sampleStepLength = 5000; //采样步长(m)
    double sample_x, sample_y, sample_z; //采样点坐标(高斯坐标)
    double dNavigationAngle; //航向角（-180~180）
    double tempHeightAngle;
    double dSurvivability; //生存概率
    int iThreatAreaNumber; //所处的威胁区个数
    QVector<double> sampleNodeThreatList; //采样点威胁存续列表
    QVector<int> ThreatAreaNumberList; //威胁区序号列表
    QVector<double> sampleNodeDetectList; //采样点被探测列表
    QVector<double> sampleNodeFollowList; //采样点被跟踪列表
    QVector<double> sampleNodeDestroyList; //采样点被破坏列表
    //第一个点为起始点,最后一个点为目标点，对中间的所有点就行遍历
    qDebug() << "该航迹的航迹点个数为：" << m_routeLine.size();
    for (int i = 0; i < m_routeLine.size() - 1; i++)
    {
        qDebug() << "******第" << i + 1 << "个航迹点******";

        //非转弯点转弯起始点和高度变化点，即导弹在直飞的情况下
        SearchNode * tempNode = m_routeLine.at(i);
        if ((tempNode != nullptr) && (tempNode->NodeKind != TurnStart))
        {
            qDebug() << "******该航迹点为非转弯点******";
            //航向角，度数制转换为弧度制, cos等三角函数的参数为弧度制
            dNavigationAngle = m_routeLine.at(i)->angle * PI / 180;
            tempHeightAngle = 0;
            //以采样步长进行采样
            for (double d = 0; d <= (m_routeLine.at(i + 1)->routelen - m_routeLine.at(i)->routelen); d = d + sampleStepLength)
            {
                if ((m_routeLine.at(i + 1)->routelen - m_routeLine.at(i)->routelen) <= 0)
                {
                    qDebug() << "ERROR!!!ROUTE LENGHT!!!";
                    break;
                }
                qDebug() << "******该段航迹长度为：" << (m_routeLine.at(i + 1)->routelen - m_routeLine.at(i)->routelen);
                qDebug() << "******采样长度为：" << d;
                qDebug() << "******开始进行采样******";
                //将vector清空
                sampleNodeDetectList.clear();
                sampleNodeFollowList.clear();
                sampleNodeDestroyList.clear();
                ThreatAreaNumberList.clear();
                //当前采样点坐标
                sample_x = m_routeLine.at(i)->x + d * cos(tempHeightAngle) * cos(dNavigationAngle);
                sample_y = m_routeLine.at(i)->y + d * cos(tempHeightAngle) * sin(dNavigationAngle);
                sample_z = m_routeLine.at(i)->z + d * sin(tempHeightAngle);
                //计算被探测概率,以及存储相关威胁区的序号
                qDebug() << "******开始计算探测概率******";
                iThreatAreaNumber = detectProbability(sample_x, sample_y, sample_z, threatAreaList, ThreatAreaNumberList, sampleNodeDetectList);
                qDebug() << "******探测概率计算完毕******";
                if (iThreatAreaNumber > 0)
                {
                    //计算被跟踪概率
                    qDebug() << "******开始计算跟踪概率******";
                    followProbability(sample_x, sample_y, sample_z, d, tempNode, dNavigationAngle, m_routeLine.at(i)->speed, tempHeightAngle, threatAreaList, ThreatAreaNumberList, sampleNodeFollowList);
                    qDebug() << "******跟踪概率计算完毕******";
                    //计算被摧毁概率
                    qDebug() << "******开始计算摧毁概率******";
                    destroyProbability(sample_x, sample_y, sample_z, threatAreaList, ThreatAreaNumberList, sampleNodeDestroyList);
                    qDebug() << "******摧毁概率计算完毕******";
                }
                //计算被击落的概率
                int poss = 1;
                for (int i = 0; i < iThreatAreaNumber; i++)
                {
                    poss *=(1 - sampleNodeDetectList[i] * sampleNodeFollowList[i] * sampleNodeDestroyList[i]);
                }
                sampleNodeThreatList.push_back(1 - poss);
            }
        }
        else
        {
            qDebug() << "******该航迹点为转弯点******";
            double dNavigationAngle = m_routeLine.at(i)->angle * PI / 180; //航向角， 度数值转换为弧度制
            tempHeightAngle = m_routeLine.at(i)->heightChangeAngle * PI / 180; //高度变化角， 度数值转换为弧度制
            for (double d = 0; d <= (m_routeLine.at(i + 1)->routelen - m_routeLine.at(i)->routelen); d = d + sampleStepLength)
            {
                qDebug() << "******该段航迹长度为：" << (m_routeLine.at(i + 1)->routelen - m_routeLine.at(i)->routelen);
                qDebug() << "******采样长度为：" << d;
                qDebug() << "******开始进行采样******";
                //将vector清空
                sampleNodeDetectList.clear();
                sampleNodeFollowList.clear();
                sampleNodeDestroyList.clear();
                //当前采样点坐标
                sample_x = m_routeLine.at(i)->x + d * cos(tempHeightAngle) * cos(dNavigationAngle);
                sample_y = m_routeLine.at(i)->y + d * cos(tempHeightAngle) * sin(dNavigationAngle);
                sample_z = m_routeLine.at(i)->z + sin(tempHeightAngle);
                //计算被探测概率,以及存储相关威胁区的序号
                qDebug() << "******开始计算探测概率******";
                iThreatAreaNumber = detectProbability(sample_x, sample_y, sample_z, threatAreaList, ThreatAreaNumberList, sampleNodeDetectList);
                qDebug() << "******探测概率计算完毕******";
                if (iThreatAreaNumber > 0)
                {
                    //计算被跟踪概率
                    qDebug() << "******开始计算跟踪概率******";
                    followProbability(sample_x, sample_y, sample_z, d, tempNode, dNavigationAngle, m_routeLine.at(i)->speed, tempHeightAngle, threatAreaList, ThreatAreaNumberList, sampleNodeFollowList);
                    qDebug() << "******跟踪概率计算完毕******";
                    //计算被摧毁概率
                    qDebug() << "******开始计算摧毁概率******";
                    destroyProbability(sample_x, sample_y, sample_z, threatAreaList, ThreatAreaNumberList, sampleNodeDestroyList);
                    qDebug() << "******摧毁概率计算完毕******";
                }
                //计算被击落的概率
                int poss = 1;
                for (int i = 0; i < iThreatAreaNumber; i++)
                {
                    poss *= (1 - sampleNodeDetectList[i] * sampleNodeFollowList[i] * sampleNodeDestroyList[i]);
                }
                sampleNodeThreatList.push_back(1 - poss);
            }
        }
    }
    //计算生存概率
    dSurvivability = exp(-NumericalIntegration(sampleNodeThreatList));
    return dSurvivability;
}

//计算匹配等级
double QPathEvaluation::CalcMatchingupLevel()
{
    //根据内含式和穿越式的所占比例，采样长度为200m或者1000m的气压修正区所占比例，
    //和最后一个景象匹配区的散布大小来确定匹配水平
    int inMatchArea_Number = 0; //内含式匹配区数目
    int matchArea_Number = 0; //总的景象匹配区数目
    int airPress1000_Number = 0; //散布为1000的气压修正区数目
    int airPress_Num = 0; //总的气压修正区的数目
    int GPS_Number = 0;
    int lastFlag; //最后一个匹配区散布为50m时为1， 100m时为0
    for(int i = 0; i < m_routeLine.size(); i++)
    {
        SearchNode * tempSearchNode = m_routeLine.at(i);
        if (tempSearchNode != nullptr)
        {
            //step1 计算内涵式景象匹配区的数目
            if (tempSearchNode->NodeKind == 1)
            {
                matchArea_Number++;
                if (tempSearchNode->matchLevel == 1)
                {
                    inMatchArea_Number++;
                }
            }
            //step2 计算采样长度为200的气压修正区数目
            if (tempSearchNode->NodeKind == 2)
            {
                airPress_Num++;
                if (tempSearchNode->matchLevel == 4)
                {
                    airPress1000_Number++;
                }
            }
            if (tempSearchNode->NodeKind == 9)
            {
                GPS_Number++;
            }
        }
    }
    //查找最后一个匹配区的散布
    //方法：从目标点逆循环所有航迹点，若找到匹配区则跳出
    for (int i = m_routeLine.size() - 1; i >= 0; i--)
    {
        SearchNode * tempSearchNode = m_routeLine.at(i);
        if ((tempSearchNode != nullptr) && (tempSearchNode->NodeKind == 1))
        {
            if (tempSearchNode->ErrLevel == 50) //当导航点类型为匹配时的散布大小
                lastFlag = 1;
            else
                lastFlag = 0;
            break;

        }
    }
    //内含式景象匹配区所占总的景象匹配区比例的因子为0.5
    //采样为1000的气压修正区所占总的总的气压高度修正区比例因子为0.3
    //最后一个景象匹配区散布为50的比例因子为0.1
    //出去GPS点后的航迹点个数所占总航迹点个数的比例因子为0.1
    double totalLevel;
    if (matchArea_Number == 0)
    {
        totalLevel = lastFlag * 0.1 + 0.1 * (1 - GPS_Number / m_routeLine.size());
    }
    else if (airPress1000_Number == 0)
    {
        totalLevel = 0.5 * inMatchArea_Number / matchArea_Number + 0.1 * lastFlag + 0.1 * (1 - GPS_Number / m_routeLine.size());
    }
    else
    {
        totalLevel = 0.5 * inMatchArea_Number / matchArea_Number + 0.3 * airPress1000_Number / airPress_Num + 0.1 * lastFlag + 0.1 * (1 - GPS_Number / m_routeLine.size());
    }
    return totalLevel;
}
//航迹综合评价
double QPathEvaluation::CalcOverallEvaluation()
{
    //综合评价航迹
    //step1 : 检查各项权系数的合理性(求其和)
    //step2 : 将各单项指标标准化
    //step3 : 求综合评价


    //第一步：检查各项权系数的合理性（求其和）
    //七项指标的 权系数
    //判断矩阵结构
    //         航程   机动消耗  突防概率 匹配等级
    //   航程    1       3/5     1/3    1/2
    //机动消耗   5/3      1      5/9     5/6
    //突防概率    3       9/5      1     3/2
    //匹配概率    2       6/5    2/3     1
    double a[4][5] =
    {
        {1.0000, 0.6000, 0.3333, 0.5000, 1.0000},
        {1.6667, 1.0000, 0.5556, 0.8333, 1.0000},
        {3.0000, 1.8000, 1.0000, 1.5000, 1.0000},
        {2.0000, 1.2000, 0.6667, 1.0000, 1.0000}
    };
    double aw[4] = {0, 0, 0, 0}; //特征向量初值

    double maxtz = 0.0; //最大特征值
    double cm = 0.0; //一致性指标
    double a1 = 0.0; //临时变量

    //*******求特征向量***********
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j <4; j++)
        {
            a[i][4] = a[i][4] * a[i][j]; //第i行之积
        }
    }
    qDebug() << "各项系数为 ：";
    for (int i = 0; i < 4; i++)
    {
        a[i][4] = float(pow(a[i][4], 1/6.0));
        qDebug() << a[i][4] << " ";
        a1 = a1 + a[i][4];
    }
    qDebug() << "标准化后各项权系数为 ：";
    for (int i = 0; i < 4; i++)
    {
        a[i][4] = a[i][4] / a1; //求各项权系数
        qDebug() << a[i][4] << " ";
    }
    //*******求特征向量结束***********

    //求最大特征值
    qDebug() << "aw为：";
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            aw[i] = aw[i] +a[i][j]*a[j][6];
        }
        qDebug() << aw[i] << " ";
    }

    qDebug() <<"maxtz为：";
    for (int i = 0; i < 4; i++)
    {
        maxtz = maxtz +aw[i]/ a[i][6];
    }
    qDebug() << maxtz << endl;
    maxtz = maxtz / 4.0;
    cm = (maxtz - 6)/(3*1.24);
    //求最大特征值结束
    //权系数数组付给特定变量
    double weightLength = a[0][4];
    double weightConsume = a[1][4];
    double weightSurvive = a[2][4];
    double weightMatchup = a[3][4];
    //第二部：将各单项指标标准化
    //航迹长度的标准化处理
    m_routeLength = (MAXROUTELENGTH - m_routeLength) / MAXROUTELENGTH;
    //机动消耗标准化处理
    m_MobileConsumption = 1 - m_MobileConsumption / (MAXTURNNUMBER * MAXTURNANGLE + MAXHEIGHTCHANGENUMBER * MAXHEIGHTCHANGEANGLE);
    //突防概率标准化处理，突防概率本来就是0~1之间的数，故不需要标准化处理
    //匹配概率标准化处理，匹配概率本来就是0~1之间的数，故不需要标准化处理
    return (m_routeLength * weightLength + m_MobileConsumption * weightConsume + m_survivability * weightSurvive + m_matchingupLevel * weightMatchup);
}



//设置转弯点,添加转弯起始点和转弯结束点
bool QPathEvaluation::SetTurnPoint()
{
    //判断该航迹点的数组是否为空
    if(m_routeLine.size() <= 0)
    {
        return false;
    }
    //设置每个节点转弯角度
    for (int i = 0; i < m_routeLine.size(); i++)
    {
        if (i == m_routeLine.size() - 1)
        {
            m_routeLine.at(i)->angle = 0.0;
        }
        else
        {
            //
            double tempx_x = m_routeLine.at(i + 1)->x - m_routeLine.at(i)->x;
            double tempy_y = m_routeLine.at(i + 1)->y - m_routeLine.at(i)->y;
            m_routeLine.at(i)->angle = atan2(tempy_y, tempx_x);
        }
    }
    //计算航迹长度
    for (int i = 0; i < m_routeLine.size(); i++)
    {
        if (i == 0)
        {
            m_routeLine.at(i)->routelen = 0.0;
        }
        else
        {
            //
            double tempx_x = abs(m_routeLine.at(i)->x - m_routeLine.at(i - 1)->x);
            double tempy_y = abs(m_routeLine.at(i)->y - m_routeLine.at(i - 1)->y);
            m_routeLine.at(i)->routelen = sqrt(tempx_x * tempx_x + tempy_y * tempy_y);
        }
    }
    int tempCurrentPosition; //当前点在航迹数组的位置
    double tempTurnRadius; //当前点转弯半径
    double tempTurnX, tempTurnY; //当前点的坐标
    int i;
    for (i = 1; i < m_routeLine.size() - 1; )
    {
        if (m_routeLine.at(i)->NodeKind == TurnPoint)
        {
            //qDebug() << "转弯角度为：" << m_routeLine.at(i)->turnAngle;
            //确认前后都有航迹点
            if ((m_routeLine.at(i - 1) != nullptr) && (m_routeLine.at(i + 1) != nullptr))
            {

                tempCurrentPosition = i;
            }
            //取当前点的转弯半径
            tempTurnRadius = m_routeLine.at(i)->r;
            if (tempTurnRadius <= 0)
            {
                return false;
            }

            //取当前转弯点的坐标
            tempTurnX = m_routeLine.at(i)->x;
            tempTurnY = m_routeLine.at(i)->y;
            //计算转弯起始点以及转弯结束点的坐标
            double tempTurnStartX, tempTurnStartY;
            double tempTurnEndX, tempTurnEndY;
            //计算转弯起始点以及转完结束点到转弯点的直线长度 单位为m
            double tempLengthP2P = abs(m_routeLine.at(tempCurrentPosition)->r * tan((m_routeLine.at(tempCurrentPosition)->turnAngle / 2) * PI / 180));
            //计算转弯结束点的坐标
            tempTurnEndX =  tempTurnX + tempLengthP2P * cos((m_routeLine.at(tempCurrentPosition)->angle) * PI / 180);
            tempTurnEndY =  tempTurnY + tempLengthP2P * sin((m_routeLine.at(tempCurrentPosition)->angle) * PI / 180);
            //计算转弯起始点的坐标
            tempTurnStartX = tempTurnX - tempLengthP2P * cos((m_routeLine.at(tempCurrentPosition - 1)->angle) * PI / 180);
            tempTurnStartY = tempTurnY - tempLengthP2P * sin((m_routeLine.at(tempCurrentPosition - 1)->angle) * PI / 180);
            //圆弧长度 单位为m
            double tempArcLength = PI * m_routeLine.at(tempCurrentPosition)->turnAngle * m_routeLine.at(tempCurrentPosition)->r / 180;
            qDebug() << "转弯起始点到转弯点的距离" << tempLengthP2P;
            qDebug() << "圆弧的长度为" << tempArcLength;
            //创建转弯起始点以及转弯结束点的类对象
            SearchNode * startingNode = new SearchNode(tempTurnStartX, tempTurnStartY, m_routeLine.at(tempCurrentPosition - 1)->z, TurnStart, m_routeLine.at(tempCurrentPosition - 1)->angle, tempTurnRadius,  m_routeLine.at(tempCurrentPosition)->turnAngle, m_routeLine.at(tempCurrentPosition)->routelen - tempLengthP2P, m_routeLine.at(tempCurrentPosition - 1)->speed);
            SearchNode * endingNode = new SearchNode(tempTurnEndX, tempTurnEndY, m_routeLine.at(tempCurrentPosition)->z, TurnEnd, m_routeLine.at(tempCurrentPosition)->angle, tempTurnRadius,  m_routeLine.at(tempCurrentPosition)->turnAngle, m_routeLine.at(tempCurrentPosition)->routelen - tempLengthP2P + tempArcLength, m_routeLine.at(tempCurrentPosition)->speed);
            //qDebug() << "转弯起始点的类型为：" << startingNode.NodeKind << ",且转弯角度为：" << startingNode.turnAngle;
            //qDebug() << "转弯结束点的类型为：" << endingNode.NodeKind << ",且转弯角度为：" << endingNode.turnAngle;
            //对转弯点以后的所有点的航迹长度进行修改
            for(int j = tempCurrentPosition + 1; j < m_routeLine.size(); j++)
            {
                m_routeLine.at(j)->routelen = m_routeLine.at(j)->routelen - 2 * tempLengthP2P + tempArcLength;
            }
            //将转弯起始点和转弯结束点插入到航迹链表中
            m_routeLine.remove(i);
            m_routeLine.insert(i, startingNode);
            m_routeLine.insert(i+1, endingNode);
            qDebug() << "转弯起始点的类型为：" << m_routeLine.at(i)->NodeKind << ",且转弯角度为：" << m_routeLine.at(i)->turnAngle;
            qDebug() << "转弯结束点的类型为：" << m_routeLine.at(i+1)->NodeKind << ",且转弯角度为：" << m_routeLine.at(i+1)->turnAngle;
            i = i + 2;
        }
        else
        {
            i++;
        }
    }
    //设置每个节点的父节点
    qDebug() << "设置父节点开始！！！";
    for (int i = 0; i < m_routeLine.size(); i++)
    {

        qDebug() << "第" << i + 1 << "个节点的类型为：" << m_routeLine.at(i)->NodeKind;
        if (i == 0)
        {
            m_routeLine.at(i)->pFather = nullptr;
        }
        else
        {
            m_routeLine.at(i)->pFather = m_routeLine.at(i - 1);
        }
    }
    qDebug() << "设置父节点结束！！！";
    return true;
}


//由两向量计算角度
double calculateAngleByTwoVectors(double vector1, double vector2)
{
    return atan2(vector2, vector1);
}
//计算计算截面积
double calculateRCS(double x, double y, double z, double sample_x, double sample_y, double sample_z, double dNavigationAngle, double tempHeightAngle)
{
    //假设导弹长为10米，直径为1米
    double xy = sqrt((x - sample_x)*(x - sample_x) + (y - sample_y)*(y - sample_y));
    double theta = calculateAngleByTwoVectors((y - sample_y), (x - sample_x));
    double phi = calculateAngleByTwoVectors((z - sample_z), xy);
    if ((theta == dNavigationAngle) && (phi = tempHeightAngle))
    {
        return PI * 0.5 * 0.5;
    }
    else if (theta == dNavigationAngle)
    {
        double beta = phi - tempHeightAngle;
        if (beta > 0 && beta <= 90)
        {
            return 1 * 10 * sin(beta);
        }
        else if (beta < 0)
        {
            return 1 * 10 * sin(-beta);
        }
        else
        {
            return 1 * 10 * cos(beta - 90);
        }
    }
    else if (phi == tempHeightAngle)
    {
        double alpha = theta - dNavigationAngle;
        if (abs(alpha) == 90)
        {
            return 1 * 10;
        }
        else if(theta = -dNavigationAngle)
        {
            return PI * 0.5 * 0.5;
        }
        else if (alpha > 0 && alpha < 90)
        {
            return 1 * 10 * sin(alpha);
        }
        else if (alpha > -90 && alpha < 0)
        {
            return 1 * 10 * sin(-alpha);
        }
        else if (alpha > 90 && alpha < 180)
        {
            return 1 * 10 * cos(alpha - 90);
        }
        else
        {
            return 1 * 10 * cos(-alpha - 90);
        }
    }
    else
    {
            return 1 * 10;
    }
}

//计算被探测概率， 包括固定威胁，突发威胁，移动威胁
int detectProbability(double x, double y, double z, QVector<ThreatArea> ThreatAreaList, QVector<int> & ThreatAreaNumberList,  QVector<double> & sampleNodeDetectList)
{
    double RCS = 1; //假定雷达所测到的目标的截面积均为1
    double detectProbability; //被一个威胁区探测到的概率
    int number = 0; //所处的威胁区数量
    //遍历威胁区数组，探测航迹点是否处于威胁区内以及所在威胁区的序号
    for (int i = 0; i < ThreatAreaList.size(); i++)
    {
        //威胁区的球心坐标，假定雷达和发射装置的位置均为威胁区球心坐标
        double sample_x = ThreatAreaList.at(i).x;
        double sample_y = ThreatAreaList.at(i).y;
        double sample_z = ThreatAreaList.at(i).z;

        //威胁区雷达参数
        int c1 = ThreatAreaList.at(i).c1;
        double c2 = ThreatAreaList.at(i).c2;
        double radarRadius = ThreatAreaList.at(i).detectRadius;

        //采样点与威胁区中心的距离(单位为km)
        double distance = DistanceBetweenTwoPoints(x, y, z, sample_x, sample_y, sample_z) / 1000;
        if ((distance >= 0) && (distance <= radarRadius) && (z > sample_z))
        {
            number++;
            ThreatAreaNumberList.push_back(i);
            //计算导弹被一个威胁区探测到概率
            detectProbability = 1.0 / (1.0 + c2 * pow((pow(distance, 4)/RCS), c1));
            sampleNodeDetectList.push_back(detectProbability);
        }
    }
    return number;
}

//计算被探测概率， 包括固定威胁，突发威胁，移动威胁
double detectProbability(double x, double y, double z, double dNavigationAngle, double tempHeightAngle, QVector<ThreatArea> ThreatAreaList)
{
    double RCS; //截面积
    double detectProbability; //被一个威胁区探测到的概率
    double undetectProbability = 1; //未被多个威胁区探测到的概率
    double distance;
    //遍历威胁区数组，探测航迹点是否处于威胁区内以及所在威胁区的序号
    for (int i = 0; i < ThreatAreaList.size(); i++)
    {
        double sample_x = ThreatAreaList.at(i).x;
        double sample_y = ThreatAreaList.at(i).y;
        double sample_z = ThreatAreaList.at(i).z;
        int c1 = ThreatAreaList.at(i).c1;
        double c2 = ThreatAreaList.at(i).c2;
        distance = DistanceBetweenTwoPoints(x, y, z, sample_x, sample_y, sample_z) / 1000; //从单位为M转换为KM
        if ((distance >= 0) && (distance <= ThreatAreaList.at(i).detectRadius))
        {
            //计算导弹的截面积
            RCS = calculateRCS(x, y, z, sample_x, sample_y, sample_z, dNavigationAngle, tempHeightAngle);
            //计算导弹被一个威胁区探测到概率
            detectProbability = 1.0 / (1.0 + c2*pow((pow(distance, 4)/RCS), c1));
            undetectProbability *= (1 - detectProbability);
        }
    }
    return (1 - undetectProbability);
}

//计算被跟踪概率
double followProbability(double x, double y, double z, double sampleStepLength, SearchNode * node, double dNavigationAngle, double speeding, double tempHeightAngle, QVector<ThreatArea> ThreatAreaList, QVector<int> ThreatAreaNumberList, QVector<double> & sampleNodeFollowList)
{
    double size = ThreatAreaNumberList.size();
    navigationNode detectPointInAdvance; //提前探测点

    for (int i = 0; i < ThreatAreaNumberList.size(); i++)
    {
        //每个威胁区发射装置的发射反应时间
        double tDelay = ThreatAreaList.at(ThreatAreaNumberList.at(i)).tDelay;
        //每个威胁区拦截弹的飞行速度
        double speed = ThreatAreaList.at(ThreatAreaNumberList.at(i)).speed;
        //导航点距离每个威胁区中心的距离
        double distance = DistanceBetweenTwoPoints(x, y, z, ThreatAreaList.at(ThreatAreaNumberList.at(i)).x, ThreatAreaList.at(ThreatAreaNumberList.at(i)).y, ThreatAreaList.at(ThreatAreaNumberList.at(i)).z) / 1000;
        //每个威胁区的提前跟踪时间
        double followTime = tDelay + distance / speed;
        //提前跟踪距离
        double distanceInAdvance = followTime * speeding;
        //计算提前跟踪点的位置
        if (node->NodeKind == TurnStart)
        {
            qDebug() << "******该点为转弯起始点******";
            //如果采样点前一个航迹点时转弯起始点，即采样点在圆弧上
            if (sampleStepLength >= distanceInAdvance)
            {
                //如果采样步长大于提前跟踪距离，则提前跟踪点也在圆弧上
                double normalAngle; //法线方向
                double navigationAngle = node->angle; //转弯起始点的航向角
                if (node->turnAngle > 0)
                {
                    normalAngle = navigationAngle + 90;
                }
                else
                {
                    normalAngle = navigationAngle - 90;
                }
                //得到圆心坐标
                double circleX = node->x + node->r * cos(normalAngle);
                double circleY = node->y + node->r * sin(normalAngle);

                if (node->turnAngle > 0)
                {
                    //计算圆弧扫过的角度
                    double tempArcAngle = ((sampleStepLength - distanceInAdvance)* 180 / (PI * node->r));
                    //采样点相对于圆心的角度
                    double tempAngle = 90 - (node->angle + node->turnAngle) + (node->turnAngle - tempArcAngle);
                    detectPointInAdvance.x = circleX + node->r * cos(tempAngle);
                    detectPointInAdvance.y = circleY + node->r * sin(tempAngle);
                    detectPointInAdvance.z = node->z;
                }
            }
            else
            {
                //跟踪起始点不在圆弧上，在上一段航迹上
                int distanceDiff = distanceInAdvance - sampleStepLength;
                detectPointInAdvance.x = node->x + distanceDiff * cos(node->angle);
                detectPointInAdvance.y = node->y + distanceDiff * sin(node->angle);
                detectPointInAdvance.z = node->z;
            }
        }
        else
        {
            qDebug() << "******该点为非转弯起始点******";
            //如果采样点前一个航迹点不是转弯起始点，即采样点在直线上
            //先计算当前采样点到前一个航迹点的直线距离
            if (sampleStepLength >= distanceInAdvance)
            {
                //提前跟踪点刚好在该段航迹上
                detectPointInAdvance.x = node->x - distanceInAdvance * cos(node->angle);
                detectPointInAdvance.y = node->y - distanceInAdvance * sin(node->angle);
                detectPointInAdvance.z = node->z;
            }
            else
            {
                //采样点在航迹点之前的那段航迹上
                if (node->pFather == nullptr)
                {
                    qDebug() << "******父节点为空！！！******";
                    //如果父节点为空
                    sampleNodeFollowList.push_back(0);
                    continue;
                }
                else
                {
                    if (node->pFather->NodeKind == TurnStart)
                    {
                        //取当前点的航向角，并得到其法线方向
                        double normalAngle; //法线方向
                        double navigationAngle = node->angle;
                        if (node->turnAngle > 0)
                        {
                            normalAngle = navigationAngle + 90;
                        }
                        else
                        {
                            normalAngle = navigationAngle - 90;
                        }
                        //得到圆心坐标
                        double circleX = node->x + node->r * cos(normalAngle);
                        double circleY = node->y + node->r * sin(normalAngle);
                        //弧长
                        double tempArcLengh = distanceInAdvance - sampleStepLength;
                        //弧长所占的角度
                        double tempARCAngle = tempArcLengh * 180 / (node->pFather->r * PI);
                        //采样点相对于圆心的位置，以及采样点的坐标
                        if (node->turnAngle > 0)
                        {
                            detectPointInAdvance.x = circleX + node->r * cos(90 - abs(node->angle) + tempARCAngle);
                            detectPointInAdvance.y = circleY + node->r * sin(90 - abs(node->angle) + tempARCAngle);
                            detectPointInAdvance.z = node->z;
                        }
                        else
                        {
                            detectPointInAdvance.x = circleX + node->r * cos(90 - abs(node->angle) - tempARCAngle);
                            detectPointInAdvance.y = circleY + node->r * sin(90 - abs(node->angle) - tempARCAngle);
                            detectPointInAdvance.z = node->z;
                        }
                    }
                    else
                    {
                        qDebug() << "该点不是转弯起始点" << ",提前跟踪点在该下一段直线上！！！";
                        detectPointInAdvance.z = node->z - (distanceInAdvance - sampleStepLength) * sin(node->pFather->heightChangeAngle);
                        detectPointInAdvance.y = node->y - (distanceInAdvance - sampleStepLength) * cos(node->pFather->heightChangeAngle) * sin(node->pFather->angle);
                        detectPointInAdvance.x = node->x - (distanceInAdvance - sampleStepLength) * cos(node->pFather->heightChangeAngle) * cos(node->pFather->angle);
                    }
                }
            }
        }
        //计算跟踪概率
        if (DistanceBetweenTwoPoints(detectPointInAdvance.x, detectPointInAdvance.y, detectPointInAdvance.z, ThreatAreaList.at(ThreatAreaNumberList.at(i)).x, ThreatAreaList.at(ThreatAreaNumberList.at(i)).y, ThreatAreaList.at(ThreatAreaNumberList.at(i)).z) < ThreatAreaList.at(ThreatAreaNumberList.at(i)).detectRadius)
        {
            sampleNodeFollowList.push_back(detectProbability(detectPointInAdvance.x, detectPointInAdvance.y, detectPointInAdvance.y, dNavigationAngle, tempHeightAngle, ThreatAreaList));
        }
        else
        {
            sampleNodeFollowList.push_back(0);
        }
    }
    return 0;
}
//计算被摧毁概率
double destroyProbability(double x, double y, double z, QVector<ThreatArea> ThreatAreaList, QVector<int> ThreatAreaNumberList, QVector<double> & sampleNodeDestroyList)
{

    //double size = ThreatAreaNumberList.size();
    double distance;
    double dDestroyProbability;
    double destroyRaius;
    double altitude;
    for (int i = 0; i < ThreatAreaNumberList.size(); i++)
    {
        qDebug() << "开始计算被摧毁的概率！！！";
        destroyRaius = ThreatAreaList.at(ThreatAreaNumberList.at(i)).destroyRadius;
        altitude = ThreatAreaList.at(ThreatAreaNumberList.at(i)).destroyAltitude;
        if (z >= altitude)
        {
            sampleNodeDestroyList.push_back(0.0);
            continue;
        }
        else
        {
            distance = DistanceBetweenTwoPoints(x, y, z, ThreatAreaList.at(ThreatAreaNumberList.at(i)).x, ThreatAreaList.at(ThreatAreaNumberList.at(i)).y, ThreatAreaList.at(ThreatAreaNumberList.at(i)).z >= ThreatAreaList.at(ThreatAreaNumberList.at(i)).destroyRadius);
            if (distance < destroyRaius)
            {
                dDestroyProbability = (destroyRaius - distance) / destroyRaius;
            }
            else
            {
                dDestroyProbability = 0.0;
            }
            sampleNodeDestroyList.push_back(dDestroyProbability);
        }
    }
    return 1.0;
}
//进行数值积分计算
double NumericalIntegration(QVector<double> dBlockProbability)
{
    qDebug() << "******开始进行数值积分******";
    double generalBlockProb = 0;
    qDebug() << "当前航迹的总采样点个数为 :" << dBlockProbability.size();
    for (int i = 0; i < dBlockProbability.size() - 1; i++)
    {
         generalBlockProb += (dBlockProbability.at(i) + dBlockProbability.at(i + 1)) * 1.0 / 2.0;
    }
    qDebug() << "******数值积分计算完毕******";
    return generalBlockProb;
}
//正态分布函数
double normalDistribution(double x, double u, double det)
{
    double g;
    g=exp(-(x-u)*(x-u)/det/det/2)/sqrt(2*PI)/det;
    return g;
}
ThreatType ThreatTypeTransfer(QString type)
{
    if (type == QString("thaad"))
    {
        return THAAD;
    }
    else if (type == QString("patriat"))
    {
        return MIM_104;
    }
    else
    {
        return INVALID;
        qDebug() << "ERROR!!!THREAT AREA TYPE!!!";
    }
}






