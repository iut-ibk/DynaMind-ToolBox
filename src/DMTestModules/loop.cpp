/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Markus Sengthaler

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

#include "loop.h"
#include <sstream>
#include <dmsimulation.h>
#include <validation.h>

DM_DECLARE_NODE_NAME(Loop, Modules)
Loop::Loop()
{
	steps = 0;
	counter = 0;
	this->addInPort("in");
	this->addOutPort("true");
	this->addOutPort("false");
	this->addParameter("steps", DM::INT, &steps);
	this->addParameter("counter", DM::INT, &counter);
}

void Loop::run() 
{	
    DM::Logger(DM::Debug) << "checking condition";

    if(counter < steps)
	{
		// TODO (simenv adaption)
		counter++;
		DM::Logger(DM::Debug) << "condition true ( " << counter << " < " << steps << " )";
		//this->setOutPortData("true", this->getInPortData("in"));
	}
	else
	{
		DM::Logger(DM::Debug) << "condition false ( " << counter << " < " << steps << " )";
		//this->setOutPortData("false", this->getInPortData("in"));
	}

    DM::Logger(DM::Debug) << "checking condition ... done";
}

void Loop::Init()
{
	// just resets the counter
	counter = 0;
}