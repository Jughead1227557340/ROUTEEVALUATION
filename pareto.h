#ifndef PARETO_H
#define PARETO_H

struct vectorOf4D
{
public:
    double length;
    double consume;
    double survive;
    double match;
};
struct Pareto
{
public:
    //角惩罚分数
    double TotalAnglePenaltyScore;
    //被支配的次数
    int DominatedTimesCount;
    //最终得分
    double score;
};

#endif // PARETO_H
