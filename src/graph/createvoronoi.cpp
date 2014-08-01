/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#include <createvoronoi.h>
#include <examinationroomviewdef.h>
#include <tbvectordata.h>
#include <voronoi.h>

DM_DECLARE_NODE_NAME(CreateVoronoiDiagram,Graph)

CreateVoronoiDiagram::CreateVoronoiDiagram()
{
    //Define model input
    DM::ER::ViewDefinitionHelper defhelper_er;
    std::vector<DM::View> views;

    //Possible edges and nodes
    vc = DM::View("VORONIO_CELL",DM::FACE,DM::WRITE);
	vc.addAttribute("area",DM::Attribute::DOUBLE,DM::WRITE);
    views.push_back(vc);

    DM::View vn = defhelper_graph.getView(DM::GRAPH::NODES,DM::WRITE);
	vn.addAttribute("VoronoiCell",DM::Attribute::STRING,DM::WRITE);
    views.push_back(vn);

    //examination room
    views.push_back(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

    this->addData("Layout", views);
}

void CreateVoronoiDiagram::run()
{
    //Define vars
    DM::ER::ViewDefinitionHelper defhelper_er;

    //Get System information
    this->sys = this->getData("Layout");
	std::vector<DM::Component*> nm = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::NODES,DM::READ));
	std::vector<DM::Component*> fm = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

    if(nm.size()==0)
    {
        DM::Logger(DM::Warning) << "No nodes found for voronoi diagram";
        return;
    }

    if(fm.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one face is allowed";
        return;
    }

    Voronoi::createVoronoi(nm,fm[0],this->sys,vc);
}
