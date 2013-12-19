/**
 * @file
 * @author  Christian Mikovits <christian.mikovits@uibk.ac.at>
 * @version 0.1a
 * @section LICENSE
 * Module for [temporal] ranking of areas for development
   Copyright (C) 2013 Christian Mikovits

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**/

#include <numeric>

#include<urbandevelRankYear.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>


DM_DECLARE_NODE_NAME(urbandevelRankYear, DynAlp)

urbandevelRankYear::urbandevelRankYear()
{
    // declare parameters
    rank_function = "linear";
    rank_function_factor = 1;
    rank_weight = 1;
    this->addParameter("ranking function", DM::STRING, &this->rank_function); // ranking function
    this->addParameter("ranking funcktion faktor", DM::DOUBLE, &this->rank_function_factor);
    this->addParameter("ranking weight", DM::DOUBLE, &this->rank_weight);
}

urbandevelRankYear::~urbandevelRankYear()
{
}

void urbandevelRankYear::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    city = DM::View("CITY", DM::NODE, DM::READ);

    // attach new attributes to view
    superblock.addAttribute("develyear", DM::Attribute::DOUBLE, DM::READ);
    superblock.addAttribute("type", DM::Attribute::DOUBLE, DM::READ);
    superblock.addAttribute("develrank", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(superblock);
    this->addData("data", views);
}

void urbandevelRankYear::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> superblocks = sys->getAllComponentsInView(superblock);
    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);

    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component* currentcity = cities[0];

    int startyear = static_cast<int>(currentcity->getAttribute("startyear")->getDouble());
    int endyear = static_cast<int>(currentcity->getAttribute("endyear")->getDouble());

    std::vector<double> year;
    std::vector<int> oldrank;
    std::vector<int> rank;
    bool rnk_exists = FALSE;

    for (int i = 0; i < superblocks.size(); i++)
    {
        int currentyear = static_cast<int>(superblocks[i]->getAttribute("develyear")->getDouble());
        if ( currentyear <= startyear ) { currentyear = startyear + 1; }
        if ( currentyear >= endyear ) { currentyear = endyear - 1; }
        year.push_back(currentyear);
        oldrank.push_back(static_cast<int>(superblocks[i]->getAttribute("develrank")->getDouble()));
        if ( oldrank[i] > 0 ) { rnk_exists = TRUE; }
    }
    DAHelper::darank(year, rank, rank_function, rank_function_factor);
    if (rnk_exists) { DAHelper::daweight(oldrank, rank, rank_weight); }

    for (int i = 0; i < superblocks.size(); i++)
    {
        dynamic_cast<DM::Face*>(superblocks[i])->changeAttribute("develrank", rank[i]);
    }
}
