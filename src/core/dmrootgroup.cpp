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
#include "dmrootgroup.h"
#include <QThreadPool>
namespace DM {
QThreadPool * DMRootGroup::pool = NULL;

DMRootGroup::DMRootGroup()
{
    this->group = 0;
    this->Steps = 1;

}
void DMRootGroup::run() {
    Group::run();
    DMRootGroup::getThreadPool()->waitForDone();


    Logger(Debug)<<"Finished RootGroup";
}

DMRootGroup::~DMRootGroup() {
    std::cout << "Call Dectructor RootGroup" << std::endl;
}
void DMRootGroup::showstats() {

    Logger(Debug) <<  "POOL TIMEOUT: " << pool->expiryTimeout();
    Logger(Debug) <<  "POOL THREADCOUNT: "  << pool->maxThreadCount() ;
    Logger(Debug) <<  "POOL ACTIVE THREADCOUNT: "  << pool->activeThreadCount() ;
}

QThreadPool * DMRootGroup::getThreadPool() {
    if(pool == NULL){
        pool = new QThreadPool();

    }
    return pool;
}
}
