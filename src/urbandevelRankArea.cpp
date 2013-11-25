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

#include<urbandevelRankArea.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>


DM_DECLARE_NODE_NAME(urbandevelRankArea, DynAlp)

urbandevelRankArea::urbandevelRankArea()
{
    // declare parameters
    usecontrolweight = TRUE;
    this->addParameter("Weight value from Control", DM::BOOL, &this->usecontrolweight); // if set the weighting factor from the control module will be used, otherwise its 1
}

urbandevelRankArea::~urbandevelRankArea()
{
}

void urbandevelRankArea::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    city = DM::View("CITY", DM::NODE, DM::READ);

    // attach new attributes to view
    superblock.getAttribute("develyear");
    superblock.getAttribute("type");
    superblock.addAttribute("rank");

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(superblock);
    this->addData("data", views);
}

void urbandevelRankArea::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");
    std::map<std::string,DM::Component *> cities = sys->getAllComponentsInView(city);
    std::map<std::string,DM::Component *> superblocks = sys->getAllComponentsInView(superblock);

    mforeach(DM::Component* currentcity, cities)
    {
        // get max,min and mean devel years and areas
        double areafactor = currentcity->getAttribute("yearfactor")->getDouble();
        int min_area, max_area, mean_area = 0;
        std::vector<double> area_vec;

        mforeach(DM::Component* currentsuperblock, superblocks)
        {
            double area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)currentsuperblock);
            area_vec.push_back(area);
        }

        double area_sum = std::accumulate(area_vec.begin(), area_vec.end(), 0.0);
        min_area = *std::min_element(area_vec.begin(), area_vec.end());
        max_area = *std::max_element(area_vec.begin(), area_vec.end());
        mean_area = area_sum / area_vec.size();

        DM::Logger(DM::Error) << "min|max|mean area" << min_area << "|" << max_area << "|" << mean_area;

        mforeach(DM::Component* currentsuperblock, superblocks)
        {
            double area_factor = 000;
            currentsuperblock->changeAttribute("rank", area_factor);
        }
    }
}
