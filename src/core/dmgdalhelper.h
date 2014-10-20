#ifndef DMGDALHELPER_H
#define DMGDALHELPER_H

#include <dmcompilersettings.h>

#include <ogrsf_frmts.h>
#include <vector>
#include <string>


namespace DM
{
class DM_HELPER_DLL_EXPORT DMFeature
{
public:
	static bool SetDoubleList(OGRFeature * f, const std::string & name, const std::vector<double> & values);

	static bool GetDoubleList(OGRFeature *f, const std::string & name, std::vector<double> & values);
};
}

#endif // DMGDALHELPER_H
