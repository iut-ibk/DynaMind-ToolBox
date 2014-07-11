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
#include "dmmoduleregistry.h"

#include "QThread"
#include <iostream>
#include "dmnodefactory.h"
#include <QLibrary>
#include <dmmodule.h>
#include "dmlogger.h"

using namespace std;
using namespace DM;

ModuleRegistry::ModuleRegistry() 
{
}

bool ModuleRegistry::addNodeFactory(INodeFactory *factory)
{
	if(!contains(factory->getNodeName()))
	{
		std::vector<std::string> m = moduleMap[factory->getFileName()];
		m.push_back(factory->getNodeName());
		moduleMap[factory->getFileName()] = m;
	}
	registry[factory->getNodeName()] = factory;
	return true;
}

bool ModuleRegistry::addNativePlugin(const std::string &plugin_path) {

	QLibrary lib(QString::fromStdString(plugin_path));
	if (!lib.load()) 
	{
		//std::cout << "Error loading module " << plugin_path << " : " << lib.errorString().toStdString() << std::endl;
		return false;
	}
	//else
	//std::cout << "successfully loaded module " << plugin_path << std::endl;

	regNodeFunProto regNodeFunc = (regNodeFunProto) lib.resolve("registerModules");

	if (regNodeFunc)
		regNodeFunc(this);
	else 
	{
		Logger(Debug) << plugin_path << " has no node register hook";
		return false;
	}

	return true;
}

typedef std::pair<std::string, INodeFactory *> snf;
std::list<std::string> ModuleRegistry::getRegisteredModules() const {
	std::list<std::string> names;

	foreach(snf item, registry) {
		names.push_back(item.first);
	}

	return names;
}

Module *ModuleRegistry::createModule(const std::string &name) const {
	Logger(Debug)  << "Try to create " << name;
	if (registry.count(name) == 0) 
	{
		Logger(Error) << "module '" << name << "' not found";
		return 0;
	}

	Module *newmod = 0;

	try
	{
		newmod = registry.find(name)->second->createNode();
	}
	catch(...)
	{
		Logger(Error) << "error creating module '" << name << "'";
		return 0;
	}

	return newmod;
}

bool ModuleRegistry::contains(const std::string &name) const {
	return registry.find(name) != registry.end();
}
