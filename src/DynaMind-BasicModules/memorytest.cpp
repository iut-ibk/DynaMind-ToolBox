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

#include "memorytest.h"
#include <dm.h>
DM_DECLARE_NODE_NAME( MemoryTest,Modules)

MemoryTest::MemoryTest()
{
    DM::View v("STREET", DM::EDGE, DM::WRITE);
    std::vector<DM::View> views;
    views.push_back(v);
    this->addData("V", views);
}

void MemoryTest::run() {
    DM::System * city = this->getData("V");
    for (int i = 0; i < 500000; i++) {
        city->addNode(i,i,i);
    }
}


