/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Urich

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

#ifndef SWMMWRITEANDREAD_H
#define SWMMWRITEANDREAD_H

#include <dmmodule.h>
#include <dm.h>
#include <QDir>
#include <sstream>

class SWMMWriteAndRead
{
public:
	SWMMWriteAndRead(std::map<string, DM::ViewContainer *>, std::string rainfile, std::string filename  = "");
	void setRainFile(std::string rainfile);
	void setClimateChangeFactor(int cf);
	void writeSWMMFile();
	void setupSWMM();
	std::string getSWMMUUIDPath();
	void readInReportFile();
	void runSWMM();

	//Node ID
	std::map<int, double> getFloodedNodes();
	std::map<int, double> getNodeDepthSummery();

	//Junction ID
	std::map<int, double>getLinkFlowSummeryCapacity();
	std::map<int, double>  getLinkFlowSummeryVelocity();

	double getVp();
	double getVSurfaceRunoff();
	double getVSurfaceStorage();
	double getVwwtp();
	double getVout();
	double getTotalImpervious();
	double getContinuityError();
	double getImperiousInfiltration();
	double getAverageCapacity();

	double getWaterLeveleBelow0();
	double getWaterLeveleBelow10();
	double getWaterLeveleBelow20();


	/** @brief set caculation timestep for flow rounting in sec */
	void setCalculationTimeStep(int timeStep);

	/** @brief set built_year considered */
	void setBuildYearConsidered(bool buildyear);

	~SWMMWriteAndRead();

	bool getDeleteSWMMWhenDone() const;
	void setDeleteSWMMWhenDone(bool value);

private:
	bool built_year_considered;
	bool deleteSWMMWhenDone;
	int setting_timestep;

	int GLOBAL_Counter;
	std::map<std::string, DM::ViewContainer*> data_map;
	DM::ViewContainer * conduits;
	DM::ViewContainer * inlets;
	DM::ViewContainer * junctions;
	//DM::ViewContainer * endnodes;
	DM::ViewContainer * catchments;
	DM::ViewContainer * outfalls;
	DM::ViewContainer * nodes;


//	DM::View weir;
//	DM::View wwtp;
//	DM::View storage;
//	DM::View pumps;
//	DM::View globals;

	QDir SWMMPath;


	std::map<DM::Component*, int> UUIDtoINT;

	void writeSWMMheader(std::fstream &inp);
	void writeSubcatchments(std::fstream &inp);
	void writeJunctions(std::fstream &inp);
	void writeDWF(std::fstream &inp);
	void writeStorage(std::fstream &inp);
	void writeOutfalls(std::fstream &inp);
	void writeConduits(std::fstream &inp);
	void writeXSection(std::fstream &inp);
	void writeWeir(std::fstream &inp);
	void writePumps(std::fstream &inp);
	void writeCoordinates(std::fstream &inp);
	void writeLID_Controlls(std::fstream &inp);
	void writeLID_Usage(std::fstream &inp);
	void writeCurves(std::fstream & inp);


	void writeRainFile();

	void createViewDefinition();

	std::stringstream curves;
	std::string rainfile;

	std::vector<DM::Node*> PointList;


	double Vp;
	double VsurfaceRunoff;
	double VSurfaceStorage;
	double Vwwtp;
	double Vout;
	double TotalImpervious; //ha
	double Impervious_Infiltration; //m2
	double ContinuityError;
	double climateChangeFactor;

	void evalWaterLevelInJunctions();

	std::map<int, double>  floodedNodesVolume;

	std::map<int, double> nodeDepthSummery;

	std::map<int, double> linkFlowSummery_capacity;

	std::map<int, double> linkFlowSummery_velocity;

	//Water level in percent
	double water_level_below_0;
	double water_level_below_10;
	double water_level_below_20;

	double currentYear;

	QFile reportFile;

};

#endif // SWMMWRITEANDREAD_H
