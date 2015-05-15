/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2014 Christian Urich

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
#include "gdaldmswmm.h"
#include <fstream>

#include <QDir>
#include <QUuid>
#include <QProcess>
#include <QTextStream>
#include <QSettings>
#include <math.h>
#include <algorithm>
#include "swmmwriteandread.h"
#include "swmmreturnperiod.h"
#include "drainagehelper.h"
#include "ogrsf_frmts.h"

using namespace DM;
DM_DECLARE_NODE_NAME(GDALDMSWMM, Sewer)
GDALDMSWMM::GDALDMSWMM()
{
	GDALModule = true;

	GLOBAL_Counter = 1;
	internalTimestep = 0;

	conduit = DM::ViewContainer("conduit", DM::EDGE, DM::READ);
	conduit.addAttribute("start_id", "node", DM::READ);
	conduit.addAttribute("end_id", "node", DM::READ);
	conduit.addAttribute("diameter", DM::Attribute::DOUBLE, DM::READ);
	conduit.addAttribute("capacity", DM::Attribute::DOUBLE, DM::WRITE);
	conduit.addAttribute("velocity", DM::Attribute::DOUBLE, DM::WRITE);
	conduit.addAttribute("inlet_offset",  DM::Attribute::DOUBLE, DM::READ);
	conduit.addAttribute("outlet_offset",  DM::Attribute::DOUBLE, DM::READ);

	inlet = DM::ViewContainer("inlet", DM::NODE, DM::READ);
	inlet.addAttribute("node_id", "node", DM::READ);
	inlet.addAttribute("sub_catchment_id", "sub_catchment", DM::READ);

	junctions = DM::ViewContainer("junction", DM::NODE, DM::READ);
	junctions.addAttribute("node_id", "node", DM::READ);
	junctions.addAttribute("d", DM::Attribute::DOUBLE, DM::READ);
	junctions.addAttribute("invert_elevation", DM::Attribute::DOUBLE, DM::READ);

	junctions.addAttribute("flooding_volume", DM::Attribute::DOUBLE, DM::WRITE);
	junctions.addAttribute("node_depth", DM::Attribute::DOUBLE, DM::WRITE);

	catchment = DM::ViewContainer("sub_catchment", DM::FACE, DM::READ);
	catchment.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	catchment.addAttribute("impervious_fraction", DM::Attribute::DOUBLE, DM::READ);

	outfalls= DM::ViewContainer("outfall", DM::NODE, DM::READ);
	outfalls.addAttribute("node_id", "node", DM::READ);
	outfalls.addAttribute("invert_elevation", DM::Attribute::DOUBLE, DM::READ);

	nodes= DM::ViewContainer("node", DM::NODE, DM::READ);



	city = DM::ViewContainer("city", DM::COMPONENT, DM::READ);
	city.addAttribute("SWMM_ID", DM::Attribute::STRING, DM::WRITE);
	city.addAttribute("v_p", DM::Attribute::DOUBLE, DM::WRITE);
	city.addAttribute("v_r", DM::Attribute::DOUBLE, DM::WRITE);
	city.addAttribute("v_wwtp", DM::Attribute::DOUBLE, DM::WRITE);
	city.addAttribute("v_outfall", DM::Attribute::DOUBLE, DM::WRITE);
	city.addAttribute("continuity_error", DM::Attribute::DOUBLE, DM::WRITE);
	city.addAttribute("average_capacity", DM::Attribute::DOUBLE, DM::WRITE);


	this->FileName = "/tmp/swmm";
	this->climateChangeFactor = 1;
	this->RainFile = "";
	this->use_euler = true;
	this->return_period = 1;
	this->use_linear_cf = true;
	this->writeResultFile = false;
	this->climateChangeFactorFromCity = false;
	this->calculationTimestep = 1;
	this->consider_built_time = false;
	this->deleteSWMM = true;
	years = 0;

	this->isCombined = false;
	this->addParameter("Folder", DM::STRING, &this->FileName);
	this->addParameter("RainFile", DM::FILENAME, &this->RainFile);
	this->addParameter("ClimateChangeFactor", DM::DOUBLE, & this->climateChangeFactor);
	this->addParameter("use euler", DM::BOOL, & this->use_euler);
	this->addParameter("return period", DM::DOUBLE, &this->return_period);
	this->addParameter("combined system", DM::BOOL, &this->isCombined);
	this->addParameter("use_linear_cf", DM::BOOL, &this->use_linear_cf);
	this->addParameter("writeResultFile", DM::BOOL, &this->writeResultFile);
	this->addParameter("climateChangeFactorFromCity", DM::BOOL, &this->climateChangeFactorFromCity);
	this->addParameter("calculationTimestep", DM::INT, & this->calculationTimestep);
	this->addParameter("consider_build_time", DM::BOOL, & this->consider_built_time);
	this->addParameter("deleteSWMM", DM::BOOL, & this->deleteSWMM);

	counterRain = 0;

	std::vector<DM::ViewContainer*> data_stream;

	data_stream.push_back(&inlet);
	data_stream.push_back(&junctions);
	data_stream.push_back(&catchment);
	data_stream.push_back(&outfalls);
	data_stream.push_back(&conduit);
	data_stream.push_back(&nodes);
	data_stream.push_back(&city);


	data_map["inlet"] = &this->inlet;
	data_map["junction"] = &this->junctions;
	//data_map["endnode"] = &this->endnodes;
	data_map["catchment"] = &this->catchment;
	data_map["outfall"]  = &this->outfalls;
	data_map["conduit"]  = &this->conduit;
	data_map["node"]  = &this->nodes;
	data_map["city"]  = &this->city;
	this->registerViewContainers(data_stream);

	unique_name = QUuid::createUuid().toString().toStdString();

}

void GDALDMSWMM::init() {

}

string GDALDMSWMM::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-Sewer/blob/master/doc/DMSWMM.md";
}


void GDALDMSWMM::run() {

	if (!QDir(QString::fromStdString(this->FileName)).exists()){
		DM::Logger(DM::Standard) <<  this->FileName << "  does not exist but created";
		QDir::current().mkpath(QString::fromStdString(this->FileName));
		//return;
	}

	double cf = this->climateChangeFactor;

	if (this->climateChangeFactorFromCity) {
		this->city.resetReading();
		OGRFeature * city_f;
		while(city_f = this->city.getNextFeature()){
			cf = city_f->GetFieldAsDouble("climate_change_factor");
		}
	}

	if (this->use_linear_cf) cf = 1. + years / 20. * (this->climateChangeFactor - 1.);

	this->years++;

	if (this->internalTimestep == this->calculationTimestep)
		this->internalTimestep = 0;
	this->internalTimestep++;

	if (this->internalTimestep != 1) {
		return;
	}

	SWMMWriteAndRead * swmm;

	if (!this->use_euler)
		swmm = new SWMMWriteAndRead(data_map, this->RainFile, this->FileName);
	else {
		std::stringstream rfile;
		rfile << "/tmp/rain_"<< QUuid::createUuid().toString().toStdString();
		DrainageHelper::CreateEulerRainFile(60, 5, this->return_period, cf, rfile.str());
		swmm = new SWMMWriteAndRead(data_map, rfile.str(), this->FileName);
	}
	swmm->setDeleteSWMMWhenDone(this->deleteSWMM);
	swmm->setBuildYearConsidered(this->consider_built_time);

	swmm->setClimateChangeFactor(cf);
	swmm->setupSWMM();
	swmm->runSWMM();
	swmm->readInReportFile();

	std::map<int, double> flooded_nodes = swmm->getFloodedNodes();
	std::map<int, double> node_depths = swmm->getNodeDepthSummery();

	OGRFeature * junction;
	junctions.resetReading();
	while (junction = junctions.getNextFeature()) {
		int id =junction->GetFieldAsInteger("node_id");
		if (flooded_nodes.find(id) != flooded_nodes.end()) {
			junction->SetField("flooding_volume", flooded_nodes[id]);
		}
		if (node_depths.find(id) != node_depths.end()) {
			junction->SetField("node_depth", node_depths[id]);
		}
	}

	std::map<int, double> flow_cap = swmm->getLinkFlowSummeryCapacity();
	std::map<int, double> flow_vel = swmm->getLinkFlowSummeryVelocity();

	OGRFeature * c;
	this->conduit.resetReading();
	while (c = conduit.getNextFeature()) {
		int id = c->GetFID();
		if (flow_cap.find(id) != flow_cap.end()) {
			c->SetField("capacity", flow_cap[id]);
		}
		if (flow_vel.find(id) != flow_vel.end()) {
			c->SetField("velocity", flow_vel[id]);
		}
	}

	DM::Logger(DM::Standard) << "Start write output files";

	delete swmm;
}
