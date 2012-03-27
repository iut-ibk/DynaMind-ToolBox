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
#include "timeareamethod.h"
#include "tbvectordata.h"

DM_DECLARE_NODE_NAME(TimeAreaMethod, Sewer)
TimeAreaMethod::TimeAreaMethod()
{

    conduit = DM::View("CONDUIT", DM::EDGE, DM::READ);
    conduit.addAttribute("Diameter");
    conduit.addAttribute("Length");

    inlet = DM::View("INLET", DM::NODE, DM::READ);
    inlet.getAttribute("Connected");
    inlet.addAttribute("WasteWater");
    inlet.addAttribute("InfiltrationWater");
    inlet.addAttribute("Area");
    inlet.addAttribute("QrKrit");
    inlet.addAttribute("Impervious");
    inlet.getAttribute("ID_CATCHMENT");

    shaft = DM::View("JUNCTION", DM::NODE, DM::READ);
    shaft.addAttribute("WasteWaterPerShaft");
    shaft.addAttribute("InfiltrationWaterPerShaft");
    shaft.addAttribute("QrKritPerShaft");
    shaft.addAttribute("AreaPerShaft");

    wwtps = DM::View("WWTP", DM::NODE, DM::READ);
    catchment = DM::View("CATCHMENT", DM::FACE, DM::READ);
    storage = DM::View("STORAGE", DM::NODE, DM::WRITE);
    storage.addAttribute("StorageV");
    storage.addAttribute("Storage");
    storage.addAttribute("StorageA");
    storage.addAttribute("ConnectedStorageArea");
    catchment.getAttribute("Population");
    catchment.getAttribute("Area");
    catchment.getAttribute("Impervious");
    catchment.getAttribute("WasteWater");

    std::vector<DM::View> views;

    outfalls= DM::View("OUTFALL", DM::NODE, DM::WRITE);
    weir = DM::View("WEIR", DM::NODE, DM::WRITE);
    weir.addAttribute("InletOffset");

    globals = DM::View("GLOBALS_SEWER", DM::NODE, DM::WRITE);
    globals.addAttribute("CONNECTEDPOP");

    views.push_back(conduit);
    views.push_back(inlet);
    views.push_back(shaft);
    views.push_back(wwtps);
    views.push_back(catchment);
    views.push_back(outfalls);
    views.push_back(weir);
    views.push_back(storage);
    views.push_back(globals);

    this->v = 1;
    this->r15 = 150;

    this->addParameter("v", DM::DOUBLE, & this->v);
    this->addParameter("r15", DM::DOUBLE, & this->r15);

    this->addData("City", views);




}

double TimeAreaMethod::caluclateAPhi(DM::Component * attr, double r15)  const {

    double n = 1.;
    double T = 10.;
    double phi10 = (38./(T+9.)*(pow(n,(-1/4))-0,369));
    //double r10 = r15 * phi10;

    T = attr->getAttribute("Time")->getDouble() / 60.;
    if (T < 10) {
        T = 10;
    }
    double phiT = (38./(T+9.)*(pow(n,(-1/4))-0,369));

    return phiT/phi10;


}


void TimeAreaMethod::run() {

    DM::System *city= this->getData("City");
    std::vector<std::string> InletNames;
    InletNames = city->getNamesOfComponentsInView(inlet);
    std::vector<std::string> ConduitNames;
    ConduitNames = city->getNamesOfComponentsInView(conduit);

    //Create Connection List
    foreach(std::string name , ConduitNames)  {
        DM::Edge * e = city->getEdge(name);
        DM::Node * startnode = city->getNode(e->getStartpointName());
        std::vector<DM::Edge*> v = ConnectedEdges[startnode];
        v.push_back(e);
        ConnectedEdges[startnode] = v;

        DM::Node * Endnode = city->getNode(e->getEndpointName());
        v = ConnectedEdges[Endnode];
        v.push_back(e);
        ConnectedEdges[Endnode] = v;
    }


    foreach(std::string name , ConduitNames)  {
        DM::Edge * e = city->getEdge(name);
        DM::Node * startnode = city->getNode(e->getStartpointName());
        std::vector<DM::Edge*> v = StartNodeSortedEdges[startnode];
        v.push_back(e);
        StartNodeSortedEdges[startnode] = v;


        DM::Node * Endnode = city->getNode(e->getEndpointName());
        v = EndNodeSortedEdges[Endnode];
        v.push_back(e);
        EndNodeSortedEdges[Endnode] = v;

    }
    //Calculate Waste Water
    double Population_sum = 0;





    double WasterWaterPerPerson = 0.0052;
    double InfiltrationWater =  0.003;


    //Calculate Water Water per Node
    foreach(std::string id_inlet, InletNames) {

        DM::Component *  inlet_attr = city->getComponent(id_inlet);
        if (inlet_attr->getAttribute("Connected")->getDouble() < 0.01 )
            continue;
        DM::Component *  catchment_attr = city->getComponent(inlet_attr->getAttribute("ID_CATCHMENT")->getString());



        double pop =  catchment_attr->getAttribute("Population")->getDouble();
        double area = catchment_attr->getAttribute("Area")->getDouble();
        double imp = catchment_attr->getAttribute("Impervious")->getDouble();
        if(imp < 0.2) {
            imp = 0.2;
        }

        inlet_attr->addAttribute("InfiltrationWater", pop * InfiltrationWater);
        inlet_attr->addAttribute("WasterWater",  pop * WasterWaterPerPerson);
        inlet_attr->addAttribute("Area",area*imp);
        inlet_attr->addAttribute("QrKrit", 15.*area/10000*imp);
        inlet_attr->addAttribute("Impervious",imp);

        Population_sum += catchment_attr->getAttribute("Population")->getDouble();

    }

    DM::Node sewerGlobal = DM::Node(0,0,0);
    DM::Node * sg = city->addNode(sewerGlobal, this->globals);
    sg->addAttribute("CONNECTEDPOP", Population_sum);

    std::vector<std::string> endPointNames = city->getNamesOfComponentsInView(wwtps);
    foreach(std::string name, endPointNames) {
        this->EndPointList.push_back(city->getNode(name));
        DM::Node * p = city->getNode(name);
        p->addAttribute("StorageV",0);
        p->addAttribute("Storage",1);
        city->addComponentToView(p,storage);
    }

    //AddStorageToWWtp
    foreach (std::string name, city->getNamesOfComponentsInView(conduit)) {
        DM::Edge * con = city->getEdge(name);
        DM::Node * start = city->getNode(con->getStartpointName());
        DM::Node * end = city->getNode(con->getEndpointName());
        DM::Node dp = *(start) - *(end);
        double l = sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
        con->addAttribute("Length", l);

    }

    foreach(std::string name, InletNames) {

        DM::Component * inlet_attr;
        inlet_attr = city->getComponent(name);

        double wastewater = inlet_attr->getAttribute("WasterWater")->getDouble();
        double infiltreationwater = inlet_attr->getAttribute("InfiltrationWater")->getDouble();
        double area = inlet_attr->getAttribute("Area")->getDouble();
        double QrKrit = inlet_attr->getAttribute("QrKrit")->getDouble();
        DM::Node * id = city->getNode(name);
        bool ReachedEndPoint = false;
        DM::Node * idPrev = 0;
        //Length is reset if Outfall is hit
        double StrangL = 0;
        //Length is reset if total length
        double StrangL_Total = 0;
        double QKrit = 0;
        double DeltaA = 0;
        do {
            id->getAttribute("WasteWaterPerShaft")->setDouble(id->getAttribute("WasteWaterPerShaft")->getDouble() + wastewater);
            id->getAttribute("InfiltrationWaterPerShaft")->setDouble(id->getAttribute("InfiltrationWaterPerShaft")->getDouble() + infiltreationwater);
            id->getAttribute("AreaPerShaft")->setDouble(id->getAttribute("AreaPerShaft")->getDouble() + area);
            id->getAttribute("QrKritPerShaft")->setDouble(id->getAttribute("QrKritPerShaft")->getDouble() + QrKrit);

            if (idPrev != 0) {
                DM::Node dp = *(idPrev) - *(id);

                StrangL += sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
                StrangL_Total += sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
            }

            //Search for Outfall
            if (id->getAttribute("Outfall")->getDouble() > 0.01) {
                StrangL = 0;
                QKrit =  id->getAttribute("QrKritPerShaft")->getDouble();
                DeltaA = id->getAttribute("AreaPerShaft")->getDouble();

                if (id->isInView(storage)) {
                    //Storage Capacity
                    double capacity = id->getAttribute("Storage")->getDouble();
                    id->addAttribute("ConnectedStorageArea", DeltaA);

                    QKrit =  id->getAttribute("QrKritPerShaft")->getDouble() * (1-capacity);
                    DeltaA =  id->getAttribute("AreaPerShaft")->getDouble() * (1-capacity);
                    StrangL_Total = 0;

                }

            }

            if ( id->getAttribute("StrandLength")->getDouble() < StrangL)
                id->getAttribute("StrandLength")->setDouble(StrangL);

            if ( id->getAttribute("StrandLengthTotal")->getDouble() < StrangL_Total)
                id->getAttribute("StrandLengthTotal")->setDouble(StrangL_Total);

            id->getAttribute("QrKritPerShaft_total")->setDouble(QKrit);
            id->getAttribute("Area_total")->setDouble(id->getAttribute("AreaPerShaft")->getDouble() -DeltaA);



            idPrev = id;


            DM::Node * nextid_tmp = 0;
            DM::Edge * outgoing_id = 0;

            std::vector<DM::Edge*>  downstreamEdges = StartNodeSortedEdges[id];
            //o---o---x
            if (downstreamEdges.size() == 1) {
                DM::Edge * e = downstreamEdges[0];
                if (city->getNode(e->getEndpointName()) != id) {
                    nextid_tmp = city->getNode(e->getEndpointName());
                    outgoing_id = e;
                }
            }

            DM::Logger(DM::Debug) <<  "QrKritPerShaft" << id->getAttribute("QrKritPerShaft")->getDouble();
            DM::Logger(DM::Debug) <<  "QrKritPerShaft_total" << id->getAttribute("QrKritPerShaft_total")->getDouble();

            id = nextid_tmp;

            if (id == 0) {
                break;
            }

            foreach(DM::Node * p, this->EndPointList) {
                if (id == p ){
                    ReachedEndPoint = true;
                    id->getAttribute("WasteWaterPerShaft")->setDouble(id->getAttribute("WasteWaterPerShaft")->getDouble() + wastewater);
                    id->getAttribute("InfiltrationWaterPerShaft")->setDouble(id->getAttribute("InfiltrationWaterPerShaft")->getDouble() + infiltreationwater);
                    id->getAttribute("AreaPerShaft")->setDouble(id->getAttribute("AreaPerShaft")->getDouble() + area);
                    id->addAttribute("ConnectedStorageArea", id->getAttribute("AreaPerShaft")->getDouble() + area);

                    if (idPrev != 0) {
                        DM::Node dp = *(idPrev) - *(id);
                        StrangL += sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
                        StrangL_Total += sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
                    }

                    id->getAttribute("StrandLength")->getDouble();
                    if ( id->getAttribute("StrandLength")->getDouble() < StrangL)
                        id->getAttribute("StrandLength")->setDouble(StrangL);
                    if ( id->getAttribute("StrandLengthTotal")->getDouble() < StrangL_Total)
                        id->getAttribute("StrandLengthTotal")->setDouble(StrangL_Total);
                    break;
                }
            }

        }while (!ReachedEndPoint && id != 0);
        if (id == 0)
            continue;
    }

    //Write Data to shaft
    std::vector<std::string> junctionnames = city->getNamesOfComponentsInView(shaft);

    for (int i = 0; i <junctionnames.size(); i++) {
        DM::Node * p = city->getNode(junctionnames[i]);

        p->getAttribute("Time")->setDouble( p->getAttribute("StrandLength")->getDouble()/this->v + 1*60);
        p->getAttribute("TimeTotal")->setDouble( p->getAttribute("StrandLengthTotal")->getDouble()/this->v + 1*60);
        double QrKritPerShaft_total =  p->getAttribute("QrKritPerShaft_total")->getDouble();


        p->addAttribute("APhi", this->caluclateAPhi(p, this->r15));
        //rkrit can be reduced to 7.5 if T > 120min
        if ( p->getAttribute("TimeTotal")->getDouble()/60 > 120) {
            p->addAttribute("QrKrit_total", QrKritPerShaft_total/2. );
        } else {
            p->addAttribute("QrKrit_total", QrKritPerShaft_total*120/(p->getAttribute("TimeTotal")->getDouble()/60+120) );

        }



    }

    //Dimensioning
    std::vector<std::string> edgesname = city->getNamesOfComponentsInView(conduit);
    foreach(std::string  en, edgesname) {
        DM::Edge * e = city->getEdge(en);
        DM::Node * attr = city->getNode(e->getStartpointName());
        double QWasteWater = attr->getAttribute("WasterWater")->getDouble() +  attr->getAttribute("InfitrationWater")->getDouble();
        double QRainWater =  attr->getAttribute("Area_total")->getDouble()*attr->getAttribute("APhi")->getDouble()*this->r15/10000. +  attr->getAttribute("QrKrit_total")->getDouble();
        double Area_tot = attr->getAttribute("Area_total")->getDouble();
        double APhi = attr->getAttribute("APhi")->getDouble();
        double QBem = (QRainWater + QWasteWater)/1000.; //m³

        e->addAttribute("Diameter", this->chooseDiameter(sqrt((QBem)/3.14*4))); //in mm
        e->addAttribute("QBem", QBem);
        e->addAttribute("Area_tot", Area_tot);
        e->addAttribute("QrKrittotal", (attr->getAttribute("QrKrit_total")->getDouble()));

        DM::Logger(DM::Debug) << e->getStartpointName() << "\tArea total:\t" <<attr->getAttribute("Area_total")->getDouble() << "\tQrKrit total:\t" <<attr->getAttribute("QrKrit_total")->getDouble()<< "\tDiameter:\t" <<e->getAttribute("Diameter")->getDouble() << "Q:\t" <<e->getAttribute("QBem")->getDouble() ;

    }

    //Dimensioning of Outfalls
    std::vector<std::string> outfallconduits = city->getNamesOfComponentsInView(weir);

    foreach (std::string of, outfallconduits) {
        DM::Edge * weir = city->getEdge(of);
        DM::Node * StartNode = city->getNode(weir->getStartpointName());

        //Get Upstream Nodes

        std::vector<DM::Edge*> upstream = EndNodeSortedEdges[StartNode];
        double maxDiameter = 0;
        foreach (DM::Edge * c, upstream) {
            if (c->getAttribute("Diameter")->getDouble() > maxDiameter)
                maxDiameter = c->getAttribute("Diameter")->getDouble();
        }


        double inletOffset = 0;
        //If storage

        if (StartNode->getAttribute("Storage")->getDouble() > 0.01) {
            inletOffset = 0.6/1000 * maxDiameter;
        } else {
            inletOffset = 0.85/1000 * maxDiameter;
        }
        weir->addAttribute("InletOffset", inletOffset);
    }

    //Dimensiong Pipe to WWTP
    foreach (std::string nwwtp, city->getNamesOfComponentsInView(wwtps)) {
        DM::Node * wwtp = city->getNode(nwwtp);
        std::vector<DM::Edge*> edges = this->StartNodeSortedEdges[wwtp];
        if (edges.size() == 1) {
            double QWasteWater = 2*wwtp->getAttribute("WasteWaterPerShaft")->getDouble() +  wwtp->getAttribute("InfiltrationWaterPerShaft")->getDouble();
            DM::Edge * e = edges[0];
            double diameter = sqrt(QWasteWater/1000)/3.14*4;
            e->addAttribute("Diameter", this->chooseDiameter(diameter));

        }
    }

    //Dimensioning Storage
    foreach (std::string nstorage, city->getNamesOfComponentsInView(storage)) {
        DM::Node * storage = city->getNode(nstorage);
        std::vector<DM::Edge*> edges = this->EndNodeSortedEdges[storage];
        double maxDiameter = 0;
        foreach (DM::Edge * c, edges) {
            if (c->getAttribute("Diameter")->getDouble() > maxDiameter)
                maxDiameter = c->getAttribute("Diameter")->getDouble();
        }
        double VStorage = storage->getAttribute("ConnectedStorageArea")->getDouble() * 15/10000;


        //Connected Weir
        double maxdepth = maxDiameter*0.6;

        double area = VStorage / maxdepth;
        storage->addAttribute("D", 2+maxdepth/1000.);
        storage->addAttribute("StorageA", area);
        storage->addAttribute("StorageV", VStorage);
    }


    DM::Logger(DM::Standard) << "Sum over Population " << Population_sum;

    //Create Storage Building at the End
    /*std::vector<std::string> wwtp_con_names = VectorDataHelper::findElementsWithIdentifier("WWTPConduit", this->Network_out->getEdgeNames());
    foreach(std::string name, wwtp_con_names) {
        std::vector<Point> points = this->Network_out->getPoints(name);
        Point p = points[0];
        Attribute attr = VectorDataHelper::findAttributeFromPoints(*this->Network_out, p, this->IdentifierShaft, 10);
        double QBem = 2* (attr.getAttribute("WasterWater") +  attr.getAttribute("InfitrationWater"))/1000.;
        Attribute attr_cond = this->Network_out->getAttributes(name);
        attr_cond.setAttribute("Diameter", this->choosDiameter(sqrt((QBem)/3.14*4)));
        this->Network_out->setAttributes(name, attr_cond);
    }
    Logger(vibens::Standard) << "Sum over Population " << Population_sum;*/


    //TODO: Seperate Module
    /*foreach (std::string nc, city->getNamesOfComponentsInView(conduit)) {
        DM::Edge * e = city->getEdge(nc);
        if (e->getAttribute("Strahler")->getDouble()  < 2) {
            city->removeComponentFromView(e, conduit);
            DM::Node * start = city->getNode(e->getStartpointName());
            city->removeComponentFromView(start, shaft);
        }
    }*/
}

double TimeAreaMethod::chooseDiameter(double diameter) {
    QVector<double> vd;
    vd.append(150);
    vd.append(200);
    vd.append(250);
    vd.append(300);
    vd.append(350);
    vd.append(400);
    vd.append(450);
    vd.append(500);
    vd.append(600);
    vd.append(700);
    vd.append(800);
    vd.append(900);
    vd.append(1000);
    vd.append(1100);
    vd.append(1200);
    vd.append(1300);
    vd.append(1400);
    vd.append(1500);
    vd.append(1600);
    vd.append(1800);
    vd.append(2000);
    vd.append(2200);
    vd.append(2400);
    vd.append(2600);
    vd.append(2800);
    vd.append(3000);
    vd.append(3200);
    vd.append(3400);
    vd.append(3600);
    vd.append(3800);
    vd.append(4000);

    double d = 0;
    for (int i = 0; i < vd.size(); i++) {
        if (d == 0 && diameter*1000 <= vd.at(i) ) {
            d =  vd.at(i);
        }
    }
    if (d == 0.) {
        d = 4000;
    }
    return d;

};
