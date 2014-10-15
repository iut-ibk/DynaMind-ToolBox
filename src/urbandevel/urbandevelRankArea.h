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

    std::string getHelpUrl();

private:
    DM::View rankview;

    std::string rankview_name;
    bool reduction;
    std::string rank_function;
    double rank_function_factor;
    double rank_weight;

    std::string attribute_name;
};

#endif // urbandevelRankArea_H
