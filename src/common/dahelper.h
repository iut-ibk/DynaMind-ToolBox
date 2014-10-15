#ifndef DAHELPER_H
#define DAHELPER_H

#include <dm.h>

class DAHelper
{
public:

    /** @brief ranks a set of DM::Components according to a given DM::Double Attribute vector
            ranking is possible: linear or power(x) **/
    static bool darank(std::vector<double>& distance ,std::vector<int>& rank, std::string function = "lin", double factor = 1);
    static bool daweight(std::vector<int>& oldrank, std::vector<int>& newrank, double rank_weight = 1);

};

#endif // DAHELPER_H
