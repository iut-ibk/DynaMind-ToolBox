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
#include <dmsimulationobserver.h>
#include <QVector>
#include <QString>
#include <QPointF>
#include <qobject.h>

class ModelNode;
class PortNode;
class SimulationTab;
class QTabWidget;
namespace DM
{
class Group;
}

#if defined _MSC_VER && defined _WIN32
enum PortType;
#else
enum PortType : unsigned int;
#endif

class DM_HELPER_DLL_EXPORT GUISimulation :  public DM::Simulation
{
public:
    GUISimulation(QWidget * parent, QTabWidget* tabWidget);
    void clearSimulation();

	bool addLink(DM::Module* source, std::string outPort, DM::Module* dest, std::string inPort, bool checkStream = true);

	bool removeLink(PortNode* out, PortNode* in);

	// overloaded
	DM::Module* addModule(std::string moduleName, DM::Module* parent, bool callInit = true);
    void removeModule(DM::Module* m);

	SimulationTab* addTab(DM::Group* parentGroup);	// parent = NULL for root tab
	void closeTab(int index);
	void closeTab(SimulationTab* tab);
	SimulationTab* getTab(int index);	// 0 = root*/
	QList<SimulationTab*> getTabs();	// 0 = root*/
	SimulationTab* getSelectedTab();
	void selectTab(int index);

	QTabWidget* getTabWidget(){return tabWidget;};
	
    /** @brief adds a simulation saved in a file to the current simulation 
				for gui we load the positions of the modules too */
    bool loadSimulation(std::string filePath);
	/** @brief writes the simulation to a xml file 
			for gui we add the positions of the modules too */
    void writeSimulation(std::string filename);

	void appendGuiInformation(QIODevice* dest, std::list<DM::Module*> modules);

	ModelNode* getModelNode(DM::Module *m){return modelNodes[m];};
    QString currentDocument;
private:
	std::map<DM::Module*, ModelNode*> modelNodes;
	QList<SimulationTab*> tabs;
	QTabWidget* tabWidget;

	PortNode* getPortNode(DM::Module* m, std::string portName, 
		DM::PortType type, bool fromInnerGroup);
	QWidget * parent;
};

#endif // GUISIMULATION_H

