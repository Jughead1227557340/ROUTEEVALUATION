#include "qpathevaluationdlg.h"
#include "ui_qpathevaluationdlg.h"
#include <QFileDialog>
#include <qDebug>
#include <iostream>
#include <QList>
#include <QAbstractItemView>
#include <vector>
QPathEvaluationDlg::QPathEvaluationDlg(QWidget *parent) :
    QDialog(parent)
{
    //ui->setupUi(this);

    this->setWindowTitle("航迹评价与选优");

    m_topLayout = new QHBoxLayout;

    m_labChooseRouteFile = new QLabel(this);
    m_labChooseRouteFile->setText("选择航迹文件:");


    m_edtRouteFileDirectory = new QLineEdit(this);
    m_labChooseRouteFile->setBuddy(m_edtRouteFileDirectory);

    m_BtnBrowser = new QPushButton(this);
    m_BtnBrowser->setText("浏览...");
    connect(m_BtnBrowser, &QPushButton::pressed, this, &QPathEvaluationDlg::Read_RouteFile);

    m_topLayout->addWidget(m_labChooseRouteFile);
    m_topLayout->addWidget(m_edtRouteFileDirectory);
    m_topLayout->addWidget(m_BtnBrowser);

    m_BtnEvalPath1by1 = new QPushButton(this);
    m_BtnEvalPath1by1->setText("逐个评价航迹");
    connect(m_BtnEvalPath1by1, &QPushButton::pressed, this, &QPathEvaluationDlg::EvaluationRoute1by1);

    m_BtnSortPathOverall = new QPushButton(this);
    m_BtnSortPathOverall->setText(tr("航迹综合排序"));
    connect(m_BtnSortPathOverall, &QPushButton::pressed, this, &QPathEvaluationDlg::SortRouteOverall);

    m_BtnSaveEvalResult = new QPushButton(this);
    m_BtnSaveEvalResult->setText("保存评价结果");
    connect(m_BtnSaveEvalResult, &QPushButton::pressed, this, &QPathEvaluationDlg::SaveEvalResult);

    m_BtnOK = new QPushButton(this);
    m_BtnOK->setText("确定");
    connect(m_BtnOK, &QPushButton::pressed, this, &QDialog::close);

    m_BtnCancel = new QPushButton(this);
    m_BtnCancel->setText("取消");
    connect(m_BtnCancel, &QPushButton::pressed, this, &QDialog::close);


    m_middleLayout = new QHBoxLayout;
    //TableView数据显示
    m_tableDisplay = new QTableView(this);
    m_tableDisplay->setShowGrid(true);
    m_tableDisplay->setGridStyle(Qt::SolidLine);
    m_tableDisplay->horizontalHeader()->setStretchLastSection(true);
    m_tableDisplay->verticalHeader()->setStretchLastSection(true);
    m_tableDisplay->setSortingEnabled(true);//enable the sorting function, The default value is false.
    m_middleLayout->addWidget(m_tableDisplay);
    m_tableDisplay->horizontalHeader()->setStretchLastSection(true);
    m_tableDisplay->verticalHeader()->setStretchLastSection(true);
    m_tableDisplay->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_dataModel = new QStandardItemModel;
    m_tableDisplay->setModel(m_dataModel);
    QStringList m_tableColumn;
    m_tableColumn << "语言综合评价" << "数值综合评价" << "航程(km)" << "机动消耗" << "突防概率" << "匹配概率";
    m_dataModel->setHorizontalHeaderLabels(m_tableColumn);

    m_bottomLayout = new QHBoxLayout;
    m_bottomLayout->addWidget(m_BtnEvalPath1by1);
    m_bottomLayout->addWidget(m_BtnSortPathOverall);
    m_bottomLayout->addWidget(m_BtnSaveEvalResult);
    m_bottomLayout->addStretch();
    m_bottomLayout->addWidget(m_BtnOK);
    m_bottomLayout->addWidget(m_BtnCancel);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addLayout(m_topLayout);
    m_mainLayout->addLayout(m_middleLayout);
    m_mainLayout->addLayout(m_bottomLayout);
    this->setLayout(m_mainLayout);


}

QPathEvaluationDlg::~QPathEvaluationDlg()
{

}


void QPathEvaluationDlg::Read_RouteFile()
{
    //read and display the route directories
    QStringList RouteFileDirectories = QFileDialog::getOpenFileNames(this, tr("打开航迹文件"), "/home", tr("XML文档(*.xml)"));
    m_edtRouteFileDirectory->setText(RouteFileDirectories.join(";"));

    //temporary variables used when reading the route threatAreaFile
    QPathEvaluation* pTempPathEvaluation = new QPathEvaluation;
    m_pathEvaluationList.clear(); //清空
    int nRouteCount; // 航迹条数
    nRouteCount = RouteFileDirectories.size();
    qDebug() << "************开始读取航迹文件******************";
    for (int i = 0; i < nRouteCount; ++i)
    {
        //打开航迹文件

        QDomDocument XMLDocument;
        QFile RouteFile(RouteFileDirectories.at(i));
        //-----------------------------保护-----------------------//
        if (!RouteFile.open(QIODevice::ReadOnly | QFile::Text))
        {
             RouteFile.close();
             qDebug()<<"RouteFile.open(QFile::ReadOnly | QFile::TCxt)error!\n";
        }
        else if (!XMLDocument.setContent(&RouteFile))
        {
             qDebug() << "XMLDocument.setContent() error!\n";
             RouteFile.close();
        }
        RouteFile.close();
        //-----------------------------保护-----------------------//
        //----------------------------打开航迹文件--------------------------//

        QDomElement docElement = XMLDocument.documentElement(); //读取根节点<Route>
        if (docElement.isNull())
        {
            return ;
        }
        //获取航迹中导航点的个数
        QDomNodeList RouteNodes = docElement.firstChild().toElement().childNodes();
        int NodeNumber = RouteNodes.at(0).toElement().text().toInt();
        if (NodeNumber != RouteNodes.size() - 1)
        {
            return;
        }
        else
        {
            for (int j = 1; j < RouteNodes.size(); j++)
            {
                 SearchNode * pTempNavigationNode = new SearchNode;
                 QDomNodeList RouteNodesList = RouteNodes.at(j).toElement().childNodes(); //添加子节点到列表
                 //读取航迹点的坐标x
                 pTempNavigationNode->x = RouteNodesList.at(1).toElement().text().toDouble();
                 //读取航迹点的坐标y
                 pTempNavigationNode->y = RouteNodesList.at(2).toElement().text().toDouble();
                 //读取航迹点的海拔高度z
                 pTempNavigationNode->z = RouteNodesList.at(3).toElement().text().toDouble();
                 //读取导航点的速度speed
                 pTempNavigationNode->speed = RouteNodesList.at(4).toElement().text().toDouble();
                 //读取该点的航迹长度
                 pTempNavigationNode->routelen = RouteNodesList.at(5).toElement().text().toDouble();
                 //读取该航迹点的航向角
                 pTempNavigationNode->angle = RouteNodesList.at(6).toElement().text().toDouble();
                 //读取航迹点的类型
                 pTempNavigationNode->NodeKind = TypeTranslate(RouteNodesList.at(7).toElement().text());
                 if (pTempNavigationNode->NodeKind == TurnPoint)
                 {
                     //读取转弯点的转弯角度
                     pTempNavigationNode->turnAngle = RouteNodesList.at(8).toElement().text().toDouble();
                     //读取转弯点的转弯半径
                     pTempNavigationNode->r = RouteNodesList.at(9).toElement().text().toDouble();
                 }
                 else if (pTempNavigationNode->NodeKind == HeightChange)
                 {
                    //读取高度变化点的高度变化角
                     pTempNavigationNode->heightChangeAngle = RouteNodesList.at(8).toElement().text().toDouble();
                 }
                 else if (pTempNavigationNode->NodeKind == Match)
                 {
                     pTempNavigationNode->matchLevel = RouteNodesList.at(8).toElement().text().toInt();
                     pTempNavigationNode->ErrLevel = RouteNodesList.at(9).toElement().text().toInt();
                     //读取匹配点的匹配方向
                 }
                 else if (pTempNavigationNode->NodeKind == AltituteAment)
                 {
                     pTempNavigationNode->matchLevel = RouteNodesList.at(8).toElement().text().toInt();
                 }
                 else
                 {

                 }
                 pTempPathEvaluation->m_routeLine.push_back(pTempNavigationNode);
            }
        }
        m_pathEvaluationList.push_back(pTempPathEvaluation);
    }
    qDebug() << "************航迹文件读取完毕******************";
}


//对每条航迹进行单项指标的计算，并显示
void QPathEvaluationDlg::EvaluationRoute1by1()
{
    //判断是否已经进行单项航迹指标计算
    int flag = 0;
    for (int i = 0; i < m_pathEvaluationList.size(); i++)
    {
        for (int j = 2; j < 6; j++)
        {
            if (m_dataModel->item(i, j) != nullptr)
            {
                flag = 1;
                break;
            }
        }
        break;
    }
    if (flag == 0)
    {

        //...........................................读取威胁区xml文件........................................//
        QString ThreatAreaDirectory = QFileDialog::getOpenFileName(this, tr("加载威胁区"), "/home", tr("XML文档(*.xml)"));
        ThreatArea tempThreatArea;
        QDomDocument XMLDocument;
        QFile threatAreaFile(ThreatAreaDirectory);
        if (!(threatAreaFile.open(QIODevice::ReadOnly | QIODevice::Text)))
        {
            threatAreaFile.close();
            qDebug() << "ERROR!!!(threatAreaFile.open(QIODevice::ReadOnly | QIODevice::Text)";
        }
        else if (!XMLDocument.setContent(&threatAreaFile))
        {
            threatAreaFile.close();
            qDebug() << "ERROR!!!XMLDocument.setContent()";
        }
        //threatAreaFile.close();

        qDebug() << "***********开始读取威胁区数据***********";
        QDomElement docElem = XMLDocument.documentElement(); //读取根节点ALL
        if (docElem.isNull())
        {
            return;
        }
        //................................开始读取威胁区...............................//
        QDomNode DomThreatArea = docElem.firstChild(); //获得doc的第一个节点THreatArea
        QDomNodeList ThreatAreaList = DomThreatArea.toElement().childNodes(); //添加子节点列表,威胁区的个数+1
        //1.威胁区的个数
        int ThreatNumber = ThreatAreaList.at(0).toElement().text().toInt();
        qDebug() << "威胁区的个数为：" << ThreatNumber;
        //2.循环威胁区信息
        for (int i = 0; i < ThreatNumber; i++)
        {
            QDomNode subThreatArea = ThreatAreaList.at(i + 1);
            QDomNodeList subThreatAreaList = subThreatArea.toElement().childNodes(); //添加5个子节点列表
            //读取第i+1个威胁区的类型
            QString threattype1 = subThreatAreaList.at(0).toElement().text();
            qDebug() << "威胁区类型为：" << threattype1;
            tempThreatArea.threatType = ThreatTypeTransfer(threattype1);
            //qDebug() << "威胁区类型为：" << tempThreatArea.threatType;
            //读取第i+1个威胁区的xy坐标
            QString coordinate = subThreatAreaList.at(1).toElement().text();
            QStringList coordinateList = coordinate.split(" ");
            tempThreatArea.x = coordinateList.at(0).toDouble();
            //qDebug() << "威胁区坐标x为：" << tempThreatArea.x;
            tempThreatArea.y = coordinateList.at(1).toDouble();
            //qDebug() << "威胁区坐标y为：" << tempThreatArea.y;
            //读取第i+1个威胁区的z坐标
            tempThreatArea.z = subThreatAreaList.at(2).toElement().text().toDouble();
            //qDebug() << "威胁区坐标z为：" << tempThreatArea.z;

            //读取第i+1个威胁区的雷达参数
            QDomNode radarParameter = subThreatAreaList.at(3);
            QDomNodeList radarParameterList = radarParameter.toElement().childNodes();
            tempThreatArea.detectRadius = radarParameterList.at(0).toElement().text().toDouble();
            tempThreatArea.c1 = radarParameterList.at(1).toElement().text().toDouble();
            tempThreatArea.c2 = radarParameterList.at(2).toElement().text().toDouble();
            /***
            qDebug() << "威胁区雷达参数detectRaius为：" << tempThreatArea.detectRadius;
            qDebug() << "威胁区雷达参数C1为：" << tempThreatArea.c1;
            qDebug() << "威胁区雷达参数C2为：" << tempThreatArea.c2;
            ***/
            //读取第i+1个威胁区拦截弹的参数
            QDomNode weaponParameter = subThreatAreaList.at(4);
            QDomNodeList weaponParameterList = weaponParameter.toElement().childNodes();
            tempThreatArea.tDelay = weaponParameterList.at(0).toElement().text().toDouble();
            tempThreatArea.speed = weaponParameterList.at(1).toElement().text().toDouble();
            tempThreatArea.destroyRadius = weaponParameterList.at(2).toElement().text().toDouble();
            tempThreatArea.destroyAltitude = weaponParameterList.at(3).toElement().text().toDouble();
            /***
            qDebug() << "威胁区拦截弹参数tdelay为：" << tempThreatArea.tDelay;
            qDebug() << "威胁区拦截弹参数speed为：" << tempThreatArea.speed;
            qDebug() << "威胁区拦截弹参数destroyRadius为：" << tempThreatArea.destroyRadius;
            qDebug() << "威胁区拦截弹参数destroyAltitude为：" << tempThreatArea.destroyAltitude;
            ***/
            m_threatAreaList.push_back(tempThreatArea);
        }
        qDebug() << "***********威胁区列表读取完毕*************";
        //......................................威胁区读取完毕.....................................//
        qDebug() << "*********开始对航迹指标进行逐个评价****************";

        for (int row = 0; row < m_pathEvaluationList.size(); row++)
        {
            if (m_pathEvaluationList.at(row)->m_routeLine.isEmpty())
            {
                qDebug() << "ERROR!!!" << row + 1 << "th RouteLine is empty!!!";
                break;
            }
            else
            {
                qDebug() << "转弯起始点和转完结束点设置开始！！！";
                int flag = m_pathEvaluationList.at(row)->SetTurnPoint();
                qDebug() << "转弯起始点和转完结束点设置结束！！！";
                if (flag)
                {
                    for (int column = 2; column < 6; column++)
                    {
                        switch (column)
                        {
                            case 2 :
                            {
                                qDebug() << "******开始计算航迹长度******";
                                m_pathEvaluationList.at(row)->m_routeLength = m_pathEvaluationList.at(row)->CalcRouteLength();
                                qDebug() << "******航迹长度计算完毕******";
                                m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_pathEvaluationList.at(row)->m_routeLength)));
                                break;
                            }
                            case 3 :
                            {
                                qDebug() << "******开始计算机动消耗******";
                                m_pathEvaluationList.at(row)->m_MobileConsumption = m_pathEvaluationList.at(row)->CalcMobileConsumption();
                                qDebug() << "******机动消耗计算完毕******";
                                m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_pathEvaluationList.at(row)->m_MobileConsumption)));
                                break;
                            }
                             case 4 :
                            {
                                qDebug() << "******开始计算突防概率******";
                                double survive = m_pathEvaluationList.at(row)->CalcSurvivability(m_threatAreaList);
                                qDebug() << "******突防概率计算完毕******";
                                m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(survive)));
                                break;
                            }
                            case 5 :
                            {
                                qDebug() << "******开始计算匹配等级******";
                                double match = m_pathEvaluationList.at(row)->CalcMatchingupLevel();
                                qDebug() << "******匹配等级计算完毕******";
                                m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(match)));
                                break;
                            }
                            default :
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    break;
                    qDebug() << "添加转弯起始点以及转弯结束点事变！！！";
                }
            }
        }
        qDebug() << "***********航迹指标逐个评价进行完毕**************";
        m_tableDisplay->show();
    }
    else
    {
        qDebug() << "已经航迹评价完毕！！！";
        return;
    }

}
//航迹评价结果综合排序
void QPathEvaluationDlg::SortRouteOverall()
{
   int flag = 0;
   for (int i = 0; i < m_ParetoOptimalSet.size(); ++i)
   {
       if (m_dataModel->item(i, 1) != nullptr)
       {
           flag = 1;
           qDebug() << "已经综合评价完毕~~~";
           break;
       }
       for (int j = 2; j < 6; j++)
       {
           if (m_dataModel->item(i, j) == nullptr)
           {
               flag = 1;
               qDebug() << "单项航迹评价还未完成";
               break;
           }
       }
       if (flag == 0)
       {
           //确定帕累托最优解集
           qDebug() << "*********开始寻找帕累托最优解集*********";
           SearchPeratoSet();
           qDebug() << "*********帕累托最优解集确定完毕*********";
           qDebug() << "******开始对帕累托最优解集进行排序******";
           SortPeratoSet();
           qDebug() << "******对帕累托最优解集进行排序完毕******";
       }
       else
       {
           qDebug() << "ERROR!!!Route general Evaluation!!!Single Item is short!!!";
           continue;
       }
   }
}
//保存航迹评价结果
void QPathEvaluationDlg::SaveEvalResult()
{
    int flag = 0;
    for (int i = 0; i < m_pathEvaluationList.size(); i++)
    {
        if (m_dataModel->item(i, 1) == nullptr)
        {
            flag = 1;
            break;
        }
    }
    if (flag == 0)
    {

    }
    else
    {
        qDebug() << "ERROR!!!Save Route Evaluation Result!!!";
    }
}

//确定帕累托最优解集
void QPathEvaluationDlg::SearchPeratoSet()
{
    if (0 == m_pathEvaluationList.size())
    {
        qDebug() << "The number of the routes in the waiting-to-be-evaluated list is empty ";
        return ;
    }
    //clean out the vector of Pareto Set
    m_ParetoOptimalSet.clear();
    //traverse each and every route in the waiting-to-be-evaluated  list and delete the routes that are not on the Pareto front 
    for (int i = 0; i < m_pathEvaluationList.size(); i++)
    {
        if (0 == i)
        {
            //第一条航迹不可能被支配，故直接加入帕累托最优解集
            m_ParetoOptimalSet.push_back(m_pathEvaluationList.at(i));
        }
        else
        {
            int flag = 1; //标志
            //如果节点i为被支配的点，则跳过,不加入到帕累托解集中
            for (int j = 0; j < i; j++)
            {
                if ((m_pathEvaluationList.at(i)->m_routeLength > m_pathEvaluationList.at(j)->m_routeLength)
                        && (m_pathEvaluationList.at(i)->m_MobileConsumption > m_pathEvaluationList.at(j)->m_MobileConsumption)
                            && (m_pathEvaluationList.at(i)->m_survivability < m_pathEvaluationList.at(j)->m_survivability)
                                && (m_pathEvaluationList.at(i)->m_matchingupLevel < m_pathEvaluationList.at(j)->m_matchingupLevel))
                {
                    //只要找到下标i的一个支配解就退出
                    flag = 0;
                    //退出循环
                    break;
                }                   
            }
            if (flag == 0)
            {
                qDebug() << "该点为被支配点";
                continue; //跳过该点，不加到帕累托解集列表当中
            }
            else
            {
                //删除被节点i所支配的所有解
                for (int j = 0; j < i; j++)
                {
                    
                    if ((m_pathEvaluationList.at(i)->m_routeLength < m_pathEvaluationList.at(j)->m_routeLength)
                            && (m_pathEvaluationList.at(i)->m_MobileConsumption < m_pathEvaluationList.at(j)->m_MobileConsumption)
                                && (m_pathEvaluationList.at(i)->m_survivability > m_pathEvaluationList.at(j)->m_survivability)
                                    && (m_pathEvaluationList.at(i)->m_matchingupLevel > m_pathEvaluationList.at(j)->m_matchingupLevel))
                    {
                        m_ParetoOptimalSet.remove(j);
                    }
                }
                //将当前航迹i加入到帕累托解集中
                m_ParetoOptimalSet.push_back(m_pathEvaluationList.at(i));
            }
        }
    }
}
//对帕累托解集进行排序
void QPathEvaluationDlg::SortPeratoSet()
{
    //先找出每个属性的最大值和最小值
    double MaxRouteLength;
    double MinRouteLength;
    double MaxMobileConsume = 0.0;
    double MinMobileConsume = 0.0;
    double MaxSurviveProb = 0.0;
    double minSurviveProb = 0.0;
    double MaxMatchLevel = 0.0;
    double MinMatchLevel = 0.0;
    //排序后的各属性
    QVector<double> RouteLength;
    QVector<double> MobileConsume;
    QVector<double> SurviveProb;
    QVector<double> MatchLevel;
    //各属性的门限角
    double thresholdAngleOfLength;
    double thresholdAngleOfConsume;
    double thresholdAngleOfSurvive;
    double thresholdAngleOfMatch;
    //Total Angle Penalty Score of each route from 1 to N and its Dominated Times Count
    QVector<double> TAPS;
    QVector<double> DTC;
    qDebug() << "***步骤1，找到各个属性的最大最小值***";
    //step_1,对每条航迹的各个属性进行标准化处理
    //求航迹长度的最大最小值
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            MaxRouteLength = m_ParetoOptimalSet.at(i)->m_routeLength;
            MinRouteLength = m_ParetoOptimalSet.at(i)->m_routeLength;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_routeLength > MaxRouteLength)
            {
                MaxRouteLength = m_ParetoOptimalSet.at(i)->m_routeLength;
            }
            else if (m_ParetoOptimalSet.at(i)->m_routeLength < MinRouteLength)
            {
                MinRouteLength = m_ParetoOptimalSet.at(i)->m_routeLength;
            }
            else
            {
                
            }
        }
    }
    //求机动消耗的最大最小值
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            MaxMobileConsume = m_ParetoOptimalSet.at(i)->m_MobileConsumption;
            MinMobileConsume = m_ParetoOptimalSet.at(i)->m_MobileConsumption;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_MobileConsumption < MinMobileConsume)
            {
                MinMobileConsume = m_ParetoOptimalSet.at(i)->m_MobileConsumption;
            }
            else if (m_ParetoOptimalSet.at(i)->m_MobileConsumption > MaxMobileConsume)
            {
                MaxMobileConsume = m_ParetoOptimalSet.at(i)->m_MobileConsumption;
            }
            else
            {
                
            }
        }
    }
    //求突防概率的最大最小值
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            MaxSurviveProb = m_ParetoOptimalSet.at(i)->m_survivability;
            minSurviveProb = m_ParetoOptimalSet.at(i)->m_survivability;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_survivability < minSurviveProb)
            {
                minSurviveProb = m_ParetoOptimalSet.at(i)->m_survivability;
            }
            else if (m_ParetoOptimalSet.at(i)->m_survivability > MaxSurviveProb)
            {
                MaxSurviveProb = m_ParetoOptimalSet.at(i)->m_survivability;
            }
            else
            {
                
            }
        }
    }
    //求匹配概率的最大最小值
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            MinMatchLevel = m_ParetoOptimalSet.at(i)->m_matchingupLevel;
            MaxMatchLevel = m_ParetoOptimalSet.at(i)->m_matchingupLevel;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_matchingupLevel > MaxMatchLevel)
            {
                MaxMatchLevel = m_ParetoOptimalSet.at(i)->m_matchingupLevel;
            }
            else if (m_ParetoOptimalSet.at(i)->m_matchingupLevel < MinMatchLevel)
            {
                MinMatchLevel = m_ParetoOptimalSet.at(i)->m_matchingupLevel;
            }
            else
            {
                
            }
        }
    }
    qDebug() << "***步骤2，将每条航迹的每个属性标准化***";
    //将各属性标准化,并添加到列表中
    //对每个属性而言，值越大越差
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        m_ParetoOptimalSet.at(i)->m_routeLength = (m_ParetoOptimalSet.at(i)->m_routeLength - MinRouteLength) / (MaxRouteLength - MinRouteLength);
        //将每个属性分开存储到一个新的列表里
        RouteLength.push_back(m_ParetoOptimalSet.at(i)->m_routeLength);
        m_ParetoOptimalSet.at(i)->m_MobileConsumption = (m_ParetoOptimalSet.at(i)->m_MobileConsumption - MinMobileConsume) / (MaxMobileConsume - MinMobileConsume);
        MobileConsume.push_back(m_ParetoOptimalSet.at(i)->m_MobileConsumption);
        m_ParetoOptimalSet.at(i)->m_survivability = (MaxSurviveProb - m_ParetoOptimalSet.at(i)->m_survivability) / (MaxSurviveProb - minSurviveProb);
        SurviveProb.push_back(m_ParetoOptimalSet.at(i)->m_survivability);
        m_ParetoOptimalSet.at(i)->m_matchingupLevel = (MaxMatchLevel - m_ParetoOptimalSet.at(i)->m_matchingupLevel) / (MaxMatchLevel - MinMatchLevel);
        MatchLevel.push_back(m_ParetoOptimalSet.at(i)->m_matchingupLevel);
    }
    qDebug() << "***步骤3，求出每个属性的门限角***";
    //求各属性的门限角
    //先将各属性列表排序，求出interquartile以及IQm，求航迹长度的门限角
    //排序
    BubbleSort(RouteLength);
    //求出interQuartile
    double quartileValueOfLength = RouteLength.at((RouteLength.size() + 1) / 4);
    //求出平均距离
    double AverageDistanceOfLength = 0.0;
    for (int i = 0; i < RouteLength.size() - 1; i++)
    {
        if (RouteLength.size() == 1)
        {

        }
        else
        {
            AverageDistanceOfLength += RouteLength.at(i + 1) - RouteLength.at(i);
        }
    }
    AverageDistanceOfLength = AverageDistanceOfLength / (RouteLength.size() - 1);
    thresholdAngleOfLength = atan2(AverageDistanceOfLength, quartileValueOfLength);
    //求机动消耗的门限角
    //排序
    BubbleSort(MobileConsume);
    //求出interQuartilePoint
    double quartileValueOfConsume = MobileConsume.at((MobileConsume.size() + 1) / 4);
    //求平均距离
    double AverageDistanceOfConsume = 0.0;
    for (int i = 0; i < MobileConsume.size() - 1; i++)
    {
        if (MobileConsume.size() == 1)
        {

        }
        else
        {
            AverageDistanceOfConsume += MobileConsume.at(i + 1) - MobileConsume.at(i);
        }
    }
    AverageDistanceOfConsume = AverageDistanceOfConsume / (MobileConsume.size() - 1);
    thresholdAngleOfConsume = atan2(AverageDistanceOfConsume, quartileValueOfConsume);
    //求突防概率的门限角
    //排序
    BubbleSort(SurviveProb);
    //求出interQuartilePoint
    double quartileValueOfSurvive = SurviveProb.at((SurviveProb.size() + 1) / 4);
    //求平均距离
    double AverageDistanceOfSurvive = 0.0;
    for (int i = 0; i < SurviveProb.size() - 1; i++)
    {
        if (SurviveProb.size() == 1)
        {

        }
        else
        {
            AverageDistanceOfSurvive += SurviveProb.at(i + 1) - SurviveProb.at(i);
        }
    }
    AverageDistanceOfSurvive = AverageDistanceOfSurvive / (SurviveProb.size() - 1);
    thresholdAngleOfSurvive = atan2(AverageDistanceOfSurvive, quartileValueOfSurvive);
    //Calculate the angle-threshold value of matchingProbability
    //step_1, perform sorting
    BubbleSort(MatchLevel);
    //step_2, Find out the value of interQuartilePoint marked as IQS
    double quartileValueOfMatch = MatchLevel.at((MatchLevel.size() + 1) / 4);
    //step_3，calculate the average distance between two consecutive solutions
    double AverageDistanceOfMatch = 0.0;
    for (int i = 0; i < MatchLevel.size() - 1; i++)
    {
        if (MatchLevel.size() == 1)
        {

        }
        else
        {
            AverageDistanceOfMatch = MatchLevel.at(i + 1) - MatchLevel.at(i);
        }
    }
    AverageDistanceOfMatch = AverageDistanceOfMatch / (MatchLevel.size() - 1);
    //step_4, calculate the angle-threshold value based on the formula
    thresholdAngleOfMatch = atan2(AverageDistanceOfMatch, quartileValueOfMatch);
    //Calculate the dominated-or-not relationship between every and each two solutions
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        for (int j = 0; j < m_ParetoOptimalSet.size(); j++)
        {
            if (i == j)
            {
                continue;
            }
            else
            {
                //step_1, calculate the direction vector
                vectorOf4D directionVector;
                directionVector.length = m_ParetoOptimalSet.at(i)->m_routeLength - m_ParetoOptimalSet.at(j)->m_routeLength;
                directionVector.consume = m_ParetoOptimalSet.at(i)->m_MobileConsumption - m_ParetoOptimalSet.at(j)->m_MobileConsumption;
                directionVector.survive = m_ParetoOptimalSet.at(i)->m_survivability - m_ParetoOptimalSet.at(j)->m_survivability;
                directionVector.match = m_ParetoOptimalSet.at(i)->m_matchingupLevel = m_ParetoOptimalSet.at(j)->m_matchingupLevel;
                //when a certain attribute of solution i is smaller than that of solution j, it is possible that solution i is dominated by solution j
                //The first kind of situation about RouteLength
                if (m_ParetoOptimalSet.at(i)->m_routeLength < m_ParetoOptimalSet.at(j)->m_routeLength)
                {
                    //calculate the angle between two solutions for attribute of length
                    //the 3D plane equation is length = m_ParetoOptimalSet.at(j)->m_routeLength
                    //step_2, calculate the included angle based on the formula
                    double tempVariable = directionVector.length * directionVector.length + directionVector.consume * directionVector.consume + directionVector.survive * directionVector.survive + directionVector.match * directionVector.match;
                    double AngleOfLength  = acos(directionVector.length * 1.0 / sqrt(tempVariable));

                    if (AngleOfLength < thresholdAngleOfLength)
                    {
                        //first of all, DominatedTimesCount auto-add one
                        m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount++;
                        //calculate APD (Angle Penalty Degree) and PIR (Penalty Intensity Rate), respectively
                        double APDOfLength = thresholdAngleOfLength - AngleOfLength;
                        double temp = (RouteLength.at(i) - RouteLength.at(j)) / (RouteLength.at(0) - RouteLength.at(RouteLength.size() - 1));
                        double PIROfLength = 1.0 / (temp * temp);
                        m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore += APDOfLength * PIROfLength;
                    }
                }
                if (m_ParetoOptimalSet.at(i)->m_MobileConsumption < m_ParetoOptimalSet.at(j)->m_MobileConsumption)
                {
                    //calculate the angle between two solutions for attribute of consumption
                    //step_2, calculate the included angle based on the formula
                    double tempVariable = directionVector.length * directionVector.length + directionVector.consume * directionVector.consume + directionVector.survive * directionVector.survive + directionVector.match * directionVector.match;
                    double AngleOfConsume  = acos(directionVector.consume * 1.0 / sqrt(tempVariable));

                    if (AngleOfConsume < thresholdAngleOfConsume)
                    {
                        //first of all, DominatedTimesCount auto-add one
                        m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount++;
                        //calculate APD (Angle Penalty Degree) and PIR (Penalty Intensity Rate), respectively
                        double APDOfConsume = thresholdAngleOfConsume - AngleOfConsume;
                        double temp = (MobileConsume.at(i) - MobileConsume.at(j)) / (MobileConsume.at(0) - MobileConsume.at(RouteLength.size() - 1));
                        double PIROfConsume = 1.0 / (temp * temp);
                        m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore += APDOfConsume * PIROfConsume;
                    }
                }
                if (m_ParetoOptimalSet.at(i)->m_survivability < m_ParetoOptimalSet.at(j)->m_survivability)
                {
                    //calculate the angle between two solutions for objective of survivability
                    //step_2, calculate the included angle based on the formula
                    double tempVariable = directionVector.length * directionVector.length + directionVector.consume * directionVector.consume + directionVector.survive * directionVector.survive + directionVector.match * directionVector.match;
                    double AngleOfSurvive  = acos(directionVector.survive * 1.0 / sqrt(tempVariable));

                    if (AngleOfSurvive < thresholdAngleOfSurvive)
                    {
                        //first of all, DominatedTimesCount auto-add one
                        m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount++;
                        //calculate APD (Angle Penalty Degree) and PIR (Penalty Intensity Rate), respectively
                        double APDOfSurvive = thresholdAngleOfSurvive - AngleOfSurvive;
                        double temp = (SurviveProb.at(i) - SurviveProb.at(j)) / (SurviveProb.at(0) - SurviveProb.at(RouteLength.size() - 1));
                        double PIROfSurvive = 1.0 / (temp * temp);
                        m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore += APDOfSurvive * PIROfSurvive;
                    }
                }
                if (m_ParetoOptimalSet.at(i)->m_matchingupLevel < m_ParetoOptimalSet.at(j)->m_matchingupLevel)
                {
                    //calculate the angle between two solutions for objective of Matching
                    //step_2, calculate the included angle based on the formula
                    double tempVariable = directionVector.length * directionVector.length + directionVector.consume * directionVector.consume + directionVector.survive * directionVector.survive + directionVector.match * directionVector.match;
                    double AngleOfMatch  = acos(directionVector.match * 1.0 / sqrt(tempVariable));

                    if (AngleOfMatch < thresholdAngleOfMatch)
                    {
                        //first of all, DominatedTimesCount auto-add one
                        m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount++;
                        //calculate APD (Angle Penalty Degree) and PIR (Penalty Intensity Rate), respectively
                        double APDOfMatch = thresholdAngleOfMatch - AngleOfMatch;
                        double temp = (MatchLevel.at(i) - MatchLevel.at(j)) / (MatchLevel.at(0) - MatchLevel.at(RouteLength.size() - 1));
                        double PIROfMatch = 1.0 / (temp * temp);
                        m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore += APDOfMatch * PIROfMatch;
                    }
                }
            }
        }
    }
    //接下来需要对每个解的DominatedTimesCount和TotalAnglePenaltyScore进行标准化
    int maxDominatedTimesCount;
    int minDominatedTimesCount;
    double maxTotalAnglePenaltyScore;
    double minTotalAnglePenaltyScore;
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            maxDominatedTimesCount = m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount > maxDominatedTimesCount)
            {
                maxDominatedTimesCount = m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount;
            }
        }
    }

    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            minTotalAnglePenaltyScore = m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount < minDominatedTimesCount)
            {
                minDominatedTimesCount = m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount;
            }
        }
    }

    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            minTotalAnglePenaltyScore = m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore < minTotalAnglePenaltyScore)
            {
                minTotalAnglePenaltyScore = m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore;
            }
        }
    }

    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        if (i == 0)
        {
            maxTotalAnglePenaltyScore = m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore;
        }
        else
        {
            if (m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore > maxTotalAnglePenaltyScore)
            {
                maxTotalAnglePenaltyScore = m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore;
            }
        }
    }
    //接下来对每个节的属性标准化,并计算最终的分数
    for (int i = 0; i < m_ParetoOptimalSet.size(); i++)
    {
        m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount = (m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount - minDominatedTimesCount) / (maxDominatedTimesCount - minDominatedTimesCount);
        m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore = (m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore - minTotalAnglePenaltyScore) / (maxTotalAnglePenaltyScore - minTotalAnglePenaltyScore);
        m_ParetoOptimalSet.at(i)->m_pareto.score = m_ParetoOptimalSet.at(i)->m_pareto.DominatedTimesCount * m_ParetoOptimalSet.at(i)->m_pareto.TotalAnglePenaltyScore;
    }
    //根据计算得到的最终分数，对所有的解进行排序
    if (m_ParetoOptimalSet.size() >= 2)
    {
        for (int i = 0; i < m_ParetoOptimalSet.size() -  1; i++)
        {
            for (int j = i + 1; j < m_ParetoOptimalSet.size(); j++)
            {
                if (m_ParetoOptimalSet.at(j)->m_pareto.score < m_ParetoOptimalSet.at(i)->m_pareto.score)
                {
                    QPathEvaluation temp = *m_ParetoOptimalSet.at(i);
                    *m_ParetoOptimalSet.at(i) = *m_ParetoOptimalSet.at(j);
                    *m_ParetoOptimalSet.at(j) = temp;
                }
            }
        }
    }
    //对排好序的帕累托最优解集进行排序
    for (int row = 0; row < m_ParetoOptimalSet.size(); row++)
    {
        if (m_ParetoOptimalSet.at(row)->m_routeLine.isEmpty())
        {
            qDebug() << "ERROR!!!" << row + 1 << "th RouteLine is empty!!!";
            break;
        }
        else
        {
                for (int column = 1; column < 6; column++)
                {
                    switch (column)
                    {
                        case 1 :
                        {
                            m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_ParetoOptimalSet.at(row)->m_pareto.score)));
                            break;
                        }
                        case 2 :
                        {
                            m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_ParetoOptimalSet.at(row)->m_routeLength)));
                            break;
                        }
                        case 3 :
                        {
                            m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_ParetoOptimalSet.at(row)->m_MobileConsumption)));
                            break;
                        }
                         case 4 :
                        {
                            m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_ParetoOptimalSet.at(row)->m_survivability)));
                            break;
                        }
                        case 5 :
                        {
                            m_dataModel->setItem(row, column, new QStandardItem(QString("%1").arg(m_ParetoOptimalSet.at(row)->m_matchingupLevel)));
                            break;
                        }
                        default :
                        {
                            break;
                        }
                   }
              }
         }
    }
    m_tableDisplay->show();
}
void BubbleSort(QVector<double> & list)
{
    int length = list.size();
    for (int i = 0; i < length - 1; i++)
    {
        //每一轮找到最小的那个值
        for (int j = i + 1; j < length; j++)
        {
            if (list.at(j) < list.at(i))
            {
                int temp = list.at(i);
                list[i] = list.at(j);
                list[j] = temp;
            }
        }
    }
}

