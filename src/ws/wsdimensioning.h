/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#ifndef Dimensioning_H
#define Dimensioning_H

#include <dmmodule.h>
#include <dm.h>

#include <watersupplyviewdef.h>
#include <epanetdynamindconverter.h>

class Dimensioning : public DM::Module
{
	DM_DECLARE_NODE(Dimensioning)

	DM::WS::ViewDefinitionHelper wsd;
	DM::System * sys;
	boost::shared_ptr<EpanetDynamindConverter> converter;
	bool fixeddiameters, pipestatus, usemainpipe, usereservoirdata, discrete, nearestdiscretediameter;
	double maxdiameter, iterations, apprdt;
	std::vector<DM::Component*> fixedpipes;
	std::vector<DM::Node*> initknownpressure;

public:
	Dimensioning();
	void run();
	void init();

private:
	bool approximateMainPipes(bool usereservoirsdata,double totaldemand, std::vector<DM::Edge*> &reservoirpipes,bool discretediameters);
	double calcTotalDemand();
	bool SitzenfreiDimensioning();
	bool calibrateReservoirOutFlow(double totaldemand, int maxsteps,std::vector<DM::Edge*> entrypipes, bool discretediameter);
	bool findFlowPath(std::vector<DM::Node*> &nodes, std::vector<DM::Node*> &alternativepathjunction, DM::Node* currentPressurePoint, std::vector<DM::Node*> knownPressurePoints);
	bool approximatePipeSizes(bool usemainpipes,bool discretediameter);
	bool approximatePressure(bool discretediameter,std::vector<DM::Node*>initunchecked);
	bool approximatePressure(std::vector<DM::Node*> &knownPressurePoints, std::vector<DM::Node*> &uncheckedpressurepoints,std::vector<DM::Node*> &newinitunchecked);
	bool approximatePressureOnPath(std::vector<DM::Node*> nodes,std::vector<DM::Node*> &knownPressurePoints,std::vector<DM::Node*> &newinitunchecked);
	std::vector<DM::Node*> getInitialPressurepoints();
	void resetPressureOfJunctions(std::vector<DM::Node*> nodexceptions);

	void searchFixedPipes(double maxdiameter, std::vector<DM::Component*> &fixedpipes, std::vector<DM::Component*> allpipes);
};

#endif // Dimensioning_H
