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
    inlet = DM::View("INLET", DM::NODE, DM::READ);
    inlet.addAttribute("WasteWater");
    inlet.addAttribute("InfiltrationWater");
    inlet.addAttribute("Area");
    inlet.addAttribute("QrKrit");
    inlet.addAttribute("Impervious");
    inlet.getAttribute("ID_CATCHMENT");

    shaft = DM::View("JUNCTION", DM::NODE, DM::READ);
    wwtps = DM::View("WWTP", DM::NODE, DM::READ);
    catchment = DM::View("CATCHMENT", DM::FACE, DM::READ);
    storage = DM::View("STORAGE", DM::NODE, DM::WRITE);
    storage.addAttribute("StorageV");
    storage.addAttribute("Storage");
    catchment.getAttribute("Population");
    catchment.getAttribute("Area");
    catchment.getAttribute("Impervious");
    catchment.getAttribute("WasteWater");

    std::vector<DM::View> views;




    outfalls= DM::View("OUTFALL", DM::NODE, DM::WRITE);
    weir = DM::View("WEIR", DM::NODE, DM::WRITE);

    views.push_back(conduit);
    views.push_back(inlet);
    views.push_back(shaft);
    views.push_back(wwtps);
    views.push_back(catchment);
    views.push_back(outfalls);
    views.push_back(weir);
    views.push_back(storage);

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





    //double WasterWaterPerPerson = 0.0052;
    double InfiltrationWater =  0.003;


    //Calculate Water Water per Node
    foreach(std::string id_inlet, InletNames) {
        DM::Component *  inlet_attr = city->getComponent(id_inlet);
        DM::Component *  catchment_attr = city->getComponent(inlet_attr->getAttribute("ID_CATCHMENT")->getString());

        double pop =  catchment_attr->getAttribute("Population")->getDouble();
        double area = catchment_attr->getAttribute("Area")->getDouble();
        double imp = catchment_attr->getAttribute("Impervious")->getDouble();
        if(imp < 0.2) {
            imp = 0.2;
        }

        double ww = catchment_attr->getAttribute("WasteWater")->getDouble();

        inlet_attr->addAttribute("InfiltrationWater", pop * InfiltrationWater);
        inlet_attr->addAttribute("WasterWater", ww);
        inlet_attr->addAttribute("Area",area*imp);
        inlet_attr->addAttribute("QrKrit", 15.*area/10000*imp);
        inlet_attr->addAttribute("Impervious",imp);

        Population_sum += catchment_attr->getAttribute("Population")->getDouble();

    }

    std::vector<std::string> endPointNames = city->getNamesOfComponentsInView(wwtps);
    foreach(std::string name, endPointNames) {
        this->EndPointList.push_back(city->getNode(name));
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
        double StrangL = 0;
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
            }

            //Search for Outfall
            if ((int) id->getAttribute("Outfall")->getDouble() == 1) {
                StrangL = 0;
                QKrit =  id->getAttribute("QrKritPerShaft")->getDouble();
                DeltaA = id->getAttribute("AreaPerShaft")->getDouble();
            }

            if ( id->getAttribute("StrandLength")->getDouble() < StrangL)
                id->getAttribute("StrandLength")->setDouble(StrangL);


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

                    if (idPrev != 0) {
                        DM::Node dp = *(idPrev) - *(id);
                        StrangL += sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
                    }

                    id->getAttribute("StrandLength")->getDouble();
                    if ( id->getAttribute("StrandLength")->getDouble() < StrangL)
                        id->getAttribute("StrandLength")->setDouble(StrangL);
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

        p->addAttribute("APhi", this->caluclateAPhi(p, this->r15));
        p->addAttribute("QrKrit_total", p->getAttribute("QrKritPerShaft_total")->getDouble() * (45./(p->getAttribute("Time")->getDouble()/60 + 30)));

        p->addAttribute("StorageV",0);
        p->addAttribute("Storage",0);

        if (p->isInView(wwtps)) {
            p->addAttribute("Storage",1);
            city->addComponentToView(p,storage);
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

        DM::Logger(DM::Debug) <<  "Area total: " <<attr->getAttribute("Area_total")->getDouble();

    }
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
    //vd.append(150);
    //vd.append(200);
    //vd.append(250);
    //vd.append(300);
    //vd.append(350);
    //vd.append(400);
    //vd.append(450);
    //vd.append(500);
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
