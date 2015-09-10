#ifndef CGALGEOMETRYHELPER_H
#define CGALGEOMETRYHELPER_H

#include <dmcompilersettings.h>

class DM_HELPER_DLL_EXPORT CGALGeometryHelper
{
public:
	//CGALGeometryHelper();
	static double percentageFilled(const unsigned char *ogr_poly);
	static double aspectRationBB(const unsigned char *wkt_geo);
};

#endif // CGALGEOMETRYHELPER_H
