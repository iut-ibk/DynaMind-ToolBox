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
#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include "dmcompilersettings.h"
#include <QGraphicsView>
#include <QWidget>


#include <QGraphicsSceneDragDropEvent>
#include <linknode.h>

#include <dmmoduleregistry.h>
#include <dmmodule.h>
#include <mainwindow.h>
#include <dmsimulation.h>
class ModelNode;
class GroupNode;
class RootGroupNode;

class  DM_HELPER_DLL_EXPORT ProjectViewer :public QGraphicsScene
{
    Q_OBJECT
public:
    int id;
    ProjectViewer(  GroupNode * g, QWidget * parent = 0);
    void setResultViewer(MainWindow * w){this->ResultViewer = w;}
    //QVector<ModelNode* > getModelNodes () {return this->mnodes;}
    //QVector<GroupNode* > getGroupNodes () {return this->gnodes;}

    RootGroupNode * getRootNode(){return this->rootGroup;}

    ~ProjectViewer(){}


protected:
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);



private:
    QVector<ModelNode* >  mnodes;
    QVector<GroupNode* >  gnodes;



    DM::ModuleRegistry * moduleregistry;
    //GUISimulation * simulation;
    MainWindow * ResultViewer;

    RootGroupNode * rootGroup;

signals:
    void NewGroupAdded(GroupNode  *);
    void NewModule(QString name, QPointF pos, DM::Module* group);

public slots:
    void addModule(ModelNode *m);
    void addGroup(GroupNode* m);

};

#endif // PROJECTVIEWER_H
