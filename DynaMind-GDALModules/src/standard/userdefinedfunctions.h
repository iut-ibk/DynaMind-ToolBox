/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2014  Christian Urich

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
//#include "muParser.h"
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

class Num2Str : public mup::ICallback {
public:
	Num2Str() : ICallback(mup::cmFUNC, "num2str", 1)
	{}
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
	{
		// Get the argument from the argument input vector
		mup::float_type  a = a_pArg[0]->GetFloat();
		std::string val =  QString::number(a).toStdString();
		// The return value is passed by writing it to the reference ret
		*ret = val;
	}

	const mup::char_type* GetDesc() const
	{
		return "num2str(x) - converting a number to string";
	}
	IToken* Clone() const
	{
		return new Num2Str(*this);
	}
};

class VecSum : public mup::ICallback {
public:
	VecSum() : ICallback(mup::cmFUNC, "vecsum", 1)
	{}
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
	{
		mup::matrix_type m = a_pArg[0]->GetArray();
		double sum = 0;
		for (int i = 0; i < m.GetRows(); i++)
			sum+= m.At(i).GetFloat();
		// The return value is passed by writing it to the reference ret
		*ret = sum;
	}

	const mup::char_type* GetDesc() const
	{
		return "num2str(x) - converting a number to string";
	}
	IToken* Clone() const
	{
		return new VecSum(*this);
	}
};


class Str2Num : public mup::ICallback {
public:
	Str2Num() : ICallback(mup::cmFUNC, "str2num", 1)
	{}
	virtual void Eval(mup::ptr_val_type &ret, const mup::ptr_val_type *a_pArg, int a_iArgc)
	{
		// Get the argument from the argument input vector
		mup::string_type  a = a_pArg[0]->GetString();
		double val =  QString::fromStdString(a).toDouble();
		// The return value is passed by writing it to the reference ret
		*ret = val;
	}

	const mup::char_type* GetDesc() const
	{
		return "str2num(x) - converting a string to number";
	}
	IToken* Clone() const
	{
		return new Str2Num(*this);
	}
};

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



#endif // USERDEFINEDFUNCTIONS_H
