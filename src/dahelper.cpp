#include "dahelper.h"
#include <numeric>

bool DAHelper::darank(std::vector<double>& values, std::vector<int>& rank, std::string function = "linear")
{
    //double sum = std::accumulate(values.begin(), values.end(), 0.0);
    //double mean = sum / values.size();
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());

    for ( int i = 0; i < values.size(); i++)
    {
        if ( function == "linear" )
        {
            double val = values[i];
            rank.push_back(static_cast<int>( 1+(9*val-max)/(max-min)));
        }
    }
    return 1;
}
