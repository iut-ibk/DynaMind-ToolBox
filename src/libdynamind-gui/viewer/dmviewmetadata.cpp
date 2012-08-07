#include "dmviewmetadata.h"

#include "dmattribute.h"

#include <limits>
#include <algorithm>

namespace DM {

void ViewMetaData::max_vec(const double *other) {
    max[0] = std::max(other[0], max[0]);
    max[1] = std::max(other[1], max[1]);
    max[2] = std::max(other[2], max[2]);
}

void ViewMetaData::min_vec(const double *other) {
    min[0] = std::min(other[0], min[0]);
    min[1] = std::min(other[1], min[1]);
    min[2] = std::min(other[2], min[2]);
}

ViewMetaData::ViewMetaData(std::string attribute)  
    : attr(attribute), number_of_primitives(0)  {
    min[0] = std::numeric_limits<double>::max();
    min[2] = min[1] = attr_min = min[0];
    max[0] = std::numeric_limits<double>::min();
    max[2] = max[1] = attr_max = max[0];
}

void ViewMetaData::operator()(DM::System *, DM::View , DM::Face *f, DM::Node *n, iterator_pos pos) {
    if (pos == before) {
        DM::Attribute *a = f->getAttribute(attr);
        if (a->hasDouble()) {
            attr_max = std::max(attr_max, a->getDouble());
            attr_min = std::min(attr_min, a->getDouble());
        }
        if (a->hasDoubleVector()) {
            std::vector<double> dv = a->getDoubleVector();
            attr_max = std::max(attr_max, *std::max_element(dv.begin(), dv.end()));
            attr_min = std::min(attr_min, *std::min_element(dv.begin(), dv.end()));
        }
        number_of_primitives++;
    }
    if (pos != in_between) return;
    min_vec(n->get());
    max_vec(n->get());
}

void ViewMetaData::operator()(DM::System *s, DM::View v, DM::Edge *f, DM::Node *n, iterator_pos pos) {
    if (pos != in_between) return;
    min_vec(n->get());
    max_vec(n->get());
}

double ViewMetaData::radius() const {
    double span[3] = {max[0] - min[0], 
                      max[1] - min[1], 
                      0 /*max[2] - min[2]*/};
    return std::max(span[0], span[1])/2.0;
}

} // namespace DM
