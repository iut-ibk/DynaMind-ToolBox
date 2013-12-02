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

#include <totaldemandperformance.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>
#include <list>
#include <cmath>

using namespace DM;

DM_DECLARE_NODE_NAME(TotalDemandPerformance,Watersupply-Performance)

TotalDemandPerformance::TotalDemandPerformance()
{   
    std::vector<DM::View> views;

    views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
    this->addData("Watersupply-1", views);
    views.clear();
    views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
    this->addData("Watersupply-2", views);
}

void TotalDemandPerformance::run()
{
	typedef std::vector<DM::Component*> cmap;
    cmap::iterator itr;
    std::vector<DM::Node*> j1, j2;
    double totaldemandn1=0;
    double totaldemandn2=0;

    this->sys1 = this->getData("Watersupply-1");
    this->sys2 = this->getData("Watersupply-2");

    //JUNCTIONS
    cmap junctions1 = sys1->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));
    cmap junctions2 = sys2->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));

    //Extract data
    for(itr=junctions1.begin(); itr != junctions1.end(); ++itr)
		j1.push_back(static_cast<DM::Node*>((*itr)));

    for(itr=junctions2.begin(); itr != junctions2.end(); ++itr)
		j2.push_back(static_cast<DM::Node*>((*itr)));

    std::vector<double> maxp1, maxp2;

    totaldemandn1 = getDemandValue(j1);
    totaldemandn2 = getDemandValue(j2);

    //print result
    DM::Logger(DM::Standard) << "Total demand N1 [FLOW UNIT]: " << QString::number(totaldemandn1).toStdString();
    DM::Logger(DM::Standard) << "Total demand N2 [FLOW UNIT]: " << QString::number(totaldemandn2).toStdString();
    DM::Logger(DM::Standard) << "Perormance value [%] (Demand 2 / Demand 1): " << QString::number(totaldemandn2/totaldemandn1).toStdString();
    DM::Logger(DM::Standard) << "Perormance value [FLOW UNIT] (Deamnd 2 - Demand 1): " << QString::number(totaldemandn2-totaldemandn1).toStdString();
}

double TotalDemandPerformance::getDemandValue(std::vector<Node *> &nodes)
{
    double result=0;

    for(uint index=0; index < nodes.size(); index++)
    {
        DM::Node* currentnode = nodes[index];

        result += currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();
    }

    return result;
}
