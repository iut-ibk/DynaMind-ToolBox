#ifndef DMSIMULATIONCONFIG_H
#define DMSIMULATIONCONFIG_H

#include "dmcompilersettings.h"

#include <QString>
#include <QDir>


namespace DM {
class DM_HELPER_DLL_EXPORT SimulationConfig {
private:
	int CoordinateSystem; //As EPSG Code
	std::string workingDir;
	bool keepSystems;
	std::string spatialiteLocation;
	std::string defaultModulePath;
public:
	SimulationConfig();
	void setCoordinateSystem(int epsgCode) {CoordinateSystem = epsgCode;}
	int getCoorindateSystem() const {return this->CoordinateSystem;}
	bool getKeepSystems() const;
	void setKeepSystems(bool value);
	std::string getWorkingDir() const;
	void setWorkingDir(const std::string &value);
	std::string getDefaultModulePath();
	void setDefaultModulePath(const std::string & path);
	std::string getDefaultLibraryPath();
	std::string getSpatialiteModuleLocation();
	void setSpatialiteModuleLocation(const std::string & spatialiteLocation );

};

}


#endif // DMSIMULATIONCONFIG_H
