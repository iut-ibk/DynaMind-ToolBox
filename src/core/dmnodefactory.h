/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich and Michael Mair

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

#ifndef DMNODEFACTORY_H
#define DMNODEFACTORY_H

#include "dmcompilersettings.h"
#include <string>

namespace DM {

    class Module;

    class DM_HELPER_DLL_EXPORT INodeFactory {
    public:
        virtual ~INodeFactory(){}
        virtual Module *createNode() const = 0;
        virtual std::string getNodeName() const = 0;
        virtual std::string getFileName() const = 0;
    };

    template <typename T>
            class DM_HELPER_DLL_EXPORT NodeFactory
                : public INodeFactory {
                public:
        NodeFactory();
        virtual Module *createNode() const;
        virtual std::string getNodeName() const;
        virtual std::string getFileName() const;
    };

    template <typename T>
            NodeFactory<T>::NodeFactory() {           
    }

    template <typename T>
            Module *NodeFactory<T>::createNode() const {
        return new T();
    }

    template <typename T>
            std::string NodeFactory<T>::getNodeName() const {
        return T::classname;
    }
    template <typename T>
            std::string NodeFactory<T>::getFileName() const {
        return T::filename;
    }
}
#endif // NODEFACTORY_H
