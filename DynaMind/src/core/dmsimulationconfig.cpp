#include "dmsimulationconfig.h"

#include "dmlogger.h"

#include <QDir>

namespace DM {

std::string SimulationConfig::getWorkingDir() const
{
	return workingDir;
}

void SimulationConfig::setWorkingDir(const std::string &value)
{
	QDir dir(QString::fromStdString(value));
	if (!dir.exists()) {
		DM::Logger(DM::Error) << "Failed to set working directory to " << value << " does not exist";
		return;
	}
	workingDir = value;

}

std::string SimulationConfig::getDefaultModulePath()
{
	QString currentPath = QDir::currentPath();
	// Default path if run form compiled version
	if (currentPath.contains("/output")) {
		return currentPath.toStdString();
	}

	// asume install in /usr/bin
	std::cout << currentPath.toStdString() << "/../share/DynaMind" << std::endl;
	return currentPath.toStdString() + "/../share/DynaMind";
}

bool SimulationConfig::getKeepSystems() const
{
	return keepSystems;
}

void SimulationConfig::setKeepSystems(bool value)
{
	keepSystems = value;
}



}
