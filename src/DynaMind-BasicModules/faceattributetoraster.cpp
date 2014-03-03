#include "faceattributetoraster.h"
#include <dm.h>
#include <cgalgeometry.h>
#include <QPolygonF>
#include <tbvectordata.h>

DM_DECLARE_NODE_NAME(FaceAttributeToRaster, Geometry)

FaceAttributeToRaster::FaceAttributeToRaster()
{
	this->faceName = "";
	this->attributeName = "";
	this->rasterDataName = "";

	this->addParameter("FaceName", DM::STRING, &this->faceName);
	this->addParameter("attributeName", DM::STRING, &this->attributeName);
	this->addParameter("rasterDataName", DM::STRING, &this->rasterDataName);


}

void FaceAttributeToRaster::init()
{
	if (faceName.empty()) {
		DM::Logger(DM::Warning) << "No face defined";
		return;
	}
	if (attributeName.empty()) {
		DM::Logger(DM::Warning) << "No attribtue name defined";
		return;
	}
	if (rasterDataName.empty()) {
		DM::Logger(DM::Warning) << "No Raster data name defined";
		return;
	}

	this->inputFaceView = DM::View(this->faceName, DM::FACE, DM::READ);
	this->inputFaceView.addAttribute(this->attributeName);
	this->rasterDataView = DM::View(this->rasterDataName, DM::RASTERDATA, DM::MODIFY);

	std::vector<DM::View> datastream;
	datastream.push_back(inputFaceView);
	datastream.push_back(rasterDataView);

	this->addData("sys", datastream);
}

void FaceAttributeToRaster::run()
{
	DM::System * sys = this->getData("sys");
	DM::RasterData * rdata = this->getRasterData("sys", rasterDataView);


	if (!rdata) {
		DM::Logger(DM::Error) <<  "Something went wrong when getting raster data";
		return;
	}
	rdata->clear();
	double cellsize = rdata->getCellSize()/2.;

	qreal * x1 = new double;
	qreal * x2 = new double;
	qreal * y1 = new double;
	qreal * y2 = new double;

	mforeach (DM::Component * cmp, sys->getAllComponentsInView(inputFaceView)) {
		DM::Face * f = (DM::Face*)cmp;

		QPolygonF poly = TBVectorData::FaceAsQPolgonF(sys, f);
		QRectF br = poly.boundingRect();

		br.getCoords(x1, y1, x2, y2);
		double val = f->getAttribute(this->attributeName)->getDouble();

		for (double x = *x1; x< *x2; x+=cellsize) {
			for (double y = *y1; y< *y2; y+=cellsize) {
				DM::Node n1(x+0.00001,y+0.00001,0); // Otherwise first point is on the edge
				if (TBVectorData::PointWithinFace(f, &n1)) {
					rdata->setValue(x, y, val);
				}
			}
		}

	}
	delete x1;
	delete x2;
	delete y1;
	delete y2;

}

string FaceAttributeToRaster::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/FaceAttributeToRaster.md";
}


