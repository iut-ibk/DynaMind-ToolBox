/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2014  Christian Urich

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

#ifndef VIBESWMM_H
#define VIBESWMM_H

#include <dmmodule.h>
#include <dm.h>
#include <QDir>
#include <sstream>


class DM_HELPER_DLL_EXPORT GDALDMSWMM : public  DM::Module {
	DM_DECLARE_NODE (GDALDMSWMM)

	private:
		int GLOBAL_Counter;
	//DM::System * city;
	DM::ViewContainer conduit;
	DM::ViewContainer inlet;
	DM::ViewContainer junctions;
	DM::ViewContainer city;
	//DM::ViewContainer endnodes;
	DM::ViewContainer catchment;
	DM::ViewContainer outfalls;
	DM::ViewContainer nodes;
	DM::ViewContainer weir;
	//DM::View wwtp;
	//DM::View storage;
	//DM::View pumps;
	//DM::View globals;

	std::map<std::string, DM::ViewContainer*> data_map;

	std::string RainFile;
	std::string FileName;
	std::vector<DM::Node*> PointList;
	bool isCombined;
	bool hasWeir;
	bool writeResultFile;
	bool deleteSWMM;
	bool exportSubCatchmentShape;

	double climateChangeFactor;
	bool climateChangeFactorFromCity;
	std::string rainfile_from_vector;

	int calculationTimestep;
	int internalTimestep;


	void writeRainFile();
	int years;
	double counterRain;

	std::map<std::string, int> UUIDtoINT;

	std::stringstream curves;

	std::string unique_name;

public:
	GDALDMSWMM();
	void run();
	void init();
	std::string getHelpUrl();
};

#endif // VIBESWMM_H
