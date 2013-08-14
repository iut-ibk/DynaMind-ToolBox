/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013 Markus Sengthaler

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

#include "modelobserver.h"
#include "simulationtab.h"
#include "modelnode.h"
#include "guisimulation.h"
#include <QTableWidget>
#include "guiport.h"

void ModelObserver::notifyAddPort(const std::string &name, const DM::PortType type)
{
	node->addPort(name, type);
}
void ModelObserver::notifyRemovePort(const std::string &name, const DM::PortType type)
{
	node->removePort(name, type);
}
void ModelObserver::notifyChangeName(const std::string &name)
{
	if(node->isGroup())
	{
		int i = 0;
		QList<SimulationTab*> tabs = node->getSimulation()->getTabs();
		foreach(SimulationTab* tab, tabs)
		{
			if(((DM::Module*)tab->getParentGroup()) == node->getModule())
				break;
			else
				i++;
		}
		if(i < tabs.size())
			node->getSimulation()->getTabWidget()->setTabText(i, QString::fromStdString(name));
	}
}

void ModelObserver::notifyStateChange()
{
	node->update();
	foreach(PortNode* port, node->getPorts(DM::INPORT))
		port->update();
	foreach(PortNode* port, node->getPorts(DM::OUTPORT))
		port->update();
}