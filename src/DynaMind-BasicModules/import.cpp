/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#include "import.h"
#include "tbvectordata.h"
#include "simplecrypt.h"
#include <sstream>
#include <guiimport.h>
#include <algorithm>
#include <QHash>

DM_DECLARE_NODE_NAME(Import, Modules)

Import::Import()
{
	driverType = ShapeFile;

	this->FileName = "";
	this->addParameter("Filename", DM::FILENAME, &this->FileName);
	this->epsgcode=31254;
	this->addParameter("Transform to EPSG:", DM::INT, &this->epsgcode);
	this->ViewName = "";
	this->addParameter("ViewName", DM::STRING, &this->ViewName);
	this->tol = 0.00001;
	this->addParameter("Tolerance", DM::DOUBLE, &this->tol);
	devider = 100;
	this->append = false;
	this->addParameter("AppendToExisting", DM::BOOL, &this->append);
	this->attributesToImport = std::map<std::string, std::string>();
	this->addParameter("AttributesToImport", DM::STRING_MAP, &this->attributesToImport);
	this->importAll = true;
	this->addParameter("ImportAll", DM::BOOL, &this->importAll);
	this->linkWithExistingView = false;
	this->addParameter("linkWithExistingView", DM::BOOL, &this->linkWithExistingView);
	this->offsetX = 0;
	this->addParameter("offsetX", DM::DOUBLE, &this->offsetX);
	this->offsetY = 0;
	this->addParameter("offsetY", DM::DOUBLE, &this->offsetY);

	this->addParameter("viewConfig", DM::STRING_MAP, &this->viewConfig);

	//WFS Input
	this->WFSDataName = "";
	this->addParameter("WFSDataName", DM::STRING, &this->WFSDataName);
	this->WFSServer = "";
	this->addParameter("WFSServer", DM::STRING, &this->WFSServer);
	this->WFSUsername = "";
	this->addParameter("WFSUsername", DM::STRING, &this->WFSUsername);
	this->WFSPassword = "";
	this->addParameter("WFSPassword", DM::STRING, &this->WFSPassword);

	this->flip_wfs = false;
	this->addParameter("flip_wfs", DM::BOOL, &this->flip_wfs);

	fileok = false;

	view = NULL;

	initViews();
}

Import::~Import()
{
	reset();
}

void Import::reset()
{
	viewConfig.clear();
	viewConfigTypes.clear();

	if (view)
	{
		delete view;
		view = NULL;
	}
}

// INIT methods

void Import::init()
{
	//if (!moduleParametersChanged())
	//	return;

	reset();

	fileok = false;

	OGRRegisterAll();
	GDALAllRegister();	// neccessary for windows!
	OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();

	if (!this->WFSServer.empty())
	{
		/*driverType = WFS;
		// password
		SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023));
		QString pwd = crypto.decryptToString(QString::fromStdString(this->WFSPassword));

		// create server url with login
		this->server_full_name = "WFS:http://" + this->WFSUsername + ":" + pwd.toStdString() + "@" + this->WFSServer;

		OGRDataSource *poDS = OGRSFDriverRegistrar::Open(server_full_name.c_str(), FALSE);

		if (!poDS)
			return;

		OGRLayer* poLayer = this->LoadWFSLayer(poDS);
		if (!poLayer)
			return;

		fileok = true;

		this->vectorDataInit(poLayer);
		OGRDataSource::DestroyDataSource(poDS);*/
	}
	else
	{
		driverType = ShapeFile;

		if (FileName.empty())
		{
			DM::Logger(DM::Error) << "No file specified " << FileName;
			return;
		}

		if (ViewName.empty())
		{
			DM::Logger(DM::Error) << "No view specified";
			return;
		}

		OGRDataSource* poDS = OGRSFDriverRegistrar::Open(FileName.c_str(), FALSE);

		if (poDS == NULL)
		{
			GDALDataset  *poDataset = (GDALDataset*)GDALOpenShared(FileName.c_str(), GA_ReadOnly);
			if (poDataset == NULL)
				DM::Logger(DM::Error) << "Open failed.";
			else
			{
				fileok = true;
				extractLayers(poDataset);
			}
		}
		else
		{
			fileok = true;
			extractLayers(poDS);
			initViews();
			//vectorDataInit(poDS->GetLayer(0));
			OGRDataSource::DestroyDataSource(poDS);
		}
	}
}


void Import::extractLayers(OGRDataSource* dataSource)
{
	viewConfig.clear();
	viewConfigTypes.clear();

	for (int i = 0; i < dataSource->GetLayerCount(); i++)
	{
		OGRLayer* layer = dataSource->GetLayer(i);
		OGRwkbGeometryType ogrType = layer->GetGeomType();
		std::string strType = OGRGeometryTypeToName(ogrType);

		// create a view per layer
		const std::string viewName = layer->GetName();
		viewConfig[viewName] = viewName;

		switch (wkbFlatten(ogrType))
		{
		case wkbPoint:				viewConfigTypes[viewName] = DM::NODE;	break;
		case wkbPolygon:
		case wkbMultiPolygon:		viewConfigTypes[viewName] = DM::FACE;	break;
		case wkbLineString:
		case wkbMultiLineString:	viewConfigTypes[viewName] = DM::EDGE;	break;
		default:
			DM::Logger(DM::Debug) << "Geometry type not implemented: " << strType << " (" << ogrType << " )";
			fileok = false;
			return;
		}
		DM::Logger(DM::Debug) << "Found: Geometry type" << strType;

		// add attributes

		OGRFeatureDefn *ogrFieldDefn = layer->GetLayerDefn();
		for (int iField = 0; iField < ogrFieldDefn->GetFieldCount(); iField++)
		{
			OGRFieldDefn *poFieldDefn = ogrFieldDefn->GetFieldDefn(iField);

			std::string combinedViewAttributeName = viewName + "." + poFieldDefn->GetNameRef();

			viewConfig[combinedViewAttributeName] = poFieldDefn->GetNameRef();

			switch (poFieldDefn->GetType())
			{
			case OFTInteger:
			case OFTReal:
				viewConfigTypes[combinedViewAttributeName] = DM::Attribute::DOUBLE;
				break;
			default:
				viewConfigTypes[combinedViewAttributeName] = DM::Attribute::STRING;
				break;
			}
		}
	}
}

void Import::extractLayers(GDALDataset* dataSource)
{
	viewConfig.clear();
	viewConfigTypes.clear();

	isvectordata = false;
	append = false;

	double        adfGeoTransform[6];
	int           nBlockXSize, nBlockYSize;
	int           bGotMin, bGotMax;
	double        adfMinMax[2];
	GDALRasterBand  *poBand;

	DM::Logger(DM::Debug) << "Driver: " << dataSource->GetDriver()->GetDescription()
		<< "/" << dataSource->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
	DM::Logger(DM::Debug) << "Size is " << dataSource->GetRasterXSize() << " " << dataSource->GetRasterYSize()
		<< " " << dataSource->GetRasterCount();

	if (!std::string(dataSource->GetProjectionRef()).empty())
		DM::Logger(DM::Debug) << "Projection is " << " " << dataSource->GetProjectionRef();
	else
	{
		DM::Logger(DM::Error) << "No projection found";
		fileok = false;
		return;
	}

	if (dataSource->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		DM::Logger(DM::Debug) << "Origin = " << adfGeoTransform[0] << "," << adfGeoTransform[3];
		DM::Logger(DM::Debug) << "Pixel Size = " << adfGeoTransform[1] << "," << adfGeoTransform[5];
	}

	poBand = dataSource->GetRasterBand(1);
	poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
	DM::Logger(DM::Debug) << "Block=" << nBlockXSize << "x" << nBlockYSize <<
		"Type=" << GDALGetDataTypeName(poBand->GetRasterDataType()) <<
		", ColorInterp=" << GDALGetColorInterpretationName(poBand->GetColorInterpretation());

	adfMinMax[0] = poBand->GetMinimum(&bGotMin);
	adfMinMax[1] = poBand->GetMaximum(&bGotMax);
	if (!bGotMin || !bGotMax)
		GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

	DM::Logger(DM::Debug) << "Min=" << adfMinMax[0] << ", Max=" << adfMinMax[1];

	if (poBand->GetOverviewCount() > 0)
		DM::Logger(DM::Debug) << "Band has" << poBand->GetOverviewCount() << " overviews";

	if (poBand->GetColorTable() != NULL)
		DM::Logger(DM::Debug) << "Band has a color table with " << poBand->GetColorTable()->GetColorEntryCount() << " entries";

	// set config
	viewConfig["RasterData"] = "RasterData";
	viewConfigTypes["RasterData"] = DM::RASTERDATA;
}

void Import::initViews()
{
	std::map<std::string, DM::View> views;

	// add views
	for (StringMap::const_iterator it = viewConfig.begin(); it != viewConfig.end(); ++it)
		if(strchr(it->first.c_str(), '.') == NULL && !it->second.empty()) // is it a view and not empty?
			views[it->first] = DM::View(it->second, viewConfigTypes[it->first], DM::WRITE);

	// add attributes
	for (StringMap::const_iterator it = viewConfig.begin(); it != viewConfig.end(); ++it)
	{
		if (strchr(it->first.c_str(), '.') != NULL && !it->second.empty()) // is it an attribute and not empty?
		{
			QStringList parsedString = QString::fromStdString(it->first).split('.', QString::SkipEmptyParts);

			if (parsedString.size() != 2)
			{
				DM::Logger(DM::Error) << "error parsing parameter: " << it->first;
				continue;
			}

			std::string viewName = parsedString.first().toStdString();
			if (map_contains(&views, viewName))
				views[viewName].addAttribute(parsedString.last().toStdString(),
								(DM::Attribute::AttributeType)viewConfigTypes[it->first],
								DM::WRITE);
		}
	}

	// push to module base
	std::vector<DM::View> vviews;

	mforeach(const DM::View& v, views)
		vviews.push_back(v);

	if (vviews.empty())
		vviews.push_back(DM::View("dummy", 0, DM::WRITE));

	addData("out", vviews);
}

bool Import::moduleParametersChanged()
{
	bool changed = false;
	if (FileName_old != FileName)		changed = true; FileName_old = FileName;
	if (ViewName_old != ViewName)		changed = true; ViewName_old = ViewName;
	if (WFSDataName_old != WFSDataName) changed = true; WFSDataName_old = WFSDataName;
	if (WFSServer_old != WFSServer)		changed = true; WFSServer_old = WFSServer;
	if (WFSUsername_old != WFSUsername) changed = true; WFSUsername_old = WFSUsername;
	if (WFSPassword_old != WFSPassword) changed = true; WFSPassword_old = WFSPassword;
	if (append_old != append)			changed = true; append_old = append;

	return changed;
}

OGRLayer *Import::LoadWFSLayer(OGRDataSource *poDS)
{
	OGRLayer            *poLayer;
	//OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
	poDS = OGRSFDriverRegistrar::Open(server_full_name.c_str(), FALSE);

	int LayerCount = poDS->GetLayerCount();
	for (int i = 0; i < LayerCount; i++)
	{
		poLayer = poDS->GetLayer(i);
		std::string currentLayerName = poLayer->GetName();
		if (currentLayerName == this->WFSDataName)
			return poLayer;
	}
	return 0;
}

/*void Import::vectorDataInit(OGRLayer *poLayer)
{
	isvectordata = true;
	OGRFeature *poFeature = poLayer->GetNextFeature();
	poLayer->ResetReading();

	if (!poFeature)
		return;

	if (OGRGeometry *poGeometry = poFeature->GetGeometryRef())
	{
		OGRwkbGeometryType ogrType = poGeometry->getGeometryType();
		std::string strType = OGRGeometryTypeToName(ogrType);
		DM::Components dmType;

		switch (wkbFlatten(ogrType))
		{
		case wkbPoint:				dmType = DM::NODE;	break;
		case wkbPolygon:			dmType = DM::FACE;	break;
		case wkbMultiPolygon:		dmType = DM::FACE;	break;
		case wkbLineString:			dmType = DM::EDGE;	break;
		case wkbMultiLineString:	dmType = DM::EDGE;	break;
		default:
			DM::Logger(DM::Debug) << "Geometry type not implemented: " << strType << " (" << ogrType << " )";
			fileok = false;
			return;
		}
		DM::Logger(DM::Debug) << "Found: Geometry type" << strType;

		view = new DM::View(ViewName, dmType, DM::WRITE);

		OGRFeature::DestroyFeature(poFeature);
	}
	else
	{
		DM::Logger(DM::Error) << "Can't find any geometry";
		fileok = false;
		return;
	}

	OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
	for (int iField = 0; iField < poFDefn->GetFieldCount(); iField++)
	{
		OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
		std::string attrName = poFieldDefn->GetNameRef();
		// if existent, attrName will be given the value of attributesToImport[attrName]
		bool exists = map_contains(&attributesToImport, attrName, attrName);
		if (importAll || exists)
		{
			switch (poFieldDefn->GetType())
			{
			case OFTInteger:
				view->addAttribute(attrName, DM::Attribute::DOUBLE, DM::WRITE);
				break;
			case OFTReal:
				view->addAttribute(attrName, DM::Attribute::DOUBLE, DM::WRITE);
				break;
			default:
				view->addAttribute(attrName, DM::Attribute::STRING, DM::WRITE);
				break;
			}
		}
	}
	
	std::vector<DM::View> data;
	if (append)
		data.push_back(DM::View("dummy", SUBSYSTEM, READ));
	data.push_back(*view);
	this->addData("Data", data);
}

void Import::rasterDataInit(GDALDataset  *poDataset)
{
	isvectordata = false;
	append = false;

	double        adfGeoTransform[6];
	int           nBlockXSize, nBlockYSize;
	int           bGotMin, bGotMax;
	double        adfMinMax[2];
	GDALRasterBand  *poBand;

	DM::Logger(DM::Debug)	<< "Driver: " << poDataset->GetDriver()->GetDescription() 
							<< "/" << poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
	DM::Logger(DM::Debug)	<< "Size is " << poDataset->GetRasterXSize() << " " << poDataset->GetRasterYSize() 
							<< " " << poDataset->GetRasterCount();

	if (!std::string(poDataset->GetProjectionRef()).empty())
		DM::Logger(DM::Debug) << "Projection is " << " " << poDataset->GetProjectionRef();
	else
	{
		DM::Logger(DM::Error) << "No projection found";
		fileok = false;
		return;
	}

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		DM::Logger(DM::Debug) << "Origin = "		<< adfGeoTransform[0] << "," << adfGeoTransform[3];
		DM::Logger(DM::Debug) << "Pixel Size = "	<< adfGeoTransform[1] << "," << adfGeoTransform[5];
	}

	poBand = poDataset->GetRasterBand(1);
	poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
	DM::Logger(DM::Debug) << "Block=" << nBlockXSize << "x" << nBlockYSize <<
		"Type=" << GDALGetDataTypeName(poBand->GetRasterDataType()) <<
		", ColorInterp=" << GDALGetColorInterpretationName(poBand->GetColorInterpretation());

	adfMinMax[0] = poBand->GetMinimum(&bGotMin);
	adfMinMax[1] = poBand->GetMaximum(&bGotMax);
	if (!bGotMin || !bGotMax)
		GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

	DM::Logger(DM::Debug) << "Min=" << adfMinMax[0] << ", Max=" << adfMinMax[1];

	if (poBand->GetOverviewCount() > 0)
		DM::Logger(DM::Debug) << "Band has" << poBand->GetOverviewCount() << " overviews";

	if (poBand->GetColorTable() != NULL)
		DM::Logger(DM::Debug) << "Band has a color table with " << poBand->GetColorTable()->GetColorEntryCount() << " entries";

	view = new DM::View(ViewName, DM::RASTERDATA, DM::WRITE);

	std::vector<DM::View> data;
	data.push_back(*view);
	this->addData("Data", data);
}*/

// RUN methods

void Import::run()
{
	nodeList.clear();

	if (!fileok)
		DM::Logger(DM::Error) << "Cannot read file";
	else
	{
		if (isvectordata)	loadVectorData();
		else				importRasterData();
	}
}

void Import::loadVectorData()
{
	DM::System * sys = this->getData("Data");
	int features_before = sys->getAllComponentsInView(*this->view).size();

	if (this->linkWithExistingView)
		this->initPointList(sys);

	OGRSpatialReference *oSourceSRS, *oTargetSRS;

	OGRRegisterAll();

	OGRDataSource *poDS = OGRSFDriverRegistrar::Open(FileName.c_str(), FALSE);
	if (!poDS)
	{
		DM::Logger(DM::Error) << "Open failed.";
		return;
	}

	int i = 0;
	for (; i < poDS->GetLayerCount(); i++)
	{
		OGRLayer *poLayer = poDS->GetLayer(i);

		poLayer->ResetReading();

		loadLayer(poLayer, sys);
	}

	if (i == 0)
		Logger(Error) << "no layer found";

	OGRDataSource::DestroyDataSource(poDS);

#ifdef _DEBUG
	int features_after = sys->getAllComponentsInView(*this->view).size();
	Logger(Debug) << "Loaded featuers " << features_after - features_before;
#endif
}

void Import::loadLayer(OGRLayer* layer, System* sys)
{
	OGRSpatialReference* oSourceSRS;
	OGRSpatialReference* oTargetSRS;
	// GetSpatialRef: The returned object is owned by the OGRLayer and should not be modified or freed by the application.
	oSourceSRS = layer->GetSpatialRef();
	oTargetSRS = new OGRSpatialReference();
	oTargetSRS->importFromEPSG(this->epsgcode);
	// Input spatial reference system objects are assigned by copy
	// (calling clone() method) and no ownership transfer occurs.
	OGRCoordinateTransformation* poCT = OGRCreateCoordinateTransformation(oSourceSRS, oTargetSRS);

	if (!poCT)
		DM::Logger(DM::Warning) << "Unknown transformation to EPSG:" << this->epsgcode;

	while (OGRFeature* poFeature = layer->GetNextFeature())
	{
		OGRFeatureDefn *poFDefn = layer->GetLayerDefn();
		DM::Component * cmp = NULL;
		switch (view->getType())
		{
		case DM::NODE:
			cmp = this->loadNode(sys, poFeature, poCT);
			break;
		case DM::EDGE:
			cmp = this->loadEdge(sys, poFeature, poCT);
			break;
		case DM::FACE:
			cmp = this->loadFace(sys, poFeature, poCT);
			break;
		}
		if (cmp)
			this->appendAttributes(cmp, poFDefn, poFeature);
	}

	if (poCT)
		delete poCT;
}

bool Import::importRasterData()
{
	GDALDataset  *poDataset;
	GDALRasterBand  *poBand;
	double adfGeoTransform[6];
	DM::RasterData * r = this->getRasterData("Data", *view);

	poDataset = (GDALDataset *)GDALOpenShared(FileName.c_str(), GA_ReadOnly);
	poBand = poDataset->GetRasterBand(1);

	int nXSize = poBand->GetXSize();
	int nYSize = poBand->GetYSize();
	double xoff = 0;
	double yoff = 0;
	double xsize = 0;
	double ysize = 0;

	r->setNoValue(-9999999999);

	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		xsize = fabs(adfGeoTransform[1]);
		ysize = fabs(adfGeoTransform[5]);
		xoff = adfGeoTransform[0] + offsetX;
		yoff = adfGeoTransform[3] - ysize * nYSize + offsetY;
	}

	r->setSize(nXSize, nYSize, xsize, ysize, xoff, yoff);

	const int blocksInLine = (nXSize / RASTERBLOCKSIZE + 1);
	// we make scanline big enough to copy from overlapping space
	double *pafScanline = (double *)CPLMalloc(sizeof(double)*blocksInLine*RASTERBLOCKSIZE);

	double* blockLine = new double[RASTERBLOCKSIZE*RASTERBLOCKSIZE*blocksInLine];

	for (int index = 0; index < nYSize; index++)
	{
		poBand->RasterIO(GF_Read, 0, nYSize - index - 1, nXSize, 1, pafScanline, nXSize, 1, GDT_Float64, 0, 0);

		int y = index % RASTERBLOCKSIZE;	// height in block line

		if (y == 0)
			memset(blockLine, 0, sizeof(double)*RASTERBLOCKSIZE*RASTERBLOCKSIZE*blocksInLine);

		for (int x = 0; x<blocksInLine; x++)		// go over length in block line
			memcpy(&blockLine[RASTERBLOCKSIZE*RASTERBLOCKSIZE*x + RASTERBLOCKSIZE*y],
			&pafScanline[RASTERBLOCKSIZE*x], sizeof(double)*RASTERBLOCKSIZE);

		if (y == RASTERBLOCKSIZE - 1 || index == nYSize - 1)
		{
			for (int x = 0; x<blocksInLine; x++)
				r->setBlock(x, index / RASTERBLOCKSIZE, &blockLine[RASTERBLOCKSIZE*RASTERBLOCKSIZE*x]);
		}
	}

	delete blockLine;
	CPLFree(pafScanline);

	return true;
}

DM::Node * Import::addNode(DM::System * sys, double x, double y, OGRCoordinateTransformation *poCT)
{
	transform(&x, &y, poCT);
	x += this->offsetX;
	y += this->offsetY;

	// create key
	DM::Node n_tmp(x, y, 0);
	QString key = this->createHash(x,y);
	std::vector<DM::Node* > * nodes = &nodeList[key];

	foreach (DM::Node * n, *nodes)
		if (n->compare2d(&n_tmp, tol))
			return n;

	DM::Node * res_n = sys->addNode(n_tmp);
	nodes->push_back(res_n);
	return res_n;
}

void Import::appendAttributes(Component *cmp, OGRFeatureDefn *poFDefn, OGRFeature *poFeature)
{
	for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
	{
		OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
		std::string attrName = poFieldDefn->GetNameRef();

		// if existent, attrName will be given the value of attributesToImport[attrName]
		bool exists = map_contains(&attributesToImport, attrName, attrName);

		if (importAll || exists)
		{
			switch(poFieldDefn->GetType())
			{
			case OFTInteger:
				cmp->addAttribute(attrName, (double)poFeature->GetFieldAsInteger(iField));
				break;
			case OFTReal:
				cmp->addAttribute(attrName, poFeature->GetFieldAsDouble(iField));
				break;
			default:
				cmp->addAttribute(attrName, poFeature->GetFieldAsString(iField));
				break;
			}
		}
	}
}

Component *Import::loadNode(System *sys, OGRFeature *poFeature, OGRCoordinateTransformation *poCT)
{
	OGRGeometry *poGeometry = poFeature->GetGeometryRef();
	if( poGeometry == NULL || wkbFlatten(poGeometry->getGeometryType()) != wkbPoint )
		return NULL;

	OGRPoint *poPoint = (OGRPoint*) poGeometry;

	DM::Node * n = this->addNode(sys, poPoint->getX(), poPoint->getY(), poCT);
	sys->addComponentToView(n, *this->view);

	return n;
}

std::vector<Node*> Import::loadNodes(System* sys, OGRLineString *ls, OGRCoordinateTransformation *poCT)
{
	OGRRawPoint* points = new OGRRawPoint[ls->getNumPoints()];
	ls->getPoints(points);

	std::vector<Node*> nlist;
	for(int i=0; i < ls->getNumPoints(); i++)
	{
		DM::Node * n = this->addNode(sys, points[i].x, points[i].y, poCT);

		if(!vector_contains(&nlist, n))
			nlist.push_back(n);
	}

	delete points;

	return nlist;
}

Component *Import::loadLineString(System *sys, OGRLineString *lineString, OGRCoordinateTransformation *poCT)
{
	const std::vector<Node*>& nlist = loadNodes(sys, lineString, poCT);

	if (nlist.size() > 2)
	{
		Edge* firstEdge = sys->addEdge(nlist[0], nlist[1], *this->view);
		for (unsigned int i = 2; i < nlist.size(); i++)
			sys->addEdge(nlist[i - 1], nlist[i], *this->view);

		return firstEdge;
	}
}

Component *Import::loadEdge(System *sys, OGRFeature *poFeature, OGRCoordinateTransformation *poCT)
{
	OGRGeometry *poGeometry = poFeature->GetGeometryRef();
	if (!poGeometry)
		return NULL;

	if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
		return loadLineString(sys, (OGRLineString*)poGeometry, poCT);
	else if (wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString)
	{
		OGRMultiLineString *mpoMultiLine = (OGRMultiLineString*) poGeometry;
		for (int i = 0; i < mpoMultiLine->getNumGeometries(); i++)
		{
			OGRLineString *poPolygon = (OGRLineString*)mpoMultiLine->getGeometryRef(i);
		}
	}
	else
		DM::Logger(DM::Warning) << "geometry of type '" << OGRGeometryTypeToName(poGeometry->getGeometryType())  << "' not supported";

	return NULL;
}

Component *Import::loadFace(System *sys, OGRFeature *poFeature, OGRCoordinateTransformation *poCT)
{
	OGRGeometry *poGeometry = poFeature->GetGeometryRef();
	if(!poGeometry)
		return 0;

	DM::Node * n = 0;
	if( wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon )
	{
		OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
		std::vector<Node*> nlist = loadNodes(sys, (OGRLinearRing*)poPolygon->getExteriorRing(), poCT);

		if (nlist.size() >= 3)
		{
			nlist.push_back(nlist[0]);	// ring closure
			DM::Face * f = sys->addFace(nlist, *this->view);

			// add holes
			for (int i = 0; i < poPolygon->getNumInteriorRings(); i++)
			{
				std::vector<Node*> nl_hole = loadNodes(sys, (OGRLinearRing*)poPolygon->getInteriorRing(i), poCT);
				if (nl_hole.size() >= 3)
				{
					nl_hole.push_back(nl_hole[0]);	// ring closure
					f->addHole(nl_hole);
				}
			}

			return f;
		}
	}
	else if( wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon )
	{
		OGRMultiPolygon *mpoPolygon = (OGRMultiPolygon *) poGeometry;
		int number_of_faces = mpoPolygon->getNumGeometries();
		for (int i = 0; i < number_of_faces; i++)
		{
			OGRPolygon *poPolygon = (OGRPolygon *) mpoPolygon->getGeometryRef(i);
			std::vector<Node*> nlist = loadNodes(sys, (OGRLinearRing*)poPolygon->getExteriorRing(), poCT);

			if (nlist.size() >= 3)
			{
				nlist.push_back(nlist[0]);	// ring closure
				return sys->addFace(nlist, *this->view);
			}
		}
	}

	return NULL;
}

void Import::initPointList(System *sys)
{
	nodeList.clear();
	double v[3];
	foreach(Node* n, sys->getAllNodes())
	{
		n->get(v);
		QString key = this->createHash(v[0], v[1]);
		nodeList[key].push_back(n);
	}
}

QString Import::createHash(double x, double y)
{
	int ix = (int) x / devider;
	int iy = (int) y / devider;
	return QString::number(ix) + "|" +  QString::number(iy);
}

bool Import::createInputDialog()
{
	QWidget * w = new GUIImport(this);
	w->show();
	return true;
}

string Import::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/Import.md";
}

bool Import::transform(double *x, double *y, OGRCoordinateTransformation *poCT)
{
	if (this->driverType == WFS && this->flip_wfs)
	{
		double tmp_x = *x;
		*x = *y;
		*y = tmp_x;
	}

	if(!transformok)
		return false;

	if( poCT == NULL || !poCT->Transform( 1, x, y ) )
		return false;

	return true;
}

