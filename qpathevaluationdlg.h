#ifndef QPATHEVALUATIONDLG_H
#define QPATHEVALUATIONDLG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QVector>
#include <QXmlStreamAttributes>
#include "qpathevaluation.h"
#include <searchnode.h>
#include <QPixmap>
#include <QDomDocument>

#pragma execution_character_set("utf-8")



class QPathEvaluationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QPathEvaluationDlg(QWidget *parent = 0);
    ~QPathEvaluationDlg();

protected:

    void Read_RouteFile();
    void EvaluationRoute1by1();
    void SortRouteOverall();
    void SaveEvalResult();
    void SearchPeratoSet();
    void SortPeratoSet();
private:

    //待评价航迹评价列表
    PathEvaluationList m_pathEvaluationList;
    //帕累托最优解集
    PathEvaluationList m_ParetoOptimalSet;
    //威胁区列表
    QVector<ThreatArea> m_threatAreaList;
    //门限角
    QVector<double> m_ThresholdAngles;



    //顶部水平布局
    QHBoxLayout * m_topLayout;
    //中部显示区域布局
    QHBoxLayout * m_middleLayout;
    //底部水平布局
    QHBoxLayout * m_bottomLayout;
    //总体布局
    QVBoxLayout * m_mainLayout;
    //选择航迹文件
    QLabel * m_labChooseRouteFile;
    //航迹文件路径
    QLineEdit * m_edtRouteFileDirectory;
    //选择航迹文件按钮
    QPushButton * m_BtnBrowser;
    //逐个评价选定航迹
    QPushButton * m_BtnEvalPath1by1;
    //对航迹进行综合排序
    QPushButton * m_BtnSortPathOverall;
    //保存航迹评价结果
    QPushButton * m_BtnSaveEvalResult;
    //确定
    QPushButton * m_BtnOK;
    //取消
    QPushButton * m_BtnCancel;
    //航迹数据显示表格
    QTableView * m_tableDisplay;
    QStandardItemModel * m_dataModel;


};

#endif // QPATHEVALUATIONDLG_H
void BubbleSort(QVector<double> & list);
