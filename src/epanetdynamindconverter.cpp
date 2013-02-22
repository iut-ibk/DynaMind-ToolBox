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

#include <epanetdynamindconverter.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//Watersupply
#include <dmepanet.h>
#include <epanetmodelcreator.h>
#include <watersupplyviewdef.h>

using namespace DM;

bool EpanetDynamindConverter::createEpanetModel(System *sys, EPANETModelCreator *creator, string inpfilepath)
{
    DM::WS::ViewDefinitionHelper wsd;
    typedef std::map<std::string, DM::Component*> cmap;
    cmap::iterator itr;

    //SET OPTIONS
    if(!creator->setOptionUnits(EPANETModelCreator::LPS))return false;
    if(!creator->setOptionHeadloss(EPANETModelCreator::DW))return false;

    //JUNCTIONS
    cmap junctions = sys->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));

    for(itr = junctions.begin(); itr != junctions.end(); ++itr)
        if(!creator->addJunction(static_cast<DM::Node*>((*itr).second)))return false;

    //RESERVOIRS
    cmap reservoir = sys->getAllComponentsInView(wsd.getView(DM::WS::RESERVOIR, DM::READ));

    for(itr = reservoir.begin(); itr != reservoir.end(); ++itr)
        if(!creator->addReservoir(static_cast<DM::Node*>((*itr).second)))return false;

    //TANKS
    cmap tank = sys->getAllComponentsInView(wsd.getView(DM::WS::TANK, DM::READ));

    for(itr = tank.begin(); itr != tank.end(); ++itr)
        if(!creator->addTank(static_cast<DM::Node*>((*itr).second)))return false;

    //PIPES
    cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::READ));

    for(itr = pipes.begin(); itr != pipes.end(); ++itr)
        if(!creator->addPipe(static_cast<DM::Edge*>((*itr).second)))return false;


    if(!creator->save(inpfilepath))return false;

    return true;
}

bool EpanetDynamindConverter::checkENRet(int ret)
{
    if(ret>0)
    {
        const uint SIZE = 100;
        char errorstring[SIZE];
        EPANET::ENgeterror(ret,errorstring,SIZE);

        if(ret > 7)
        {
            DM::Logger(DM::Error) << "EPANET error code: " << ret << " (" << errorstring << ")";
            return false;
        }
        else
        {
            DM::Logger(DM::Warning) << "EPANET warning code: " << ret << " (" << errorstring << ")";
            return true;
        }
    }

    return true;
}
