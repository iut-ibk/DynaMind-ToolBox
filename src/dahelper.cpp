#include "dahelper.h"
#include <numeric>

bool DAHelper::darank(std::vector<double>& values, std::vector<int>& rank, std::string function, double factor)
{
    if ( function == "linear") {
        function = "power";
        factor = 1;

    }
    if ( function == "power" )
    {
        double min = pow(*std::min_element(values.begin(), values.end()), 1/factor);
        double max = pow(*std::max_element(values.begin(), values.end()), 1/factor);

        if (min == max) max+=1;

        DM::Logger(DM::Debug) << "min: " << min << "max: " << max;

        for ( int i = 0; i < values.size(); i++)
        {
            rank.push_back(static_cast<int>( 1 + ( 9 * ( pow( values[i], 1/factor) - min )) / (max-min) ) );
            DM::Logger(DM::Debug) << "val: " << values[i] << "rank: " << rank[i];
        }
    }
    return 1;
}

bool DAHelper::daweight(std::vector<int>& oldrank, std::vector<int>& newrank, double rank_weight)
{
    for ( int i = 0; i < oldrank.size(); i++)
    {
        if (rank_weight < 0) {
            DM::Logger(DM::Warning) << "rank weight equal or smaller than zero, setting to 0.01";
            rank_weight = 0.1;
        }
        if (oldrank[i] > 0 ) { newrank[i] = static_cast<int>(qRound((oldrank[i] + newrank[i]*rank_weight)/(rank_weight+1))); }
        DM::Logger(DM::Debug) << "old: " << oldrank[i] << "new :" << newrank[i];
    }
    return 1;
}
