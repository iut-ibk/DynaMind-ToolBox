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
#include "userdefinedfunctions.h"
#include <QThread>
#include <guicellularautomata.h>
#include <QWidget>
#include <QStringList>
#include <algorithm>

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
    param.offsetX = 0;
    param.offsetY = 0;
    param.DimensionOfExisting = "user defined";
    param.appendToStream = true;

    this->addParameter("Width", DM::LONG, &this->param.Width);
    this->addParameter("Height", DM::LONG, &this->param.Height);
    this->addParameter("OffsetX", DM::LONG, &this->param.offsetX);
    this->addParameter("OffsetY", DM::LONG, &this->param.offsetY);

    this->addParameter("CellSize", DM::DOUBLE, &this->param.CellSize);
    this->addParameter("Steps", DM::INT, &this->param.Steps);
    this->addParameter("Neighs", DM::STRING_MAP, &this->param.neighs);
    this->addParameter("Rules", DM::STRING_MAP, &this->param.rules);
    this->addParameter("ListOfLandscapes", DM::STRING_LIST, &param.ListOfLandscapes);
    this->addParameter("NameOfOutput", DM::STRING, &NameOfOutput);
    this->addParameter("Desicion", DM::STRING, &this->param.Desicion);
    this->addParameter("DimensionOfExisting", DM::STRING, &this->param.DimensionOfExisting);
    this->addParameter("appendToStream", DM::BOOL, &this->param.appendToStream);


    std::vector<DM::View> data;
    if (param.appendToStream)
        data.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::READ));
    param.appendToStream_old = param.appendToStream;

    this->addData("RasterDataIn", data);
}

void CellularAutomata::addLandscape(string s) {
    this->param.ListOfLandscapes.push_back(s);
    this->init();
}

void CellularAutomata::removeLandscape(string s)
{
    if (std::find(this->param.ListOfLandscapes.begin(), this->param.ListOfLandscapes.end(), s) == this->param.ListOfLandscapes.end())
        return;
    this->param.ListOfLandscapes.erase(std::find(this->param.ListOfLandscapes.begin(), this->param.ListOfLandscapes.end(), s));
}

void CellularAutomata::removeNeighboorhood(string neigh)
{
    Logger(Debug) << neigh;
    if (param.neighs.find(neigh) ==param.neighs.end()) return;
    param.neighs.erase(neigh);
}

void CellularAutomata::removeRule(string rule)
{
    Logger(Debug) << rule;
    if (param.rules.find(rule) ==param.rules.end()) return;
    param.rules.erase(rule);
}

std::vector<string> CellularAutomata::getLandscapesInStream()
{
    std::vector<std::string> landscapes;
    DM::System * sys = this->getData("RasterDataIn");
    if (!sys)
        return landscapes;
    std::vector<DM::View> views = sys->getViews();

    foreach (DM::View v, views) {
        if (v.getType() == DM::RASTERDATA)
            landscapes.push_back(v.getName());
    }
    return landscapes;

}

bool CellularAutomata::createInputDialog() {
    QWidget * w = new GUICellularAutomata(this);
    w->show();
    return true;
}
void CellularAutomata::updateInport() {
    if(this->NameOfOutput == NameOfOutput_old)
        return;

    this->removeData(this->NameOfOutput_old);
    View output(this->NameOfOutput, DM::RASTERDATA, DM::WRITE);
    std::vector<DM::View> data_out;
    data_out.push_back(output);
    this->NameOfOutput_old = NameOfOutput;
    this->addData(this->NameOfOutput, data_out);

}

void CellularAutomata::init() {

    bool changed = false;

    updateInport();

    if (param.appendToStream != param.appendToStream_old) changed = true;

    foreach (std::string s, param.ListOfLandscapes) {
        if (std::find(vExistingData.begin(), vExistingData.end(), s) != vExistingData.end()) {
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
    if (param.appendToStream)
        data.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::READ));
    else if (!data.size())
        this->removeData("RasterDataIn");
    param.appendToStream_old = param.appendToStream;
    this->addData("RasterDataIn", data);



}

void CellularAutomata::run()  {
    if (param.Desicion.empty()) {
        Logger(Error) << "No decision set";
        return;
    }

    long Width = param.Width;
    long Height = param.Height;
    double CellSizeX = param.CellSize;
    double CellSizeY = param.CellSize;
    long offsetX = param.offsetX;
    long offsetY = param.offsetY;

    if ((this->param.appendToStream) && (this->param.DimensionOfExisting != "user defined")) {
        DM::RasterData * r = this->getRasterData("RasterDataIn", DM::View(this->param.DimensionOfExisting, DM::RASTERDATA, DM::READ));

        Width = r->getWidth();
        Height = r->getHeight();
        CellSizeX = r->getCellSizeX();
        CellSizeY = r->getCellSizeX();
        offsetX = r->getXOffset();
        offsetY = r->getYOffset();
    }

    this->param.OutputMap = this->getRasterData(this->NameOfOutput,View(this->NameOfOutput, DM::RASTERDATA, DM::WRITE));
    this->param.OutputMap->setSize(Width, Height, CellSizeX,CellSizeY,offsetX,offsetY);
    std::map<std::string, std::vector<DM::View> > views =  this->getViews();

    foreach (std::string s, param.ListOfLandscapes) {
        View rdata(s, DM::RASTERDATA, DM::READ);
        this->landscapes[s] = this->getRasterData("RasterDataIn",rdata);
        if  (!this->landscapes[s] ) {
            assert (this->landscapes[s]!=NULL);
            Logger(DM::Error) << "RasterData  " << s << " not set";
            return;

        }
    }

    if (runinit == false) {
        this->initRuntime();
    }

    //Eval Expressions
    for (int x = 0; x < this->param.Width; x++) {
        for (int y = 0; y < this->param.Height; y++) {
            //Update RasterData

            foreach(std::string s, this->NeighboorhoodList) {

                RasterData * r = landscapes[this->NeighboorhoodMapName[s]];
                r->getNeighboorhood(this->NeighboorhoodMaps[s], this->NeighboohoodDimensions[s].widht,this->NeighboohoodDimensions[s].height, x, y );
            }
            for (unsigned int counter = 0; counter < this->Rules.size(); counter++) {

                Parser * p=this->Rules.at(counter);
                try
                {
                    double val =  p->Eval();
                    //Logger(Debug) << val;
                    *(this->RulesResults.at(counter)) = val;
                } catch (Parser::exception_type &e)
                {
                    Logger(Error) << p->GetExpr()  ;
                    Logger(Error) << e.GetMsg()  ;
                    return;
                }
            }
            double ressult = this->Desicion->Eval();
            this->param.OutputMap->setCell(x,y,ressult);
        }
    }


}

string CellularAutomata::getHelpUrl()
{
    return "https://docs.google.com/document/d/1aCObKJYYQV2Iq23xFfgq-uMdH1c1ecVXRg_mNC5twlk/edit#";
}
CellularAutomata::~CellularAutomata() {
    this->deinit();
}

void CellularAutomata::deinit() {
    if (runinit)
        return;
    for ( std::map<std::string, std::string>::iterator it = param.neighs.begin(); it != param.neighs.end(); ++it) {

        std::string name= it->first;
        int **Neighbourhood_Stamp = NeighboorhoodStamps[name];
        double **Neighbourhood = NeighboorhoodMaps[name];
        Dimension dim = this->NeighboohoodDimensions[name];
        int width = dim.widht;
        for (int i = 0; i < width; i++) {
            delete[] Neighbourhood[i];
            delete[] Neighbourhood_Stamp[i];
        }
        delete[] Neighbourhood;
        delete[] Neighbourhood_Stamp;

        delete this->NeighboorhoodPointerMap[name];

    }
    NeighboorhoodStamps.clear();
    NeighboorhoodMaps.clear();
    NeighboorhoodPointerMap.clear();
    NeighboohoodDimensions.clear();


    foreach (double * rs, RulesResults)
        delete rs;
    RulesResults.clear();
    foreach (Parser * p, Rules )
        delete p;
    Rules.clear();
    foreach (double * d, pRessults) {
        delete d;
    }
    pRessults.clear();


    this->runinit = false;
}

void CellularAutomata::initRuntime() {
    runinit = true;

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
        foreach(std::string neigh, NeighboorhoodList) {
            int elements = NeighboohoodDimensions[neigh].elements;
            std::stringstream ss;
            for (int i = 0; i < elements; i++) {
                if (i > 0)
                    ss << ",";
                ss << neigh << i;

                std::stringstream varname;
                varname << neigh << i;
                double ** d =  NeighboorhoodPointerMap[neigh];
                p->DefineVar(varname.str(), (double*)d[i]);
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


