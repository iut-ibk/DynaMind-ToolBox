/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Michael Mair

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
#include "dmnodefactory.h"
#include "dmmoduleregistry.h"
#include <spanningtree.h>
#include <extractnodesfromedges.h>
#include <connectnodes2graph.h>
#include <extractmaxgraphofforest.h>
#include <reducetree.h>
#include <setZfromRasterdata.h>
#include <calculatelength.h>
#include <simulatewithepanet.h>
#include <createepanetmodel.h>
#include <minimumsteinertree.h>
#include <wsdimensioning.h>
#include <initws.h>
#include <wsloopcreator.h>
#include <trimgraph.h>
#include <mapnodesattributes.h>
#include <nodedistribute.h>
#include <removeNodesOnZ.h>
#include <distributeWeightOnNodes.h>
#include <createvoronoi.h>
#include <initsewer.h>
#include <createswmmmodel.h>

#include <spatialpressureperformance.h>
#include <totaldemandperformance.h>

#include <generalgraphanalysis.h>

using namespace std;

extern "C" void DM_HELPER_DLL_EXPORT  registerModules(DM::ModuleRegistry *registry) {
	registry->addNodeFactory(new DM::NodeFactory<SpanningTree>());
	registry->addNodeFactory(new DM::NodeFactory<ExtractNodesFromEdges>());
	registry->addNodeFactory(new DM::NodeFactory<ConnectNodes2Graph>());
	registry->addNodeFactory(new DM::NodeFactory<ExtractMaxGraph>());
	registry->addNodeFactory(new DM::NodeFactory<ReduceTree>());
	registry->addNodeFactory(new DM::NodeFactory<SetZfromRasterdata>());
	registry->addNodeFactory(new DM::NodeFactory<CalculateEdgeLength>());
	registry->addNodeFactory(new DM::NodeFactory<SimulateWithEPANET>());
	registry->addNodeFactory(new DM::NodeFactory<CreateEPANETModel>());
	registry->addNodeFactory(new DM::NodeFactory<MinimumSteinerTree>());
	registry->addNodeFactory(new DM::NodeFactory<Dimensioning>());
	registry->addNodeFactory(new DM::NodeFactory<InitWaterSupplySystem>());
	registry->addNodeFactory(new DM::NodeFactory<LoopCreator>());
	registry->addNodeFactory(new DM::NodeFactory<TrimGraph>());
	registry->addNodeFactory(new DM::NodeFactory<MapNodes2Graph>());
	registry->addNodeFactory(new DM::NodeFactory<DistributeNodes>());
	registry->addNodeFactory(new DM::NodeFactory<RemoveNodesOnZ>());
	registry->addNodeFactory(new DM::NodeFactory<DistributeWeightOnNodes>());
	registry->addNodeFactory(new DM::NodeFactory<CreateVoronoiDiagram>());

	registry->addNodeFactory(new DM::NodeFactory<SpatialPressurePerformance>());
	registry->addNodeFactory(new DM::NodeFactory<TotalDemandPerformance>());

    registry->addNodeFactory(new DM::NodeFactory<InitSewerSystem>());
    registry->addNodeFactory(new DM::NodeFactory<CreateSWMMModel>());

    registry->addNodeFactory(new DM::NodeFactory<GeneralGraphAnalysis>());
}
