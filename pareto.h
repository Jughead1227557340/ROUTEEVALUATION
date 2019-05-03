#ifndef PARETO_H
#define PARETO_H

struct vectorOf4D
{
    double length;
    double consume;
    double survive;
    double match;
};
struct Pareto
{
    //角惩罚分数
    double TotalAnglePenaltyScore;
    //被支配的次数
    int DominatedTimesCount;
    //最后的得分
    double score;
};

#endif // PARETO_H
