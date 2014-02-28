#ifndef urbandevelRankEuclid_H
#define urbandevelRankEuclid_H

#include <dmmodule.h>

class urbandevelRankEuclid: public DM::Module
{
    DM_DECLARE_NODE(urbandevelRankEuclid)
public:
    urbandevelRankEuclid();
    ~urbandevelRankEuclid();

    void run();
    void init();
private:
    std::string viewname;
    bool reduction;
    std::string rank_function;
    double rank_function_factor;
    double rank_weight;

    DM::View rankview;
    DM::View rankview_centroids;
    DM::View city;
};

#endif // urbandevelRankEuclid_H
