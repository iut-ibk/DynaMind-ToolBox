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

#include <simulationtab.h>
#include <QGraphicsDropShadowEffect>
#include <qgraphicsview.h>
#include <modelnode.h>
#include <QGraphicsSceneDragDropEvent>
#include <qtreewidget.h>
#include <guisimulation.h>
#include <dmsimulation.h>
#include <dmgroup.h>
#include <QKeyEvent>
#include <qapplication.h>
#include <qclipboard.h>
#include <dmsimulationwriter.h>
#include <simulationio.h>

#include <dmlogger.h>
#include "ColorPalette.h"

SimulationTab::SimulationTab(QWidget* parent, GUISimulation *sim, DM::Group* parentGroup): 
	QGraphicsScene(parent), parentGroup(parentGroup)
{
	viewer = new QGraphicsView(this, parent);
	viewer->setRenderHints(QPainter::Antialiasing);
	viewer->setAcceptDrops(true);
	if (!this->parentGroup)
		viewer->setBackgroundBrush(QBrush(QColor(239,235,226)));

	this->sim = sim;
	hoveredGroupNode = NULL;

	connect(sim->getTabWidget(), SIGNAL(currentChanged(int)), this, SLOT(clearSelection()));

	connect(this, SIGNAL(selectionChanged()), this, SLOT(enhanceSelection()));
}

SimulationTab::~SimulationTab()
{
}

void SimulationTab::enhanceSelection()
{
	foreach(QGraphicsItem* item, selectedItems())
		if(ModelNode* n = (ModelNode*)item)
		{
			DM::Module* group = n->getModule();
			if(group->isGroup())
				foreach(DM::Module* m, sim->getModules())
				if(m->getOwner() == group)
					sim->getModelNode(m)->setSelected(true);
		}
}

void SimulationTab::mousePressEvent(QGraphicsSceneMouseEvent *event) 
{
	if(!itemAt(event->scenePos()))
		foreach(SimulationTab* t, sim->getTabs())
		t->clearSelection();

	if(event->buttons() == Qt::LeftButton)
		viewer->setDragMode(QGraphicsView::ScrollHandDrag);
	else if(event->buttons() == Qt::RightButton)
		viewer->setDragMode(QGraphicsView::RubberBandDrag);

	QGraphicsScene::mousePressEvent(event);
}

void SimulationTab::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) 
{	
	if(hoveredGroupNode && selectedItems().size() > 0)
	{
		int i = 0;
		foreach(SimulationTab* tab, sim->getTabs())
		{
			if(tab->getParentGroup() == hoveredGroupNode->getModule())
			{
				copySelection();
				deleteSelection();
				sim->selectTab(i);
				tab->pasteSelection(QPointF(0,0));

				hoveredGroupNode->setHovered(false);
				hoveredGroupNode = NULL;
				break;
			}
			i++;
		}
	}

	viewer->setDragMode(QGraphicsView::NoDrag);
	QGraphicsScene::mouseReleaseEvent(event);
}

void SimulationTab::keyPressEvent(QKeyEvent * keyEvent )
{
	if(keyEvent->key() == Qt::Key_Delete)
		deleteSelection();
	else if (keyEvent->matches(QKeySequence::Copy))
		copySelection();
	else if (keyEvent->matches(QKeySequence::Paste))
		pasteSelection(cursorPos);
	else if (keyEvent->matches(QKeySequence::Cut))
	{
		copySelection();
		deleteSelection();
	}
}

void SimulationTab::copySelection()
{
	std::list<DM::Module*> modules;
	std::set<DM::Module*> moduleSet;
	std::list<DM::Simulation::Link*> links;

	foreach(SimulationTab* t, sim->getTabs())
	{
		foreach(QGraphicsItem* item, t->selectedItems())
			if(ModelNode* node = (ModelNode*)item)
			{
				modules.push_back(node->getModule());
				moduleSet.insert(node->getModule());
			}
	}

	foreach(DM::Simulation::Link* l, sim->getLinks())
	{
		if(moduleSet.find(l->src) != moduleSet.end() &&
			moduleSet.find(l->dest) != moduleSet.end())
			links.push_back(l);
	}

	QByteArray data;
	QBuffer buffer(&data);
	DM::SimulationWriter::writeSimulation(&buffer, sim->currentDocument, modules, links, parentGroup);
	sim->appendGuiInformation(&buffer, modules);
	QApplication::clipboard()->setText(QString(data));
}

void SimulationTab::pasteSelection(const QPointF& pos)
{
	QByteArray data = QApplication::clipboard()->text().toUtf8();
	QBuffer buffer(&data);
	importSimulation(&buffer, cursorPos, "<clipboard>");
}

void SimulationTab::deleteSelection()
{
	foreach(QGraphicsItem* item, selectedItems())
		if(ModelNode* n = (ModelNode*)item)
			n->deleteModelNode();
}

void SimulationTab::importSimulation(QIODevice* source, const QPointF& target, const QString& filePath)
{
	clearSelection();

	float minx = 0;
	float miny = 0;

	std::map<std::string, DM::Module*> modMap;
	bool success = ((DM::Simulation*)sim)->loadSimulation(source,  filePath, modMap, parentGroup, true);

	GuiSimulationReader simio(source);
	std::map<QString, ModuleExEntry> moduleExInfo = simio.getEntries();


	for(std::map<QString, ModuleExEntry>::iterator it = moduleExInfo.begin();
		it != moduleExInfo.end(); ++it)
	{
		DM::Module* m = NULL;
		map_contains(&modMap, it->first.toStdString(), m);
		if(m != NULL && m->getOwner() == NULL)
		{
			minx = min(minx, (float)it->second.posX);
			miny = min(miny, (float)it->second.posY);
		}
	}

	minx -= target.x();
	miny -= target.y();

	for(std::map<QString, ModuleExEntry>::iterator it = moduleExInfo.begin();
		it != moduleExInfo.end(); ++it)
	{
		DM::Module* m = NULL;
		if(map_contains(&modMap, it->first.toStdString(), m) && m)
		{
			ModelNode* node = sim->getModelNode(m);
			if(m->getOwner() == NULL)
				node->setPos(QPointF(it->second.posX-minx, it->second.posY-miny));
			else
				node->setPos(QPointF(it->second.posX, it->second.posY));

			node->setSelected(true);
		}
	}

	sim->getTabWidget()->setCurrentWidget(this->getQGViewer());
}

void SimulationTab::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	cursorPos = event->scenePos();
	if(hoveredGroupNode)
	{
		hoveredGroupNode->setHovered(false);
		hoveredGroupNode = NULL;
	}

	if(event->buttons() == Qt::LeftButton)
		if(ModelNode* movingNode = dynamic_cast<ModelNode*>(itemAt(cursorPos)))
			foreach(QGraphicsItem* it, items(cursorPos))
		{
			ModelNode* node = dynamic_cast<ModelNode*>(it);
			if(node && !node->isGroup() && node != movingNode && node->getModule()->isGroup())
			{
				node->setHovered(true);
				hoveredGroupNode = node;
				break;
			}
		}

		QGraphicsScene::mouseMoveEvent(event);
}

void SimulationTab::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	event->accept();
}

void SimulationTab::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	DM::Group* parent = this->parentGroup;
	ModelNode* node = dynamic_cast<ModelNode*>(this->itemAt(event->scenePos()));
	if(node)
	{
		if(node->getModule()->isGroup())
			parent = (DM::Group*)node->getModule();
		else
			return;
	}

	DM::Logger(DM::Debug) << "tab drop";

	event->accept();

	QTreeWidget *moduleTree = dynamic_cast<QTreeWidget*>(event->source());

	if(!moduleTree || !moduleTree->currentItem())
		return;

	QTreeWidgetItem* item = moduleTree->currentItem();
	QString type = item->text(1);
	QString moduleName = item->text(0);

	if(type == "Module")
	{
		if(DM::Module* m = sim->addModule(moduleName.toStdString(), parent))
			sim->getModelNode(m)->setPos(event->scenePos());	// move to cursor
	}
	else if(type == "Simulation")
	{
		moduleName = item->parent()->text(0) + '\\' + moduleName;

		QFile file(moduleName);
		if (!file.exists())
			DM::Logger(DM::Error) << "file " << moduleName << " does not exist";
		else
			importSimulation(&file, event->scenePos(), moduleName);
	}
}

void SimulationTab::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	event->accept();

	float s = (event->delta()>0?1:-1) * 0.1f;
	viewer->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	viewer->scale(1+s,1+s);
}
