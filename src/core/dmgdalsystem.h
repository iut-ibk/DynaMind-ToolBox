#ifndef DMGDALSYSTEM_H
#define DMGDALSYSTEM_H


#include <dmviewcontainer.h>
#include <dmisystem.h>
#include <ogr_api.h>

#include <map>
#include <vector>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

class DM_HELPER_DLL_EXPORT OGRDataSource;
class DM_HELPER_DLL_EXPORT OGRSFDriver;
class DM_HELPER_DLL_EXPORT OGRLayer;
class DM_HELPER_DLL_EXPORT OGRFeature;

namespace DM {
class DM_HELPER_DLL_EXPORT GDALSystem : public ISystem
{
public:
	GDALSystem();

	GDALSystem(const GDALSystem& s);

	void syncAlteredFeatures(const DM::View & v, std::vector<OGRFeature *> & df);

	void syncNewFeatures(const DM::View & v, std::vector<OGRFeature *> & df);

	void updateView(const View &v);

	void updateViewContainer(DM::ViewContainer v);

	OGRFeature *createFeature(const DM::View & v);

	OGRLayer *getOGRLayer(const DM::View & v);

	OGRDataSource *getDataSource();

	void resetReading(const DM::View & v);

	GDALSystem *createSuccessor();

	OGRFeature * getFeature(const DM::View & v, long dynamind_id);

	void updateViews(const std::vector<View> &views);

	GDALSystem *getPredecessor() const;
	void setPredecessor(GDALSystem *sys);

	OGRFeature *getNextFeature(const DM::View & v);

	std::string getCurrentStateID();

private:
	OGRDataSource						*poDS;
	OGRSFDriver							*poDrive;
	std::map<std::string, OGRLayer *>	viewLayer;
	std::vector<std::string>			state_ids;

	std::vector<long>					uniqueIdsTonfid;
	long								latestUniqueId;

	OGRLayer *createLayer(const View &v);
	GDALSystem *predecessor;
	std::vector<DM::GDALSystem*> sucessors;
	QString DBID;
};
}

#endif // DMGDALSYSTEM_H
