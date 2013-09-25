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

DM_DECLARE_NODE_NAME(urbandevelRankEuclid, DynAlp)

urbandevelRankEuclid::urbandevelRankEuclid()
{
    // declare parameters
    usedevelyears = true;
    prefersmallareas = false;
    this->addParameter("use given devel years", DM::BOOL, &this->usedevelyears); // if set the years set in the develareas will be ignored
    this->addParameter("Prefer small areas", DM::BOOL, &this->prefersmallareas); // develop small areas first
}

urbandevelRankEuclid::~urbandevelRankEuclid()
{
}

void urbandevelRankEuclid::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    //superblock_centroids = DM::View("SUPERBLOCK_CENTROIDS"), DM::NODE, DM::READ);

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

    mforeach(DM::Component* currentcentroid, sb_centroids)
    {
        std::string currentsuperblock_ID = currentcentroid->getAttribute("SUPERBLOCK_ID")->getLink().uuid;
        DM::Face * currentsuperblock = static_cast<DM::Face*>(sys->getComponent(currentsuperblock_ID));
        DM::Logger(DM::Warning) << "superblock foreach";
    }
}
