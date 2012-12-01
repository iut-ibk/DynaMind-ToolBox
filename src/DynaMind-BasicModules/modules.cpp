/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
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
#include "dmnodefactory.h"
#include "dmmoduleregistry.h"
#include "cellularautomata.h"
#include "loopgroup.h"
#include "adddatatonewview.h"
#include "appendattributes.h"
#include "exportrasterdata.h"
#include "appendviewfromsystem.h"
#include "marker.h"
#include "clearview.h"
#include "exportnodestofile.h"
#include "importrasterdata.h"
#include "calculatecentroid.h"
#include "attributecalculator.h"
#include "spatiallinking.h"
#include "importwithgdal.h"
#include "calculateboundingbox.h"
#include "singlepoint.h"

using namespace std;



extern "C" void DM_HELPER_DLL_EXPORT  registerModules(ModuleRegistry *registry) {
    registry->addNodeFactory(new NodeFactory<LoopGroup>());
    registry->addNodeFactory(new NodeFactory<CellularAutomata>());
    registry->addNodeFactory(new NodeFactory<AddDataToNewView>());
    registry->addNodeFactory(new NodeFactory<ExportRasterData>());
    registry->addNodeFactory(new NodeFactory<AppendViewFromSystem>());
    registry->addNodeFactory(new NodeFactory<Marker>());
    registry->addNodeFactory(new NodeFactory<AppendAttributes>());
    registry->addNodeFactory(new NodeFactory<ClearView>());
    registry->addNodeFactory(new NodeFactory<ExportNodesToFile>());
    registry->addNodeFactory(new NodeFactory<ImportRasterData>());
    registry->addNodeFactory(new NodeFactory<CalculateCentroid>());
    registry->addNodeFactory(new NodeFactory<AttributeCalculator>());
    registry->addNodeFactory(new NodeFactory<SpatialLinking>());
    registry->addNodeFactory(new NodeFactory<ImportwithGDAL>());
    registry->addNodeFactory(new NodeFactory<CalculateBoundingBox>());
    registry->addNodeFactory(new NodeFactory<SinglePoint>());
}

