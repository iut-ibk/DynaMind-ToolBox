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

#ifndef DM_VIEWMETADATA_H
#define DM_VIEWMETADATA_H
#include <dmcompilersettings.h>
#include "dmsystemiterators.h"

namespace DM {

class System;
class View;
class Component;

struct DM_HELPER_DLL_EXPORT ViewMetaData {
    ViewMetaData(std::string attribute);
    bool fromNode;
    
    void max_vec(const double *other);
    void min_vec(const double *other);
    
    void operator()(System *, const View& , Component *f, DM::Vector3* point, DM::Vector3* color, iterator_pos pos);
    
    double radius() const;
    
    double min[3];
    double max[3];
    
    std::string attr;
    double attr_min, attr_max;
    
    int number_of_primitives;
};

} // namespace DM

#endif // DM_VIEWMETADATA_H
