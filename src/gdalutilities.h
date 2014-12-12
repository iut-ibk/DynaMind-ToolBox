#ifndef GDALUTILITIES_H
#define GDALUTILITIES_H

#include <dmattribute.h>
#include <dm.h>

class OGRFeatureDefn;
class OGRFieldDefn;

namespace DM {
	class DM_HELPER_DLL_EXPORT GDALUtilities
	{
	public:
		static int DMToOGRGeometry(int dm_geometry);
		static int OGRtoDMGeometry(OGRFeatureDefn * def);
		static DM::Attribute::AttributeType OGRToDMAttribute(OGRFieldDefn *fdef);
	};
}


#endif // GDALUTILITIES_H
