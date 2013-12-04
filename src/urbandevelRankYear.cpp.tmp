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


DM_DECLARE_NODE_NAME(urbandevelRankYear, DynAlp)

urbandevelRankYear::urbandevelRankYear()
{
    // declare parameters
    usecontrolweight = TRUE;
    this->addParameter("Weight value from Control", DM::BOOL, &this->usecontrolweight); // if set the weighting factor from the control module will be used, otherwise its 1
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
    superblock.getAttribute("develyear");
    superblock.getAttribute("type");
    superblock.addAttribute("rank");

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(superblock);
    this->addData("data", views);
}

void urbandevelRankYear::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");
    std::map<std::string,DM::Component *> cities = sys->getAllComponentsInView(city);
    std::map<std::string,DM::Component *> superblocks = sys->getAllComponentsInView(superblock);

    mforeach(DM::Component* currentcity, cities)
    {
        // get max,min and mean devel years and areas
        int sy = static_cast<int>(currentcity->getAttribute("startyear")->getDouble());
        int ey = static_cast<int>(currentcity->getAttribute("endyear")->getDouble());
        double yearfactor = currentcity->getAttribute("yearfactor")->getDouble();
        int min_dy, max_dy, mean_dy = 0;
        std::vector<int> year_vec;

        mforeach(DM::Component* currentsuperblock, superblocks)
        {
            int dy = static_cast<int>(currentsuperblock->getAttribute("develyear")->getDouble());
            if (dy == 0 || dy < sy) {dy = sy + 1;}
            if (dy >= ey) {dy = ey - 1;}
            year_vec.push_back(dy);
        }

        int year_sum = std::accumulate(year_vec.begin(), year_vec.end(), 0);

        min_dy = *std::min_element(year_vec.begin(), year_vec.end());
        max_dy = *std::max_element(year_vec.begin(), year_vec.end());
        mean_dy = year_sum / year_vec.size();

        DM::Logger(DM::Error) << "min|max|mean year" << min_dy << "|" << max_dy << "|" << mean_dy;

        mforeach(DM::Component* currentsuperblock, superblocks)
        {
            double yf = 0;

            int dy = static_cast<int>(currentsuperblock->getAttribute("develyear")->getDouble());
            if (dy == 0 || dy < sy) {dy = sy+1;}
            if (dy >= ey) {dy = ey-1;}

            yf = (dy - sy + 2)/(ey - sy); // values from 0-1
            //DM::Logger(DM::Error) << "dev year|year factor   " << dy << "|" << yf;
            currentsuperblock->changeAttribute("rank", yf);
        }
    }
}
