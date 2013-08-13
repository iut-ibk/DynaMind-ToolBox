/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013 Markus Sengthaler

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

#ifndef MODUELOBSERVER_H
#define MODUELOBSERVER_H

#include "dmmoduleobserver.h"
#include <string>
#include <dmstdutilities.h>

class ModelNode;

class ModelObserver: public DM::ModuleObserver
{
	ModelNode* node;
public:
	ModelObserver(ModelNode* node, DM::Module* module):
		node(node), ModuleObserver(module)
	{
		foreach(std::string portName, module->getInPortNames())
			notifyAddPort(portName, DM::INPORT);
		foreach(std::string portName, module->getOutPortNames())
			notifyAddPort(portName, DM::OUTPORT);
	};
	
	void notifyAddPort(const std::string &name, const DM::PortType type);
	void notifyRemovePort(const std::string &name, const DM::PortType type);
	void notifyChangeName(const std::string &name);
};

#endif // MODUELOBSERVER_H




