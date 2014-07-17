/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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


#ifndef DYNAMICINOUT_H
#define DYNAMICINOUT_H

#include "dmcompilersettings.h"
#include "dmmodule.h"

using namespace DM;

/** 
 * @brief Add user defined attribute to Inlets
 *
 * Data Set:
 * - Inport:
 *      - Read
 *          - Inlets|NODE: A, B, C
 *      - Add
 *          - Inlets|NODE: User Defined
 *
 */
class DM_HELPER_DLL_EXPORT DynamicInOut : public  Module
{
    DM_DECLARE_NODE(DynamicInOut)

   private:

        std::vector<std::string> NewAttributes;
        bool attributeChanged;
        DM::System * sys_in;
        int PrevSize;
public:
    DynamicInOut();


    /** @brief Here the dynamic is added **/
    void init();

    /** @brief Does nothing **/
    void run();

    /** @brief add a new attribute during runtime of the model */
    void addAttribute(std::string name);





};

#endif // DYNAMICINOUT_H
