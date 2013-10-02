/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Urich

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

#include "offsetface.h"
#include "cgalgeometry.h"

DM_DECLARE_NODE_NAME(OffsetFace, Geometry)

OffsetFace::OffsetFace()
{
	nameInputView = "";
	nameOutputView = "";
	offest = 0;

	this->addParameter("nameInputView", DM::STRING, &this->nameInputView);
	this->addParameter("nameOutputView", DM::STRING, &this->nameOutputView);

	this->addParameter("offset", DM::DOUBLE, &this->offest);
}

void OffsetFace::init()
{
	if (nameInputView.empty())
			return;
	if (nameOutputView.empty())
			return;

	this->inputView = DM::View(nameInputView, DM::FACE, DM::READ);
	this->outputView = DM::View(nameOutputView, DM::FACE, DM::WRITE);

	std::vector<DM::View> datastream;

	datastream.push_back(inputView);
	datastream.push_back(outputView);

	this->addData("sys", datastream);

}

DM::Face *OffsetFace::createOffest(DM::System * sys, DM::Face *f, double offset)
{
	std::vector<DM::Node> nodes = DM::CGALGeometry::OffsetPolygon(f->getNodePointers(), offset);

	std::vector<DM::Node*> face_nodes;
	foreach (DM::Node n, nodes) {
		DM::Node * np = sys->addNode(n);
		if (!np) {
			return NULL;
		}
		face_nodes.push_back(np);
	}
	if (face_nodes.size() < 3) {
		DM::Logger(DM::Warning) << "offest failed";
		return NULL;
	}

	return sys->addFace(face_nodes);
}


void OffsetFace::run()
{
	DM::System * sys = this->getData("sys");

	foreach(DM::Component * cmp, sys->getAllComponentsInView(inputView))
	{
		DM::Face * f = dynamic_cast<DM::Face*>(cmp);
		DM::Face * f_off = this->createOffest(sys, f, this->offest);
		if (!f_off)
			continue;
		sys->addComponentToView(f_off, this->outputView);
	}
}


string OffsetFace::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/OffsetFace.md";
}

std::string OffsetFace::getNameInputView() const
{
	return nameInputView;
}

void OffsetFace::setNameInputView(const std::string &value)
{
	nameInputView = value;
}

std::string OffsetFace::getNameOutPutView() const
{
	return nameOutputView;
}

void OffsetFace::setNameOutPutView(const std::string &value)
{
	nameOutputView = value;
}

double OffsetFace::getOffest() const
{
	return offest;
}

void OffsetFace::setOffest(double value)
{
	offest = value;
}
