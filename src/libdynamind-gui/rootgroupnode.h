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
#ifndef ROOTGROUPNODE_H
#define ROOTGROUPNODE_H

#include <modelnode.h>
#include <moduledescription.h>
#include "compilersettings.h"
namespace DM {
    class Module;
    class PortTuple;
}

class LinkNode;
struct LinkNodeTuple;
struct GUIPortTuple;

class VIBE_HELPER_DLL_EXPORT  RootGroupNode : public ModelNode
{
    Q_OBJECT

private:
    QVector<GUIPortTuple * > OutputTuplePorts;
    QVector<GUIPortTuple * > InPortTuplePorts;

    ModuleDescription module;
    ModelNodeButton * minimizeButton;
    bool RePosFlag;
    std::string name;
    QVector<ModelNode * > childnodes;

protected:
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) ;

public:
    RootGroupNode();
    virtual ~RootGroupNode();
    void addTuplePort(DM::PortTuple * p);
    void removeTuplePort(int Type, QString s);
    RootGroupNode( DM::Module *module, GUISimulation * s);
     virtual GUIPort * getGUIPort(DM::Port * p);
    /*QVector<LinkNodeTuple * > getInputTupleRaster(){return this->InputTupleRaster;}
    QVector<LinkNodeTuple * > getOutputTupleRaster(){return this->OutputTupleRaster;}
    QVector<LinkNodeTuple * > getInputTupleVector(){return this->InputTupleVector;}
    QVector<LinkNodeTuple * > getOutputTupleVector(){return this->OutputTupleVector;}
    QVector<LinkNodeTuple * > getInputTupleDouble(){return this->InputTupleDouble;}
    QVector<LinkNodeTuple * > getOutputTupleDouble(){return this->OutputTupleDouble;}*/

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    void setSelected ( bool selected );
    void RePosTuplePorts();
    void recalculateLandH() ;
    bool isGroup(){return true;}
    virtual void setMinimized(bool b);

    void addModelNode(ModelNode * m) ;

    void changeGroupID(QString Name);
    void setGroupZValue();
    void removeModelNode(ModelNode *m);

    virtual void updatePorts();

public slots:
    void minimize();
    void maximize();
};

#endif // ROOTGROUPNODE_H
