/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

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

#ifndef EXTRACTNETWORK_H
#define EXTRACTNETWORK_H
/*#include <dmmodule.h>
#include <dm.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "generatesewernetwork.h"
using namespace DM;
class DM_HELPER_DLL_EXPORT ExtractNetwork : public  Module {
DM_DECLARE_NODE ( ExtractNetwork )
public:
    class AgentExtraxtor : public GenerateSewerNetwork::Agent
{
    public:
    AgentExtraxtor(GenerateSewerNetwork::Pos pos):GenerateSewerNetwork::Agent(pos){};
    void run();
};


private:

RasterData * ConnectivityField;
DM::View StartPoints;
RasterData * Goals;
RasterData * Path;
RasterData * Topology;
RasterData * ForbiddenAreas;
DM::View ExportPath;
DM::View existingNetwork;
DM::View StartPointsOut;
//std::string Identifier;
std::string IdentifierConduit;
std::string IdentifierEnd;
std::string IdentifierInlet;
std::string IdentifierShaft;
double ConduitLength;
long steps;

std::vector<std::vector<DM::Node> > SimplifyNetwork(std::vector<std::vector<DM::Node> >  & points, int PReduction, double offset);
void smoothNetwork();
public:
ExtractNetwork();
void run();
};*/

#endif // EXTRACTNETWORK_H
