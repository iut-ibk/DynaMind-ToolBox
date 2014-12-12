#include "gdalutilities.h"
#include "dm.h"

#include <ogrsf_frmts.h>

namespace DM {

int GDALUtilities::DMToOGRGeometry(int dm_geometry)
{
	int type = wkbNone;
	switch(dm_geometry)
	{
	case DM::NODE:
		type = wkbPoint;
		break;
	case DM::FACE:
		type = wkbPolygon;
		break;
	case DM::EDGE:
		type = wkbLineString;
		break;
	case  DM::COMPONENT:
		type = wkbNone;
		break;
	}
	return type;
}

int GDALUtilities::OGRtoDMGeometry(OGRFeatureDefn *def)
{
	int type = -1;
	OGRwkbGeometryType ogrType = def->GetGeomType();
	std::string strType = OGRGeometryTypeToName(ogrType);
	switch(wkbFlatten(ogrType))
	{
	case wkbPoint:
		type = DM::NODE;
		break;
	case wkbPolygon:
		type = DM::FACE;
		break;
	case wkbMultiPolygon:
		type = DM::FACE;
		break;
	case wkbLineString:
		type = DM::EDGE;
		break;
	case wkbMultiLineString:
		type = DM::EDGE;
		break;
	case wkbNone:
		type = DM::COMPONENT;
		break;
	default:
		DM::Logger(DM::Warning) << "Geometry type not implemented: " << strType;
		return -1;
	}
	DM::Logger(DM::Debug) << "Found: Geometry type " << strType;
	return type;
}

DM::Attribute::AttributeType GDALUtilities::OGRToDMAttribute(OGRFieldDefn * fdef) {
	DM::Attribute::AttributeType type = DM::Attribute::NOTYPE;
	switch (fdef->GetType()) {
	case OFTInteger:
		type = DM::Attribute::INT;
		break;
	case OFTReal:
		type = DM::Attribute::DOUBLE;
		break;
	case OFTString:
		type = DM::Attribute::STRING;
		break;
	default:
		DM::Logger(DM::Warning) << "Type not supported attributed " << fdef->GetNameRef() << " not loaded";
		break;
	}
	return type;
}

}

