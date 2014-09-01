#ifndef SWMMRETURNPERIOD_H
#define SWMMRETURNPERIOD_H


#include <dmmodule.h>
#include <dm.h>
#include <QDir>
#include <sstream>

#include "swmmwriteandread.h"


class DM_HELPER_DLL_EXPORT SWMMReturnPeriod : public  DM::Module {
	DM_DECLARE_NODE (SWMMReturnPeriod)

	private:
		int GLOBAL_Counter;
	DM::System * city;
	DM::View conduit;
	DM::View inlet;
	DM::View junctions;
	DM::View endnodes;
	DM::View catchment;
	DM::View outfalls;
	DM::View weir;
	DM::View wwtp;
	DM::View storage;
	DM::View pumps;
	DM::View globals;
	DM::View flooding_area;


	DM::View vcity;


	std::string RainFile;
	std::string FileName;
	std::string outputFiles;
	std::vector<DM::Node*> PointList;
	std::string returnPeriods;
	bool isCombined;
	bool cfRand;
	bool use_linear_cf;
	int CFSamples;
	bool consider_built_time;
	double upper_cf;
	double lower_cf;
	int numberOfThreds;



	double climateChangeFactor;



	void writeRainFile();
	int years;

	int calculationTimestep;
	//used to check if it's time to run
	int internalTimestep;

	double counterRain;

	std::map<std::string, int> UUIDtoINT;

	std::string unique_name;


private:
	std::vector<double> climateChangeFactors;
public:

	SWMMReturnPeriod();
	void run();
	void init();
};


#endif // SWMMRETURNPERIOD_H
