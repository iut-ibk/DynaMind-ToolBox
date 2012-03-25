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
#include "dmswmm.h"
#include <fstream>

#include <QDir>
#include <QUuid>
#include <QProcess>
#include <QTextStream>
#include <QSettings>
using namespace DM;
DM_DECLARE_NODE_NAME(DMSWMM, Sewer)
DMSWMM::DMSWMM()
{

    GLOBAL_Counter = 1;
    conduit = DM::View("CONDUIT", DM::EDGE, DM::READ);
    conduit.getAttribute("Diameter");

    inlet = DM::View("INLET", DM::NODE, DM::READ);
    inlet.getAttribute("ID_CATCHMENT");


    shaft = DM::View("JUNCTION", DM::NODE, DM::READ);
    shaft.getAttribute("D");

    endnodes = DM::View("WWTP", DM::NODE, DM::READ);
    catchment = DM::View("CATCHMENT", DM::FACE, DM::READ);
    catchment.getAttribute("WasteWater");
    catchment.getAttribute("Area");
    catchment.getAttribute("Impervious");

    outfalls= DM::View("OUTFALL", DM::NODE, DM::READ);
    weir = DM::View("WEIR", DM::NODE, DM::READ);
    wwtp = DM::View("WWTP", DM::NODE, DM::READ);

    storage = DM::View("STORAGE", DM::NODE, DM::READ);
    storage.getAttribute("StorageV");
    storage.getAttribute("Storage");


    std::vector<DM::View> views;

    views.push_back(conduit);
    views.push_back(inlet);
    views.push_back(shaft);
    views.push_back(endnodes);
    views.push_back(catchment);
    views.push_back(outfalls);
    views.push_back(weir);
    views.push_back(wwtp);
    views.push_back(storage);

    this->FileName = "swmmfile";
    this->climateChangeFactor = 1;
    this->RainFile = "";
    this->Vr = 0;
    this->Vp = 0;
    this->Vwwtp = 0;
    years = 0;

    this->addParameter("FileName", DM::STRING, &this->FileName);
    this->addParameter("RainFile", DM::FILENAME, &this->RainFile);
    this->addParameter("ClimateChangeFactor", DM::DOUBLE, & this->climateChangeFactor);

    counterRain =0;

    this->addData("City", views);

}
void DMSWMM::writeRainFile() {
    QFile r_in;
    r_in.setFileName(QString::fromStdString(this->RainFile));
    r_in.open(QIODevice::ReadOnly);
    QTextStream in(&r_in);

    QString line;
    QString fileName = this->SWMMPath.absolutePath()+ "/"+ "rain.dat";
    std::fstream out;
    out.open(fileName.toAscii(),ios::out);

    do {
        line = in.readLine();
        QStringList splitline = line.split(QRegExp("\\s+"));
        if (splitline.size() > 2) {
            for(int i = 0; i < splitline.size() - 1;i++)
                out << QString(splitline[i]).toStdString() << " ";
            double r = QString(splitline[splitline.size()-1]).toDouble();
            out << r*(this->climateChangeFactor) << "\n";
        }

    } while (!line.isNull());

    r_in.close();
    out.close();
    counterRain++;
}

void DMSWMM::run() {

    city = this->getData("City");

    QDir tmpPath = QDir::tempPath();
    QString UUIDPath = QUuid::createUuid().toString().remove("{").remove("}");

    tmpPath.mkdir(UUIDPath);
    this->SWMMPath.setPath(tmpPath.absolutePath() + "/" + UUIDPath);
    //CreatePointList and ConduitList

    foreach(std::string name , city->getNamesOfComponentsInView(conduit))
    {


        DM::Edge * e = city->getEdge(name);
        DM::Node * p1 = city->getNode(e->getStartpointName());
        DM::Node * p2 = city->getNode(e->getEndpointName());


        if (UUIDtoINT[p1->getName()] == 0) {
            UUIDtoINT[p1->getName()] = GLOBAL_Counter++;
            this->PointList.push_back(p1);
        }

        if (UUIDtoINT[p2->getName()] == 0) {
            UUIDtoINT[p2->getName()] = GLOBAL_Counter++;
            this->PointList.push_back(p2);
        }

    }

    this->writeSWMMFile();
    this->writeRainFile();
    this->RunSWMM();
    this->readInReportFile();
}
void DMSWMM::readInReportFile() {
    std::map<int, std::string> revUUIDtoINT;

    for (std::map<std::string, int>::const_iterator it = UUIDtoINT.begin(); it !=UUIDtoINT.end(); ++it) {
        revUUIDtoINT[it->second] = it->first;
    }

    this->reportFile.setFileName(this->SWMMPath.absolutePath() + "/" + "swmm.rep");
    this->reportFile.open(QIODevice::ReadOnly);
    QTextStream in(&this->reportFile);

    bool erroraccrued = false;
    QStringList fileContent;

    QString line;

    do {
        line = in.readLine();
        fileContent << line;
    } while (!line.isNull());

    this->reportFile.close();

    //Search for Errors

    foreach (QString l, fileContent) {
        if (l.contains("WARNING"))
            Logger(Warning) << l.toStdString();
        if (l.contains("ERROR")) {
            Logger(Error) << l.toStdString();
            erroraccrued = true;
        }

    }
    if (erroraccrued) {
        Logger(Error) << "Error in SWMM";
        return;
    }

    //Start Reading Input
    //Flooded Nodes
    bool FloodSection = false;
    bool SectionSurfaceRunoff = false;
    bool SectionOutfall = false;
    double SurfaceRunOff = 0;
    double Vp = 0;
    double Vr = 0;
    double Vwwtp = 0;

    int counter = 0;
    for (int i = 0; i < fileContent.size(); i++) {
        line = fileContent[i];
        if (line.contains("Runoff Quantity Continuity") ) {
            SectionSurfaceRunoff = true;
            continue;
        }
        if (line.contains("Outfall Loading Summary") ) {
            SectionOutfall = true;
            continue;
        }
        if (line.contains("Node Flooding Summary") ) {
            FloodSection = true;
            continue;
        }
        if (SectionSurfaceRunoff) {
            if (line.contains("Surface Runoff")) {
                //Start extract
                QStringList data =line.split(QRegExp("\\s+"));
                SurfaceRunOff = QString(data[data.size()-2]).toDouble()*10;
            }
        }
        if (SectionOutfall) {

            if (line.contains("NODE")) {
                //Start extract
                QStringList data =line.split(QRegExp("\\s+"));
                for (int j = 0; j < data.size(); j++) {
                    if (data[j].size() == 0) {
                        data.removeAt(j);
                    }
                }
                //Extract Node id
                if (data.size() != 11) {
                    Logger(Error) << "Error in Extraction Outfall Nodes";

                } else {
                    QString id_asstring = data[0];
                    id_asstring.remove("NODE");
                    int id = id_asstring.toInt();
                    DM::Node * p = this->city->getNode(revUUIDtoINT[id]);
                    if (p->getAttribute("WWTP")->getDouble() > 0.01)
                        p->changeAttribute("OutfallVolume",  QString(data[4]).toDouble());
                }


            }
        }
        if (FloodSection) {
            if (line.contains("NODE")) {
                //Start extract
                QStringList data =line.split(QRegExp("\\s+"));
                for (int j = 0; j < data.size(); j++) {
                    if (data[j].size() == 0) {
                        data.removeAt(j);
                    }
                }
                //Extract Node id
                if (data.size() != 7) {
                    Logger(Error) << "Error in Extraction Flooded Nodes";

                } else {
                    QString id_asstring = data[0];
                    id_asstring.remove("NODE");
                    int id = id_asstring.toInt();
                    DM::Node * p = this->city->getNode(revUUIDtoINT[id]);
                    p->changeAttribute("FloodVolume",  QString(data[5]).toDouble());
                    Vp += QString(data[5]).toDouble();

                }


            }


        }
        if (counter > 0 && line.contains("************")) {
            FloodSection = false;
            SectionSurfaceRunoff = false;
            SectionOutfall = false;
            counter = -1;
        }
        counter ++;
    }
    Logger (Standard)  << "Vp " << Vp;
    Logger (Standard)  << "Vr " << SurfaceRunOff;
    Logger (Standard)  << "Vwwtp " << Vwwtp;
    this->Vp = Vp;
    this->Vwwtp = Vwwtp;
    this->Vr = SurfaceRunOff;
    this->sendDoubleValueToPlot(years, 1.-Vp/SurfaceRunOff);

    years++;

}

void DMSWMM::RunSWMM() {
    this->Vp = 0;
    this->Vwwtp = 0;
    this->Vr = 0;
    //Find Temp Directory
    QDir tmpPath = QDir::tempPath();
    tmpPath.mkdir("vibeswmm");

    //Path to SWMM
    QSettings settings("IUT", "DYNAMIND");
    QString swmmPath = settings.value("SWMM").toString().replace("\\","/");
    if (swmmPath.lastIndexOf("/") != swmmPath.size()) {
        swmmPath.append("/");
    }

    //Copy SWMM to tmp Path
    QString newFileName = this->SWMMPath.absolutePath() + "/"+ "swmm5.dll";
    QFile (swmmPath+"swmm5.dll").copy(newFileName);

    newFileName = this->SWMMPath.absolutePath()+  + "/"+ "swmm5.exe";
    QFile (swmmPath+"swmm5.exe").copy(newFileName);

    //newFileName  = this->SWMMPath.absolutePath()+ "/" + "rain.dat";
    //QFile (QString::fromStdString(this->RainFile)).copy(newFileName);


    QProcess process;
    QStringList argument;
    argument << this->SWMMPath.absolutePath() + "/" + "swmm5.exe" << this->SWMMPath.absolutePath() + "/"+ "swmm.inp" << this->SWMMPath.absolutePath() + "/" + "swmm.rep";
#ifdef _WIN32
    process.start("swmm5",argument);
#else
    process.start("wine",argument);
#endif
    process.waitForFinished(300000);




}

void DMSWMM::writeJunctions(std::fstream &inp)
{

    QStringList l;
    std::vector<std::string> names = city->getNamesOfComponentsInView(shaft);

    foreach (std::string name, names) {
        QString s = QString::fromStdString(name);
        if (l.indexOf(s) >= 0) {
            Logger(Error) << "Duplicated Entry";
        }
        l.append(s);
    }
    std::vector<int> checkForduplicatedNodes;
    inp<<"[JUNCTIONS]\n";
    inp<<";;Name\t\tElev\tYMAX\tY0\tYsur\tApond\n";
    inp<<";;============================================================================\n";
    foreach(std::string name, names) {
        DM::Node * p = city->getNode(name);
        /*if (p->isInView( wwtp ))
            continue;*/
        if (p->isInView( outfalls ))
            continue;
        if (p->isInView( storage ))
            continue;
        if (p->getAttribute("Storage")->getDouble() == 1)
            continue;
        //SewerTree::Node *node = this->sewerTree.getNodes().at(i);

        double depht = p->getZ();
        int id = UUIDtoINT[p->getName()];

        if (std::find(checkForduplicatedNodes.begin(), checkForduplicatedNodes.end(), id) != checkForduplicatedNodes.end())
            continue;

        checkForduplicatedNodes.push_back(id);
        inp << "NODE";
        inp << id;
        inp << "\t";
        inp << "\t";
        //Get Val
        inp << p->getAttribute("Z")->getDouble()-( p->getAttribute("D")->getDouble()-2);

        inp << "\t";
        inp <<  p->getAttribute("D")->getDouble()+1;
        inp << "\t";
        inp << "0";
        inp << "\t";
        inp << "0";
        inp << "\t";
        inp << "100000";
        inp << "\n";
    }
}


void DMSWMM::writeSubcatchments(std::fstream &inp)
{
    int subcatchCount=0;


    //SUBCATCHMENTS
    //-------------------------//
    inp<<"[SUBCATCHMENTS]\n";
    inp<<";;Name\traingage\tOutlet\tArea\tImperv\tWidth\tSlope\n";
    inp<<";;============================================================================\n";
    //for (int i=0;i<subcatchCount;i++)
    //{
    //	inp<<"  sub"<<i<<"\tRG01"<<"\t\tnode"<<i<<"\t20\t80.0\t100.0\t1.0\t1\n";
    //}

    std::vector<std::string> InletNames = city->getNamesOfComponentsInView(inlet);

    foreach(std::string name, InletNames) {
        DM::Node * inlet_attr = city->getNode(name);

        std::string id_catchment = inlet_attr->getAttribute("ID_CATCHMENT")->getString();

        if (UUIDtoINT[id_catchment] == 0) {
            UUIDtoINT[id_catchment] = GLOBAL_Counter++;
        }

        Component * catchment_attr = city->getComponent(id_catchment);

        int id = this->UUIDtoINT[inlet_attr->getName()];
        if (id == 0) {
            continue;
        }
        double area = catchment_attr->getAttribute("Area")->getDouble()/10000.;// node->area/10000.;
        double with = sqrt(area*10000.);
        double gradient = abs(catchment_attr->getAttribute("Gradient")->getDouble());
        double imp = catchment_attr->getAttribute("Impervious")->getDouble();
        if (imp < 0.2)
            imp = 0.2;
        if (gradient > 0.01)
            gradient = 0.01;
        if (gradient ==0)
            gradient = 0.001;

        if ( area > 0 ) {
            inp<<"sub"<<UUIDtoINT[id_catchment]<<"\tRG01"<<"\t\tnode"<<id<<"\t" << area << "\t" <<imp*100 << "\t"<< with << "\t"<<gradient*100<<"\t1\n";
            //inp<<"sub"<<subcatchCount++<<"\tRG01"<<"\t\tnode"<<id<<"\t" << area << "\t" <<catchment_attr.getAttribute("Impervious")*100 << "\t"<< with << "\t"<<gradient*100<<"\t1\n";
        }

    }
    inp<<"[POLYGONS]\n";
    inp<<";;Subcatchment\tX-Coord\tY-Coord\n";
    int counter = 0;
    foreach(std::string name, InletNames) {
        DM::Node * inlet_attr = city->getNode(name);

        std::string id_catchment = inlet_attr->getAttribute("ID_CATCHMENT")->getString();

        if (UUIDtoINT[id_catchment] == 0) {
            UUIDtoINT[id_catchment] = GLOBAL_Counter++;
        }

        DM::Face * catchment_attr = city->getFace(id_catchment);


        foreach(std::string s,  catchment_attr->getNodes()){
            DM::Node * n = city->getNode(s);


            inp << "sub" << UUIDtoINT[id_catchment] <<"\t" << n->getX() << "\t" << n->getY()<< "\n";
        }
        counter++;

    }

    inp<<"\n";
    //-------------------------//

    //SUBAREAS
    //-------------------------//
    inp<<"[SUBAREAS]\n";
    inp<<";;Subcatch\tN_IMP\tN_PERV\tS_IMP\tS_PERV\t%ZER\tRouteTo\n";
    inp<<";;============================================================================\n";
    foreach(std::string name, InletNames) {
        DM::Node * inlet_attr = city->getNode(name);
        std::string id_catchment = inlet_attr->getAttribute("ID_CATCHMENT")->getString();
        int id = this->UUIDtoINT[inlet_attr->getName()];
        if (id == 0) {
            continue;
        }

        inp<<"  sub"<<UUIDtoINT[id_catchment]<<"\t\t0.015\t0.2\t1.8\t5\t0\tOUTLET\n";
    }
    inp<<"\n";
    //-------------------------//


    //INFILTRATION
    //-------------------------//
    inp<<"[INFILTRATION]\n";
    inp<<";;Subcatch\tMaxRate\tMinRate\tDecay\tDryTime\tMaxInf\n";
    inp<<";;============================================================================\n";
    foreach(std::string name, InletNames) {
        DM::Node * inlet_attr = city->getNode(name);
        int id = this->UUIDtoINT[inlet_attr->getName()];
        if (id == 0) {
            continue;
        }
        std::string id_catchment = inlet_attr->getAttribute("ID_CATCHMENT")->getString();
        inp<<"  sub"<<UUIDtoINT[id_catchment]<<"\t60\t6.12\t3\t6\t0\n";
    }
    inp<<"\n";

}

void DMSWMM::writeDWF(std::fstream &inp) {
    std::vector<std::string> InletNames = city->getNamesOfComponentsInView(inlet);
    //DWF Dry Weather Flow
    //-------------------------//

    inp<<"[DWF]\n";
    inp<<";;Node\tItem\tValue\n";
    inp<<";;============================================================================\n";

    foreach(std::string name, InletNames) {
        DM::Node * inlet_attr = city->getNode(name);

        std::string id_catchment = inlet_attr->getAttribute("ID_CATCHMENT")->getString();

        if (UUIDtoINT[id_catchment] == 0) {
            UUIDtoINT[id_catchment] = GLOBAL_Counter++;
        }

        double Q = inlet_attr->getAttribute("WasteWater")->getDouble();
        if (Q > 0.000001) {
            inp<<"NODE"<<UUIDtoINT[id_catchment]<<"\tFLOW\t"<<Q<<"\n";
        }
    }
    inp<<"\n";
    //-------------------------//

    //POLLUTANTS
    //-------------------------//
    inp<<"[POLLUTANTS]\n";
    inp<<";;Name\tUnits\tCppt\tCgw\tCii\tKd\tSnow\tCoPollut\tCoFract\n";
    inp<<";;============================================================================\n";
    inp<<"ssolid\tMG/L\t0\t0\t0\t0\tNO\n";
    inp<<"vssolid\tMG/L\t0\t0\t0\t0\tNO\n";
    inp<<"cod\tMG/L\t0\t0\t0\t0\tNO\n";
    inp<<"cods\tMG/L\t0\t0\t0\t0\tNO\n";
    inp<<"nh4\tMG/L\t0\t0\t0\t0\tNO\n";
    inp<<"no3\tMG/L\t0\t0\t0\t0\tNO\n";
}
void DMSWMM::writeStorage(std::fstream &inp) {
    //-------------------------//

    //STROGAE
    //-------------------------//
    inp<<"\n";
    inp<<"[STORAGE]\n";
    inp<<";;               Invert   Max.     Init.    Shape      Shape                      Ponded   Evap.\n"  ;
    inp<<";;Name           Elev.    Depth    Depth    Curve      Params                     Area     Frac. \n"  ;
    inp<<";;-------------- -------- -------- -------- ---------- -------- -------- -------- -------- --------\n";
    //\nODE85           93.7286  6.35708  0        FUNCTIONAL 1000     0        22222    1000     0
    std::vector<std::string> storages = this->city->getNamesOfComponentsInView(storage);
    foreach(std::string name, storages) {
        Node * p = this->city->getNode(name);

        //if ((int) p->getAttribute("Storage")->getDouble() != 1)
        //continue;
        //std::vector<std::string> n_es = VectorDataHelper::findConnectedEges(*this->Network, p);
        double diameter = 4;
        /*foreach (std::string n_e, n_es) {
            Attribute attr_e = this->Network->getAttributes(n_e);
            if (attr_e.getAttribute("Diameter") > diameter)
                diameter = attr_e.getAttribute("Diameter");
        }*/

        int id = this->UUIDtoINT[p->getName()];

        inp << "NODE";
        inp << id;
        inp << "\t";
        inp << "\t";
        //Get Val
        inp << p->getAttribute("Z")->getDouble()-( p->getAttribute("D")->getDouble()-2);


        inp << "\t";
        inp <<   p->getAttribute("D")->getDouble()+1;
        inp << "\t";
        inp << "0";
        inp << "\t";
        inp << "FUNCTIONAL";
        inp << "\t";
        inp << "1000";
        inp << "\t";
        inp << "0";
        inp << "\t";

        inp << p->getAttribute("StorageV")->getDouble()/diameter*1000*0.6;
        inp << "\t";
        inp << "1000";
        inp << "0";
        inp << "\n";
    }
}

void DMSWMM:: writeOutfalls(std::fstream &inp) {
    //OUTFALLS
    //-------------------------//
    inp<<"[OUTFALLS]\n";
    inp<<";;Name	Elev	Stage	Gate\n";
    inp<<";;============================================================================\n";

    std::vector<std::string> OutfallNames = city->getNamesOfComponentsInView(outfalls);
    for ( int i = 0; i < OutfallNames.size(); i++ ) {
        DM::Node * p = city->getNode(OutfallNames[i]);

        if (UUIDtoINT[p->getName()] == 0) {
            UUIDtoINT[p->getName()] = GLOBAL_Counter++;
            this->PointList.push_back(p);
        }

        double z = p->getZ()-4.;
        inp<<"NODE"<<this->UUIDtoINT[p->getName()]<<"\t"<< z <<"\tFREE\tNO\n";
    }

}
void DMSWMM::writeConduits(std::fstream &inp) {
    inp<<"\n";
    inp<<"[CONDUITS]\n";
    inp<<";;Name	Node1	Node2	Length	N	Z1	Z2\n";
    inp<<";;============================================================================\n";

    std::vector<DM::View> conduits;
    conduits.push_back(conduit);

    foreach(DM::View con, conduits)  {
        std::vector<std::string> ConduitNames = city->getNamesOfComponentsInView(con);
        int counter = 0;
        foreach(std::string name, ConduitNames) {
            DM::Edge * link = city->getEdge(name);



            DM::Node * nStartNode = city->getNode(link->getStartpointName());
            DM::Node * nEndNode = city->getNode(link->getEndpointName());


            double x = nStartNode->getX()  - nEndNode->getX();
            double y = nStartNode->getY() - nEndNode->getY();

            double length = sqrt(x*x +y*y);

            if (UUIDtoINT[nStartNode->getName()] == 0) {
                UUIDtoINT[nStartNode->getName()] = GLOBAL_Counter++;
            }
            if (UUIDtoINT[nEndNode->getName()] == 0) {
                UUIDtoINT[nEndNode->getName()] = GLOBAL_Counter++;
            }
            if (UUIDtoINT[link->getName()] == 0) {
                UUIDtoINT[link->getName()] = GLOBAL_Counter++;
            }

            int EndNode = UUIDtoINT[nEndNode->getName()];
            int StartNode =  UUIDtoINT[nStartNode->getName()];
            double offest = 0;
            if (EndNode != -1 && StartNode != -1 && EndNode != StartNode)
                inp<<"LINK"<< UUIDtoINT[link->getName()]<<"\tNODE"<<EndNode<<"\tNODE"<<StartNode<<"\t"<<length<<"\t"<<"0.01	" << offest  <<"\t"	<< offest << "\n";

        }
    }

}
//void DMSWMM::writeLID_Controlls(std::fstream &inp) {

//    std::vector<std::string> InletNames;
//    InletNames = VectorDataHelper::findElementsWithIdentifier(this->IdentifierInlet,this->Network->getPointsNames());

//    inp << "\n";
//    inp << "[LID_CONTROLS]" << "\n";
//    inp << ";;               Type/Layer Parameters" << "\n";
//    inp << ";;-------------- ---------- ----------" << "\n";

//    foreach(std::string name, InletNames) {
//        Attribute inlet_attr;
//        inlet_attr = this->Network->getAttributes(name);
//        std::vector<Point> points = this->Network->getPoints(name);

//        std::string id_catchment;
//        id_catchment = QString::number( (int) inlet_attr.getAttribute(this->IdentifierCatchment+"ID") ).toStdString();

//        Attribute catchment_attr = this->Catchments->getAttributes(this->IdentifierCatchment+id_catchment);

//        double area = catchment_attr.getAttribute("Area");// node->area/10000.;

//        if ((int)catchment_attr.getAttribute("NumberOfInfiltrationTrenches") == 0 || area == 0)
//            continue;
//        /**  [LID_CONTROLS]
//    ;;               Type/Layer Parameters
//    ;;-------------- ---------- ----------
//    Infitration      IT
//    Infitration      SURFACE    depth        vegetataive cover       surface roughness       surface slope           5
//    Infitration      STORAGE    height        void ratio       conductivity         clogging factor
//    Infitration      DRAIN      Drain Coefficient          Crain Exponent        Drain Offset Height          6  */

//        inp << "Infiltration"<< id_catchment << "\t" << "IT" <<  "\n";
//        inp << "Infiltration" << id_catchment<< "\t"   << "SURFACE" <<    "\t" <<  catchment_attr.getAttribute("h")*1000<<    "\t"     <<   "0.0"  <<    "\t"  <<   "0.1"<<    "\t" <<      fabs( catchment_attr.getAttribute("Gradient") )* 100 <<    "\t" <<      "5" << "\n";
//        inp << "Infiltration" << id_catchment<< "\t"   << "STORAGE" <<    "\t" <<  "200"<<    "\t"     <<   "0.75"  <<    "\t"  <<   catchment_attr.getAttribute("kf") * 1000<<    "\t" <<       "0" << "\n";
//        inp << "Infiltration"<< id_catchment << "\t"   << "DRAIN" <<    "\t" <<  "0" <<    "\t"     <<   "0.5"  <<     "\t"<< "0"<< "\t"<< "6" << "\n";
//    }
//}
//void DMSWMM::writeLID_Usage(std::fstream &inp) {


//    std::vector<std::string> InletNames;
//    InletNames = VectorDataHelper::findElementsWithIdentifier(this->IdentifierInlet,this->Network->getPointsNames());

//    inp << "\n";
//    inp << "[LID_USAGE]" << "\n";
//    foreach(std::string name, InletNames) {
//        Attribute inlet_attr;
//        inlet_attr = this->Network->getAttributes(name);
//        std::vector<Point> points = this->Network->getPoints(name);

//        std::string id_catchment;
//        id_catchment = QString::number( (int) inlet_attr.getAttribute(this->IdentifierCatchment+"ID") ).toStdString();


//        Attribute catchment_attr = this->Catchments->getAttributes(this->IdentifierCatchment+id_catchment);

//        double imp = catchment_attr.getAttribute("Impervious")*100;
//        double inf = catchment_attr.getAttribute("RoofAreaInfitrated")/(200*200)*100;

//        double treated = inf/imp*100;
//        double area = catchment_attr.getAttribute("Area");// node->area/10000.;


//        if ((int)catchment_attr.getAttribute("NumberOfInfiltrationTrenches") == 0 || area == 0)
//            continue;
//        /*[LID_USAGE]
//    ;;Subcatchment   LID Process      Number  Area       Width      InitSatur  FromImprv  ToPerv     Report File
//    ;;-------------- ---------------- ------- ---------- ---------- ---------- ---------- ---------- -----------
//    sub68            Infitration      1       40000.00   200        90         100        0          "report1.txt"*/
//        inp << "\n";
//        inp << "[LID_USAGE]" << "\n";
//        inp << "sub" <<id_catchment<< "\t"  << "Infiltration"<< id_catchment <<    "\t" <<  catchment_attr.getAttribute("NumberOfInfiltrationTrenches") <<    "\t"     <<   catchment_attr.getAttribute("As")   <<    "\t"  <<   "1"<<    "\t" <<       "0" <<    "\t" <<      treated <<    "\t" <<    "0" <<    "\n" ; //<< "\"report"<< id_catchment << ".txt\"" << "\n";
//    }
//}

void DMSWMM::writeXSection(std::fstream &inp) {
    std::vector<std::string> OutfallNames = city->getNamesOfComponentsInView(weir);

    std::vector<std::string> WWTPNames = city->getNamesOfComponentsInView(wwtp);
    //XSection
    inp<<"\n";
    inp<<"[XSECTIONS]\n";
    inp<<";;Link	Type	G1	G2	G3	G4\n";
    inp<<";;==========================================\n";

    int counter = 0;



    std::vector<DM::View> condies;
    condies.push_back(weir);
    condies.push_back(conduit);

    foreach (DM::View condie, condies) {


        std::vector<std::string> ConduitNames = city->getNamesOfComponentsInView(condie);
        foreach(std::string name, ConduitNames) {
            DM::Edge * link = city->getEdge(name);


            DM::Node * nStartNode = city->getNode(link->getStartpointName());
            DM::Node * nEndNode = city->getNode(link->getEndpointName());


            double x = nStartNode->getX()  - nEndNode->getX();
            double y = nStartNode->getY() - nEndNode->getY();

            if (UUIDtoINT[nStartNode->getName()] == 0) {
                UUIDtoINT[nStartNode->getName()] = GLOBAL_Counter++;
            }
            if (UUIDtoINT[nEndNode->getName()] == 0) {
                UUIDtoINT[nEndNode->getName()] = GLOBAL_Counter++;
            }

            int EndNode = UUIDtoINT[nEndNode->getName()];
            int StartNode =  UUIDtoINT[nStartNode->getName()];

            double d = link->getAttribute("Diameter")->getDouble()/1000;
            if (EndNode != -1 && StartNode != -1 && EndNode != StartNode) {
                if (condie.getName().compare(conduit.getName()) == 0)
                    inp << "LINK" << UUIDtoINT[link->getName()] << "\tCIRCULAR\t"<< d <<" \t0\t0\t0\n";

                if (condie.getName().compare(weir.getName()) == 0)
                    inp << "WEIR" << UUIDtoINT[link->getName()] << "\tRECT_OPEN\t"<< "10" <<" \t6\t0\t0\n";
            }

        }
    }
    inp<<"\n";
}


void DMSWMM::writeWeir(std::fstream &inp)
{

    inp<<"\n";
    inp<<"[WEIRS]\n";
    inp<<";;               Inlet            Outlet           Weir         Crest      Disch.     Flap End      End \n";
    inp<<";;Name           Node             Node             Type         Height     Coeff.     Gate Con.     Coeff.\n";
    inp<<";;-------------- ---------------- ---------------- ------------ ---------- ---------- ---- -------- ----------\n";
    //LINK984          NODE109          NODE985          TRANSVERSE   0          1.80       NO   0        0
    std::vector<std::string> namesWeir = this->city->getNamesOfComponentsInView(weir);
    for ( int i = 0; i < namesWeir.size(); i++ ) {


        DM::Edge * weir = this->city->getEdge(namesWeir[i]);
        DM::Node * startn = this->city->getNode(weir->getStartpointName());
        DM::Node * outfall = this->city->getNode(weir->getEndpointName());
        double diameter = 0;
        bool storage = false;
        //Get Upper Points Connected with the  Weir
        /*std::vector<std::string> connectedConduits = VectorDataHelper::findConnectedEges((*this->Network), WeirPoints[0], 1, BOTH, this->IdentifierConduit);

        foreach (std::string s, connectedConduits ) {
            double d = this->Network->getAttributes(s).getAttribute("Diameter");
            if ( diameter < d ) {
                diameter = d;

            }
        }*/

        double x = startn->getX() -  outfall->getX();
        double y = startn->getY() -  outfall->getY();



        if (UUIDtoINT[startn->getName()] == 0) {
            UUIDtoINT[startn->getName()] = GLOBAL_Counter++;
        }
        if (UUIDtoINT[outfall->getName()] == 0) {
            UUIDtoINT[outfall->getName()] = GLOBAL_Counter++;
        }

        if (UUIDtoINT[weir->getName()] == 0) {
            UUIDtoINT[weir->getName()] = GLOBAL_Counter++;
        }



        inp<<"WEIR"<<UUIDtoINT[weir->getName()]<<"\tNODE"<<UUIDtoINT[startn->getName()]<<"\tNODE"<<UUIDtoINT[outfall->getName()]<<"\t";
        inp<<"TRANSVERSE" << "\t";
        if (storage == true ) {
            inp<< diameter*0.6/1000. << "\t";
        } else {
            inp<< diameter*0.85/1000. << "\t";
        }
        inp<<"1.8" << "\t";
        inp<<"NO" << "\t";
        inp<<"0" << "\t";
        inp<<"0" << "\t" << "\n";


    }
}
void DMSWMM::writeCoordinates(std::fstream &inp)
{
    std::vector<std::string> WWTPs = city->getNamesOfComponentsInView(wwtp);
    std::vector<std::string> OutfallNames = city->getNamesOfComponentsInView(outfalls);
    //COORDINATES
    //-------------------------//
    inp << "\n";
    inp<<"[COORDINATES]\n";
    inp<<";;Node\tXcoord\tyCoord\n";
    inp<<";;============================================================================\n";


    for ( int i = 0; i < this->PointList.size(); i++ ) {
        DM::Node * node = this->PointList[i];
        double x = node->getX();
        double y = node->getY();

        inp << "NODE" << UUIDtoINT[node->getName()] ;
        inp << "\t";
        inp << x;
        inp << "\t";
        inp << y;
        inp << "\n";

    }

    for ( int i = 0; i < OutfallNames.size(); i++ ) {
        DM::Node * node = city->getNode(OutfallNames[i]);
        double x = node->getX();
        double y = node->getY();

        inp << "NODE" << UUIDtoINT[node->getName()] ;
        inp << "\t";
        inp << x;
        inp << "\t";
        inp << y;
        inp << "\n";
    }

    /*for ( int i = 0; i < WWTPs.size(); i++ ) {
        DM::Node * node = city->getNode(WWTPs[i]);
        double x = node->getX();
        double y = node->getY();

        int id = i;
        inp << "WWTP" << id ;
        inp << "\t";
        inp << x;
        inp << "\t";
        inp << y;
        inp << "\n";
    }*/

}

void DMSWMM::writeSWMMFile() {
    QString fileName = this->SWMMPath.absolutePath()+ "/"+ "swmm.inp";
    std::fstream inp;
    inp.open(fileName.toAscii(),ios::out);
    writeSWMMheader(inp);
    writeSubcatchments(inp);
    //writeLID_Controlls(inp);
    //writeLID_Usage(inp);
    writeJunctions(inp);
    writeOutfalls(inp);
    writeStorage(inp);
    writeConduits(inp);
    writeWeir(inp);
    writeXSection(inp);
    writeDWF(inp);
    writeCoordinates(inp);

    inp.close();

}

void DMSWMM::writeSWMMheader(std::fstream &inp)
{
    inp<<"[TITLE]\n";
    inp<<"VIBe\n\n";
    //-------------------------//

    //OPTIONS
    //-------------------------//
    inp<<"[OPTIONS]\n";
    //inp<<"FLOW_UNITS LPS\n";
    //inp<<"FLOW_ROUTING DYNWAVE\n";
    //inp<<"START_DATE 1/1/2000\n";
    //inp<<"START_TIME 00:00\n";
    //inp<<"END_DATE 1/2/2000\n";
    //inp<<"END_TIME 00:00\n";
    //inp<<"WET_STEP 00:01:00\n";
    //inp<<"DRY_STEP 01:00:00\n";
    //inp<<"ROUTING_STEP 00:05:00\n";
    //inp<<"VARIABLE_STEP 0.99\n";
    //inp<<"REPORT_STEP  00:01:00\n";
    //inp<<"REPORT_START_DATE 1/1/2000\n";
    //inp<<"REPORT_START_TIME 00:00\n\n";

    inp<<"FLOW_UNITS\t\tLPS\n";
    inp<<"INFILTRATION\t\tHORTON\n";
    inp<<"FLOW_ROUTING\t\tDYNWAVE\n";
    //inp<<"START_DATE\t\t7/1/2008\n";
    inp<<"START_DATE\t\t1/1/2000\n";
    inp<<"START_TIME\t\t00:00\n";
    //inp<<"END_DATE\t\t7/31/2008\n";
    inp<<"END_DATE\t\t1/2/2000\n";
    inp<<"END_TIME\t\t00:00\n";
    inp<<"REPORT_START_DATE\t1/1/2000\n";
    inp<<"REPORT_START_TIME\t00:00\n";


    inp<<"SWEEP_START\t\t01/01\n";
    inp<<"SWEEP_END\t\t12/31\n";
    inp<<"DRY_DAYS\t\t0\n";
    inp<<"REPORT_STEP\t\t00:05:00\n";
    inp<<"WET_STEP\t\t00:01:00\n";
    inp<<"DRY_STEP\t\t00:01:00\n";
    inp<<"ROUTING_STEP\t\t0:00:20\n";
    inp<<"ALLOW_PONDING\t\tNO\n";
    inp<<"INERTIAL_DAMPING\tPARTIAL\n";
    inp<<"VARIABLE_STEP\t\t0.75\n";
    inp<<"LENGTHENING_STEP\t300\n";
    inp<<"MIN_SURFAREA\t\t0\n";
    inp<<"NORMAL_FLOW_LIMITED\tBOTH\n";
    inp<<"SKIP_STEADY_STATE\tNO\n";
    inp<<"IGNORE_RAINFALL\t\tNO\n";
    inp<<"FORCE_MAIN_EQUATION\tH-W\n";
    inp<<"LINK_OFFSETS\t\tDEPTH\n\n";
    //-------------------------//

    //REPORT
    //-------------------------//
    inp<<"[REPORT]\n";
    inp<<"INPUT NO\n";
    inp<<"CONTINUITY YES\n";
    inp<<"FLOWSTATS YES\n";
    inp<<"CONTROLS NO\n";
    inp<<"SUBCATCHMENTS NONE\n";
    inp<<"NODES NONE\n";
    inp<<"LINKS NONE\n\n";
    //-------------------------//

    //RAINFILE
    //-------------------------//
    inp<<"[RAINGAGES]\n";
    inp<<";;Name\tFormat\tInterval\tSCF\tDataSource\tSourceName\tunits\n";
    inp<<";;============================================================================\n";
    inp<<"RG01\tVOLUME\t0:01\t1.0\tFILE\t";
    //inp<< "rain.dat";
    inp<< this->SWMMPath.absolutePath().toStdString() + "/" + "rain.dat";
    //mag mich nicht
    //inp<<rainFile.toAscii();

    inp<<"\tSTA01 MM\n";
    inp<<"\n";
    //-------------------------//

}
/*bool DMSWMM::checkPoint(const Point &p) {
    foreach (Point p1, this->PointList) {
        if (p.compare2d(p1))
            return true;
    }
    return false;

}*/
