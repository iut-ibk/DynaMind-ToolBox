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

#ifndef __csg_s_rule
#define __csg_s_rule

#include "dmcompilersettings.h"
//
// C++ Implementation: csg_s_rule
//
// Description: 
//
//
// Author: Christian Urich <christian.urich@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dataLayer.h"
#include "csg_s_neighbourhood.h"
#include <iostream>
#include "csg_s_position.h"

//class dataLayer;

namespace csg_s
{

class DM_HELPER_DLL_EXPORT rule
{

    friend class DM::dataLayer;

public:
    //void setRuleLayer(dataLayer&);
    rule(DM::dataLayer *layer_);
    ~rule();
    virtual double run(Position* pos) = 0;
    void setRuleLayer(DM::dataLayer *l) {layer_=l;};



protected:
    void applySwitch_1();
    void applySwitch_2();
    DM::dataLayer *layer_;
    neighbourhood neighbours;
    double *nMoore3x3;

};
}
#endif
