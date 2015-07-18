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

#ifndef GDALHOTSTARTER_H
#define GDALHOTSTARTER_H

#include <dmmodule.h>
#include <dm.h>

class OGRLayer;

/**
 * @brief The GDALHoststarter is used to hot start a simulation with a previous database. This is particularly useful for
 bigger simulation where the data preparation and linking of data takes a significant amount of time, but once established,
 can be used as basis for many simulations.
 */
class GDALHotStarter: public DM::Module
{
	DM_DECLARE_NODE(GDALHotStarter)
private:
	std::string hotStartDatabase;

	DM::ViewContainer *viewContainerFactory(OGRLayer * lyr);
public:
	GDALHotStarter();
	void init();
	void run();

	std::string getHelpUrl();
};

#endif // GDALHOTSTARTER_H
