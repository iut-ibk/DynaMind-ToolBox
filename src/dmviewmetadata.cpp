/**
 * @file
 * @author  Gregor Burger <burger.gregor@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Gregor Burger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

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
    max[0] = -std::numeric_limits<double>::max();
    max[2] = max[1] = attr_max = max[0];
    fromNode = true;
}

void ViewMetaData::operator()(DM::System *sys, DM::View v, DM::Component *c, DM::Node *n, iterator_pos pos) {
    if (pos == before && !fromNode) {
        DM::Attribute *a = c->getAttribute(attr);
        if (a->getType() == Attribute::DOUBLE) {
            attr_max = std::max(attr_max, a->getDouble());
            attr_min = std::min(attr_min, a->getDouble());
        }
        if (a->getType() == Attribute::DOUBLEVECTOR ||
            a->getType() == Attribute::TIMESERIES) {
            
            std::vector<double> dv = a->getDoubleVector();
            if (dv.size()) {
                attr_max = std::max(attr_max, *std::max_element(dv.begin(), dv.end()));
                attr_min = std::min(attr_min, *std::min_element(dv.begin(), dv.end()));
            }
        }

        number_of_primitives++;
    }
    //If Rasterdata use z as Attribute
    if (v.getType() == DM::RASTERDATA) {
        DM::ComponentMap cmp = sys->getAllComponentsInView(v);
        DM::RasterData * r = 0;
        for (DM::ComponentMap::const_iterator it = cmp.begin();
             it != cmp.end();
             ++it) {
            r = (DM::RasterData *) it->second;
        }
        attr_max = r->getMaxValue();
        attr_min = r->getMinValue();
    }
    if (pos == in_between && fromNode) {
        DM::Attribute *a = c->getAttribute(attr);
        if (a->getType() == Attribute::DOUBLE) {
            attr_max = std::max(attr_max, a->getDouble());
            attr_min = std::min(attr_min, a->getDouble());
        }
        if (a->getType() == Attribute::DOUBLEVECTOR ||
            a->getType() == Attribute::TIMESERIES) {

            std::vector<double> dv = a->getDoubleVector();
            if (dv.size()) {
                attr_max = std::max(attr_max, *std::max_element(dv.begin(), dv.end()));
                attr_min = std::min(attr_min, *std::min_element(dv.begin(), dv.end()));
            }
        }
        number_of_primitives++;
    }
    if (pos != in_between) return;

    const double tmp[3] = {n->getX(), n->getY(), n->getZ()};
    min_vec(tmp);
    max_vec(tmp);

}

double ViewMetaData::radius() const {
    double span[3] = {max[0] - min[0], 
                      max[1] - min[1], 
                      0 /*max[2] - min[2]*/};
    return std::max(span[0], span[1])/2.0;
}

} // namespace DM
