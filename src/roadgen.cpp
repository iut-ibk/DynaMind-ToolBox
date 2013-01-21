/**
 * @file
 * @author Christian Mikovits <christian.mikovits@uibk.ac.at>
 * @version 0.1
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013 Christian Mikovits
 
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

#include <roadgen.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
#include <list>
#include <cmath>
//LSystem
#include <lsystem.h>
#include <roadgen.h>

DM_DECLARE_NODE_NAME(RoadGen,UrbanDevelopment)

RoadGen::RoadGen() {
    std::vector<DM::View> views;
    DM::View view;

    // load road network
    //view = DM::View("STREETS", DM::NODE, DM::READ);
    //views.push_back(view);
    //viewdef["STREETS"]=view;

    // load shapefile with blocked but bridgable areas, eg. water, railroad, motorway
    //view = DM::View("BRIDGABLE", DM::FACE, DM::READ);
    //views.push_back(view);
    //viewdef["BRIDGEABLE"]=view;

    //view = DM::View("POPULATION_DENSITY", DM::FACE, DM::READ);
    //views.push_back(view);
    //viewdef["POPULATION_DENSITY"];

    // declare areas where to build roads
    //view = DM::View("POSSIBLE_AREA", DM::FACE, DM::READ);
    //views.push_back(view);
    //viewdef["POSSIBLE_AREA"];

    // height map of the area [raster]
    //view = DM::View("ELEVATION", DM::RASTERDATA, DM::READ);
    //views.push_back(view);
    //viewdef["ELEVATION"]=view;

    view = DM::View("SUPERBLOCK", DM::FACE, DM::READ);
    views.push_back(view);

    this->addData("Layout", views);
}

/* Implement basically following rules:
 *
initialize priority queue Q with a single entry: r(0, r0, q0)
initialize segment list S to empty

  until Q is empty
    pop smallest r(ti, ri, qi) from Q (i.e., smallest ‘t’)
    accepted = localConstraints(&r) // snaps a road to local segments and evaluates if road should be accepted
    if (accepted) {
      add segment(ri) to S
      foreach r(tj, rj, qj) produced by globalGoals(ri, qi)
        add r(ti + 1 + tj, rj, qj) to Q
    }
//
// desc
//
    i -> index
    r(ti, ri, qi) ->
        ti: timestep delay before evaluation (t = t+1)
        ri: road <vector>
        qi: global goals

    road constraints:
        - minimum distance
        - [min/max] angle
        - [min/max] length to split
        - gradient
        - pop density
*/

void RoadGen::run()
{

    sys = this->getData("Layout");

    rule_map treerules = { { 'X', "(F-[[X]+X]+F[+FX]-X)" },
                     { 'F', "FF" } };
    LSystem treesystem("X", treerules);
// segment_length, angle_left, angle_right, angle_initial
// translation
// scale
    RoadCalc_Config rc_config(1, 0.25f, -0.25f, 0.9f);
// setup lsystem
    RoadCalc RC(treesystem, rc_config);
    RC.init();

    for (int i=0; i < 5; i++) {
        RC.walk();
    }

}
