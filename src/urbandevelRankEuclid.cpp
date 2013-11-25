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

#include<urbandevelRankEuclid.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>


DM_DECLARE_NODE_NAME(urbandevelRankEuclid, DynAlp)

urbandevelRankEuclid::urbandevelRankEuclid()
{
    // declare parameters
    usedevelyears = TRUE;
    prefersmallareas = FALSE;
    this->addParameter("use devel years", DM::BOOL, &this->usedevelyears); // if set the years set in the develareas will be ignored
    this->addParameter("prefer small areas", DM::BOOL, &this->prefersmallareas); // develop small areas first
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
    superblock.getAttribute("develyear");
    superblock.getAttribute("type");
    superblock.addAttribute("rank");

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(superblock);
    this->addData("data", views);
}

void urbandevelRankEuclid::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::map<std::string,DM::Component *> superblocks = sys->getAllComponentsInView(superblock);
    std::map<std::string,DM::Component *> sb_centroids = sys->getAllComponentsInView(superblock_centroids);
    std::map<std::string,DM::Component *> cities = sys->getAllComponentsInView(city);

    mforeach(DM::Component* currentcity, cities)
    {
        double sy = currentcity->getAttribute("startyear")->getDouble();
        double ey = currentcity->getAttribute("endyear")->getDouble();
        double yearfactor = currentcity->getAttribute("yearfactor")->getDouble();
        double areafactor = currentcity->getAttribute("areafactor")->getDouble();

        mforeach(DM::Component* currentcentroid, sb_centroids)
        {
          std::string currentsuperblock_ID = currentcentroid->getAttribute("SUPERBLOCK")->getLink().uuid;
          DM::Face * currentsuperblock = static_cast<DM::Face*>(sys->getComponent(currentsuperblock_ID));

          int distance = static_cast<int>(TBVectorData::calculateDistance((DM::Node*)currentcity, (DM::Node*)currentcentroid));

          double year_factor, area_factor = 0;

          if (usedevelyears) {
              double dy = static_cast<int>(currentsuperblock->getAttribute("develyear")->getDouble());
              if (dy == 0 || dy < sy) {dy = sy+1;}
              if (dy >= ey) {dy = ey-1;}

              year_factor = 1/(-((dy - sy)- (ey - sy)) / ( 10 / yearfactor));
              DM::Logger(DM::Error) << "dev year|year factor   " << dy << "|" << year_factor;
          }
          if (prefersmallareas) {
              double area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)currentsuperblock)/10000;
              area_factor = 10/area*areafactor;
              DM::Logger(DM::Error) << "area|area factor   " << area << "|" << area_factor;
          }

          int rank = static_cast<int>(distance * year_factor * area_factor / 10);
          DM::Logger(DM::Error) << "distance|rank   " << distance << "|" << rank << "\n";
          currentsuperblock->changeAttribute("rank", distance);
        }
    }
}
