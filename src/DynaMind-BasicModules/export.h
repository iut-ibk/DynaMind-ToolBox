/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2015 Markus Sengthaler

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

#ifndef EXPORT_H
#define EXPORT_H

#include <dmmodule.h>

class OGRLayer;
class OGRDataSource;

using namespace DM;

typedef std::map<std::string, std::string> StringMap;

class DM_HELPER_DLL_EXPORT Export : public Module
{
	DM_DECLARE_NODE(Export)

public:
	Export();
	~Export();
	void run();
	void init();
	void reset();
	bool  createInputDialog();

	void initViews();

	std::string path;
	int epsgCode;
	StringMap viewConfig;
	StringMap viewEPSGConfig;
	std::map<std::string, int> viewConfigTypes;
private:

	OGRLayer* prepareNewLayer(const DM::View& view, OGRDataSource* data);
	void exportLayer(const DM::View& view, OGRLayer* layer, System* system);
};

#endif // EXPORT_H
