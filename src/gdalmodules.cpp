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
#include "gdalimportdata.h"
#include "parceling.h"
#include "gdalcreaterectangle.h"
#include "lsystems.h"
#include "nonspatiallinking.h"
#include "gdalattributecalculator.h"
#include "gdalspatiallinking.h"
#include "gdalerase.h"
#include "gdaloffset.h"
#include "gdalparcelsplit.h"
#include "gdalpublishresults.h"


/**
  * @addtogroup TestModules
  */

using namespace std;
extern "C" void DM_HELPER_DLL_EXPORT  registerModules(DM::ModuleRegistry *registry) {
	registry->addNodeFactory(new DM::NodeFactory<GDALImportData>());
	registry->addNodeFactory(new DM::NodeFactory<GDALParceling>());
	registry->addNodeFactory(new DM::NodeFactory<GDALCreateRectangle>());
	registry->addNodeFactory(new DM::NodeFactory<LSystems>());
	registry->addNodeFactory(new DM::NodeFactory<NonSpatialLinking>());
	registry->addNodeFactory(new DM::NodeFactory<GDALSpatialLinking>());
	registry->addNodeFactory(new DM::NodeFactory<GDALAttributeCalculator>());
	registry->addNodeFactory(new DM::NodeFactory<GDALErase>());
	registry->addNodeFactory(new DM::NodeFactory<GDALOffset>());
	registry->addNodeFactory(new DM::NodeFactory<GDALParcelSplit>());
	registry->addNodeFactory(new DM::NodeFactory<GDALPublishResults>());
}
