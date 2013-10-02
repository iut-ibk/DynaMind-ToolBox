#ifndef CREATEHOUSES_H
#define CREATEHOUSES_H

#include <dm.h>
#include <dmcompilersettings.h>

using namespace DM;
class DM_HELPER_DLL_EXPORT CreateHouses : public Module
{
	DM_DECLARE_NODE(CreateHouses);

private:
	DM::View houses;
	DM::View parcels;
	DM::View building_model;
	DM::View footprint;
	DM::View cityView;

	double heatingT;
	double coolingT;
	int buildyear;
	int stories;
	double alpha;
	double l;
	double b;
	bool onSingal;
	bool l_on_parcel_b;
	bool yearFromCity;

	void createWindows(DM::Face * f, double distance, double width, double height);
public:
	CreateHouses();
	void run();
	void init();
	std::string getHelpUrl();

};

#endif // CREATESINGLEFAMILYHOUSES_H
