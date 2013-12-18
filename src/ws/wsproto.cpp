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

#include <wsproto.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
#include <map>
#include <math.h>
#include <algorithm>
#include <numeric>

#define _USE_MATH_DEFINES
#include <math.h>

DM_DECLARE_NODE_NAME(WSProto,Watersupply)

WSProto::WSProto()
{
	this->q=0;
	this->h = 0;
	this->l=0;
	this->k=0;
	//Define input parameter
	this->addParameter("q [m^3/s]", DM::DOUBLE, &this->q);
	this->addParameter("h [m]", DM::DOUBLE, &this->h);
	this->addParameter("l [m]", DM::DOUBLE, &this->l);
	this->addParameter("k [m]", DM::DOUBLE, &this->k);
}

void WSProto::run()
{
	double d = calcDiameter(k,l,q,h);

	DM::Logger(DM::Standard) << "Found Diameter: " << d;

	return;
}

double WSProto::calcDiameter(double k, double l, double q, double h)
{
	if(h < 0)
		return -1;

	double maxerror = 0.001;
	double d = 0.0001;
	double maxd = 6;
	double mind = 0;
	double currenterror = h-calcFrictionHeadLoss(d,k,l,q);

	while (std::fabs(currenterror) > maxerror)
	{
		if(currenterror < 0)
		{
			mind=d;
			d = d+(maxd-mind)/2;
		}

		if(currenterror > 0)
		{
			maxd=d;
			d = d-(maxd-mind)/2;
		}

		double frictionhl = calcFrictionHeadLoss(d,k,l,q);
		double olderror = currenterror;
		currenterror = h-frictionhl;

		if(olderror==currenterror)
			break;

		DM::Logger(DM::Debug) << "Friction: " << frictionhl;
	}

	return d;
}

double WSProto::calcFrictionHeadLoss(double d, double k, double l, double q)
{
	double headloss = 0;

	headloss =
			calcLambda(k,d,q)*
			(l/d)*
			(std::pow(q/((std::pow(d,2)/4.0)*M_PI),2)/(2*9.81));

	return headloss;
}

double WSProto::calcLambda(double k, double d, double q, double lambda)
{
	double maxerror = 0.01;

	if(lambda==0)
	{
		lambda = 1.0/std::pow(2*std::log10((3.71*d)/k),2);
		return calcLambda(k,d,q,lambda);
	}
	else
	{
		double viscosity = 1.3*std::pow(10,-6);
		double v = q/((std::pow(d,2)/4.0)*M_PI);
		double Re = (v*d)/viscosity;
		double tmpl = -2*std::log10((2.51/(Re*std::sqrt(lambda)))+(k/(3.71*d)));
		double error = std::fabs(1/std::sqrt(lambda) - tmpl);
		double oldlambda = lambda;
		lambda = 1/std::pow(-tmpl,2);

		if(error > maxerror && oldlambda != lambda)
		{
			return calcLambda(k,d,q,lambda);
		}
		else
		{
			return lambda;
		}
	}

	return lambda;
}

void WSProto::initmodel()
{
}
