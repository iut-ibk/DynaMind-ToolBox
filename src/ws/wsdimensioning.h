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
	bool fixeddiameters, pipestatus, usemainpipe, usereservoirdata;
	double maxdiameter, iterations;

public:
	Dimensioning();
	void run();
	void init();

private:
	bool approximateMainPipes(bool usereservoirsdata,double totaldemand, std::vector<DM::Edge*> &reservoirpipes,bool discretediameters);
	double calcTotalDemand();
	bool SitzenfreiDimensioning();
	bool calibrateReservoirOutFlow(double totaldemand, int maxsteps, double diameterstepsize,std::vector<DM::Edge*> entrypipes, bool discretediameter);
	bool findFlowPath(std::vector<DM::Node*> &nodes, std::vector<DM::Node*> &alternativepathjunction, DM::Node* currentPressurePoint, std::vector<DM::Node*> knownPressurePoints);
	bool approximatePipeSizes(bool usemainpipes,bool discretediameter);
	bool approximatePressure(bool discretediameter);
	bool approximatePressure(std::vector<DM::Node*> &knownPressurePoints, std::vector<DM::Node*> &uncheckedpressurepoints,std::vector<DM::Node*> &newpressurepoint, bool nonewpressurepoints);
	bool approximatePressureOnPath(std::vector<DM::Node*> nodes,std::vector<DM::Node*> &knownPressurePoints,std::vector<DM::Node*> &newpressurepoint,bool nonewpressurepoints);
	std::vector<DM::Node*> getFlowNeighbours(DM::Node* junction);
	std::vector<DM::Node*> getInverseFlowNeighbours(DM::Node* junction);
	DM::Node* getNearestPressure(DM::Node* currentpressurepoint, std::vector<DM::Node*> &nodes);
	DM::Node* getNearestFlowPoint(DM::Node* currentpoint, std::vector<DM::Node*> &nodes);

	double calcDiameter(double k, double l, double q, double h, double maxdiameter,bool discretediameters);
	double calcFrictionHeadLoss(double d, double k, double l, double q);
	double calcLambda(double k, double d, double q, double lambda = 0);
};

#endif // Dimensioning_H
