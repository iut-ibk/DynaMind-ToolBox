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
#include "rootmodule.h"
#include <iostream>

RootModule::RootModule() {

}

boost::shared_ptr<DM::Module> RootModule::clone() const {
    return boost::shared_ptr<Module>(new RootModule());
}

void RootModule::run() {
      DM::Logger(DM::Debug) << "Run Root";

}

const RasterData &RootModule::getRasterData(const std::string &name, int T) const {
    (void) name;
    (void) T;
    std::cerr << "RasterData " << name << " could not be found" << std::endl;
    throw 0;
    //assert(false);
}
/*const VectorData &RootModule::getVectorData(const std::string &name) const {
    (void) name;
    std::cerr << "VectorData " << name << " could not be found" << std::endl;
    throw 0;
    //assert(false);
}*/
