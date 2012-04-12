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
#include "generatesewernetwork.h"

#include "csg_s_operations.h"


DM_DECLARE_NODE_NAME(GenerateSewerNetwork, Sewer)

GenerateSewerNetwork::Agent::Agent(Pos StartPos) {
    this->successful = false;
    this->alive = true;
    this->currentPos.x = StartPos.x;
    this->currentPos.y = StartPos.y;
    this->currentPos.z = 0;
    this->currentPos.h = 3;
    this->lastdir = -1;
    this->neigh = ublas::vector<double>(9);
    this->decissionVector = ublas::vector<double>(9);
    this->ProbabilityVector = ublas::vector<double>(9);

}

void GenerateSewerNetwork::Agent::run() {
    this->path.clear();
    for (int i = 0; i < this->steps; i++) {
        this->currentPos.z = this->Topology->getValue(currentPos.x, currentPos.y);
        double Hcurrent = this->currentPos.z;
        double hcurrent = this->currentPos.h;
        this->path.push_back(currentPos);

        //Influence Topology
        Topology->getMoorNeighbourhood(neigh, currentPos.x,currentPos.y);
        double currentHeight = neigh[4];
        int index = GenerateSewerNetwork::indexOfMinValue(neigh);
        for (int i = 0; i < 9; i++) {
            if (currentHeight + (this->Hmin - this->currentPos.h) >= neigh[i]) {
                decissionVector[i] = 1*this->AttractionTopology;

            } else {
                decissionVector[i] = 0;
            }
            if (index == i )
                decissionVector[i] += MultiplyerCenterTop*this->AttractionTopology;
        }

        //Influence Topology
        ConnectivityField->getMoorNeighbourhood(neigh, currentPos.x,currentPos.y);
        currentHeight = neigh[4];
        index = GenerateSewerNetwork::indexOfMinValue(neigh);
        for (int i = 0; i < 9; i++) {
            if (currentHeight > neigh[i]) {
                decissionVector[i] += 1*this->AttractionConnectivity;
            } else {
                decissionVector[i] += 0;
            }
            if (index == i)
                decissionVector[i] += MultiplyerCenterCon*this->AttractionConnectivity;

        }


        //Forbidden Areas
        ForbiddenAreas->getMoorNeighbourhood(neigh, currentPos.x,currentPos.y);
        for (int i = 0; i < 9; i++) {
            decissionVector[i]*=neigh[i];
        }
        if (lastdir > -1){
            decissionVector[lastdir]= decissionVector[lastdir]* StablizierLastDir;
        }

        /*double neumann[9];
        neumann[0] = 0;
        neumann[1] = 1;
        neumann[2] = 0;
        neumann[3] = 1;
        neumann[4] = 1;
        neumann[5] = 1;
        neumann[6] = 0;
        neumann[7] = 1;
        neumann[8] = 0;
        for (int i = 0; i < 9; i++) {
            decissionVector[i]*=neumann[i];
        }*/


        ProbabilityVector = decissionVector / ublas::sum(decissionVector) * 100;

        int ra = rand()%100;
        // Logger(vibens::Standard) << ra;
        double prob = 0;
        int direction = -1;
        for (int i = 0; i < 9; i++) {
            prob += ProbabilityVector[i];

            if (ra <= prob) {
                direction = i;
                break;
            }
        }

        lastdir = direction;

        this->currentPos.x+=csg_s::csg_s_operations::returnPositionX(direction);
        this->currentPos.y+=csg_s::csg_s_operations::returnPositionY(direction);
        double deltaH = Hcurrent - this->Topology->getValue(currentPos.x, currentPos.y);
        if (deltaH > 0) {
            currentPos.h = hcurrent - deltaH;
            if (currentPos.h < 3) {
                currentPos.h = 3;
            }
        } else {
            currentPos.h=hcurrent-deltaH;
        }


        MarkPath->setValue(currentPos.x, currentPos.y, 1);

        if (currentPos.x < 0 || currentPos.y < 0 || currentPos.x > Topology->getWidth()-2 || currentPos.y >  Topology->getHeight()-2) {
            this->alive = false;
            //this->successful = true;
            //this->path.push_back(currentPos);
            //Logger(vibens::Debug) << "Successful";
            break;


        }

        //Check current Pos is < 3 to secure connections
        if (Goals->getValue(currentPos.x, currentPos.y ) > 0 && currentPos.h < this->Hmin) {
            this->alive = false;
            this->successful = true;
            this->path.push_back(currentPos);
            break;
        }


    }

    this->alive = false;
}

void GenerateSewerNetwork::addRadiusValueADD(int x, int y, RasterData *layer, int rmax, double value) {
    int level = rmax;
    if (rmax > 500) {
        rmax = 500;
    }
    int i = x - rmax;
    int j = y - rmax;

    if (i < 0) i = 0;
    if (j < 0) j = 0;

    int imax = 10;
    int jmax = 10;
    int k = 0;
    double z = rmax*rmax;
    for (i; i < rmax+x;  i++) {
        j = y - rmax;

        for ( j;  j < rmax+y; j++) {
            double r = double((i-x)*(i-x) + (j-y)*(j-y));
            double val = ((r-z)/10.)*(value);
            if (layer->getValue(i,j) > val )
                layer->setValue(i,j, val);
        }
    }
}
void GenerateSewerNetwork::addRadiusValue(int x, int y, RasterData * layer, int rmax, double value, double ** stamp) {

    if (rmax > 500) {
        rmax = 500;
    }
    int i = x - rmax;
    int j = y - rmax;

    if (i < 0) i = 0;
    if (j < 0) j = 0;

    int i_small = 0;
    int limitx =  rmax+x;
    int limity =  rmax+y;
    for (i; i < limitx;  i++ ) {
        j = y - rmax;
        int j_small = 0;
        for ( j;  j < limity; j++) {
            double val =  stamp[i_small][j_small] * value;
            if (layer->getValue(i,j) > val ) {
                layer->setValue(i,j,val );
            }
            j_small++;
        }
        i_small++;
    }
}

void GenerateSewerNetwork::MarkPathWithField(const std::vector<Pos> & path, RasterData * ConnectivityField, int ConnectivityWidth) {

    if (path.size() < 1) {
        Logger(DM::Debug) << "MarkPathWithField: Path Size = 0" ;
        return;
    }
    int last = path.size() - 1;
    RasterData Buffer;
    Buffer.setSize(ConnectivityField->getWidth(), ConnectivityField->getHeight(), ConnectivityField->getCellSize());
    Buffer.clear();


    //Cost Function for Length
    double x = path[0].x - path[last].x;
    double y = path[0].y - path[last].y;

    //Calculate Optimal Length
    double r_opt = sqrt(x * x + y * y);




    //Mark Field
    int level = ConnectivityWidth;
    int rmax = ConnectivityWidth;
    if (rmax > 500) {
        rmax = 500;
    }


    double** stamp = new double*[rmax*2];
    for (int i = 0; i < rmax*2; i++) {
        stamp[i] = new double[rmax*2];
    }
    int x_p = rmax;
    for (int i = 0; i < rmax*2;  i++) {
        for (int j = 0; j < rmax*2;  j++) {
            if (i != x_p || j != x_p) {
                double r = sqrt(double((i-x_p)*(i-x_p) + (j-x_p)*(j-x_p)));
                double val = (-level/10. * 1./r);
                stamp[i][j]  = val;
            } else {
                double val = (-level/10. *( 2.) );
                stamp[i][j] = val;
            }
        }
    }

    for (int i = 0; i < path.size(); i++) {

        double val = 1 - ((double) i / (double) path.size());

        double r = last+1;

        val = val * r_opt / r;



        // GenerateSewerNetwork::addRadiusValueADD(
        //    path[last - i].x,  path[last - i].y, & Buffer, ConnectivityWidth, val);

        GenerateSewerNetwork::addRadiusValue(path[last - i].x,  path[last - i].y, & Buffer, ConnectivityWidth,  val, stamp);

    }

    for (int i = 0; i < rmax; i++) {
        delete stamp[i];
    }
    delete stamp;

    x = path[last].x;
    y = path[last].y;
    for (int i = 0; i < ConnectivityField->getHeight(); i++) {
        for (int j = 0; j < ConnectivityField->getWidth(); j++) {

            double val = ConnectivityField->getValue(j, i);
            double val2 = Buffer.getValue(j, i);
            if (val > val2) {
                ConnectivityField->setValue(j, i, val2);
            }
        }
    }
}

int GenerateSewerNetwork::indexOfMinValue(const ublas::vector<double> &vec) {
    double val = vec[0];
    int index = 0;
    for (int i = 1; i < 9; i++) {
        if (vec[i] < val) {
            val = vec[i];
            index = i;
        }
    }

    //Check if alone
    int n = 0;
    for (int i = 0; i < 9; i++) {
        if (vec[i] == val) {
            n++;
        }
    }
    if (n > 1)
        index = -1;
    return index;
}

GenerateSewerNetwork::GenerateSewerNetwork()
{


    this->ConnectivityWidth = 9;
    this->AttractionTopology = 1;
    this->AttractionConnectivity = 1;
    this->IdentifierStartPoins = "";
    this->steps = 1000;
    this->Hmin = 8;
    MultiplyerCenterCon = 1;
    MultiplyerCenterTop = 1;
    StablizierLastDir = 1;



    this->addParameter("MaxDeph", DM::DOUBLE, &this->Hmin);
    this->addParameter("Steps", DM::LONG, & this->steps);
    this->addParameter("ConnectivityWidth", DM::LONG, & this->ConnectivityWidth);
    this->addParameter("AttractionTopology", DM::DOUBLE, & this->AttractionTopology);
    this->addParameter("AttractionConnectivity", DM::DOUBLE, & this->AttractionConnectivity);
    this->addParameter("MultiplyerCenterCon", DM::DOUBLE, & this->MultiplyerCenterCon);
    this->addParameter("MultiplyerCenterTop", DM::DOUBLE, & this->MultiplyerCenterTop);
    this->addParameter("StablizierLastDir", DM::INT, &this->StablizierLastDir);


    Topology = DM::View("Topology", DM::RASTERDATA, DM::READ);
    std::vector<DM::View> city;

    Inlets = DM::View("INLET", DM::NODE, DM::READ);
    Inlets.getAttribute("New");
    Inlets.getAttribute("ID_CATCHMENT");

    catchment = DM::View("CATCHMENT", DM::FACE, DM::READ);
    catchment.getAttribute("Population");

    city.push_back(Topology);
    city.push_back(Inlets);

    this->addData("City", city);




    ForbiddenAreas = DM::View("ForbiddenAreas", DM::RASTERDATA, DM::READ);
    Goals = DM::View("Goals", DM::RASTERDATA, DM::READ);

    std::vector<DM::View> sewerGeneration_in;
    sewerGeneration_in.push_back(ForbiddenAreas);
    sewerGeneration_in.push_back(Goals);
    this->addData("sewerGeneration_In", sewerGeneration_in);

    std::vector<DM::View> sewerGeneration_con;
    ConnectivityField_in = DM::View("ConnectivityField_in", DM::RASTERDATA, DM::READ);
    sewerGeneration_con.push_back(ConnectivityField_in);
    this->addData("sewerGeneration_con", sewerGeneration_con);

    std::vector<DM::View> sewerGeneration_out;

    ConnectivityField = DM::View("ConnectivityField_in", DM::RASTERDATA, DM::WRITE);
    Path = DM::View("Path", DM::RASTERDATA, DM::WRITE);
    sewerGeneration_out.push_back(Path);
    sewerGeneration_out.push_back(ConnectivityField);
    this->addData("sewerGeneration_Out", sewerGeneration_out);

}
void GenerateSewerNetwork::run() {
    this->city = this->getData("City");

    DM::System * sewerGeneration_in = this->getData("sewerGeneration_In");
    DM::System * sewerGeneration_out = this->getData("sewerGeneration_Out");

    DM::System * sewerGeneration_con = this->getData("sewerGeneration_con");
    rTopology = this->getRasterData("City", Topology);


    rConnectivityField_in = this->getRasterData("sewerGeneration_con", ConnectivityField_in);
    rForbiddenAreas  = this->getRasterData("sewerGeneration_In", ForbiddenAreas);
    rGoals = this->getRasterData("sewerGeneration_In", Goals);

    rPath  = this->getRasterData("sewerGeneration_Out", Path);
    rConnectivityField = this->getRasterData("sewerGeneration_Out", ConnectivityField);


    long width = this->rTopology->getWidth();
    long height = this->rTopology->getHeight();
    double cellSize = this->rTopology->getCellSize();

    this->rConnectivityField->setSize(width, height, cellSize);
    Logger(Debug) << "Conn Max " << this->rConnectivityField_in->getMaxValue();
    Logger(Debug) << "Conn Min " << this->rConnectivityField_in->getMinValue();
    Logger(Debug) << "DebugVal " << this->rConnectivityField_in->getDebugValue();
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++ ) {
            this->rConnectivityField->setValue(i,j, this->rConnectivityField_in->getValue(i,j)*0.5);
        }
    }
    this->rConnectivityField->setDebugValue(rConnectivityField_in->getDebugValue()+1);
    this->rPath->setSize(width, height, cellSize);
    this->rPath->clear();
    std::vector<Agent * > agents;


    std::vector<DM::Node*> StartPos;
    foreach (std::string inlet, city->getNamesOfComponentsInView(Inlets))  {
        DM::Node * n = city->getNode(inlet);
        std::string ID_CA = n->getAttribute("ID_CATCHMENT")->getString();
        //DM::Face * catchment = city->getFace(ID_CA);
        //Just For Now
        //if (catchment->getAttribute("Population")->getDouble() > 10) {
            StartPos.push_back(n);
        //}
    }
    //Create Agents
    int attrtopo = this->AttractionTopology - this->internalCounter;
    if (attrtopo < 0)
        attrtopo = 0;
    int attrcon = this->AttractionConnectivity + this->internalCounter;
    if (attrcon < 0)
        attrcon = 0;
    foreach(DM::Node * p, StartPos) {
        long x = (long) p->getX()/cellSize;
        long y = (long) p->getY()/cellSize;
        Agent * a = new Agent(Pos(x,y));
        a->Topology = this->rTopology;
        a->MarkPath = this->rPath;
        a->ConnectivityField = this->rConnectivityField;
        a->Goals = this->rGoals;
        a->AttractionTopology = attrtopo;
        a->AttractionConnectivity = attrcon;
        a->MultiplyerCenterCon = MultiplyerCenterCon;
        a->MultiplyerCenterTop = MultiplyerCenterTop;
        a->steps = this->steps;
        a->Hmin = this->Hmin;
        a->ForbiddenAreas = this->rForbiddenAreas;
        a->StablizierLastDir = this->StablizierLastDir;
        agents.push_back(a);

    }
    long successfulAgents = 0;
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < agents.size(); j++) {
            Agent * a = agents[j];
            if (a->alive) {
                a->run();
            }
        }
        for (int j = 0; j < agents.size(); j++) {
            Agent * a = agents[j];
            if (a->successful) {
                successfulAgents++;
                GenerateSewerNetwork::MarkPathWithField(a->path, this->rConnectivityField, this->ConnectivityWidth);
            }
        }

    }
    Logger(DM::Debug) << "Successful " << successfulAgents;


    for (int j = 0; j < agents.size(); j++) {
        delete agents[j];
    }
    agents.clear();

}
