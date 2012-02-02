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

#ifndef GROUPTEST_H
#define GROUPTEST_H

#include "compilersettings.h"
#include "group.h"
#include "DM.h"
#include "DMview.h"
#include <iostream>
#include <vector>
using namespace DM;
class VIBE_HELPER_DLL_EXPORT GroupTest : public  Group {
VIBe_DECLARE_GROUP(GroupTest)

    public:
        GroupTest();
        virtual ~GroupTest(){}
        void run();
        void init();
        std::vector<DM::View> InViews;
        std::vector<DM::View> OutViews;
        int Runs;
        int i;

        void addOutView (std::string view);
};


#endif // GROUPTEST_H
