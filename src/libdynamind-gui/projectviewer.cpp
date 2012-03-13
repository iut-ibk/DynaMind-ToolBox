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
#include "projectviewer.h"
#include <QDropEvent>
#include <iostream>
#include <QListWidget>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <boost/foreach.hpp>
#include <dmmoduleregistry.h>
#include <modelnode.h>
#include <groupnode.h>
#include <rootgroupnode.h>
#include <sstream>
#include <guisimulation.h>
using namespace boost;
ProjectViewer::ProjectViewer( GroupNode *g,  QWidget *parent) : QGraphicsScene(parent)

{

    this->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->id = 0;
    this->mnodes;

    //Cretae New Visual Representation

    RootGroupNode * rg = new RootGroupNode(g->getVIBeModel(),g->getSimulation());
    rg->setPos(0,0);
    this->rootGroup = rg;
    this->addItem(rg);


}

void ProjectViewer::addModule(ModelNode *m)
{

    //Only add if root group is the same
    if (m->getVIBeModel()->getGroup() != this->rootGroup->getVIBeModel()) {
        return;
    }
    this->rootGroup->addModelNode(m);
    this->addItem(m);
}

void ProjectViewer::addGroup(GroupNode *g) {
    //Only add if root group is the same
    if (g->getVIBeModel()->getGroup() != this->rootGroup->getVIBeModel()) {
        return;
    }
    this->rootGroup->addModelNode(g);
    this->addItem(g);

}

void ProjectViewer::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();

}

void ProjectViewer::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
    std::stringstream ss;
    QTreeWidget * lw = (QTreeWidget*) event->source();
    //lw->currentItem()->text()
    QString classname =  lw->currentItem()->text(0);
    std::string type = lw->currentItem()->text(1).toStdString();
    if (type.compare("Module") == 0) {
        //ss << module << "_" << this->id++;
        emit NewModule(classname, event->scenePos(), this->rootGroup->getVIBeModel());


    } else {
        this->ResultViewer->importSimulation( lw->currentItem()->text(2), event->scenePos());
    }

}

