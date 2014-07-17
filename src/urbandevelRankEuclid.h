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
    DM::View gravityview;
    DM::View rankview;
    DM::View rankview_centroids;

    std::string gravityview_name;
    std::string rankview_name;
    bool reduction;
    std::string rank_function;
    double rank_function_factor;
    double rank_weight;
    bool nodeweight;
};

#endif // urbandevelRankEuclid_H
