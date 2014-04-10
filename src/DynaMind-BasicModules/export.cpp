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
#include "guiexport.h"
#include <dmcomponent.h>
#include <dmlogger.h>
#include <dmsystem.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmface.h>

DM_DECLARE_NODE_NAME(Export, Modules)

#define INPORT "inport"
#define DEFAULT_EPSG "31254"

Export::Export()
{
	this->epsgCode = 31254;
	this->addParameter("Filename", FILENAME, &this->path);
	this->addParameter("epsgCode", INT, &this->epsgCode);
	this->addParameter("viewConfig", STRING_MAP, &this->viewConfig);
	this->addParameter("viewEPSGConfig", DM::STRING_MAP, &this->viewEPSGConfig);
}

Export::~Export()
{
}

void Export::init()
{	
	std::map<std::string, View> viewsInStream = getViewsInStream()[INPORT];
	// add missing views
	mforeach(const View& v, viewsInStream)
	{
		if (!map_contains(&viewConfig, v.getName()))
		{
			Components viewType = (Components)v.getType();
			if (viewType == RASTERDATA || viewType == COMPONENT || viewType == SUBSYSTEM)
				continue;

			viewConfig[v.getName()] = "";
			foreach(const std::string& attrName, v.getAllAttributes())
			{
				Attribute::AttributeType attType = v.getAttributeType(attrName);
				if (attType == Attribute::NOTYPE || attType == Attribute::TIMESERIES 
					|| attType == Attribute::LINK || attType == Attribute::STRINGVECTOR)
					continue;

				viewConfig[v.getName() + "." + attrName] = "";
				viewConfigTypes[v.getName() + "." + attrName] = attType;
			}
		}
	}
	// add types
	mforeach(const View& v, viewsInStream)
	{
		viewConfigTypes[v.getName()] = v.getType();
		viewEPSGConfig[v.getName()] = DEFAULT_EPSG;
		foreach(const std::string& attrName, v.getAllAttributes())
			viewConfigTypes[v.getName() + "." + attrName] = v.getAttributeType(attrName);
	}
	// remove obsolete views
	if (viewsInStream.size() != 0)
	{
		StringMap newViewConfig = viewConfig;

		for (StringMap::iterator it = viewConfig.begin(); it != viewConfig.end(); ++it)
		{
			// split if we have a separator, otherwhise the string stays untouched
			std::string viewName = QString::fromStdString(it->first).split('.').first().toStdString();

			if (!map_contains(&viewsInStream, viewName))
			{
				newViewConfig.erase(it->first);
				viewConfigTypes.erase(it->first);
			}
		}

		viewConfig = newViewConfig;
	}
			
	initViews();
}

void Export::initViews()
{
	std::map<std::string, View> views;
	std::map<std::string, View> viewsInStream = getViewsInStream()[INPORT];
	if (viewsInStream.size() != 0)
	{
		for (StringMap::const_iterator it = viewConfig.begin(); it != viewConfig.end(); ++it)
		{
			if (!it->second.empty())
			{
				if (strchr(it->first.c_str(), '.') == NULL)
				{
					const View& originalView = viewsInStream[it->first];
					views[it->first] = View(it->first, originalView.getType(), originalView.getAccessType());
				}
			}
		}

		for (StringMap::const_iterator it = viewConfig.begin(); it != viewConfig.end(); ++it)
		{
			if (!it->second.empty())
			{
				if (strchr(it->first.c_str(), '.') != NULL)
				{
					QStringList viewAttributePair = QString::fromStdString(it->first).split(".");
					if (viewAttributePair.size() != 2)
					{
						Logger(Error) << "importer: viewConfig corrupt";
						return;
					}
					const View& originalView = viewsInStream[viewAttributePair.first().toStdString()];
					View& view = views[viewAttributePair.first().toStdString()];
					std::string attName = viewAttributePair.last().toStdString();
					view.addAttribute(attName,
						originalView.getAttributeType(attName),
						originalView.getAttributeAccessType(attName));
				}
			}
		}
	}
	std::vector<View> vviews;
	mforeach(const View& v, views)
		vviews.push_back(v.clone(DM::READ));

	if (vviews.size() == 0)
		vviews.push_back(View("dummy", 0, READ));

	addData(INPORT, vviews);
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
	// get layer name
	const char* newLayerName = viewConfig[view.getName()].c_str();
	if (strlen(newLayerName) == 0)
		return NULL;

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
	OGRErr epsgError = OGRERR_NONE;
	int epsg = atoi(viewEPSGConfig[view.getName()].c_str());
	OGRLayer* layer = data->GetLayerByName(newLayerName);
	if (!layer)
	{
		// spat ref for coord sys with desired epsg code
		OGRSpatialReference spatRef;
		epsgError = spatRef.importFromEPSG(epsg);
		if (!epsgError)
		{
			// create layer
			layer = data->CreateLayer(newLayerName, &spatRef, geomType);
			if (!layer)
			{
				Logger(Error) << "export: creating layer from view '" << view.getName() << "' failed.";
				return NULL;
			}
		}
	}
	else
	{
		Logger(Warning) << "export: overwriting existing layer '" 
			<< newLayerName << "' with data from view '" << view.getName() << "'";
		epsgError = layer->GetSpatialRef()->importFromEPSG(epsg);
	}

	if (epsgError)
	{
		Logger(Error) << "export: invalid epsg code '" << epsg << "' for exporting view '" << view.getName() << "'";
		return NULL;
	}


	// add attribute fields
	foreach(const std::string& attName, view.getAllAttributes())
	{
		// get name
		std::string searchString = view.getName() + "." + attName;
		const char* newAttrName = viewConfig[searchString].c_str();
		if (strlen(newAttrName) == 0)
			continue;
		// get type
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
		
		OGRFieldDefn fieldDef(newAttrName, fieldType);
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

				if (ring.getNumPoints() < 3)
					continue;

				// ring closure
				OGRPoint first;
				ring.getPoint(0, &first);
				ring.addPoint(&first);

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
			const char* newAttrName = viewConfig[view.getName() + "." + a->getName()].c_str();
			if (strlen(newAttrName) != 0)
			{
				switch (a->getType())
				{
				case Attribute::DOUBLE:
					feat.SetField(newAttrName, a->getDouble());
					break;
				case Attribute::DOUBLEVECTOR:
				{
					const std::vector<double>& values = a->getDoubleVector();
					feat.SetField(newAttrName, values.size(), const_cast<double*>(&values.front()));
				}
					break;
				case Attribute::STRING:
					feat.SetField(newAttrName, a->getString().c_str());
					break;
					//case Attribute::STRINGVECTOR:
					//	break;
				}
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
		Logger(Debug) << "preparing view '" << view.getName() << "'";
		OGRLayer* layer = prepareNewLayer(view, data);
		if (!layer)	// unsupported type or error
			continue;

		Logger(Debug) << "exporting layer '" << viewConfig[view.getName()] << "'";
		exportLayer(view, layer, system);
		Logger(Debug) << "finished layer " << ++i << "/" << views.size();
	}

	// cleanup
	delete data;
}

bool Export::createInputDialog()
{
	QWidget * w = new GUIExport(this);
	w->show();
	return true;
}