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
public:
	SimulationConfig() : CoordinateSystem(0), workingDir(QString(QDir::tempPath()+"/dynamind").toStdString()), keepSystems(false){}
	void setCoordinateSystem(int epsgCode) {CoordinateSystem = epsgCode;}
	int getCoorindateSystem() const {return this->CoordinateSystem;}
	bool getKeepSystems() const;
	void setKeepSystems(bool value);
	std::string getWorkingDir() const;
	void setWorkingDir(const std::string &value);
	std::string getDefaultModulePath();
	std::string getDefaultLibraryPath();
};

}


#endif // DMSIMULATIONCONFIG_H
