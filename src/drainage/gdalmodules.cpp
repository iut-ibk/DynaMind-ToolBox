/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2014  Christian Urich

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
#include "gdaldmswmm.h"
#include "gdalclusternetwork.h"
#include "directnetwork.h"
#include "gdaljoincluster.h"
#include "gdalextractnodes.h"
#include "dm_strahler.h"
#include "dm_simplifynetwork.h"

using namespace std;
extern "C" void DM_HELPER_DLL_EXPORT  registerModules(DM::ModuleRegistry *registry) {
	registry->addNodeFactory(new DM::NodeFactory<GDALDMSWMM>());
	registry->addNodeFactory(new DM::NodeFactory<GDALClusterNetwork>());
	registry->addNodeFactory(new DM::NodeFactory<GDALDirectNetwork>());
	registry->addNodeFactory(new DM::NodeFactory<GDALJoinCluster>());
	registry->addNodeFactory(new DM::NodeFactory<GDALExtractNodes>());
	registry->addNodeFactory(new DM::NodeFactory<GDALExtractNodes>());
	registry->addNodeFactory(new DM::NodeFactory<DM_Strahler>());
	registry->addNodeFactory(new DM::NodeFactory<DM_SimplifyNetwork>());
}
