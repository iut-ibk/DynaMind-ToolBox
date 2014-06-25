/**
 * @file
 * @author Michael Mair <michael.mair@uibk.ac.at>
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
#include "createnewview.h"
#include "guicreatenewview.h"
#include <algorithm>


DM_DECLARE_NODE_NAME(CreateNewView, Modules)
CreateNewView::CreateNewView()
{
	this->forceRefreshSimulation(true);
	sys_in = NULL;
	this->viewType=0;
	this->modifyExistingView=true;
	this->startNewStream=false;
	data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addData("Data", data);
	this->addParameter("NameOfView", DM::STRING, &this->NameOfView);
	this->addParameter("ViewType", DM::INT,&this->viewType);
	this->addParameter("newAttributes", DM::STRING_MAP, &this->newAttributes);
	this->addParameter("ModifyExistingView",DM::BOOL,&this->modifyExistingView);
	this->addParameter("StartNewStream",DM::BOOL,&this->startNewStream);
}

void CreateNewView::run()
{
	this->getData("Data");
}

void CreateNewView::init()
{
	DM::ACCESS a;

	if(NameOfView=="")
		return;

	if(!this->modifyExistingView)
		a=DM::WRITE;
	else
		a=DM::MODIFY;

	if(startNewStream)
	{
		modifyExistingView=false;
		a=DM::WRITE;
	}

	DM::View view(this->NameOfView,this->viewType,a);

	std::map<std::string,std::string>::iterator it = newAttributes.begin();

	for(it;it!=newAttributes.end();it++)
		view.addAttribute((*it).first,(DM::Attribute::AttributeType)atoi(((*it).second).c_str()),a);

	data.clear();
	if(!startNewStream)
		data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	data.push_back(view);
	this->addData("Data", data);
}

bool CreateNewView::createInputDialog() {
	QWidget * w = new GUICreateNewView(this);
	w->show();
	return true;
}

string CreateNewView::getHelpUrl()
{
	return "";
}
