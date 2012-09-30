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

#ifndef INOUT2MODULE_H
#define INOUT2MODULE_H

#include "dmcompilersettings.h"
#include "dmmodule.h"
#include <dm.h>
using namespace DM;

/** @ingroup TestModules
 * @brief The Module addes the Attributes F to Inlets and F to Conduits
 *
 * Data Set:
 * - Inport:
 *      - Read
 *          - Inlets|NODE: A, B, C, D
 *          - Conduits|EDGE
 *      - Add
 *          - Inlets|NODE: F
 *          - Conduits|EDGE: F
 *
 */
class DM_HELPER_DLL_EXPORT InOut2 : public  Module {

DM_DECLARE_NODE( InOut2 )

    private:
         DM::View inlets;
         DM::View conduits;
         DM::View streets;

    public:
        DM::System * sys_in;
        double a;


        InOut2();
          /** @brief Adds to nodes to the Inlets (0,0,2) and (0,0,3) */
        void run();
        virtual ~InOut2();
};


#endif // INOUT2MODULE_H
