/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
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

#ifndef GROUPNODE_H
#define GROUPNODE_H

#include <modelnode.h>

class SimulationTab;

class DM_HELPER_DLL_EXPORT  GroupNode: public ModelNode
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	void setParent(ModelNode* parent){this->parent = parent;};
	void resize();

	GroupNode(DM::Module* module, GUISimulation* sim, SimulationTab* tab, ModelNode* parent);
	~GroupNode();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

	bool isGroup(){return true;}
private:
	ModelNode* parent;
	SimulationTab* owningTab;
};

#endif // GROUPNODE_H
