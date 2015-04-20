/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2014  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

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
