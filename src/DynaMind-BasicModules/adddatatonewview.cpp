/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author Michael Mair <michael.mair@gmail.com>
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
#include "adddatatonewview.h"
#include "guiadddatatonewview.h"
#include <algorithm>


DM_DECLARE_NODE_NAME(AddDataToNewView, Modules)
AddDataToNewView::AddDataToNewView()
{
    sys_in = NULL;
    this->NameOfNewView = "";
    this->onlySelected = false;

    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addData("Data", data);

    this->addParameter("NameOfNewView", DM::STRING, &this->NameOfNewView);
    this->addParameter("NameOfExistingView", DM::STRING, &this->NameOfExistingView);
    this->addParameter("newAttributes", DM::STRING_LIST, &this->newAttributes);
    this->addParameter("onlySelected", DM::BOOL, &this->onlySelected);
}

void AddDataToNewView::run()
{
	DM::System * sys = this->getData("Data");
	DM::View * v_existing= sys->getViewDefinition(NameOfExistingView);
	if (!v_existing) 
	{
		DM::Logger(DM::Error) << "input view '" << NameOfExistingView << "' does not exist";
		return;
	}
	DM::View * v_new= sys->getViewDefinition(NameOfNewView);
	if(!v_new)
	{
		DM::Logger(DM::Error) << "output view '" << NameOfNewView << "' does not exist (AddDataToNewView::init went wrong)";
		return;
	}

	if(this->onlySelected)
	{
		DM::ComponentMap cmp = sys->getAllComponentsInView(*v_existing);
		for (DM::ComponentMap::const_iterator it = cmp.begin(); it != cmp.end(); ++it) 
		{
			DM::Component * c = sys->getComponent(it->first);
			if(DM::Attribute* a = c->getAttribute("selected"))
				if(a->getDouble() < 0.0001)
					sys->addComponentToView(c, *v_new);
		}
	}
}

void AddDataToNewView::init()
{
    // TODO: Works fine until someone is changing something upstream -> no update downstream!
    sys_in = this->getData("Data");
    if (!sys_in || this->NameOfExistingView.empty() || this->NameOfNewView.empty())
        return;

	// debug output
    std::vector<std::string> views = sys_in->getNamesOfViews();
    foreach (std::string s, views)
        DM::Logger(DM::Debug) << s;

    DM::View* inView = sys_in->getViewDefinition(NameOfExistingView);
    if (!inView) 
	{
        DM::Logger(DM::Warning) << "View does not exist " << NameOfExistingView << this->getName() << this->getUuid();
        return;
    }

    readView = DM::View(inView->getName(), inView->getType(), DM::READ);
    bool modify = false;
    bool changed = false;
    if (NameOfNewView == NameOfExistingView)
        modify = true;

    if (NameOfNewView != NameOfNewView_old)
	{
        changed = true;
        NameOfNewView_old = NameOfNewView;
        writeView = DM::View(getParameterAsString("NameOfNewView"), readView.getType(), DM::WRITE);
    }

    // Get Attributes from existing View
    if(sys_in->getComponent(inView->getIdOfDummyComponent()) == NULL)
        return;

    DM::AttributeMap attributes = sys_in->getComponent(inView->getIdOfDummyComponent())->getAllAttributes();

    for (DM::AttributeMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
        writeView.addAttribute(it->first);

    foreach (std::string s, getParameter<std::vector<std::string> >("newAttributes")) 
	{
        std::vector<std::string>  writes_already = writeView.getWriteAttributes();
        if (std::find(writes_already.begin(), writes_already.end(), s) != writes_already.end())
		{
			writeView.addAttribute(s);
			changed = true;
		}
    }

    if (changed == true) 
	{
        std::vector<DM::View> data;
        if(modify)
            this->writeView.setAccessType(DM::MODIFY);
        else
            data.push_back(readView);

        data.push_back(writeView);
        this->addData("Data", data);
    }
}

bool AddDataToNewView::createInputDialog() {
    QWidget * w = new GUIAddDatatoNewView(this);
    w->show();
    return true;
}

DM::System * AddDataToNewView::getSystemIn() {
    return this->sys_in;
}

void AddDataToNewView::addView()
{
}

void AddDataToNewView::addAttribute(string s) {
    this->newAttributes.push_back(s);
}

string AddDataToNewView::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-ToolBox/wiki/AddDataToNewView";
}
