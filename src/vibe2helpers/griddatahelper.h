/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
 *
 * Copyright (C) 2011  Christian Urich

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
 * @section DESCRIPTION
 *
 * The class contains provides the interface to the VIBe2 simulation
 */


#ifndef GIRDDATAHELPER_H
#define GIRDDATAHELPER_H
#include "compilersettings.h"
#include "vectordata.h"

struct VIBE_HELPER_DLL_EXPORT MinMax {
    double minVal;
    double maxVal;
};


class VIBE_HELPER_DLL_EXPORT GriddataHelper
{
public:
    //GirddataHelper();
    static  void createColorTable(VectorData * vec , std::string Identifier, std::string Attribute);
    static  double sum (VectorData * vec , std::string Identifier, std::string Attribute);
    static  MinMax MinMaxValues (VectorData * vec , std::string Identifier, std::string Attribute);
};

#endif // GIRDDATAHELPER_H
