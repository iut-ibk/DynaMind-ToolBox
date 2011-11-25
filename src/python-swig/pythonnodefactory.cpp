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
#include "pythonnodefactory.h"
#include <Python.h>
#include "module.h"
#include "pymodule.h"
#include "pythonenv.h"
#include <vibe_logger.h>

using namespace boost::python;
using namespace std;
namespace vibens {
    struct PNodeFactoryPriv {
        object klass;
        std::string name;
        std::string filename;

    };

    PythonNodeFactory::PythonNodeFactory(object klass, std::string filename) {
        try {
            priv = new PNodeFactoryPriv();
            priv->klass = klass;
            priv->name = extract<std::string>(priv->klass.attr("__name__"));
           // priv->filename = extract<std::string>(priv->klass.attr("__module__"));
            priv->filename = filename;
        } catch(error_already_set const &) {
            Logger(Error)  << __FILE__ << ":" << __LINE__ ;
            handle_python_exception();
        }

    }

    PythonNodeFactory::~PythonNodeFactory() {
        delete priv;
    }

    Module *PythonNodeFactory::createNode() const {
        ScopedGILRelease scoped;
        try {
            object node = priv->klass();
            ModuleWrapper * apn = extract<ModuleWrapper * >(node);
            apn->setClassName(priv->name);
            apn->setFileName(priv->filename);
            apn->setSelf(node);
            apn->setPythonModule(true);
            return apn;
        } catch(error_already_set const &) {
           Logger(Error)  << __FILE__ << ":" << __LINE__ ;
            handle_python_exception();
        }
        return 0;
    }

    std::string PythonNodeFactory::getNodeName() {
        return priv->name;
    }

    std::string PythonNodeFactory::getFileName() {
        return priv->filename;
    }
}
