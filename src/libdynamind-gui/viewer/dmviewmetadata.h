#ifndef DM_VIEWMETADATA_H
#define DM_VIEWMETADATA_H

#include "dmsystemiterators.h"

namespace DM {

class System;
class View;
class Component;

struct ViewMetaData {
    ViewMetaData(std::string attribute);
    
    void max_vec(const double *other);
    void min_vec(const double *other);
    
    void operator()(System *, View , Component *f, Node *n, iterator_pos pos);
    
    double radius() const;
    
    double min[3];
    double max[3];
    
    std::string attr;
    double attr_min, attr_max;
    
    int number_of_primitives;
};

} // namespace DM

#endif // DM_VIEWMETADATA_H
