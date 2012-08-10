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

#include "spatiallinking.h"

DM_DECLARE_NODE_NAME(SpatialLinking, Modules)

SpatialLinking::SpatialLinking()
{
    this->city = 0;
    this->sInView1 = "";
    this->sInView2 = "";

    this->addParameter("Base", DM::STRING, & this->sInView1);
    this->addParameter("Link", DM::STRING, & this->sInView2);

    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addData("Data", data);

}

void SpatialLinking::init() {
    city = this->getData("Data");
    if (city == 0)
        return;
    if (sInView1.empty())
        return;
    if (sInView2.empty())
        return;

    DM::View * v1 = this->city->getViewDefinition(sInView1);
    DM::View * v2 = this->city->getViewDefinition(sInView2);

    this->inView1 = DM::View(v1->getName(), v1->getType(), READ);
    this->inView2 = DM::View(v2->getName(), v2->getType(), READ);

    this->inView1.addLinks(sInView2, inView2);
    this->inView2.addLinks(sInView1, inView1);

    std::vector<DM::View> data;
    data.push_back(inView1);
    data.push_back(inView2);
    this->addData("Data", data);
}

void SpatialLinking::run() {

}
