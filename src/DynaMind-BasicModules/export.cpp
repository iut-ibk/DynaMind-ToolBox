/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2015 Markus Sengthaler

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

#include "export.h"
#include "ogrsf_frmts.h"
#include "gdal_priv.h"
#include <dmcomponent.h>
#include <dmlogger.h>
#include <dmsystem.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmface.h>

DM_DECLARE_NODE_NAME(Export, Modules)

#define INPORT "inport"

Export::Export()
{
	this->epsgCode = 31254;
	this->addParameter("Filename", FILENAME, &this->path);
	this->addParameter("epsgCode", INT, &this->epsgCode);
}

Export::~Export()
{
}

void Export::init()
{
	std::vector<View> views;

	foreach(const View& v, getViewsInStream(INPORT))
		views.push_back(v.clone(DM::READ));

	if (views.size() == 0)
		views.push_back(View("dummy", 0, READ));

	addData(INPORT, views);
}

void Export::reset()
{
}

static std::string attributeTypeStrings[] =
{
	"NOTYPE",
	"DOUBLE",
	"STRING",
	"TIMESERIES",
	"LINK",
	"DOUBLEVECTOR",
	"STRINGVECTOR"
};

OGRLayer* Export::prepareNewLayer(const DM::View& view, OGRDataSource* data)
{
	// get geom type
	OGRwkbGeometryType geomType;

	switch (view.getType())
	{
	case RASTERDATA:
		Logger(Error) << "Rasterdata currently not supported by exporter";
		return NULL;
	case SUBSYSTEM:
		Logger(Error) << "Subsystems currently not supported by exporter";
		return NULL;
	case COMPONENT:
		Logger(Error) << "Raw components currently not supported by exporter";
		return NULL;
	case NODE:
		geomType = wkbPoint;
		break;
	case EDGE:
		geomType = wkbLineString;
		break;
	case FACE:
		geomType = wkbPolygon;
		break;
	}

	// spat ref for coord sys with desired epsg code
	OGRSpatialReference spatRef;
	spatRef.importFromEPSG(epsgCode);

	// create layer
	OGRLayer* layer = data->CreateLayer(view.getName().c_str(), &spatRef, geomType);
	// add attribute fields
	foreach(const std::string& attName, view.getAllAttributes())
	{
		OGRFieldType fieldType;
		switch (view.getAttributeType(attName))
		{
		case Attribute::DOUBLE:
			fieldType = OGRFieldType::OFTReal;
			break;
		case Attribute::DOUBLEVECTOR:
			fieldType = OGRFieldType::OFTRealList;
			break;
		case Attribute::STRING:
			fieldType = OGRFieldType::OFTString;
			break;
		//case Attribute::STRINGVECTOR:
		//	fieldType = OGRFieldType::OFTStringList;
		//	break;
		default:
			Logger(Warning) << "Attribute of type " << attributeTypeStrings[view.getAttributeType(attName)]
				<< " not supported";
			continue;
		}

		OGRFieldDefn fieldDef(attName.c_str(), fieldType);
		layer->CreateField(&fieldDef);
	}
	return layer;
}

void Export::exportLayer(const DM::View& view, OGRLayer* layer, System* system)
{
	OGRFeatureDefn* featDef = layer->GetLayerDefn();
	OGRwkbGeometryType geomType = layer->GetGeomType();

	foreach(Component* cmp, system->getAllComponentsInView(view))
	{
		OGRFeature feat(featDef);
		// add geometry
		switch (geomType)
		{
		case wkbPoint:
			{
				Node* n = (Node*)cmp;
				OGRPoint point(n->getX(), n->getY(), n->getZ());
				feat.SetGeometry(&point);
			}
			break;
		case wkbLineString:
			{
				Edge* e = (Edge*)cmp;
				OGRLineString line;

				Node* n = e->getStartNode();
				line.addPoint(n->getX(), n->getY(), n->getZ());
				n = e->getEndNode();
				line.addPoint(n->getX(), n->getY(), n->getZ());

				feat.SetGeometry(&line);
			}
			break;
		case wkbPolygon:
			{
				Face* f = (Face*)cmp;
				OGRPolygon poly;
				OGRLinearRing ring;

				foreach(Node* n, f->getNodePointers())
					ring.addPoint(n->getX(), n->getY(), n->getZ());

				poly.addRing(&ring);

				feat.SetGeometry(&poly);
			}
			break;
		default:
			continue;
		}
		// add attributes
		foreach(Attribute* a, cmp->getAllAttributes())
		{
			switch (a->getType())
			{
			case Attribute::DOUBLE:
				feat.SetField(a->getName().c_str(), a->getDouble());
				break;
			case Attribute::DOUBLEVECTOR:
				{
					const std::vector<double>& values = a->getDoubleVector();
					feat.SetField(a->getName().c_str(), values.size(), const_cast<double*>(&values.front()));
				}
				break;
			case Attribute::STRING:
				feat.SetField(a->getName().c_str(), a->getString().c_str());
				break;
			//case Attribute::STRINGVECTOR:
			//	break;
			}
		}

		feat.SetFID(0);
		layer->CreateFeature(&feat);
	}
}

void Export::run()
{
	// get datastream
	System* system = getData(INPORT);

	// init gdal
	OGRRegisterAll();
	GDALAllRegister();	// neccessary for windows!
	OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
	
	// create file
	OGRSFDriver* driver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
	OGRDataSource* data = driver->CreateDataSource(path.c_str());

	if (!driver)
	{
		Logger(Error) << "Error in OGR: ESRI Shapefile driver not found";
		return;
	}
	else if (!data)
	{
		Logger(Error) << "Error in OGR: can't create data source at '" << path << "'";
		return;
	}
	// get views to export
	std::map<std::string, View> views = getAccessedViews()[INPORT];

	// finally write
	int i = 0;
	mforeach(const View& view, views)
	{
		Logger(Debug) << "preparing layer '" << view.getName() << "'";
		OGRLayer* layer = prepareNewLayer(view, data);
		if (!layer)	// unsupported type
			continue;

		Logger(Debug) << "exporting layer '" << view.getName() << "'";
		exportLayer(view, layer, system);
		Logger(Debug) << "finished layer " << ++i << "/" << views.size();
	}

	// cleanup
	delete data;
}
