#ifndef CGALGEOMETRYHELPER_H
#define CGALGEOMETRYHELPER_H

class CGALGeometryHelper
{
public:
	//CGALGeometryHelper();
	static double percentageFilled(const unsigned char *ogr_poly);
	static double aspectRationBB(const unsigned char *wkt_geo);
};

#endif // CGALGEOMETRYHELPER_H
