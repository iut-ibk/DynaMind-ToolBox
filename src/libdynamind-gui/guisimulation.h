/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich, Markus Sengthaler
 
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

#ifndef GUISIMULATION_H
#define GUISIMULATION_H
#include <dmcompilersettings.h>
#include <dmsimulation.h>
#include <QVector>
#include <QString>
#include <QPointF>

class ModelNode;
class PortNode;
//class GroupNode;
//class GUILink;

class DM_HELPER_DLL_EXPORT GUISimulation :  public DM::Simulation//, public QObject
{
    //Q_OBJECT
public:
    GUISimulation();
    void registerRootNode();
    //GroupNode * getGroupNode(DM::Group * g);
    //void changeGroupName(GroupNode*);
    void clearSimulation();

	/** @brief loads modules from default locations */
    void loadModulesFromDefaultLocation();
	/** @brief Add the modules set in the QSetting **/
    bool loadModulesFromSettings();

	
	bool addLink(PortNode* out, PortNode* in);
	bool removeLink(PortNode* out, PortNode* in);

	//void guiUpdatePorts(ModelNode* node);
	ModelNode* guiAddModule(QString moduleName);
private:
	void loadPythonModulesFromDirectory(std::string path);
    QVector<ModelNode*> modelNodes;
    //QVector<GroupNode*> groupNodes;

signals:
    void addedModule(ModelNode*);
    //void addedGroup(GroupNode*);
    //void GroupNameChanged(GroupNode*);
    //void showHelpForModule(std::string classname, std::string uuid);


public slots:
    //void GUIaddModule( QString  name, QPointF pos, DM::Module *group);
    //void GUIaddModule(DM::Module *, QPointF pos);
    //void updateSimulation();
    //void resetSimulation();
    void showHelp(std::string classname, std::string uuid);

	//std::map<ModelNode*,DM::Module*> moduleGuiMap;
};

#endif // GUISIMULATION_H

