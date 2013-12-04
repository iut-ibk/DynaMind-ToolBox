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


DM_DECLARE_NODE_NAME(urbandevelRankEuclid, DynAlp)

urbandevelRankEuclid::urbandevelRankEuclid()
{
    // declare parameters
    usedevelyears = TRUE;
    prefersmallareas = TRUE;
    this->addParameter("use devel years", DM::BOOL, &this->usedevelyears); // if set the years set in the develareas will be ignored, factor has to be set in View: city->yearfactor
    this->addParameter("prefer small areas", DM::BOOL, &this->prefersmallareas); // develop small areas first, factor has to be set in View: city->areafactor
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

//    mforeach(DM::Component* currentcity, cities)
//    {
    DM::Component* currentcity = cities[0];
        // get max,min and mean devel years and areas
        int sy = static_cast<int>(currentcity->getAttribute("startyear")->getDouble());
        int ey = static_cast<int>(currentcity->getAttribute("endyear")->getDouble());
        double yearfactor = currentcity->getAttribute("yearfactor")->getDouble();
        double areafactor = currentcity->getAttribute("areafactor")->getDouble();
        int min_dy, max_dy, mean_dy = 0;
        int min_area, max_area, mean_area = 0;
        std::vector<int> year_vec;
        std::vector<int> area_vec;

        for (int index = 0; index < superblocks.size(); index++)
        {
            DM::Component * currentsuperblock = superblocks[index];
            int dy = static_cast<int>(currentsuperblock->getAttribute("develyear")->getDouble());
            double area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)currentsuperblock);

            if (dy == 0 || dy < sy) {dy = sy + 1;}
            if (dy >= ey) {dy = ey - 1;}

            year_vec.push_back(dy);
            area_vec.push_back(area);

        }

        int year_sum = std::accumulate(year_vec.begin(), year_vec.end(), 0);
        double area_sum = std::accumulate(area_vec.begin(), area_vec.end(), 0.0);

        min_dy = *std::min_element(year_vec.begin(), year_vec.end());
        max_dy = *std::max_element(year_vec.begin(), year_vec.end());
        mean_dy = year_sum / year_vec.size();

        min_area = *std::min_element(area_vec.begin(), area_vec.end());
        max_area = *std::max_element(area_vec.begin(), area_vec.end());
        mean_area = area_sum / area_vec.size();

        DM::Logger(DM::Error) << "min|max|mean year" << min_dy << "|" << max_dy << "|" << mean_dy;
        DM::Logger(DM::Error) << "min|max|mean area" << min_area << "|" << max_area << "|" << mean_area;


/*        mforeach(DM::Component* currentcentroid, sb_centroids)
        {
          std::string currentsuperblock_ID = currentcentroid->getAttribute("SUPERBLOCK")->getLink().uuid;
          DM::Face * currentsuperblock = static_cast<DM::Face*>(sys->getComponent(currentsuperblock_ID));

          int distance = static_cast<int>(TBVectorData::calculateDistance((DM::Node*)currentcity, (DM::Node*)currentcentroid));

          double yf, af = 0;

          if (usedevelyears) {
              int dy = static_cast<int>(currentsuperblock->getAttribute("develyear")->getDouble());
              if (dy == 0 || dy < sy) {dy = sy+1;}
              if (dy >= ey) {dy = ey-1;}

              yf = (dy - sy + 2)/(ey - sy); // values from 0-1
              //DM::Logger(DM::Error) << "dev year|year factor   " << dy << "|" << yf;
          }
          if (prefersmallareas) {
              double area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)currentsuperblock)/10000;
              af = 1/area;
              //DM::Logger(DM::Error) << "area|area factor   " << area << "|" << af;
          }

          int rank = static_cast<int>(distance * yf * yearfactor * af *areafactor);
        //  DM::Logger(DM::Error) << "distance|rank   " << distance << "|" << rank << "\n";
          currentsuperblock->changeAttribute("rank", distance);
        }
    }
    */
}
