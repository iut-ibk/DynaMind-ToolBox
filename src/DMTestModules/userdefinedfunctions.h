/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
 */
#include "compilersettings.h"
#include "muParser.h"
#include "module.h"
#ifndef USERDEFINEDFUNCTIONS_H
#define USERDEFINEDFUNCTIONS_H

namespace mu {
double numberOfValues(const double* values, int index);
double random(double value);
double printValue(double value);
static double ContUrbanFabric;
static double DisContUrbanFabric;
static double RoadRailNetwork;
static double WaterBodies;
static double AgriculturalAreas;
static double ForestsSemiNatural;

inline void addCorineConstants(mu::Parser * p){

    mu::ContUrbanFabric = DM::ContUrbanFabric;
    mu::DisContUrbanFabric= DM::DisContUrbanFabric;
    mu:: RoadRailNetwork= DM::RoadRailNetwork;
    mu::WaterBodies= DM::WaterBodies;
    mu::AgriculturalAreas= DM::AgriculturalAreas;
    mu::ForestsSemiNatural= DM::ForestsSemiNatural;



    /*  p->DefineVar("ContUrbanFabric", new double( vibens::VIBe2::ContUrbanFabric ));
        p->DefineVar("DisContUrbanFabric", new double(vibens::VIBe2::DisContUrbanFabric ));
        p->DefineVar("RoadRailNetwork", new double(vibens::VIBe2::RoadRailNetwork ));
        p->DefineVar("AgriculturalAreas", new double(vibens::VIBe2::AgriculturalAreas ));
        p->DefineVar("ForestsSemiNatural", new double(vibens::VIBe2::ForestsSemiNatural ));
        p->DefineVar("WaterBodies",new double( vibens::VIBe2::WaterBodies ));*/

    p->DefineVar("ContUrbanFabric", & mu::ContUrbanFabric);
    p->DefineVar("DisContUrbanFabric",& mu::DisContUrbanFabric);
    p->DefineVar("RoadRailNetwork", & mu::RoadRailNetwork);
    p->DefineVar("AgriculturalAreas",& mu::AgriculturalAreas);
    p->DefineVar("ForestsSemiNatural", & mu::ForestsSemiNatural);
    p->DefineVar("WaterBodies",& mu::WaterBodies);


}
}

#endif // USERDEFINEDFUNCTIONS_H
