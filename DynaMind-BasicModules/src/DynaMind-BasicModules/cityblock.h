/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich
 
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

#ifndef CREATEPARCELS_H
#define CREATEPARCELS_H

#include <dmmodule.h>
#include <dm.h>
#include <QHash>


class CityBlock : public DM::Module
{
	DM_DECLARE_NODE(CityBlock)
	public:
		CityBlock();
	void run();
	void init();
	std::string getHelpUrl();


private:
	double height;
	double width;
	double offset;
	bool createStreets;
	std::string SuperBlockName;
	std::string BlockName;
	std::string EdgeName;
	std::string CenterCityblockName;
	std::string InterSectionsName;

	DM::View superblock;
	DM::View intersections;
	DM::View cityblock;
	DM::View streets;
	DM::View centercityblock;

	std::map<DM::Node*, std::map<DM::Node*, DM::Edge* > > StartAndEndNodeList;
	double devider;
	DM::Edge * getAlreadyCreateEdge(DM::Node * n1, DM::Node* n2);
	void addEdge(DM::Edge * e, DM::Node *n1, DM::Node *n2);

};

#endif // CREATEPARCELS_H
