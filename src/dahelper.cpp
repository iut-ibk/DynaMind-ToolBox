#include "dahelper.h"
#include <numeric>

bool DAHelper::darank(pair< std::vector<double>,std::vector<int> >& vec)
{

    std::vector<double> values = vec.first;
    std::vector<int> rank = vec.second;

    double sum = std::accumulate(values.begin(), values.end(), 0.0);

    return 1;
}
