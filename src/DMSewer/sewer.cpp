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

#include "dmcompilersettings.h"
#include "dmnodefactory.h"
#include "dmmoduleregistry.h"
#include "timeareamethod.h"
#include "dmswmm.h"
#include "networkanalysis.h"
#include "directnetwork.h"
#include "generatesewernetwork.h"
#include "extractnetwork.h"
#include "outfallplacement.h"
#include "reconstructparameter.h"



/*#include "pickstartpoints.h"

#include "infiltrationtrench.h"
#include "selectinfiltration.h"*/

#include <iostream>

using namespace std;

/**
  * @addtogroup Sewer
  * @brief Sewer Modules
  * @section Data Structure
  * @subsection Nodes
  * - INLET
  * - CONDUIT
  * - JUNCTION
  * - OUTFALL
  * @subsection Edges
  * - CONDUIT
  * @author Christian Urich
  */

extern "C" void DM_HELPER_DLL_EXPORT  registerModules(DM::ModuleRegistry *registry) {
    registry->addNodeFactory(new DM::NodeFactory<TimeAreaMethod>());
    registry->addNodeFactory(new DM::NodeFactory<DMSWMM>());
    registry->addNodeFactory(new DM::NodeFactory<NetworkAnalysis>());
    registry->addNodeFactory(new DM::NodeFactory<DirectNetwork>());
    registry->addNodeFactory(new NodeFactory<GenerateSewerNetwork>());
    registry->addNodeFactory(new NodeFactory<ExtractNetwork>());
    registry->addNodeFactory(new NodeFactory<OutfallPlacement>());
    registry->addNodeFactory(new NodeFactory<ReconstructParameter>());
    /*registry->addNodeFactory(new NodeFactory<PickStartPoints>());

    registry->addNodeFactory(new NodeFactory<InfiltrationTrench>());
    registry->addNodeFactory(new NodeFactory<SelectInfiltration>());*/

}
