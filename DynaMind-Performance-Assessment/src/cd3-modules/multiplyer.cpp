/**
 * CityDrain3 is an open source software for modelling and simulating integrated
 * urban drainage systems.
 *
 * Copyright (C) 2014 Christian Urich
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include "multiplyer.h"
#include <flow.h>
#include <flowfuns.h>

CD3_DECLARE_NODE_NAME(Multiplyer)

Multiplyer::Multiplyer() {
	in = Flow();
	out1 = Flow();
	out2 = Flow();

	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out1));
	addOutPort(ADD_PARAMETERS(out2));

	multiplyer = 1;
	addParameter(ADD_PARAMETERS(multiplyer));
}

Multiplyer::~Multiplyer() {
}

bool Multiplyer::init(ptime start, ptime end, int dt) {
	return true;
}

int Multiplyer::f(ptime time, int dt) {
	(void) time;

	out1[0] = in[0] * multiplyer;
	out2[0] = in[0] * multiplyer;
	return dt;
}
