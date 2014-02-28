#ifndef urbandevelRankArea_H
#define urbandevelRankArea_H

#include <dmmodule.h>

class urbandevelRankArea: public DM::Module
{
    DM_DECLARE_NODE(urbandevelRankArea)
public:
    urbandevelRankArea();
    ~urbandevelRankArea();

    void run();
    void init();
private:
    std::string viewname;
    bool reduction;
    std::string rank_function;
    double rank_function_factor;
    double rank_weight;

    DM::View rankview;
    DM::View city;
};

#endif // urbandevelRankArea_H
