#ifndef urbandevelRankYear_H
#define urbandevelRankYear_H

#include <dmmodule.h>

class urbandevelRankYear: public DM::Module
{
    DM_DECLARE_NODE(urbandevelRankYear)
public:
    urbandevelRankYear();
    ~urbandevelRankYear();

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

#endif // urbandevelRankYear_H
