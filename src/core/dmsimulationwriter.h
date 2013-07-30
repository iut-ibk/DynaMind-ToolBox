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

#ifndef DMSIMULAITONWRITER_H
#define DMSIMULAITONWRITER_H

#include "dmcompilersettings.h"
#include <string>
#include <list>
#include <dmsimulation.h>


#define ADDRESS_TO_INT(x) *(int*)(&x)
class QIODevice;
class QString;

namespace DM 
{
	class DM_HELPER_DLL_EXPORT SimulationWriter
	{
	private:
		//static std::string  writeLink(Port * p);
	public:
		//SimulationWriter();
		static void writeSimulation(QIODevice* dest, QString filePath, 
									const std::list<Module*>& modules, 
									const std::list<Simulation::Link*>& links,
									Module* root = NULL);
	};

}

#endif // SIMULAITONWRITER_H
