#ifndef GDALCREATEHOUSEHOLDS_H
#define GDALCREATEHOUSEHOLDS_H

#include <dmmodule.h>
#include <dm.h>
#include <boost/random.hpp>

class DM_HELPER_DLL_EXPORT GDALCreateHouseholds : public DM::Module
{
	DM_DECLARE_NODE(GDALCreateHouseholds)

public:
	GDALCreateHouseholds();
	void run();
private:
	DM::ViewContainer district;
	DM::ViewContainer building;
	DM::ViewContainer household;
	DM::ViewContainer hh_income;
	DM::ViewContainer education;

	std::vector<std::string> hh_income_names;
	std::vector<std::string> education_names;

	std::vector<std::string> hh_income_names_p;
	std::vector<std::string> education_names_p;

	std::vector<double> hh_income_v;
	std::vector<double> education_v;
	boost::random::mt19937 generator;
	//boost::random::default_random_engine generator;
	boost::random::uniform_real_distribution<double> distribution;

	std::string sampler(std::vector<std::string> & names, std::vector<double> & devec);

	void fill_income_cdf(int id, std::string filtername, DM::ViewContainer & container,
						 std::vector<std::string> & names, std::vector<double> &return_vec);
};

#endif // GDALCREATEHOUSEHOLDS_H
