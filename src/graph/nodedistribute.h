/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair
 
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

#ifndef DistributeNodes_H
#define DistributeNodes_H

#include <dmmodule.h>
#include <dm.h>
#include <boost/random.hpp>

#include <graphviewdef.h>

class DistributeNodes : public DM::Module
{
    DM_DECLARE_NODE(DistributeNodes)

private:
    DM::System *sys;
    DM::GRAPH::ViewDefinitionHelper defhelper_graph;
    bool uniform, normal;
    double nodesum;
    double nodeweight;

public:
    DistributeNodes();

    void run();
    void initmodel();
    double getNormalDist(double min, double max, boost::mt19937 &rng);
    double getUniformDist(double min, double max, boost::mt19937 &rng);
};

#endif // DistributeNodes_H
