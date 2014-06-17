#include "dmgdalsystem.h"

#include <dmlogger.h>

#include <QUuid>
#include <qglobal.h>

namespace DM {

GDALSystem::GDALSystem()
{
	OGRRegisterAll();

	poDrive = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( "SQLite" );
	char ** options = NULL;
	options = CSLSetNameValue( options, "OGR_SQLITE_CACHE", "1024" );
	QString dbname = "/tmp/" + QUuid::createUuid().toString() + ".db";
	poDS = poDrive->CreateDataSource(dbname.toStdString().c_str() ,options );

	if( poDS == NULL ) {
		DM::Logger(DM::Error) << "couldn't create source";
	}
}

void GDALSystem::updateSystemView(const View &v)
{
	if (viewLayer.find(v.getName()) == viewLayer.end()) {
		Logger(Debug) << "Create Layer";
		OGRLayer * lyr = poDS->CreateLayer("components", NULL, wkbNone, NULL );

		if (lyr == NULL) {
			DM::Logger(DM::Error) << "couldn't create layer";
		}
		OGRFieldDefn oField( "dynamind_id", OFTString );
		lyr->CreateField(&oField);

		viewLayer[v.getName()] = lyr;
	}
}

OGRFeature *GDALSystem::createFeature(const View &v)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	OGRFeature * f = OGRFeature::CreateFeature(lyr->GetLayerDefn());
	f->SetField(0, QUuid::createUuid().toString().toStdString().c_str());

	dirtyFeatures.push_back(f);
	return f;

}

void GDALSystem::syncFeatures(const DM::View & v)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	//Sync all features
	int counter = 0;
	lyr->StartTransaction();
	foreach (OGRFeature * f, dirtyFeatures) {
		counter++;
		if (counter == 10000) {
			lyr->CommitTransaction();
				lyr->StartTransaction();
			counter=0;
		}
		lyr->CreateFeature(f);
		OGRFeature::DestroyFeature(f);
	}
	lyr->CommitTransaction();

	dirtyFeatures.clear();
}

}
