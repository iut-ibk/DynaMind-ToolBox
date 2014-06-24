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
#include <QWidget>

class ModelNodeButton;
class RootGroupNode;
class PortNode;
class GUISimulation;
class GUIModelNode;

class ModelNode;

class  DM_HELPER_DLL_EXPORT ModelNode : public  QObject, public QGraphicsItem//, public DM::ModuleObserver
{
	Q_OBJECT
		Q_INTERFACES(QGraphicsItem)


public:
	enum { Type = UserType + 1 };
	ModelNode(DM::Module* m, GUISimulation* sim);
	~ModelNode();

	virtual void setSelected ( bool selected ){QGraphicsItem::setSelected ( selected );}
	virtual void recalculateLandH(){}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	virtual QRectF boundingRect() const;
	virtual bool isGroup(){return false;}

	void addPort(const std::string &name, const DM::PortType type);
	void removePort(const std::string &name, const DM::PortType type);

	PortNode*			getPort(std::string portName, const DM::PortType type);
	QVector<PortNode*>	getPorts(DM::PortType type);

	void		setChild(ModelNode* child){this->child = child;}
	ModelNode*	getChild(){return child;}

	GUISimulation * getSimulation() {return this->simulation;}
	DM::Module*		getModule() const {return module;}

	bool setHovered(bool on){return hovered = on;};
	void setPos(const QPointF &pos);

	void resize();
public slots:
	void deleteModelNode();
	void editModelNode();
	void editName();
	void printData();
	void viewOutportData(QString portName);
	void viewInportData(QString portName);
	void showHelp();
	void changeSuccessorMode();
	void updateSimulation();
signals:
	void showHelp(std::string, std::string);

protected:
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) ;
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void hoverEnterEvent( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );

	int width;
	int height;
	bool hovered;
	QVector<PortNode*>	ports;
	GUISimulation*		simulation;
	DM::Module*			module;
	GUIModelNode *ng;
private:
	ModelNode* child;
};

#endif // MODELNODE_H
