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
#include "cellularautomata.h"
#include <QString>
#include <string>
#include <boost/numeric/ublas/vector.hpp>
#include "userdefinedfunctions.h"
#include <QThread>
#include <guicellularautomata.h>
#include <QWidget>
#include <QStringList>
#include <boost/foreach.hpp>

using namespace boost;
namespace   ublas = boost::numeric::ublas;

DM_DECLARE_NODE_NAME( CellularAutomata, Modules )

CellularAutomata::CellularAutomata()
{
    runinit = false;
    param.Width = 200;
    param.Height = 200;
    param.CellSize = 20;
    param.Steps = 1;
    NameOfOutput = "";
    NameOfOutput_old  = "";
    this->addParameter("Width", DM::LONG, &this->param.Width);
    this->addParameter("Height", DM::LONG, &this->param.Height);
    this->addParameter("CellSize", DM::DOUBLE, &this->param.CellSize);
    this->addParameter("Steps", DM::INT, &this->param.Steps);
    this->addParameter("Neighs", DM::STRING_MAP, &this->param.neighs);
    this->addParameter("Rules", DM::STRING_MAP, &this->param.rules);
    this->addParameter("ListOfLandscapes", DM::STRING_LIST, &param.ListOfLandscapes);
    this->addParameter("NameOfOutput", DM::STRING, &NameOfOutput);
    this->addParameter("Desicion", DM::STRING, &this->param.Desicion);
}

void CellularAutomata::addLandscape(string s) {
    this->param.ListOfLandscapes.push_back(s);
    this->init();
}

bool CellularAutomata::createInputDialog() {
    QWidget * w = new GUICellularAutomata(this);
    w->show();
    return true;
}
void CellularAutomata::updateInport() {
    if(this->NameOfOutput.compare(this->NameOfOutput_old) == 0)
        return;


    View output(this->NameOfOutput, DM::RASTERDATA, DM::WRITE);
    std::vector<DM::View> data_out;
    data_out.push_back(output);
    this->NameOfOutput_old = NameOfOutput;
    this->addData(this->NameOfOutput, data_out);

}

void CellularAutomata::init() {

    updateInport();

    bool changed = false;
    foreach (std::string s, param.ListOfLandscapes) {
        if (find(vExistingData.begin(), vExistingData.end(), s) != vExistingData.end()) {
            continue;
        }
        changed = true;

    }
    if (changed == false)
        return;
    std::vector<DM::View> data;
    foreach (std::string s, param.ListOfLandscapes) {
        View rdata(s, DM::RASTERDATA, DM::READ);
        data.push_back(rdata);
        vExistingData.push_back(s);
    }

    if (changed == true)
        this->addData("RasterDataIn", data);


}

void CellularAutomata::run()  {
    this->param.OutputMap = this->getRasterData(this->NameOfOutput,View(this->NameOfOutput, DM::RASTERDATA, DM::WRITE));
    this->param.OutputMap->setSize(param.Width, param.Height, param.CellSize);
    DM::System * systest1 = this->getData(this->NameOfOutput);
    std::map<std::string, std::vector<DM::View> > views =  this->getViews();
    std::vector<View> data = views["RasterDataIn"];

    foreach (std::string s, param.ListOfLandscapes) {
        View rdata(s, DM::RASTERDATA, DM::READ);
        DM::System * systest = this->getData("RasterDataIn");

        DM::RasterData * r = this->getRasterData("RasterDataIn",rdata);
        this->landscapes[s] = this->getRasterData("RasterDataIn",rdata);
    }

    if (runinit == false) {
        this->initRuntime();
    }

    //Eval Expressions
    for (int x = 0; x < this->param.Width; x++) {
        for (int y = 0; y < this->param.Height; y++) {
            //Update RasterData


            BOOST_FOREACH(std::string s, this->NeighboorhoodList) {

                RasterData * r = landscapes[this->NeighboorhoodMapName[s]];
                r->getNeighboorhood(this->NeighboorhoodMaps[s], this->NeighboohoodDimensions[s].widht,this->NeighboohoodDimensions[s].height, x, y );
            }
            for (int counter = 0; counter < this->Rules.size(); counter++) {

                Parser * p=this->Rules.at(counter);
                try
                {
                    double val =  p->Eval();

                    *(this->RulesResults.at(counter)) = val;
                } catch (Parser::exception_type &e)
                {
                    Logger(Error) << p->GetExpr()  ;
                    Logger(Error) << e.GetMsg()  ;
                }
            }
            this->param.OutputMap->setValue(x,y,this->Desicion->Eval());
        }
    }


}


void CellularAutomata::initRuntime() {
    runinit =true;

    for ( std::map<std::string, std::string>::iterator it = param.neighs.begin(); it != param.neighs.end(); ++it) {
        std::string name= it->first;
        QString  value = QString::fromStdString(it->second);
        QStringList list = value.split("+|+");
        std::string dataFromPort = list[0].toStdString();

        //Find Matrix Dimension;
        list[2]  = list[2].remove("|");
        QStringList rows = list[2].split(";");
        double **Neighbourhood;
        int **Neighbourhood_Stamp;
        int width = rows.size();
        Neighbourhood = new double*[width];
        Neighbourhood_Stamp = new int*[width];
        int height;
        int i = 0;
        foreach(QString s, rows) {
            //Logger(Debug) << s.toStdString()  ;
            height = s.split(",").size();
            Neighbourhood[i] = new double[height];
            Neighbourhood_Stamp[i] = new int[height];
            int j = 0;
            foreach(QString v, s.split(",")) {
                // Logger(Debug) << v.toInt();
                Neighbourhood_Stamp[i][j] = v.toInt();
                j++;
            }
            i++;
        }
        this->NeighboorhoodMaps[name] = Neighbourhood;
        this->NeighboorhoodStamps[name] = Neighbourhood_Stamp;
        this->NeighboorhoodMapName[name] = dataFromPort;
        Dimension dim;
        dim.widht = width;
        dim.height = height;


        //Calculate Neighboorhoud PointerMap
        int sum = 0;
        for (int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                sum = sum + Neighbourhood_Stamp[x][y];
            }
        }
        //Logger(Debug) << sum  ;
        double **d = new double*[sum];
        i = 0;
        for (int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                if (Neighbourhood_Stamp[x][y] > 0 ) {
                    d[i] = & Neighbourhood[x][y];
                    i++;
                }
            }
        }
        this->NeighboorhoodPointerMap[name] = d;
        dim.elements = sum;
        this->NeighboohoodDimensions[name] = dim;
        NeighboorhoodList.push_back(name);
    }
    //Init Rules;

    for(std::map<std::string, std::string>::iterator it = param.rules.begin(); it != param.rules.end(); ++it) {
        std::string Name = it->first;
        QString rule = QString::fromStdString(it->second);
        Parser * p = new Parser();
        mu::addCorineConstants(p);
        p->DefineFun("nov", numberOfValues);
        p->DefineFun("rand", random, false);
        //Replace Names
        BOOST_FOREACH(std::string neigh, NeighboorhoodList) {
            int elements = NeighboohoodDimensions[neigh].elements;
            std::stringstream ss;
            for (int i = 0; i < elements; i++) {
                if (i > 0)
                    ss << ",";
                ss << neigh << i;

                std::stringstream varname;
                varname << neigh << i;
                double ** d =  NeighboorhoodPointerMap[neigh];
                p->DefineVar(varname.str(), d[i]);
            }
            rule.replace(QString::fromStdString(neigh),QString::fromStdString(ss.str()));
        }
        p->SetExpr(rule.toStdString());
        Rules.push_back( p );
        RulesResults.push_back(new double);

    }
    //Define Descision
    Desicion = new Parser();
    Desicion->DefineFun("nov", numberOfValues);
    Desicion->DefineFun("rand", random, false);
    int i = 0;
    mu::addCorineConstants(Desicion);
    for(std::map<std::string, std::string>::iterator it = param.rules.begin(); it != param.rules.end(); ++it) {
        std::string Name = it->first;
        Desicion->DefineVar(Name, RulesResults.at(i));
        i++;
    }
    Desicion->SetExpr(param.Desicion);
}


