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
#ifndef TIMEAREAMETHOD_H
#define TIMEAREAMETHOD_H

#include <module.h>
#include <DM.h>


/*
* @ingroup Sewer
* @brief TimeAreaMethod
*
* @author Christian Urich
*/
class DM_HELPER_DLL_EXPORT TimeAreaMethod : public  DM::Module {

DM_DECLARE_NODE (TimeAreaMethod)

public:
    TimeAreaMethod();
void run();
private:
    DM::View conduit;
    DM::View inlet;
    DM::View shaft;
    DM::View endnodes;
    DM::View catchment;

    std::vector<DM::Node *> PointList;
    std::vector<DM::Edge *> EdgeList;

    std::vector<DM::Node*> EndPointList;

    double v;
    double r15;
    bool checkPoint(DM::Node *p);
    std::vector<DM::Edge *> findConnectedEdges(DM::System * city, DM::Node* ID);
    DM::Node* findDownStreamNode(DM::System *city, DM::Node * ID);
    double chooseDiameter(double diameter);
    double caluclateAPhi(DM::Component *  attr, double r15) const;





};

#endif // TIMEAREAMETHOD_H
