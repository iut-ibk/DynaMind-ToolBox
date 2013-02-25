/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair
 
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

#include <createepanetmodel.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

//Watersupply
#include <dmepanet.h>
#include <epanetdynamindconverter.h>

using namespace DM;

DM_DECLARE_NODE_NAME(CreateEPANETModel,Watersupply)

CreateEPANETModel::CreateEPANETModel()
{   
    this->inpfilepath="";
    this->addParameter("Path of inp file", DM::FILENAME, &this->inpfilepath);

    std::vector<DM::View> views;
    views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
    views.push_back(wsd.getCompleteView(WS::PIPE,DM::READ));
    views.push_back(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
    views.push_back(wsd.getCompleteView(WS::TANK,DM::READ));
    this->addData("Watersupply", views);
}

void CreateEPANETModel::run()
{
    typedef std::map<std::string, DM::Component*> cmap;
    cmap::iterator itr;

    this->sys = this->getData("Watersupply");

    EpanetDynamindConverter converter;
    EPANETModelCreator *creator = converter.getCreator();

    if(!this->inpfilepath.size())
    {
        DM::Logger(DM::Error) << "No inp file path set";
        return;
    }

    //SET OPTIONS
    creator->setOptionUnits(EPANETModelCreator::LPS);
    creator->setOptionHeadloss(EPANETModelCreator::DW);

    //JUNCTIONS
    cmap junctions = sys->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));

    for(itr = junctions.begin(); itr != junctions.end(); ++itr)
        converter.addJunction(static_cast<DM::Node*>((*itr).second));

    //RESERVOIRS
    cmap reservoir = sys->getAllComponentsInView(wsd.getView(DM::WS::RESERVOIR, DM::READ));

    for(itr = reservoir.begin(); itr != reservoir.end(); ++itr)
        converter.addReservoir(static_cast<DM::Node*>((*itr).second));

    //TANKS
    cmap tank = sys->getAllComponentsInView(wsd.getView(DM::WS::TANK, DM::READ));

    for(itr = tank.begin(); itr != tank.end(); ++itr)
        converter.addTank(static_cast<DM::Node*>((*itr).second));

    //PIPES
    cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::READ));

    for(itr = pipes.begin(); itr != pipes.end(); ++itr)
        converter.addPipe(static_cast<DM::Edge*>((*itr).second));


    creator->save(inpfilepath);
}
