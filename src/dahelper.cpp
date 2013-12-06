#include "dahelper.h"
#include <numeric>

bool DAHelper::darank(std::vector<double>& values, std::vector<int>& rank, int function = 0)
{
    //double sum = std::accumulate(values.begin(), values.end(), 0.0);
    //double mean = sum / values.size();
    if ( function == 0 )
    {
        double min = *std::min_element(values.begin(), values.end());
        double max = *std::max_element(values.begin(), values.end());

        for ( int i = 0; i < values.size(); i++)
        {
            double val = values[i];
            rank.push_back(static_cast<int>( 1 + (9*val-max)/(max-min)));
        }
    }
    if ( function == 1 )
    {
        double min = pow(*std::min_element(values.begin(), values.end()), 2);
        double max = pow(*std::max_element(values.begin(), values.end()), 2);

        DM::Logger(DM::Error) << "min: " << min << "max: " << max;

        for ( int i = 0; i < values.size(); i++)
        {
            double val = pow(values[i],2);
            rank.push_back(static_cast<int>( 2 + (9*val-max)/(max-min)));
            DM::Logger(DM::Error) << "val: " << val << "rank: " << rank[i];
        }
    }
    return 1;
}
