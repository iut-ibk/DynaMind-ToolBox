/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Markus Sengthaler

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

#ifndef DMMODULEOBSERVER_H
#define DMMODULEOBSERVER_H

#include "dmmodule.h"

namespace DM
{

/** brief provides an interface for observers */
class ModuleObserver
{
protected:
	DM::Module* module;
public:
	ModuleObserver(Module* m): module(m)
	{
		module->addObserver(this);
	}
	virtual ~ModuleObserver(){}
	/** @brief gets called if a port is added */
	virtual void notifyAddPort(const std::string &name, const PortType type) = 0;
	/** @brief gets called if a port is removed */
	virtual void notifyRemovePort(const std::string &name, const PortType type) = 0;
	/** @brief gets called if the module name changed */
	virtual void notifyChangeName(const std::string &name) = 0;
	/** @brief gets called if the module state changed*/
	virtual void notifyStateChange() = 0;

};

}

#endif // DMMODULEOBSERVER_H

