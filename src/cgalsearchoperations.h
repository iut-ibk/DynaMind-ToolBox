#ifndef CGALSEARCHOPERATIONS_H
#define CGALSEARCHOPERATIONS_H

#include <dm.h>
#include <vector>
class DM_HELPER_DLL_EXPORT CGALSearchOperations
{
public:
   /** @brief sorts a number of Points by its distance */
    static std::vector<DM::Node> NearestPoints(const std::vector<DM::Node*> & nodes,  DM::Node * n);
};

#endif // CGALSEARCHOPERATIONS_H
