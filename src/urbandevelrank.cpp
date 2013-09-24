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

#include<urbandevelrank.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(UrbanDevelRank, DynAlp)

UrbanDevelRank::UrbanDevelRank()
{
    // declare parameters
    yearcycle = 5;
    wp_com = 10;
    wp_ind = 60;
    this->addParameter("Start year", DM::INT, &this->startyear); // if not set first year of data will be used
    this->addParameter("End year", DM::INT, &this->endyear); // if not set last year of data will be used
    this->addParameter("Years per Cycle", DM::INT, &this->yearcycle);
    this->addParameter("Share of commercial workplaces", DM::INT, &this->wp_com);
    this->addParameter("Share of industrial workplaces", DM::INT, &this->wp_ind);
}

UrbanDevelRank::~UrbanDevelRank()
{
}

void UrbanDevelRank::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    city = DM::View("CITY", DM::NODE, DM::MODIFY);
    // attach new attributes to view
    city.getAttribute("year");
    city.getAttribute("population");
    city.addAttribute("yearcycle");
    city.addAttribute("wp_com"); //workplaces
    city.addAttribute("wp_ind");
    city.addAttribute("popdiffperyear");

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    this->addData("data", views);
}

void UrbanDevelRank::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::map<std::string,DM::Component *> cities = sys->getAllComponentsInView(city);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    mforeach(DM::Component* currentcity, cities)
    {
        //year->2000,2010,2020
        //pop->x,y,z
        QString year = QString::fromStdString(currentcity->getAttribute("year")->getString()).simplified();
        QString pop = QString::fromStdString(currentcity->getAttribute("population")->getString()).simplified();

        DM::Logger(DM::Debug) << "year: " << year << "  population: " << pop;

        QStringList yrlist = year.split(",");
        QStringList poplist = pop.split(",");

        if (yrlist.size() != poplist.size())
            DM::Logger(DM::Warning) << "no of years = " << yrlist.size() << "no of popdata = " << poplist.size() << "... must be the same";

        int sy = startyear;
        int ey = endyear;

        if (startyear == 0) int sy = yrlist.at(0).toInt();
        if (endyear == 0) int ey = yrlist.at(yrlist.size()).toInt();

        if (sy > ey)
            DM::Logger(DM::Warning) << "start year = " << sy << ">" << "end year = " << ey;

        std::vector<double> popdiffvector;

        for (int i=sy; i <= ey; i++) // cycle through the years from startyear to endyear
        {
            for (int j=0; j < (yrlist.size()-1); j++) // cycle through the given population data
            {
                int yr1 = yrlist.at(j).toInt();
                int yr2 = yrlist.at(j+1).toInt();

                if (i > yr1 && i < yr2)
                {
                    int yrdiff = yrlist.at(j+1).toInt() - yrlist.at(j).toInt();
                    int popdiff = poplist.at(j+1).toInt() - poplist.at(j).toInt();
                    popdiffvector.push_back(popdiff/yrdiff);
                    DM::Logger(DM::Debug) << "year: " << i << "popdiffperyear: " << popdiff/yrdiff;
                }
            }
        }
        DM::Attribute* popdiffperyear = currentcity->getAttribute("popdiffperyear");
        popdiffperyear->setDoubleVector(popdiffvector);
    }
}
