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

#define OUTPORT_NAME "data"
#define RASTERVIEWNAME "RasterData"

#define UNKNOWN_TRAFO_STRING "<unknown>"

Import::Import()
{
	driverType = DataError;

	append = false;

	this->FileName = "";
	this->FileName_old = "";
	this->addParameter("Filename", DM::FILENAME, &this->FileName);
	this->epsgcode=31254;
	this->addParameter("Transform to EPSG:", DM::INT, &this->epsgcode);
	this->tol = 0.00001;
	this->addParameter("Tolerance", DM::DOUBLE, &this->tol);
	devider = 100;
	this->append = false;
	this->addParameter("AppendToExisting", DM::BOOL, &this->append);
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
}

Import::~Import()
{
	reset();
}

void Import::reset()
{
}

// INIT methods

void Import::init()
{
	if (FileName_old != FileName)
	{
		FileName_old = FileName;
		reloadFile();
	}
}

void Import::reloadFile()
{
	driverType = DataError;

	OGRRegisterAll();
	GDALAllRegister();	// neccessary for windows!
	OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();

	/*if (!this->WFSServer.empty())
	{
		// password
		SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023));
		QString pwd = crypto.decryptToString(QString::fromStdString(this->WFSPassword));

		// create server url with login
		std::string server_full_name = "WFS:http://" + this->WFSUsername + ":" + pwd.toStdString() + "@" + this->WFSServer;

		OGRDataSource *poDS = OGRSFDriverRegistrar::Open(server_full_name.c_str(), FALSE);

		if (!poDS)
			return;

		OGRLayer* poLayer = this->LoadWFSLayer(poDS, server_full_name);
		if (!poLayer)
			return;

		driverType = WFS;

		this->vectorDataInit(poLayer);
		OGRDataSource::DestroyDataSource(poDS);
	}
	else*/
	{
		if (FileName.empty())
		{
			DM::Logger(DM::Error) << "No file specified " << FileName;
			return;
		}

		if (OGRDataSource* poDS = OGRSFDriverRegistrar::Open(FileName.c_str(), FALSE))
		{
			driverType = ShapeFile;

			StringMap newViewConfig;
			std::map<std::string, int> newViewConfigTypes;
			StringMap newEPSGCodes;

			if (!ExtractLayers(poDS, newViewConfig, newViewConfigTypes, newEPSGCodes, this->epsgcode))
				driverType = DataError;

			adoptViewConfig(newViewConfig, newViewConfigTypes);

			// transfer any previously made epsg setting
			for (StringMap::iterator it = newEPSGCodes.begin(); it != newEPSGCodes.end(); ++it)
			{
				if (it->second == UNKNOWN_TRAFO_STRING)
				{
					std::string value;
					if (map_contains(&viewEPSGConfig, it->first, value) && value != UNKNOWN_TRAFO_STRING)
						newEPSGCodes[it->first] = value;
				}
			}
			viewEPSGConfig = newEPSGCodes;

			initViews();
			OGRDataSource::DestroyDataSource(poDS);
		}
		else
		{
			GDALDataset  *poDataset = (GDALDataset*)GDALOpenShared(FileName.c_str(), GA_ReadOnly);
			if (poDataset == NULL)
				DM::Logger(DM::Error) << "Open failed.";
			else
			{
				driverType = RasterData;
				append = false;

				StringMap newViewConfig;
				std::map<std::string, int> newViewConfigTypes;

				if(!ExtractLayers(poDataset, newViewConfig, newViewConfigTypes))
					driverType = DataError;

				adoptViewConfig(newViewConfig, newViewConfigTypes);

				initViews();
			}
		}
	}
}

OGRCoordinateTransformation* getTrafo(OGRLayer* layer, int targetEPSG, int sourceEPSG = 0)
{
	OGRSpatialReference* oSourceSRS;
	OGRSpatialReference* oTargetSRS;
	// GetSpatialRef: The returned object is owned by the OGRLayer and should not be modified or freed by the application.
	oSourceSRS = layer->GetSpatialRef();
	if (sourceEPSG != 0)
		oSourceSRS->importFromEPSG(targetEPSG);

	oTargetSRS = new OGRSpatialReference();
	oTargetSRS->importFromEPSG(targetEPSG);
	// Input spatial reference system objects are assigned by copy
	// (calling clone() method) and no ownership transfer occurs.
	return OGRCreateCoordinateTransformation(oSourceSRS, oTargetSRS);
}


bool Import::ExtractLayers(OGRDataSource* dataSource, StringMap& viewConfig, 
	std::map<std::string, int>& viewConfigTypes, StringMap& viewEPSGConfig,
	int targetEPSG)
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
			DM::Logger(DM::Debug) << "Geometry type not implemented: " << strType << " (" << (int)ogrType << " )";
			return false;
		}
		DM::Logger(DM::Debug) << "Found: Geometry type" << strType;

		// add epsg entry

		if (OGRCoordinateTransformation* trafo = getTrafo(layer, targetEPSG))
			delete trafo;
		else
			viewEPSGConfig[viewName] = UNKNOWN_TRAFO_STRING;

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

	return true;
}

bool Import::ExtractLayers(GDALDataset* dataSource, StringMap& viewConfig, std::map<std::string, int>& viewConfigTypes)
{
	viewConfig.clear();
	viewConfigTypes.clear();

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
		return false;
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
	viewConfig[RASTERVIEWNAME] = RASTERVIEWNAME;
	viewConfigTypes[RASTERVIEWNAME] = DM::RASTERDATA;

	return true;
}

void Import::adoptViewConfig(StringMap& newViewConfig, std::map<std::string, int>& newViewConfigTypes)
{
	for (StringMap::iterator it = newViewConfig.begin(); it != newViewConfig.end(); it++)
	{
		// if there is already a config for this view/attribute and the type is the same, take it
		if (map_contains(&this->viewConfig, it->first))
			it->second = this->viewConfig[it->first];
	}

	viewConfig = newViewConfig;
	viewConfigTypes = newViewConfigTypes;
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
				views[viewName].addAttribute(it->second,
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

	addData(OUTPORT_NAME, vviews);
}

bool Import::moduleParametersChanged()
{
	bool changed = false;
	if (FileName_old != FileName)		changed = true; FileName_old = FileName;
	if (WFSDataName_old != WFSDataName) changed = true; WFSDataName_old = WFSDataName;
	if (WFSServer_old != WFSServer)		changed = true; WFSServer_old = WFSServer;
	if (WFSUsername_old != WFSUsername) changed = true; WFSUsername_old = WFSUsername;
	if (WFSPassword_old != WFSPassword) changed = true; WFSPassword_old = WFSPassword;
	if (append_old != append)			changed = true; append_old = append;

	return changed;
}

OGRLayer *Import::LoadWFSLayer(OGRDataSource *poDS, const std::string& server_full_name)
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

// RUN methods

void Import::run()
{
	nodeList.clear();

	switch (driverType)
	{
	case DataError:
		DM::Logger(DM::Error) << "Cannot read file/data";
		break;
	case ShapeFile:
	case WFS:
		loadVectorData();
		break;
	case RasterData:
		loadRasterData();
		break;
	}
}

void Import::loadVectorData()
{
	DM::System * sys = this->getData(OUTPORT_NAME);

#ifdef _DEBUG
	int features_before = sys->getAllChilds().size();
#endif

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
	int features_after = sys->getAllChilds().size();
	Logger(Debug) << "Loaded featuers " << features_after - features_before;
#endif
}

void Import::loadLayer(OGRLayer* layer, System* sys)
{
	const std::string viewName = viewConfig[layer->GetName()];
	if (viewName.empty())
		return;

	int sourceEPSG = 0;
	std::string value;
	if (map_contains(&viewEPSGConfig, viewName, value))
	{
		sourceEPSG = atoi(value.c_str());
		DM::Logger(DM::Debug) << "Overriding source trafo to " << sourceEPSG;
	}
		
	OGRCoordinateTransformation* poCT = getTrafo(layer, this->epsgcode, sourceEPSG);
	
	if (!poCT)
		DM::Logger(DM::Warning) << "Unknown transformation to EPSG:" << this->epsgcode;

	const DM::View view = getAccessedViews()[OUTPORT_NAME][viewName];
	const DM::Components dmType = (DM::Components)view.getType();

	OGRFeatureDefn *layerDef = layer->GetLayerDefn();

	while (OGRFeature* poFeature = layer->GetNextFeature())
	{
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();

		switch (wkbFlatten(poGeometry->getGeometryType()))
		{
		case wkbPoint:
			if (dmType == DM::NODE)
				loadPoint(	sys, (OGRPoint*)poGeometry, 
							poCT, layerDef, poFeature, view);
			break;
		case wkbLineString:
			if (dmType == DM::EDGE)
				loadLineString(	sys, (OGRLineString*)poGeometry, 
								poCT, layerDef, poFeature, view);
			break;
		case wkbMultiLineString:
			if (dmType == DM::EDGE)
			{
				OGRMultiLineString *mpoMultiLine = (OGRMultiLineString*)poGeometry;
				for (int i = 0; i < mpoMultiLine->getNumGeometries(); i++)
					loadLineString(	sys, (OGRLineString*)mpoMultiLine->getGeometryRef(i), 
									poCT, layerDef, poFeature, view);
			}
			break;
		case wkbPolygon:
			if (dmType == DM::FACE)
				loadPolygon(sys, (OGRPolygon*)poGeometry, 
							poCT, layerDef, poFeature, view);
		case wkbMultiPolygon:
			if (dmType == DM::FACE)
			{
				OGRMultiPolygon *mpoMultiPoly = (OGRMultiPolygon*)poGeometry;
				for (int i = 0; i < mpoMultiPoly->getNumGeometries(); i++)
					loadPolygon(sys, (OGRPolygon*)mpoMultiPoly->getGeometryRef(i), 
								poCT, layerDef, poFeature, view);
			}
			break;
		}
	}

	if (poCT)
		delete poCT;
}

void Import::loadPoint(System *sys, OGRPoint *point, OGRCoordinateTransformation *poCT,
	OGRFeatureDefn* featureDef, OGRFeature* curFeature, const DM::View& view)
{
	Node* n = this->addNode(sys, point->getX(), point->getY(), poCT, &view);
	// add attributes
	this->appendAttributes(n, featureDef, curFeature, view);
}

void Import::loadLineString(System *sys, OGRLineString *lineString, OGRCoordinateTransformation *poCT,
	OGRFeatureDefn* featureDef, OGRFeature* curFeature, const DM::View& view)
{
	const int nPoints = lineString->getNumPoints();
	if (nPoints < 2)	// line with one or zero points won't make sense
		return;

	double* x = new double[nPoints];
	double* y = new double[nPoints];
	Node** nodes = new Node*[nPoints];

	lineString->getPoints(x, sizeof(double), y, sizeof(double));

	nodes[0] = this->addNode(sys, x[0], y[0], poCT);
	for (int i = 1; i < nPoints; i++)
	{
		nodes[i] = this->addNode(sys, x[i], y[i], poCT);
		Edge* e = sys->addEdge(nodes[i - 1], nodes[i]);
		// add attributes
		this->appendAttributes(e, featureDef, curFeature, view);
		// assign to view
		sys->addComponentToView(e, view);
	}

	delete[] nodes;
	delete[] x;
	delete[] y;
}

std::vector<Node*> Import::addFaceNodes(System* sys, const OGRLineString *ring, OGRCoordinateTransformation *poCT)
{
	const int nPoints = ring->getNumPoints();
	if (nPoints < 3)	// poly with less then 3 points doesn't make sense
		return std::vector<Node*>();

	double* x = new double[nPoints];
	double* y = new double[nPoints];
	ring->getPoints(x, sizeof(double), y, sizeof(double));

	std::vector<Node*> nodes;
	nodes.resize(nPoints+1);

	for (int i = 1; i < nPoints; i++)
		nodes[i] = this->addNode(sys, x[i], y[i], poCT);

	// ring closure
	nodes[nPoints] = this->addNode(sys, x[0], y[0], poCT);

	delete[] x;
	delete[] y;

	return nodes;
}

void Import::loadPolygon(System *sys, OGRPolygon *polygon, OGRCoordinateTransformation *poCT,
	OGRFeatureDefn* featureDef, OGRFeature* curFeature, const DM::View& view)
{
	// get nodes
	const OGRLinearRing* outerRing = polygon->getExteriorRing();
	const std::vector<Node*>& nodes = addFaceNodes(sys, outerRing, poCT);
	if (nodes.size() < 3)
		return;

	if (Face* f = sys->addFace(nodes, view))
	{
		// add holes
		const int nHoles = polygon->getNumInteriorRings();
		for (int i = 0; i < nHoles; i++)
		{
			const std::vector<Node*>& hole_nodes = addFaceNodes(sys, polygon->getInteriorRing(i), poCT);
			Face* h = sys->addFace(hole_nodes);
			f->addHole(h);
		}

		// add attributes
		this->appendAttributes(f, featureDef, curFeature, view);
	}
}

bool Import::loadRasterData()
{
	GDALDataset  *poDataset;
	GDALRasterBand  *poBand;
	double adfGeoTransform[6];
	DM::RasterData * r = this->getRasterData(OUTPORT_NAME, View(RASTERVIEWNAME, DM::RASTERDATA, DM::WRITE));

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

DM::Node * Import::addNode(DM::System * sys, double x, double y, OGRCoordinateTransformation *poCT, const View* view)
{
	transform(&x, &y, poCT);
	x += this->offsetX;
	y += this->offsetY;

	// create key
	Node* new_node = new Node(x, y, 0);
	QString key = this->createHash(x, y);
	std::vector<DM::Node* > * nodes = &nodeList[key];

	foreach(DM::Node * n, *nodes)
	{
		if (n->compare2d(new_node, tol))
		{
			delete new_node;

			if (view)
				sys->addComponentToView(n, *view);

			return n;
		}
	}
	// add to system
	if (view)
		sys->addNode(new_node, *view);
	else
		sys->addNode(new_node);

	// add to search list
	nodes->push_back(new_node);

	return new_node;
}

void Import::appendAttributes(Component *cmp, OGRFeatureDefn *poFDefn, OGRFeature *poFeature, const View& view)
{
	for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
	{
		OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

		std::string attrName = poFieldDefn->GetNameRef();
		attrName = viewConfig[view.getName() + "." + attrName];

		if (!attrName.empty())
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

	if( poCT == NULL || !poCT->Transform( 1, x, y ) )
		return false;

	return true;
}

