/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2013  Christian Urich

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
#ifndef USERDEFINEDFUNCTIONS_H
#define USERDEFINEDFUNCTIONS_H

#include "dmcompilersettings.h"
#include "muParser.h"
#include "parser/mpParser.h"
#include "dmmodule.h"
#include <cmath>
#include <cstdlib>

namespace dm {
double numberOfValues(const double* values, int index);
double random(double value);
double printValue(double value);
double round(double vaule);
static double ContUrbanFabric;
static double DisContUrbanFabric;
static double RoadRailNetwork;
static double WaterBodies;
static double AgriculturalAreas;
static double ForestsSemiNatural;

class Random : public mup::ICallback {
public:
	Random() : ICallback(mup::cmFUNC, "rand", 1)
	{}
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
	{
		// Get the argument from the argument input vector
		mup::float_type a = a_pArg[0]->GetFloat();
		double val = rand() % (int) a;
		// The return value is passed by writing it to the reference ret
		*ret = val;
	}

	const mup::char_type* GetDesc() const
	{
		return "rand(x) - a random number";
	}

	IToken* Clone() const
	{
		return new Random(*this);
	}
};

class Round : public mup::ICallback {
public:
	Round() : ICallback(mup::cmFUNC, "round", 1)
	{}
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
	{
		// Get the argument from the argument input vector
		mup::float_type  a = a_pArg[0]->GetFloat();
		double val =  (a > 0.0) ? floor(a + 0.5) : ceil(a - 0.5);
		// The return value is passed by writing it to the reference ret
		*ret = val;
	}

	const mup::char_type* GetDesc() const
	{
		return "round(x) - round value";
	}
	IToken* Clone() const
	{
		return new Round(*this);
	}
};

}
inline void addCorineConstants(mu::Parser * p){

	dm::ContUrbanFabric		= 2;
	dm::DisContUrbanFabric	= 3;
	dm::RoadRailNetwork		= 4;
	dm::WaterBodies			= 5;
	dm::AgriculturalAreas	= 6;
	dm::ForestsSemiNatural	= 7;

	p->DefineVar("ContUrbanFabric", & dm::ContUrbanFabric);
	p->DefineVar("DisContUrbanFabric",& dm::DisContUrbanFabric);
	p->DefineVar("RoadRailNetwork", & dm::RoadRailNetwork);
	p->DefineVar("AgriculturalAreas",& dm::AgriculturalAreas);
	p->DefineVar("ForestsSemiNatural", & dm::ForestsSemiNatural);
	p->DefineVar("WaterBodies",& dm::WaterBodies);
}



#endif // USERDEFINEDFUNCTIONS_H
