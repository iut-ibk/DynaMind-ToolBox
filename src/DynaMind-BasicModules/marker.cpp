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

    this->addParameter("Width", DM::LONG, & param.Width);
    this->addParameter("Height", DM::LONG, & param.Height);
    this->addParameter("CellSize", DM::DOUBLE, &param.CellSize);
    this->addParameter("RExpression", DM::STRING, &param.RExpression);
    this->addParameter("rExpression", DM::STRING, &param.rExpression);
    this->addParameter("maxExpression", DM::STRING, &param.maxExpression);

    this->addParameter("PlacementOption", DM::STRING, &param.PlacementOption);
    this->addParameter("Points", DM::INT, &param.Points);
    this->addParameter("Edges", DM::INT, &param.Edges);
    this->addParameter("Identifier", DM::STRING, &param.Identifier);


    //this->addParameter("InputRasterData", VIBe2::USER_DEFINED_RASTERDATA_IN, &inputRasterData);
    //this->addParameter("VectorData", VIBe2::VECTORDATA_IN, & vectorData);
    //this->addParameter("OutPutMap", VIBe2::RASTERDATA_OUT, &OutputMap);

    DM::View outputview("result", DM::RASTERDATA, DM::WRITE);


    std::vector<DM::View> data;
    sys_in = 0;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::READ) );
    this->addData("Data", data);


    std::vector<DM::View> rdata;
    rdata.push_back(outputview);

    this->addData("Result", rdata);
}

void Marker::init() {
        sys_in = this->getData("Data");
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
    long Vcounter = 0;
    long Rcounter = 0;

    RRasterData.clear();
    RRasterVariables.clear();
    for ( std::vector<std::string>::const_iterator it = param.RData.begin(); it != param.RData.end(); ++it) {
        std::cout << "Try: " << *it << std::endl;
        if (paramRaw.find(*it) != paramRaw.end()) {
            std::string buf;
            std::stringstream ss(paramRaw[*it]);
            std::vector<std::string> tokens;
            while (ss >> buf)
                tokens.push_back(buf);
            //RRasterData.push_back(& this->getRasterData(tokens[0]) );
            RRasterVariables.push_back(new double);
            p->DefineVar(*it, RRasterVariables[Rcounter++] );

        } else {
            std::cout << "Error: Variable not defined" << std::endl;
            QThread::currentThread()->exit();;
        }
    }




    p->DefineVar("CellValue", this->CellValue);
    p->DefineFun("nov", mu::numberOfValues);
    p->DefineFun("rand", mu::random , false);
    p->SetExpr(param.RExpression);
    RExpression = p;


}

double Marker::evaluateExpresion_R(int index, Node &p) {
    try {


        for ( unsigned int i = 0; i < RDoubleAttributes.size(); i++ ) {
            *RVariables.at(i) = RDoubleAttributes.at(i)->at(index);
        }
        for ( unsigned int i = 0; i < RRasterData.size(); i++ ) {
            *RRasterVariables.at(i) = RRasterData.at(i)->getValue(p.getX(), p.getY());
        }

        *R = RExpression->Eval();
        return *R;

    }
    catch (mu::Parser::exception_type &e) {
        std::cout << e.GetMsg() << std::endl;
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

    long Vcounter = 0;
    long rcounter = 0;
    /*for ( std::vector<std::string>::const_iterator it = param.rVariables.begin(); it != param.rVariables.end(); ++it) {
        if (paramRaw.find(*it) != paramRaw.end()) {
            std::string buf;
            std::stringstream ss(paramRaw.at(*it));
            std::vector<std::string> tokens;
            while (ss >> buf)
                tokens.push_back(buf);

            if (tokens[0].compare("value") <= 0) {
                if ( tokens[1].compare("DoubleAttributes")<= 0) {
                    rDoubleAttributes.push_back(& this->getVectorData(tokens[2]).getDoubleAttributes(tokens[3]) );
                    rVariables.push_back(new double);
                    rExpression->DefineVar(*it, rVariables[Vcounter++] );
                }

            }
            if (tokens[0].compare("RasterData") <= 0) {
                rRasterData.push_back(& this->getRasterData(tokens[1], this->getT() ) ) ;
                rRasterVariables.push_back(new double);
                rExpression->DefineVar(*it, rRaste    p->rVariables[Rcounter++] );
            }
        } else {
            std::cout << "Error: Variable not defined" << std::endl;
            QThread::currentThread()->exit();;
        }
    }*/
    rRasterData.clear();
    rRasterVariables.clear();
    for ( std::vector<std::string>::const_iterator it = param.RData.begin(); it != param.RData.end(); ++it) {
        std::cout << "Try: " << *it << std::endl;
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
            std::cout << "Error: Variable not defined" << std::endl;
            QThread::currentThread()->exit();;
        }
    }

    p->DefineFun("nov", mu::numberOfValues);
    p->DefineFun("rand", mu::random, false);
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
            *rRasterVariables.at(i) = rRasterData.at(i)->getValue(p.getX(), p.getY());
        }
        //Calculate
        return rExpression->Eval();
    }
    catch (mu::Parser::exception_type &e) {
        std::cout << e.GetMsg() << std::endl;
    }

}
void Marker::run() {

    DM::View vIdentifier;
    if (param.Points == true )
        vIdentifier = DM::View(param.Identifier, DM::NODE, DM::READ);
    if (param.Edges == true )
        vIdentifier = DM::View(param.Identifier, DM::EDGE, DM::READ);
    sys_in = this->getData("Data");
    this->OutputMap = this->getRasterData("Result", DM::View("result", DM::RASTERDATA, DM::WRITE));
    this->OutputMap->setSize(param.Width, param.Height, param.CellSize);
    this->OutputMap->clear();

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
    rExpression->DefineFun("rand", mu::random, false);
    rExpression->DefineVar("Value", V);
    RExpression->DefineVar("Value", V);
    RExpression->DefineFun("rand", mu::random, false);
    maxExpression->DefineVar("R", R);
    maxExpression->DefineVar("r", r);
    maxExpression->DefineVar("Value", V);
    maxExpression->DefineFun("rand", mu::random, false);
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
            points.push_back(Node((long) n->getX()/param.CellSize,(long) n->getY()/param.CellSize,n->getZ()));
        }
    }
    //AddLines as Points
    if ( param.Edges == true ) {
        DM::ComponentMap cmp = sys_in->getAllComponentsInView(vIdentifier);
        for (DM::ComponentMap::const_iterator it = cmp.begin(); it != cmp.end(); ++it) {
            DM::Edge * e = sys_in->getEdge(it->first);

            Node * p1 = sys_in->getNode(e->getStartpointName());
            Node * p2 = sys_in->getNode(e->getEndpointName());
            double dx = p2->getX()/param.CellSize - p1->getX()/param.CellSize;
            double dy = p2->getY()/param.CellSize - p1->getY()/param.CellSize;

            long steps = (long) sqrt(dx*dx+dy*dy);

            long x0 = (long) p1->getX()/param.CellSize;
            long y0 = (long) p1->getY()/param.CellSize;
            for ( int i = 0; i < steps; i++ ) {
                Node p;
                p.setX( x0 + i* (dx /  steps) );
                p.setY( y0 + i* (dy /  steps) );
                points.push_back(p);
            }
        }
    }

    //Create Mark for every point
    foreach(Node p, points) {
        //Set Values

        *(V) = p.getZ();
        foreach(std::string s, VariableNames) {
            double * d = VariableMap[s];
            *(d) = inputRasterData[s]->getValue(p.getX(), p.getY());
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
        if (maxI > param.Width)
            maxI = param.Width;
        if (maxJ > param.Height)
            maxJ = param.Height;
        Node currentPoint;

        for ( unsigned long  i = minI; i < maxI; i++ ) {
            for ( unsigned long j =  minJ; j < maxJ; j++ ) {
                currentPoint.setX( (double) i+0.5 );
                currentPoint.setY( (double) j+0.5 );
                *r = calculater(CenterPoint, currentPoint);
                if (*r <= *R ) {
                    double value;
                    try {
                        value = rExpression->Eval();
                    } catch (mu::Parser::exception_type &e) {
                        Logger(Error) << e.GetMsg();
                    }
                    if (param.PlacementOption.compare("KeepHigherValue") == 0) {
                        if (  this->OutputMap->getValue(i,j) > value ) {
                            value = this->OutputMap->getValue(i,j);
                        }
                    } else if ( param.PlacementOption.compare("KeepLowerValue") == 0 ) {
                        if ( this->OutputMap->getValue(i,j) < value && this->OutputMap->getValue(i,j) != 0 ) {
                            value = this->OutputMap->getValue(i,j);
                        }
                    } else if ( param.PlacementOption.compare("KeepValue") == 0 ) {
                        if ( this->OutputMap->getValue(i,j) != 0) {
                            value = this->OutputMap->getValue(i,j);

                        }
                    } else if ( param.PlacementOption.compare("Add") == 0 ) {
                        double val =  this->OutputMap->getValue(i,j);

                        value = value + val;
                    }
                    if (!this->param.maxExpression.empty()) {
                        double maxValue = maxExpression->Eval();
                        value = (value > maxValue ) ?  maxValue : value;
                    }

                    if (value > 1000) {
                        Logger(Debug) << value;
                    }

                    this->OutputMap->setValue(i,j,value);
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
