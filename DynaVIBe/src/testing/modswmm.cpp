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

#include <modswmm.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

//Sewer
#include <swmmdynamindconverter.h>
#include <sewerviewdef.h>

using namespace DM;

DM_DECLARE_NODE_NAME(ModifySWMMModel, Sewersystems)

ModifySWMMModel::ModifySWMMModel()
{
    S::ViewDefinitionHelper sd;

	this->inpfilepath="";
    this->rainfilepath="";
	this->returnp = false;

	this->addParameter("Path of inp file", DM::FILENAME, &this->inpfilepath);
	this->addParameter("Returnperiod: ", DM::BOOL, &this->returnp);

	std::vector<DM::View> views;
    views.push_back(sd.getCompleteView(S::JUNCTION,DM::READ));
    views.push_back(sd.getCompleteView(S::CONDUIT,DM::READ));
    views.push_back(sd.getCompleteView(S::INLET,DM::READ));
    views.push_back(sd.getCompleteView(S::CATCHMENT,DM::READ));

	DM::View n = DM::View("NODES",DM::NODE,DM::MODIFY);
	DM::View v = DM::View("VORONIO_CELL",DM::FACE,DM::MODIFY);
	v.addAttribute("area",DM::Attribute::DOUBLE,DM::READ);
	v.addAttribute("imperv",DM::Attribute::DOUBLE,DM::MODIFY);

	views.push_back(n);
	views.push_back(v);

	DM::View ex = DM::View("EXAMINATIONROOM",DM::FACE,DM::MODIFY);
	ex.addAttribute("returnperiod",DM::Attribute::DOUBLE,DM::MODIFY);
	views.push_back(ex);

    this->addData("Sewer", views);
}

void ModifySWMMModel::run()
{
	S::ViewDefinitionHelper sd;
	DM::View n = DM::View("NODES",DM::NODE,DM::MODIFY);
	DM::View v = DM::View("VORONIO_CELL",DM::FACE,DM::MODIFY);
	v.addAttribute("area",DM::Attribute::DOUBLE,DM::READ);
	v.addAttribute("imperv",DM::Attribute::DOUBLE,DM::MODIFY);

	DM::View ex = DM::View("EXAMINATIONROOM",DM::FACE,DM::MODIFY);
	ex.addAttribute("returnperiod",DM::Attribute::DOUBLE,DM::MODIFY);



	QStringList subcatchments, subareas, infiltration,  polygons, raingages;

    this->sys = this->getData("Sewer");

	std::vector<DM::Component*> j = this->sys->getAllComponentsInView(sd.getCompleteView(S::JUNCTION,DM::MODIFY));
	std::vector<DM::Component*> c = this->sys->getAllComponentsInView(v);
	std::vector<DM::Component*> cnodes = this->sys->getAllComponentsInView(n);

	std::vector<DM::Component*> faces = this->sys->getAllComponentsInView(ex);

	double pp = 0;

	if(faces.size())
	{
		pp = faces[0]->getAttribute("returnperiod")->getDouble();

		for(int index=0; index < j.size(); index++)
			j[index]->changeAttribute("returnperiod", pp);

	}

	if(!returnp)
	{
		double totalcatchmentarea = 0;

		for(int index=0; index < c.size(); index++)
		{
			DM::Face* currentcatchment = dynamic_cast<DM::Face*>(c[index]);

			//find catchment node
			DM::Node* inletnode=0;
			DM::Node* cnode=0;

			for(int i=0; i < cnodes.size(); i++)
			{
				if(TBVectorData::PointWithinFace(currentcatchment,dynamic_cast<DM::Node*>(cnodes[i])))
				{
					std::vector<DM::Node*> nearest = TBVectorData::findNearestNeighbours(dynamic_cast<DM::Node*>(cnodes[i]),10000,j);
					if(nearest.size())
					{
						inletnode = nearest[0];
						cnode = dynamic_cast<DM::Node*>(cnodes[i]);
						break;
					}
				}
			}

			if(!inletnode)
				continue;

			double imperv = currentcatchment->getAttribute("imperv")->getDouble();
			currentcatchment->changeAttribute("imperv",imperv+5);

			double area = currentcatchment->getAttribute("area")->getDouble()/10000;
			totalcatchmentarea += area;

			subcatchments.append("New_" + QString::number(index) +
								 "\t3\t" +
								 inletnode->getAttribute("Name")->getString().c_str() +
								 "\t" + QString::number(area,'f',4) +
								 "\t" + QString::number(imperv,'f',4) +
								 "\t" + QString::number(sqrt(area*10000.0),'f',4) +
								 "\t0.5" +
								 "\t0");

			subareas.append("New_" + QString::number(index) +
							"\t0.015\t0.2\t1.8\t5\t0\tOUTLET");

			infiltration.append("New_" + QString::number(index) +
								"\t160\t6\t3\t6\t0");

			std::vector<DM::Node*> carea = currentcatchment->getNodePointers();

			for(int ci=0; ci < carea.size(); ci++)
			{
				polygons.append("New_" + QString::number(index) +
								"\t" + QString::number(carea[ci]->getX(),'f',4) +
								"\t" + QString::number(carea[ci]->getY(),'f',4));
			}
		}

		for(int index=0; index < j.size(); index++)
		{
			DM::Component* currentcomp = j[index];
			currentcomp->changeAttribute("carea",totalcatchmentarea);
		}
	}
	else
	{
		//Define raingages
		raingages.append("1                VOLUME    0:05   1.0    FILE       \"rain.dat\"       " + QString::number(pp,'f',0) + "          MM");
		raingages.append("2                VOLUME    0:05   1.0    FILE       \"rain.dat\"       " + QString::number(pp,'f',0) + "          MM");
		raingages.append("3                VOLUME    0:05   1.0    FILE       \"rain.dat\"       " + QString::number(pp,'f',0) + "          MM");
		raingages.append("5                VOLUME    0:05   1.0    FILE       \"rain.dat\"       " + QString::number(pp,'f',0) + "          MM");
	}

	//WRITE NEW FILE
	QFile ofile("/home/csae6550/work/dev/dynamindwps/wpstmp/newibkmodel.inp");

	ofile.open(QIODevice::ReadWrite | QIODevice::Text);

	QTextStream ostream(&ofile);


	if(!QFile::exists(QString::fromStdString(inpfilepath)))
	{
		DM::Logger(DM::Error) << "File does not exist: " << inpfilepath;
		return;
	}

	QFile file(QString::fromStdString(inpfilepath));

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		DM::Logger(DM::Error) << "Cannot open file: " << inpfilepath;
		return;
	}

	//read inp file
	QTextStream stream(&file);

	QStringList currentline = getNextValidLine(&stream, ostream);


	while(!stream.atEnd() && currentline.size() > 0)
	{
		//check for inp sections
		if(currentline[0].startsWith('[') && currentline[0].endsWith(']'))
		{
			QString sectionname = currentline[0].mid(1,currentline[0].size()-2).toUpper();
			bool raingagesection=false;

			if(sectionname=="SUBCATCHMENTS" && !returnp)
				ostream << subcatchments.join("\n") + "\n";

			if(sectionname=="POLYGONS" && !returnp)
				ostream << polygons.join("\n") + "\n";

			if(sectionname=="INFILTRATION" && !returnp)
				ostream << infiltration.join("\n") + "\n";

			if(sectionname=="SUBAREAS" && !returnp)
				ostream << subareas.join("\n") + "\n";

			if(sectionname=="RAINGAGES" && returnp)
			{
				raingagesection=true;
				ostream << raingages.join("\n") + "\n";
			}

			currentline = getNextValidLine(&stream, ostream,raingagesection);

			//fill table with values
			while(!stream.atEnd() && currentline.size() > 0 && !currentline[0].startsWith('[') && !currentline[0].endsWith(']'))
			{
				//tables[sectionname]->insert(currentline[0], new QStringList(currentline));
				currentline = getNextValidLine(&stream, ostream,raingagesection);
				continue;
			}

			if(raingagesection)
				ostream << currentline.join("\t") << "\n";
		}
		else
		{
			currentline = getNextValidLine(&stream, ostream);
		}
	}

	file.close();
	ofile.close();
}

QStringList ModifySWMMModel::getNextValidLine(QTextStream *stream, QTextStream &ostream, bool skip)
{
	while (!stream->atEnd())
	{
		QString line = stream->readLine();
		//ostream << line << "\r\n";
		QString trimmedline = line.trimmed().simplified();

		//tokenize line
		QStringList tokens = trimmedline.split(" ");

		//skip empty line
		if( trimmedline.isEmpty() )
			continue;

		//skip comment line
		if(trimmedline.startsWith(';'))
			continue;

		if(trimmedline.split(";").size() > 1)
			tokens = trimmedline.split(";")[0].trimmed().simplified().split(" ");

		if(!skip)
			ostream << line << "\n";

		return tokens;
	}

	return QStringList();
}
