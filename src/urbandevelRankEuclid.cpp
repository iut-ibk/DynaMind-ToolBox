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

#include<urbandevelRankEuclid.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>


DM_DECLARE_NODE_NAME(urbandevelRankEuclid, DynAlp)

urbandevelRankEuclid::urbandevelRankEuclid()
{
    // declare parameters
    rank_fun = 1;
    this->addParameter("ranking function", DM::INT, &this->rank_fun); // ranking function
}

urbandevelRankEuclid::~urbandevelRankEuclid()
{
}

void urbandevelRankEuclid::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    superblock_centroids = DM::View("SUPERBLOCK_CENTROIDS", DM::NODE, DM::READ);
    city = DM::View("CITY", DM::NODE, DM::READ);

    // attach new attributes to view
    superblock.addAttribute("develyear", DM::Attribute::DOUBLE, DM::READ);
    superblock.addAttribute("type", DM::Attribute::DOUBLE, DM::READ);
    superblock.addAttribute("rank", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(superblock);
    this->addData("data", views);
}

void urbandevelRankEuclid::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> superblocks = sys->getAllComponentsInView(superblock);
    std::vector<DM::Component *> sb_centroids = sys->getAllComponentsInView(superblock_centroids);
    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);

    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component* city = cities[0];

    std::vector<double> distance;
    std::vector<int> rank;

    for (int i = 0; i < superblocks.size(); i++)
    {
        std::vector<DM::Component*> link = superblocks[i]->getAttribute("SUPERBLOCK_CENTROIDS")->getLinkedComponents();

        if(link.size() < 1)
        {
            DM::Logger(DM::Error) << "no superblock - centroid link";
            return;
        }

        DM::Node * centroid = dynamic_cast<DM::Node*>(link[0]);

        distance.push_back(TBVectorData::calculateDistance((DM::Node*)city, (DM::Node*)centroid));
    }
    int method = 1;
    DAHelper::darank(distance, rank, method);
    for (int i = 0; i < superblocks.size(); i++)
    {
        dynamic_cast<DM::Face*>(superblocks[i])->changeAttribute("rank", rank[i]);
    }
}
