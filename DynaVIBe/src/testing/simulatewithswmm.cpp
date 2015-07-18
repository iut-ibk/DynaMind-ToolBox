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

#include <simulatewithswmm.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

#include <list>
#include <cmath>

//Sewer
#include <dmswmm.h>
//#include <error.h>

#include <sewerviewdef.h>

using namespace DM;

DM_DECLARE_NODE_NAME(SimulateWithSWMM,Sewersystems)

SimulateWithSWMM::SimulateWithSWMM()
{
	inpfilepath = "";
	this->addParameter("Path of inp file", DM::FILENAME, &this->inpfilepath);

	DM::S::ViewDefinitionHelper sd;

	std::vector<View> all = sd.getAll(DM::MODIFY);
	DM::View ex = DM::View("EXAMINATIONROOM",DM::FACE,DM::MODIFY);
	ex.addAttribute("returnperiod",DM::Attribute::DOUBLE,DM::MODIFY);
	all.push_back(ex);

	this->addData("Sewer", all);
}

void SimulateWithSWMM::run()
{
	DM::S::ViewDefinitionHelper sd;
	DM::View ex = DM::View("EXAMINATIONROOM",DM::FACE,DM::MODIFY);
	ex.addAttribute("returnperiod",DM::Attribute::DOUBLE,DM::MODIFY);

	this->sys = this->getData("Sewer");

	std::vector<DM::Component*> faces = this->sys->getAllComponentsInView(ex);

	if(faces.size())
	{
		double p = faces[0]->getAttribute("returnperiod")->getDouble();
		DM::Logger(DM::Error) << "CURRENT RETURN PERIOD: " << p;
	}

	QString dir = QDir::tempPath();
	//std::string inpfilename = dir.toStdString() + "/test.inp";
	std::string  inpfilename = inpfilepath;
	std::string rptfilename = dir.toStdString() + "/test.rpt";
	std::string frdfilename = dir.toStdString() + "/test.frd";

	//DM::Logger(DM::Standard) << "Writing file: " << inpfilename;
	//

	this->sys = this->getData("Sewer");

	//DM::Logger(DM::Standard) << "Creating SWMM model";
	//

	DM::Logger(DM::Standard) << "Simulation with SWMM";

	//loading library
	QLibrary swmmLib("libswmm5");
	typedef int (*MyPrototype)();
	typedef int (*SRun)(char* f1, char* f2, char* f3);
	MyPrototype swmm_getVersion = (MyPrototype) swmmLib.resolve("swmm_getVersion");
	SRun swmm_run = (SRun) swmmLib.resolve("swmm_run");


	DM::Logger(DM::Standard) << "Found SWMM version: " << swmm_getVersion();

	//change working directory
	QFileInfo info(inpfilename.c_str());
	QDir::setCurrent(info.absolutePath());

	//run swmm simulation
	int error = swmm_run(const_cast<char*>(inpfilename.c_str()),const_cast<char*>(rptfilename.c_str()),const_cast<char*>(frdfilename.c_str()));
	if(error > 0)
	{
		DM::Logger(DM::Error) << "SWMM5 error code: " << SWMM::error_getMsg(error);
		return;
	}


	//DM::Logger(DM::Standard) << "Extracting results from EPANET run";
	//find flooded nodes (only Testing)

	QFile rfile(rptfilename.c_str());
	if(!rfile.open(QIODevice::ReadOnly))
		DM::Logger(DM::Error) << "Cannot open result file: " << rptfilename;

	QTextStream rstream(&rfile);

	bool infloodingsection = false;
	QStringList floodinglist;
	double totalflood = 0.0;
	uint floodednodes=0;

	while(!rstream.atEnd())
	{
		QString line = rstream.readLine();
		if(line.contains("Node Flooding Summary"))
		{
			infloodingsection=true;
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
			line = rstream.readLine();
		}

		if(line.trimmed().simplified()=="")
			infloodingsection=false;

		if(infloodingsection)
		{
			if(line.trimmed().simplified().split(" ")[5].toDouble() > 0.05)
			{
				floodinglist.append(line.trimmed().simplified().split(" ")[0]);
				totalflood+=line.trimmed().simplified().split(" ")[5].toDouble();
				floodednodes++;
			}
		}
	}

	std::vector<DM::Component*> nodes = this->sys->getAllComponentsInView(sd.getCompleteView(S::JUNCTION,DM::READ));

	for(uint index=0; index < nodes.size(); index++)
	{
		std::string currentname = nodes[index]->getAttribute("Name")->getString();
		if(currentname!="" && floodinglist.contains(currentname.c_str()))
			nodes[index]->changeAttribute("Flooded",true);
		else
			nodes[index]->changeAttribute("Flooded",false);

	}

	DM::Logger(DM::Error) << "TOTAL FLOOD: " << totalflood << " 10^6 ltr" ;
	DM::Logger(DM::Error) << "TOTAL Number of flooded nodes: " << (int)floodednodes;
	rfile.close();
}
