#include "dm_exportsimulationdb.h"
#include "dmgdalsystem.h"

DM_DECLARE_CUSTOM_NODE_NAME(DM_ExportSimulationDB, Export Simulation DB , Data Import and Export)


#include <dmgroup.h>
#include <QFile>

DM_ExportSimulationDB::DM_ExportSimulationDB()
{
	GDALModule = true;

	this->steps = 1;
	this->addParameter("steps", DM::INT, &steps);

	this->file_name = "";
	this->addParameter("file_name", DM::FILENAME, &file_name);

	DM::ViewContainer v("dummy", DM::SUBSYSTEM, DM::MODIFY);


	std::vector<DM::ViewContainer> datastream;
	datastream.push_back(v);
	this->addGDALData("city", datastream);
}

void DM_ExportSimulationDB::run()
{
	if (this->file_name.empty()) {
		return;
	}
	DM::Group* lg = dynamic_cast<DM::Group*>(getOwner());
	int interal_counter = -1;
	if(lg) {
		interal_counter = lg->getGroupCounter();
	}
	DM::Logger(DM::Debug) << interal_counter;

	if (interal_counter % this->steps != 0 && interal_counter != -1) {
		return;
	}

	DM::Logger(DM::Debug) << "Start Inster";

	DM::GDALSystem * sys =  (DM::GDALSystem *)this->getInPortData("city");
	if(!sys)
	{
		DM::Logger(DM::Error) << "Something went wrong while getting the data";
		return;
	}
	QString fileName;
	if (interal_counter != -1)
		 fileName = QString::fromStdString(this->file_name) + "_" + QString::number(interal_counter) + ".sqlite";
	else
		 fileName = QString::fromStdString(this->file_name) + ".sqlite";
	DM::Logger(DM::Error) << "Copy file to " << fileName.toStdString();
	QFile::copy(QString::fromStdString(sys->getDBID()), fileName);
}


string DM_ExportSimulationDB::getHelpUrl()
{
	return "/DynaMind-GDALModules/dm_exportsimulationdb.html";
}
