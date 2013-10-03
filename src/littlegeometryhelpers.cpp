/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012   Christian Urich

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

#include "littlegeometryhelpers.h"
#include "cgalgeometry.h"
#include "tbvectordata.h"
#include <algorithm>
#include <math.h>
#include <QPointF>
#include <QPolygonF>
#include <QTransform>
#include <cgalgeometry.h>

std::vector<DM::Face*> LittleGeometryHelpers::CreateHolesInAWall(DM::System *sys, DM::Face *f, double distance, double width, double height, double parapet)
{
	std::vector<DM::Node*> nodes = TBVectorData::getNodeListFromFace(sys, f);
	//And again we rotate or wall again into x,y assuming. we assume that z is becoming our new x
	double E[3][3];
	TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

	double E_to[3][3];

	TBVectorData::CorrdinateSystem( *(nodes[0]), *(nodes[1]), *(nodes[2]), E_to);


	DM::Node dN1 = *(nodes[1]) - *(nodes[0]);
	DM::Node dN2 = *(nodes[2]) - *(nodes[0]);
	DM::Node orientationOriginal = TBVectorData::NormalVector(dN1, dN2);


	double alphas[3][3];
	TBVectorData::RotationMatrix(E, E_to, alphas);
	
	DM::Node n = TBVectorData::RotateVector(alphas, *(nodes[0]));
	double x = n.getX();
	double y = n.getY();
	double z_const = n.getZ();
	double xmin = x;
	double xmax = x;
	double ymin = y;
	double ymax = y;

	for (unsigned int i = 1; i < nodes.size(); i++)
	{
		n = TBVectorData::RotateVector(alphas, *(nodes[i]));
		x = n.getX();
		y = n.getY();
		xmin = min(xmin,x);
		xmax = max(xmax,x);
		ymin = min(ymin,y);
		ymax = max(ymax,y);
	}

	double l = ymax - ymin;

	int numberOfSegments = l / distance;

	std::vector<DM::Node> window_nodes;

	//We will distribute the windows equally as parapet height we asume 0.80 m
	for (int i = 0; i < numberOfSegments; i++) {
		//Center of the new window
		double delta_y = l / (numberOfSegments+1) * (i+1) + ymin;

		DM::Node n1(xmin + parapet, delta_y - width/2, z_const);
		DM::Node n2(xmin + parapet + height,  delta_y - width/2, z_const);
		DM::Node n3(xmin + parapet + height,  delta_y + width/2, z_const);
		DM::Node n4(xmin + parapet,  delta_y + width/2, z_const);

		window_nodes.push_back(n4);
		window_nodes.push_back(n3);
		window_nodes.push_back(n2);
		window_nodes.push_back(n1);

	}
	//Transform Coordinates back
	double alphas_t[3][3];
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			alphas_t[j][i] =  alphas[i][j];
		}
	}


	std::vector<DM::Face * > windows;

	for ( unsigned int i = 0; i < window_nodes.size()/4; i++){
		std::vector<DM::Node*> window_nodes_t;
		for (int j = 0; j < 4; j++) {
			DM::Node n = TBVectorData::RotateVector(alphas_t, window_nodes[j+i*4]);
			window_nodes_t.push_back(sys->addNode(n));
		}
		window_nodes_t.push_back(window_nodes_t[0]);


		DM::Node dN1_1 = *(window_nodes_t[1]) - *(window_nodes_t[0]);
		DM::Node dN2_1  = *(window_nodes_t[2]) - *(window_nodes_t[0]);
		DM::Node orientationOriginal_window = TBVectorData::NormalVector(dN1_1, dN2_1);

		DM::Node checkDir = orientationOriginal - orientationOriginal_window;
		if (fabs(checkDir.getX()) > 0.0001 ||
				fabs(checkDir.getY()) > 0.0001 ||
				fabs(checkDir.getZ()) > 0.0001) {
			std::reverse(window_nodes_t.begin(),window_nodes_t.end());
		}

		windows.push_back(sys->addFace(window_nodes_t));

		f->addHole(window_nodes_t);

		//DM::Logger(DM::Debug) << "Add Window";
	}
	return windows;
}

void LittleGeometryHelpers::CreateStandardBuilding(DM::System * city, DM::View & buildingView, DM::View & geometryView, DM::Component *BuildingInterface, std::vector<DM::Node * >  & footprint, int stories, bool createWindows)
{
	std::vector<double> roofColor;
	roofColor.push_back(0.66);
	roofColor.push_back(0.66);
	roofColor.push_back(0.66);
	std::vector<double> wallColor;
	wallColor.push_back(196./255.);
	wallColor.push_back(196./255.);
	wallColor.push_back(196./255.);
	std::vector<double> windowColor;
	windowColor.push_back(204./255.);
	windowColor.push_back(229./255.);
	windowColor.push_back(1);

	//Set footprint as floor
	DM::Face * base_plate = city->addFace(footprint, geometryView);
	BuildingInterface->getAttribute("Geometry")->addLink(base_plate, "Geometry");
	base_plate->getAttribute("Parent")->addLink(BuildingInterface, buildingView.getName());
	base_plate->addAttribute("type", "ceiling_cellar");

	//The Building is extruded stepwise. Housenodes is used as a starting point for the extusion
	std::vector<DM::Node*> houseNodes = footprint;
	//Create Walls
	for (int story = 0; story < stories; story++) {
		std::vector<DM::Face*> extruded_faces = TBVectorData::ExtrudeFace(city, geometryView, houseNodes, 3);
		int lastID = extruded_faces.size();
		for (int i = 0; i < lastID; i++) {
			DM::Face * f = extruded_faces[i];
			if (i != lastID-1) {
				f->addAttribute("type", "wall_outside");
				f->getAttribute("color")->setDoubleVector(wallColor);
				if (createWindows){
					std::vector<DM::Face* > windows = LittleGeometryHelpers::CreateHolesInAWall(city, f, 5, 1.5, 1);
					foreach (DM::Face * w, windows) {
						w->addAttribute("type", "window");
						w->getAttribute("color")->setDoubleVector(windowColor);
						BuildingInterface->getAttribute("Geometry")->addLink(w, "Geometry");
						w->getAttribute("Parent")->addLink(BuildingInterface, buildingView.getName());
						city->addComponentToView(w,geometryView);
					}
				}

			}
			else if (story != stories -1){
				f->addAttribute("type", "ceiling");
				f->getAttribute("color")->setDoubleVector(wallColor);
				houseNodes = TBVectorData::getNodeListFromFace(city, f);
			} else {
				/*std::vector<DM::Node*> roof = TBVectorData::getNodeListFromFace(city, f);
				std::reverse(roof.begin(), roof.end());
				f = city->addFace(roof, geometryView);*/

				f->addAttribute("type", "ceiling_roof");

				f->getAttribute("color")->setDoubleVector(roofColor);
			}
			BuildingInterface->getAttribute("Geometry")->addLink(f, "Geometry");
			f->getAttribute("Parent")->addLink(BuildingInterface, buildingView.getName());

		}
	}



}

void addFace(DM::System* city, DM::Node* n1, DM::Node* n2, DM::Node* n3, DM::Node* n4, std::string type, std::vector<double> color,
			 DM::View & buildingView, DM::View &geometryView, DM::Component *BuildingInterface)
{
	std::vector<DM::Node* > vf;
	vf.push_back(n1);
	vf.push_back(n2);
	vf.push_back(n3);
	vf.push_back(n4);
	vf.push_back(n1);

	DM::Face * f =  city->addFace(vf, geometryView);
	f->getAttribute("Parent")->addLink(BuildingInterface, buildingView.getName());
	f->addAttribute("type", type);
	f->getAttribute("color")->setDoubleVector(color);
	BuildingInterface->getAttribute("Geometry")->addLink(f, "Geometry");
}


void LittleGeometryHelpers::CreateRoofRectangle(DM::System *city, DM::View & buildingView, DM::View &geometryView, DM::Component *BuildingInterface, std::vector<DM::Node * >  &footprint, double height, double alpha)
{
	std::vector<double> roofColor;
	roofColor.push_back(178./255.);
	roofColor.push_back(34./255.);
	roofColor.push_back(34./255.);
	std::vector<double> wallColor;
	wallColor.push_back(196./255.);
	wallColor.push_back(196./255.);
	wallColor.push_back(196./255.);
	/*std::vector<double> windowColor;
	windowColor.push_back(204./255.);
	windowColor.push_back(229./255.);
	windowColor.push_back(1);*/

	const double pi =  3.14159265358979323846;
	//Create roof for minimal bounding box
	std::vector<DM::Node> b_box;
	std::vector<double> dimension;

	double angle = DM::CGALGeometry::CalculateMinBoundingBox(footprint, b_box, dimension);
	DM::Face * foot = city->addFace(footprint);
	DM::Node center = DM::CGALGeometry::CalculateCentroid(city, foot);

	QTransform t;
	t.rotate(angle);

	double l = dimension[0];
	double b = dimension[1];

	double h = cos (alpha / 180. * pi) * b / 2.;
	QPointF qcenter(center.getX(), center.getY());
	QPointF q_top_1 = t.map(QPointF (  -l/2., 0)) + qcenter;
	QPointF q_top_2 =  t.map(QPointF (l/2.,  0)) + qcenter;
	/**
	  * 4--------3
	  * |        |
	  * t1-------t2
	  * |        |
	  * 1--------2
	  */

	QPointF q_n1 =  t.map(QPointF (  -l/2.,  b/2.)) + qcenter;
	QPointF q_n2 =  t.map(QPointF (  l/2., b/2.)) + qcenter;
	QPointF q_n3 =  t.map(QPointF (  l/2.,  -b/2.)) + qcenter;
	QPointF q_n4 =  t.map(QPointF (  -l/2.,  -b/2.)) + qcenter;

	DM::Node * n4 = city->addNode(q_n1.x(), q_n1.y(), height);
	DM::Node * n3 = city->addNode(q_n2.x(), q_n2.y(), height);
	DM::Node * n2 = city->addNode(q_n3.x(), q_n3.y(), height);
	DM::Node * n1 = city->addNode(q_n4.x(), q_n4.y(), height);

	DM::Node * t1 = city->addNode(q_top_1.x(), q_top_1.y(), height + h/2.);
	DM::Node * t2 = city->addNode(q_top_2.x(), q_top_2.y(), height + h/2.);
	
	addFace(city, n1, n2, t2, t1, "roof", roofColor, buildingView, geometryView, BuildingInterface);
	addFace(city, t1, t2, n3, n4, "roof", roofColor, buildingView, geometryView, BuildingInterface);
	addFace(city, n1, t1, n4, n1, "roof_wall", wallColor, buildingView, geometryView, BuildingInterface);
	addFace(city, n2, n3, t2, n2, "roof_wall", wallColor, buildingView, geometryView, BuildingInterface);
}
