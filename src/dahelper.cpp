#include "dahelper.h"
#include <numeric>

bool DAHelper::darank(std::vector<double>& values, std::vector<int>& rank, std::string function, int factor)
{
    //double sum = std::accumulate(values.begin(), values.end(), 0.0);
    //double mean = sum / values.size();
    if ( function == "linear" )
    {
        double min = *std::min_element(values.begin(), values.end());
        double max = *std::max_element(values.begin(), values.end());

        for ( int i = 0; i < values.size(); i++)
        {
            double val = values[i];
            rank.push_back(static_cast<int>( 1 + (9*val-max)/(max-min)));
        }
    }
    if ( function == "power" )
    {
        double min = pow(*std::min_element(values.begin(), values.end()), factor);
        double max = pow(*std::max_element(values.begin(), values.end()), factor);

        DM::Logger(DM::Error) << "min: " << min << "max: " << max;

        for ( int i = 0; i < values.size(); i++)
        {
            double val = pow(values[i], factor);
            rank.push_back(static_cast<int>( 2 + (9*val-max)/(max-min)));
            DM::Logger(DM::Error) << "val: " << val << "rank: " << rank[i];
        }
    }
    return 1;
}
