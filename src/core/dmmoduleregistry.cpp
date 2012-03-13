/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#include <boost/foreach.hpp>
#include <dmmodule.h>
using namespace std;
namespace DM {
    ModuleRegistry::ModuleRegistry() {
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

void ModuleRegistry::addNativePlugin(const std::string &plugin_path) {

    QLibrary l(QString::fromStdString(plugin_path));
    bool loaded = l.load();
    if (!loaded)
        std::cout << "Error: Module " << plugin_path << " not loaded" << std::endl;
    assert(loaded);
    regNodeFunProto regNodeFun = (regNodeFunProto) l.resolve("registerModules");

    if (regNodeFun) {
        regNodeFun(this);
    } else {
        Logger(Debug) << plugin_path << " has no node register hook";
    }
}

typedef std::pair<std::string, INodeFactory *> snf;
std::list<std::string> ModuleRegistry::getRegisteredModules() const {
    std::list<std::string> names;

    BOOST_FOREACH(snf item, registry) {
        names.push_back(item.first);
    }

    return names;
}

Module *ModuleRegistry::createModule(const std::string &name) const {
     Logger(Debug)  << "Try to create " << name;
    if (registry.count(name) == 0) {
        Logger(Error) << "Error: no such node class registered";
        QThread::currentThread()->exit();
    }

    Module *newmod = 0;

    try
    {
        newmod = registry.find(name)->second->createNode();
    }
    catch(...)
    {
        Logger(Error) << "Cannot create Module";
        return 0;
    }

    return newmod;
}

bool ModuleRegistry::contains(const std::string &name) const {
    return registry.find(name) != registry.end();
}
}
