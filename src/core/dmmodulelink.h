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
#ifndef DMLINK_H
#define DMLINK_H
#include "dmcompilersettings.h"
#include <iostream>

namespace DM {
    class Port;
    /**
    * @ingroup DynaMind-Core
    *
    * @brief A ModuleLink is used to Link ports together.
    *
    * Out ports can be linked with several in ports. To support
    * loops (see Groups) module linkes can be marked as back link. A in port can have one module link and one
    * module link that is defined as back link. If the module is called the first time data
    * are fetched from the out port connected via the standard link. For the following runs from the ou port connected
    * via the back link
    */
    class DM_HELPER_DLL_EXPORT  ModuleLink
    {
    private:
        Port * InPort;
        Port * OutPort;
        bool backLink;
        ModuleLink * getPortFromTuplePort(const ModuleLink * origin, bool fromInportTuple) const;
        //bool isBackLinkFromOrigin;
    public:
        /** @brief Creats a new ModuleLink*/
        ModuleLink(Port * InPort, Port * OutPort,bool isBackPort = false);
        /** @brief Retruns pointer to in port*/
        Port * getInPort() const;
        /** @brief Retruns pointer to out port*/
        Port * getOutPort() const;
        /** @brief Retruns true if module link is back link*/
        bool isBackLink();
        /** @brief Retruns true there is a back link in a link chain.
         *
         * Link chains are common if a module ports are connected with group ports. Since a module out
         * port is usally connected with a standard link to an group in port. If the group out port is now connected
         * with a module in port we have a link chain. If on element in the link chain is a back port
         * the mtehod returns true
         */
        bool isBackLinkInChain();
        /** @brief sets if module link is a back link */
        void setBackLink(bool b){this->backLink = b;}
        /** @brief Returns name of the data connected to the out port
         *
         * If port is tuple port, the link chain is followed to the origin */
        std::string getDataNameFromOutPort() const;
        /** @brief Returns module uuid connected to the out port
         *
         * If port is tuple port, the link chain is followed to the origin */
        std::string getUuidFromOutPort();
        /** @brief Sets in port */
        void setInPort(Port *);
        /** @brief Sets out port */
        void setOutPort(Port *);
        ~ModuleLink();

    };
}
#endif // LINK_H
