/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
    inlet = DM::View("INLET", DM::NODE, DM::READ);
    inlet.addAttribute("WasteWater");
    inlet.addAttribute("InfiltrationWater");
    inlet.addAttribute("Area");
    inlet.addAttribute("QrKrit");
    inlet.addAttribute("Impervious");
    inlet.getAttribute("ID_CATCHMENT");

    shaft = DM::View("SHAFT", DM::NODE, DM::READ);
    endnodes = DM::View("ENDPOINT", DM::NODE, DM::READ);
    catchment = DM::View("CATCHMENT", DM::NODE, DM::READ);

    catchment.getAttribute("Population");
    catchment.getAttribute("Area");
    catchment.getAttribute("Impervious");
    catchment.getAttribute("WasteWater");

    std::vector<DM::View> views;

    views.push_back(conduit);
    views.push_back(inlet);
    views.push_back(shaft);
    views.push_back(endnodes);
    views.push_back(catchment);

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

bool TimeAreaMethod::checkPoint(DM::Node *p) {
    foreach (DM::Node * p1, this->PointList) {
        if (p->compare2d(p1))
            return true;
    }
    return false;

}

DM::Node * TimeAreaMethod::findDownStreamNode(DM::System * city, DM::Node  * ID) {
    std::vector<DM::Edge *> ids = this->findConnectedEdges(city, ID);
    foreach(DM::Edge * e, ids) {

        if (city->getComponent(e->getEndpointName()) != ID)
            return (DM::Node *)city->getComponent(e->getEndpointName());
    }

    return 0;

}

std::vector<DM::Edge *> TimeAreaMethod::findConnectedEdges(DM::System * city, DM::Node *ID) {
    std::vector<DM::Edge *> ress;
    foreach(DM::Edge * e, this->EdgeList) {
        if (city->getComponent(e->getStartpointName()) == ID || city->getComponent(e->getEndpointName()) == ID) {
            ress.push_back(e);
        }
    }
    return ress;
}

void TimeAreaMethod::run() {
    //Generate List Of Start Points

    //Calculate Waste Water
    double Population_sum = 0;
    std::vector<std::string> InletNames;
    DM::System *city= this->getData("City");
    InletNames = city->getNamesOfComponentsInView(inlet);


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
    //Create Point List

    foreach(std::string name , city->getNamesOfComponentsInView(conduit)) {
        DM::Edge * e = city->getEdge(name);

        DM::Node * p1 = city->getNode(e->getStartpointName());
        DM::Node * p2 = city->getNode(e->getEndpointName());
        if (!this->checkPoint(p1))
            this->PointList.push_back(p1);
        if (!this->checkPoint(p2))
            this->PointList.push_back(p2);

        this->EdgeList.push_back(e);

   }

    std::vector<double> WasteWaterPerShaft;
    std::vector<double> AreaPerShaft;
    std::vector<double> InfiltrartionWaterPerShaft;
    std::vector<double> QrKritPerShaft;
    std::vector<double> QrKritPerShaft_total;
    std::vector<double> Area_total;
    std::vector<double> Lengths;
    std::vector<std::vector<string> > ConnectedInletNodes;

    for (int i = 0; i < this->PointList.size(); i++) {
        WasteWaterPerShaft.push_back(0);
        InfiltrartionWaterPerShaft.push_back(0);
        AreaPerShaft.push_back(0);
        Lengths.push_back(0);
        QrKritPerShaft.push_back(0);
        QrKritPerShaft_total.push_back(0);
        Area_total.push_back(0);
        std::vector<std::string> vec;
        ConnectedInletNodes.push_back(vec);

    }
    std::vector<std::string> endPointNames = city->getNamesOfComponentsInView(endnodes);
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


            //Check if Endpoint is reached
            //std::vector<std::string> nodes = ConnectedInletNodes[id];
            //nodes.push_back( name );
            //ConnectedInletNodes[id] = nodes;

            id->getAttribute("WasteWaterPerShaft")->setDouble(id->getAttribute("WasteWaterPerShaft")->getDouble() + wastewater);
            id->getAttribute("InfiltrationWaterPerShaft")->setDouble(id->getAttribute("InfiltrationWaterPerShaft")->getDouble() + infiltreationwater);
            id->getAttribute("AreaPerShaft")->setDouble(id->getAttribute("AreaPerShaft")->getDouble() + area);
            id->getAttribute("QrKritPerShaft")->setDouble(id->getAttribute("QrKritPerShaft")->getDouble() + QrKrit);

            if (idPrev != 0) {
                DM::Node dp = *(idPrev) - *(id);
                StrangL += sqrt(dp.getX()*dp.getX() + dp.getY()*dp.getY() + dp.getZ()*dp.getZ());
            }

            //Search for Outfall
            /*std::string name = VectorDataHelper::findPointID(*(this->Network), this->PointList[id], this->IdentifierShaft);
            Attribute attr = this->Network->getAttributes(name);
            if ((int) attr.getAttribute("Outfall") == 1) {
                StrangL = 0;
                QKrit = QrKritPerShaft[id];
                DeltaA = AreaPerShaft[id];
            }*/

            if ( id->getAttribute("StrandLength")->getDouble() < StrangL)
                id->getAttribute("StrandLength")->setDouble(StrangL);


            id->getAttribute("QrKritPerShaft_total")->setDouble(QKrit);
            id->getAttribute("Area_total")->setDouble(id->getAttribute("AreaPerShaft")->getDouble() -DeltaA);

            //QrKritPerShaft_total[id] = QKrit;
            //Area_total[id] = AreaPerShaft[id] - DeltaA;

            idPrev = id;
            id = this->findDownStreamNode(city, id);

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
    for (int i = 0; i < this->PointList.size(); i++) {
        DM::Node * p = this->PointList[i];

        p->getAttribute("Time")->setDouble( p->getAttribute("StrandLength")->getDouble()/this->v + 1*60);

        p->addAttribute("APhi", this->caluclateAPhi(p, this->r15));
        p->addAttribute("QrKrit_total", p->getAttribute("QrKritPerShaft_total")->getDouble() * (45./(p->getAttribute("Time")->getDouble()/60 + 30)));

        p->addAttribute("StorageV",0);
        p->addAttribute("Storage",0);
        /*if (attr.getAttribute("WWTP") == 1) {
            attr.setAttribute("Storage",1);
            attr.setAttribute("StorageV",attr.getAttribute("QrKrit_total"));
        }

        this->Network_out->setAttributes(id, attr);*/

    }

    //Dimensioning
    foreach(DM::Edge * e, EdgeList) {
        /*Point p = this->PointList[e.getID1()];
        std::string id;
        foreach(std::string name,VectorDataHelper::findElementsWithIdentifier(this->IdentifierShaft,this->Network->getPointsNames())) {
            Point p_tmp = this->Network->getPoints(name)[0];
            if (p.compare2d(p_tmp)) {
                id = name;
                break;
            }
        }

        Attribute attr;
        attr = this->Network_out->getAttributes(id);


        std::string id_cond = VectorDataHelper::findEdgeID(*(this->Network_out), p, this->PointList[e.getID2()], this->IdentifierConduit);

        //Get Existing Attribute
        //Only change Diamater if Dimaeter = 0 -> newly placed Pipe, or if redesign is set true
        Attribute attr_cond = this->Network_out->getAttributes(id_cond);
        if (attr_cond.getAttribute("Diameter") == 0  || attr_cond.getAttribute("Redesign") == 1)
        {*/
        DM::Node * attr = city->getNode(e->getStartpointName());
        double QWasteWater = attr->getAttribute("WasterWater")->getDouble() +  attr->getAttribute("InfitrationWater")->getDouble();
        double QRainWater =  attr->getAttribute("Area_total")->getDouble()*attr->getAttribute("APhi")->getDouble()*this->r15/10000. +  attr->getAttribute("QrKrit_total")->getDouble();
        double Area_tot = attr->getAttribute("Area_total")->getDouble();
        double APhi = attr->getAttribute("APhi")->getDouble();
        double QBem = (QRainWater + QWasteWater)/1000.; //m³

        e->addAttribute("Diameter", this->chooseDiameter(sqrt((QBem)/3.14*4))); //in mm

        DM::Logger(DM::Debug) <<  "Area total: " <<attr->getAttribute("Area_total")->getDouble();
        //}

    }
    //Create Storage Building at the End
 /*   std::vector<std::string> wwtp_con_names = VectorDataHelper::findElementsWithIdentifier("WWTPConduit", this->Network_out->getEdgeNames());
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
