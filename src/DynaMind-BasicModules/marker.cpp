/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

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
#include "marker.h"

#include <QString>
#include <sstream>
#include "userdefinedfunctions.h"
#include <QStringList>
#include <QThread>
#include <guimarker.h>

DM_DECLARE_NODE_NAME( Marker, Modules )
Marker::Marker()
{
    param.Width = 200;
    param.Height = 200;
    param.CellSize = 20;
    param.PlacementOption = "";
    param.RExpression = "";
    param.rExpression = "";
    param.maxExpression = "";
    param.Identifier = "";
    param.Points = true;
    param.Edges = false;
    param.resultName = "Result";
    param.selected = false;
    param.DimensionOfExisting = "user defined";

    param.OffsetX = 0;
    param.OffsetY = 0;

    this->addParameter("Width", DM::LONG, & param.Width);
    this->addParameter("Height", DM::LONG, & param.Height);
    this->addParameter("CellSize", DM::DOUBLE, &param.CellSize);
    this->addParameter("OffsetX", DM::LONG, &param.OffsetX);
    this->addParameter("OffsetY", DM::LONG, &param.OffsetY);

    this->addParameter("RExpression", DM::STRING, &param.RExpression);
    this->addParameter("rExpression", DM::STRING, &param.rExpression);
    this->addParameter("maxExpression", DM::STRING, &param.maxExpression);

    this->addParameter("PlacementOption", DM::STRING, &param.PlacementOption);
    this->addParameter("Points", DM::BOOL, &param.Points);
    this->addParameter("Edges", DM::BOOL, &param.Edges);
    this->addParameter("Identifier", DM::STRING, &param.Identifier);
    this->addParameter("resultName", DM::STRING, &param.resultName);
    this->addParameter("selected", DM::BOOL, &this->param.selected);
    this->addParameter("DimensionOfExisting", DM::STRING, &this->param.DimensionOfExisting);

    DM::View outputview(param.resultName, DM::RASTERDATA, DM::WRITE);


    std::vector<DM::View> data;
    sys_in = 0;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::READ) );
    this->addData("Data", data);


    std::vector<DM::View> rdata;
    rdata.push_back(outputview);

    this->addData("Result", rdata);
}

void Marker::init() {
    //sys_in = this->getData("Data");
    DM::View outputview = DM::View (param.resultName, DM::RASTERDATA, DM::WRITE);
    std::vector<DM::View> rData;
    rData.push_back(outputview);

    this->addData("Result", rData);
}

double Marker::calculater(const Node &sp, const Node &cp) {
    double x = cp.getX() - sp.getX();
    double y = cp.getY()- sp.getY();

    double l = sqrt(x*x+y*y);
    return l;
}

bool Marker::createInputDialog() {
    QWidget * w = new GUIMarker(this);
    w->show();
    return true;
}


void Marker::initRExpression () {
    R = new double;
    mu::Parser * p  = new mu::Parser();

    long Rcounter = 0;

    RRasterData.clear();
    RRasterVariables.clear();
    for ( std::vector<std::string>::const_iterator it = param.RData.begin(); it != param.RData.end(); ++it) {
        if (paramRaw.find(*it) != paramRaw.end()) {
            std::string buf;
            std::stringstream ss(paramRaw[*it]);
            std::vector<std::string> tokens;
            while (ss >> buf)
                tokens.push_back(buf);
            RRasterVariables.push_back(new double);
            p->DefineVar(*it, RRasterVariables[Rcounter++] );

        } else {
            Logger(Error) << "Error: Variable not defined";
            return;
        }
    }




    p->DefineVar("CellValue", this->CellValue);
    p->DefineFun("nov", dm::numberOfValues);
    p->DefineFun("rand", dm::random , false);
    p->SetExpr(param.RExpression);
    RExpression = p;


}

double Marker::evaluateExpresion_R(int index, Node &p) {
    try {
        for ( unsigned int i = 0; i < RDoubleAttributes.size(); i++ ) {
            *RVariables.at(i) = RDoubleAttributes.at(i)->at(index);
        }
        for ( unsigned int i = 0; i < RRasterData.size(); i++ ) {
            *RRasterVariables.at(i) = RRasterData.at(i)->getCell(p.getX(), p.getY());
        }

        *R = RExpression->Eval();
        return *R;

    }
    catch (mu::Parser::exception_type &e) {
        Logger(Error) << e.GetMsg();
        return 0;
    }

}
void Marker::initMaxMinExpression() {
    this->maxSet = false;
    this->minSet = false;
    if ( param.maxExpression.size() > 0 ) {
        maxExpression = new mu::Parser();
        maxExpression->DefineVar("R", R);
        maxExpression->DefineVar("r", r);
        maxExpression->SetExpr(param.maxExpression);
        maxSet = true;
    }
}

void Marker::initrExpression () {
    mu::Parser * p  = new mu::Parser();

    long rcounter = 0;
    rRasterData.clear();
    rRasterVariables.clear();
    for ( std::vector<std::string>::const_iterator it = param.RData.begin(); it != param.RData.end(); ++it) {
        if (paramRaw.find(*it) != paramRaw.end()) {
            std::string buf;
            std::stringstream ss;
            std::string s = paramRaw[*it];
            ss << s;
            std::vector<std::string> tokens;
            while (ss >> buf)
                tokens.push_back(buf);
            //rRasterData.push_back(& this->getRasterData(tokens[0]) );
            rRasterVariables.push_back(new double);
            p->DefineVar(*it, rRasterVariables[rcounter] );
            rcounter++;

        } else {
            Logger(Error) << "Error: Variable not defined";
            return;
        }
    }

    p->DefineFun("nov", dm::numberOfValues);
    p->DefineFun("rand", dm::random, false);
    p->DefineVar("r", r);
    p->DefineVar("R", R);
    p->DefineVar("CellValue", this->CellValue);
    p->SetExpr(param.rExpression);
    this->rExpression = p;
}


double Marker::evaluateExpresion_r(int index, Node &p ) {
    try  {

        for ( unsigned int i = 0; i < rDoubleAttributes.size(); i++ ) {
            *rVariables.at(i) = rDoubleAttributes.at(i)->at(index);
        }
        for (unsigned int i = 0; i < rRasterData.size(); i++ ) {
            *rRasterVariables.at(i) = rRasterData.at(i)->getCell(p.getX(), p.getY());
        }
        //Calculate
        return rExpression->Eval();
    }
    catch (mu::Parser::exception_type &e) {
        Logger(Error) << e.GetMsg();
        return 0;
    }

}
void Marker::run() {

    DM::View vIdentifier;
    if (param.Points == true )
        vIdentifier = DM::View(param.Identifier, DM::NODE, DM::READ);
    if (param.Edges == true )
        vIdentifier = DM::View(param.Identifier, DM::EDGE, DM::READ);
    sys_in = this->getData("Data");

    long Width = param.Width;
    long Height = param.Height;
    double CellSizeX = param.CellSize;
    double CellSizeY = param.CellSize;
    long offsetX = param.OffsetX;
    long offsetY = param.OffsetY;

    if (this->param.DimensionOfExisting != "user defined") {
        DM::RasterData * r = this->getRasterData("Data", DM::View(this->param.DimensionOfExisting, DM::RASTERDATA, DM::READ));

        Width = r->getWidth();
        Height = r->getHeight();
        CellSizeX = r->getCellSizeX();
        CellSizeY = r->getCellSizeX();
        offsetX = r->getXOffset();
        offsetY = r->getYOffset();
    }

    this->OutputMap = this->getRasterData("Result", DM::View (param.resultName, DM::RASTERDATA, DM::WRITE));
    this->OutputMap->setSize(Width,Height,CellSizeX,CellSizeY,offsetX,offsetY);
    this->OutputMap->clear();
    double noValue = this->OutputMap->getNoValue();

    //Init MuParser
    std::map<std::string, double*> VariableMap;
    mu::Parser * RExpression = new mu::Parser();
    mu::Parser * rExpression = new mu::Parser();
    mu::Parser * maxExpression = new mu::Parser();
    std::vector<std::string> VariableNames;
    double * R = new double;
    double * r = new double;
    double * V = new double;
    for (std::map<std::string, RasterData * >::iterator it = inputRasterData.begin(); it != inputRasterData.end(); ++it) {
        VariableMap[it->first] = new double;
        RExpression->DefineVar(it->first, VariableMap[it->first]);
        rExpression->DefineVar(it->first, VariableMap[it->first]);
        maxExpression->DefineVar(it->first, VariableMap[it->first]);
        VariableNames.push_back(it->first);
    }
    rExpression->DefineVar("R", R);
    rExpression->DefineVar("r", r);
    rExpression->DefineFun("rand", dm::random, false);
    rExpression->DefineVar("Value", V);
    RExpression->DefineVar("Value", V);
    RExpression->DefineFun("rand", dm::random, false);
    maxExpression->DefineVar("R", R);
    maxExpression->DefineVar("r", r);
    maxExpression->DefineVar("Value", V);
    maxExpression->DefineFun("rand", dm::random, false);
    //Set Expressions
    rExpression->SetExpr(param.rExpression);
    RExpression->SetExpr(param.RExpression);
    maxExpression->SetExpr(param.maxExpression);
    //Init Points
    //AddPoints

    std::vector<DM::Node> points;

    if ( param.Points == true ) {
        DM::ComponentMap cmp = sys_in->getAllComponentsInView(vIdentifier);
        for (DM::ComponentMap::const_iterator it = cmp.begin(); it != cmp.end(); ++it) {
            DM::Node * n = sys_in->getNode(it->first);
            if (param.selected) {
                if (n->getAttribute("selected")->getDouble() < 0.001) continue;
            }

            ulong X = (ulong) ( n->getX() - offsetX  ) / CellSizeX;
            ulong Y = (ulong) ( n->getY() - offsetY  ) / CellSizeY;

            if ( X >= Width)
                continue;
            if ( Y >= Height)
                continue;

            points.push_back(Node(X,Y ,n->getZ()));
        }
    }
    //AddLines as Points
    if ( param.Edges == true ) {
        DM::ComponentMap cmp = sys_in->getAllComponentsInView(vIdentifier);
        for (DM::ComponentMap::const_iterator it = cmp.begin(); it != cmp.end(); ++it) {
            DM::Edge * e = sys_in->getEdge(it->first);

            if (!e) {
                Logger(Warning) << "Not an Edge " << vIdentifier.getName();
                continue;
            }

            if (param.selected) {
                if (e->getAttribute("selected")->getDouble() < 0.001) continue;
            }
            Node * p1 = sys_in->getNode(e->getStartpointName());
            Node * p2 = sys_in->getNode(e->getEndpointName());
            double dx = p2->getX()/CellSizeX - p1->getX()/CellSizeX;
            double dy = p2->getY()/CellSizeY- p1->getY()/CellSizeY;

            long steps = (long) sqrt(dx*dx+dy*dy);

            long x0 = (long) (p1->getX() - offsetX ) / CellSizeX;
            long y0 = (long) (p1->getY() - offsetY ) /CellSizeY;
            for ( int i = 0; i < steps; i++ ) {
                long X = (long) ( x0 + i* (dx /  steps) );
                long Y = (long) ( y0 + i* (dy /  steps) );

                if ( X >= Width)
                    continue;
                if ( Y >= param.Height)
                    continue;
                Node p;

                p.setX( X );
                p.setY( Y );
                points.push_back(p);
            }
        }
    }

    //Create Mark for every point
    Logger(Standard) << "Points to mark " <<points.size();
    foreach(Node p, points) {
        //Set Values

        *(V) = p.getZ();
        foreach(std::string s, VariableNames) {
            double * d = VariableMap[s];
            *(d) = inputRasterData[s]->getCell(p.getX(), p.getY());
        }
        //Evaluate R
        try {
            *(R) = RExpression->Eval();
        } catch (mu::Parser::exception_type &e) {
            Logger(Error) << e.GetMsg();
        }

        //Mark Celss
        Node CenterPoint = p;
        long  minI = p.getX() - *R-2;
        long  minJ = p.getY() - *R-2;
        long  maxI = p.getX() + *R+2;
        long  maxJ = p.getY() + *R+2;
        if (minI < 0)
            minI = 0;
        if (minJ < 0)
            minJ = 0;
        if (maxI > Width)
            maxI = Width;
        if (maxJ > Height)
            maxJ = Height;
        Node currentPoint;

        for ( unsigned long  i = minI; i < maxI; i++ ) {
            for ( unsigned long j =  minJ; j < maxJ; j++ ) {
                currentPoint.setX( (double) i);//+0.5 );
                currentPoint.setY( (double) j);//+0.5 );
                *r = calculater(CenterPoint, currentPoint);
                if (*r <= *R ) {
                    double value;
                    try {
                        value = rExpression->Eval();
                    } catch (mu::Parser::exception_type &e) {
                        Logger(Error) << e.GetMsg();
                    }
                    if (param.PlacementOption.compare("KeepHigherValue") == 0) {
                        if (  this->OutputMap->getCell(i,j) > value ) {
                            value = this->OutputMap->getCell(i,j);
                        }
                    } else if ( param.PlacementOption.compare("KeepLowerValue") == 0 ) {
                        if ( this->OutputMap->getCell(i,j) < value && this->OutputMap->getCell(i,j) != noValue ) {
                            value = this->OutputMap->getCell(i,j);
                        }
                    } else if ( param.PlacementOption.compare("KeepValue") == 0 ) {
                        if ( this->OutputMap->getCell(i,j) != noValue) {
                            value = this->OutputMap->getCell(i,j);

                        }
                    } else if ( param.PlacementOption.compare("Add") == noValue ) {
                        double val =  this->OutputMap->getCell(i,j);

                        value = value + val;
                    }
                    if (!this->param.maxExpression.empty()) {
                        double maxValue = maxExpression->Eval();
                        value = (value > maxValue ) ?  maxValue : value;
                    }

                    if (value > 1000) {
                        Logger(Debug) << value;
                    }

                    this->OutputMap->setCell(i,j,value);
                }
            }
        }
    }
    delete R;
    delete r;
    delete V;
    delete RExpression;
    delete rExpression;
    delete maxExpression;
    foreach(std::string s, VariableNames)
        delete VariableMap[s];


}

DM::System * Marker::getSystemIn() {
    return this->sys_in;
}


std::vector<string> Marker::getLandscapesInStream()
{
    std::vector<std::string> landscapes;
    DM::System * sys = this->getData("Data");
    if (!sys)
        return landscapes;
    std::vector<DM::View> views = sys->getViews();

    foreach (DM::View v, views) {
        if (v.getType() == DM::RASTERDATA)
            landscapes.push_back(v.getName());
    }
    return landscapes;

}
