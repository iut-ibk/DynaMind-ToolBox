#ifndef DRAINAGEHELPER_H
#define DRAINAGEHELPER_H

#include <dmcompilersettings.h>
#include <dm.h>
#include "swmmwriteandread.h"

class DM_HELPER_DLL_EXPORT DrainageHelper
{
public:
	/** @brief Write SWMM results in output file for further analysis
	 *
	 */
	static void WriteOutputFiles(std::string filename, DM::System * sys, SWMMWriteAndRead &swmmreeadfile, std::map<std::string, std::string> additional);

	static void CreateEulerRainFile(double duration, double deltaT, double return_period, double cf, string rfile);
};

#endif // DRAINAGEHELPER_H
