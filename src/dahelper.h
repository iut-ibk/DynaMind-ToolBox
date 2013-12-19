/**
 * @file
 * @author  Christian Mikovits <christian.mikovits@uibk.ac.at>
 * @version 0.1
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Mikovits

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef DAHELPER_H
#define DAHELPER_H

#include <dm.h>

class DAHelper
{
public:

    /** @brief ranks a set of DM::Components according to a given DM::Double Attribute vector
            ranking is possible: linear or power(x) **/
    static bool darank(std::vector<double>& distance ,std::vector<int>& rank, std::string function = "lin", double factor = 1);
    static bool daweight(std::vector<int>& oldrank, std::vector<int>& newrank, double rank_weight = 1);

};

#endif // DAHELPER_H
