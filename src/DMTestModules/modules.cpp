/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#include "testmodule.h"
/*#include "inoutmodule.h"
#include "inout2.h"
#include "dynamicinout.h"
#include "grouptest.h"*/
#include "createnodes.h"
/*
#include "createallcomponents.h"
#include "reallocator.h"
*/
/**
  * @addtogroup TestModules
  */

using namespace std;

namespace DM{

extern "C" void DM_HELPER_DLL_EXPORT  registerModules(ModuleRegistry *registry) {
    registry->addNodeFactory(new NodeFactory<TestModule>());
    /*registry->addNodeFactory(new NodeFactory<InOut>());
    registry->addNodeFactory(new NodeFactory<InOut2>());
    registry->addNodeFactory(new NodeFactory<DynamicInOut>());
    registry->addNodeFactory(new NodeFactory<GroupTest>());*/
    registry->addNodeFactory(new NodeFactory<CreateNodes>());
    /*registry->addNodeFactory(new NodeFactory<CreateAllComponenets>());
    registry->addNodeFactory(new NodeFactory<Reallocator>());
    registry->addNodeFactory(new NodeFactory<CheckAllComponenets>());
    registry->addNodeFactory(new NodeFactory<SuccessorCheck>());
	*/
}
}
