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

#ifndef MODELNODE_H
#define MODELNODE_H

#include "dmcompilersettings.h"
#include "moduledescription.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QGraphicsSimpleTextItem>
#include <QPainter>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QVector>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <guiportobserver.h>
#include <QWidget>

class ModelNodeButton;
class RootGroupNode;
class PortNode;
class GUISimulation;

#if defined _MSC_VER && defined _WIN32
enum PortType;
#else
enum PortType : unsigned int;
#endif

class ModelNode;

class GUIModelObserver: public DM::ModuleObserver
{
	ModelNode* node;
public:
	GUIModelObserver(ModelNode* node, DM::Module* module):
		node(node), ModuleObserver(module)
	{
		foreach(std::string portName, module->getInPortNames())
			notifyAddPort(portName, DM::INPORT);
		foreach(std::string portName, module->getOutPortNames())
			notifyAddPort(portName, DM::OUTPORT);
	};
	
	void notifyAddPort(const std::string &name, const DM::PortType type);
	void notifyRemovePort(const std::string &name, const DM::PortType type);
};

class  DM_HELPER_DLL_EXPORT ModelNode : public  QObject, public QGraphicsItem//, public DM::ModuleObserver
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    enum { Type = UserType + 1 };

private:
	ModelNode* child;
public:

	PortNode* getPort(std::string portName, const DM::PortType type);
	QVector<PortNode*>	getPorts(DM::PortType type);
	void setChild(ModelNode* child){this->child = child;}
	ModelNode* getChild(){return child;}
protected:
	int width, height;

    QVector<PortNode*> ports;

    int id;
    bool minimized;
	bool hovered;

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) ;
    virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

	virtual void hoverEnterEvent( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );
    GUISimulation * simulation;

	DM::Module* module;

public:
	bool setHovered(bool on){return hovered = on;};
    ModelNode(DM::Module* m, GUISimulation* sim);
	void setPos(const QPointF &pos);
    ~ModelNode();
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getID(){return this->id;}

    bool isMinimized(){return this->minimized;}

    virtual bool isGroup(){return false;}
	void resize();

    //View
    virtual void recalculateLandH(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual QRectF boundingRect() const;

    GUISimulation * getSimulation() {return this->simulation;}
	
	void addPort(const std::string &name, const DM::PortType type);
	void removePort(const std::string &name, const DM::PortType type);

    virtual void setSelected ( bool selected ){QGraphicsItem::setSelected ( selected );}

	DM::Module* getModule() const
	{
		return module;
	}

public slots:
    //void addGroup();
    //void removeGroup();
    void deleteModelNode();
    void editModelNode();
    //void renameModelNode();
    void printData();
    void viewData(int portIndex);
    void showHelp();
	//void showDialog();
    //void setDebug();
    //void setRelease();
    //void setResetModule();
	void changeSuccessorMode();
   signals:
    void showHelp(std::string, std::string);
    //void showHelp(std::string);

};

#endif // MODELNODE_H
