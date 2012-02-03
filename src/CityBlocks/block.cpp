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

#include "block.h"
#include <DM.h>
#include <DMview.h>
//Creates a Block with 1 per 1 meter


DM_DECLARE_NODE_NAME( Block,BlockCity )
Block::Block()
{
    std::vector<DM::View> views;
    DM::View block = DM::View("BLOCK", DM::FACE, DM::WRITE);
    views.push_back(block);

    this->addData("City", views);

}
void Block::run() {

    DM::System * blocks = this->getData("City");




    DM::Node * n1 = blocks->addNode(0,0,0);
    DM::Node * n2 = blocks->addNode(1000,0,0);
    DM::Node * n3 = blocks->addNode(1000,1000,0);
    DM::Node * n4 = blocks->addNode(0,1000,0);

    DM::Edge * e1 = blocks->addEdge(n1, n2);
    DM::Edge * e2 = blocks->addEdge(n2, n3);
    DM::Edge * e3 = blocks->addEdge(n3, n4);
    DM::Edge * e4 = blocks->addEdge(n4, n1);

    std::vector<DM::Edge*> ve;
    ve.push_back(e1);
    ve.push_back(e2);
    ve.push_back(e3);
    ve.push_back(e4);


    DM::Face * f = blocks->addFace(ve, "BLOCK");


}
