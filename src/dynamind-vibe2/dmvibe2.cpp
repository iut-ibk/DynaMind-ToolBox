

#include <compilersettings.h>
#include <simulation.h>
#include <database.h>
#include <datamanagement.h>
#include <QThreadPool>
#include <vibe_log.h>
#include <vibe_logger.h>
#include <vibe_logsink.h>
#include <pythonenv.h>
#include <porttuple.h>
#include <port.h>
#include <rasterdata.h>
#include <vectordata.h>
#include <vectordatahelper.h>
#include <tbvectordata.h>

#include "dmvibe2.h"

DM_DECLARE_NODE_NAME(DMVIBe2, DMVIBe2)

DMVIBe2::DMVIBe2()
{
    height = 250;
    width = 400;
    FileInitalVIBeCity = "testmodels/InitialCity.vib";

    this->addParameter("Height", DM::LONG, &height);
    this->addParameter("Width", DM::LONG, &width);


    maxCPopDensity = 200;
    maxDCPopDensity = 100;
    maxOBPopDensity = 30;
    popChangeLow = 95;
    popCUFRand = 10;
    popDCUFRand = 12;
    popAGRIRand = 19;
    InitialCityCenter = 3;
    Steps = 16;
    PopSteps = 3;

    this->addParameter("maxCPopDensity", DM::DOUBLE, &maxCPopDensity);
    this->addParameter("maxDCPopDensity", DM::DOUBLE, &maxDCPopDensity);
    this->addParameter("maxOBPopDensity", DM::DOUBLE, &maxOBPopDensity);
    this->addParameter("popChangeLow", DM::DOUBLE, &popChangeLow);
    this->addParameter("popCUFRand", DM::DOUBLE, &popCUFRand);
    this->addParameter("popDCUFRand", DM::DOUBLE, &popDCUFRand);
    this->addParameter("popAGRIRand", DM::DOUBLE, &popAGRIRand);
    this->addParameter("InitialCityCenter", DM::INT, &InitialCityCenter);
    this->addParameter("Steps", DM::INT, &Steps);
    this->addParameter("PopSteps", DM::INT, &PopSteps);
    this->addParameter("FileInitalVIBeCity", DM::STRING, &FileInitalVIBeCity);

    landuse = DM::View("Landuse", DM::RASTERDATA, DM::WRITE);
    population = DM::View("Population", DM::RASTERDATA, DM::WRITE);
    topology = DM::View("Topology", DM::RASTERDATA, DM::WRITE);

    mainSewer = DM::View("MainSewer", DM::EDGE, DM::WRITE);
    wwtp = DM::View("WWTP", DM::NODE, DM::WRITE);
    wwtp.addAttribute("D");
    conduit = DM::View("CONDUIT", DM::EDGE, DM::WRITE);

    globals = DM::View("GLOBALS", DM::NODE, DM::WRITE);
    globals.addAttribute("Population");
    globals.addAttribute("Area");

    junction = DM::View("JUNCTION", DM::NODE, DM::WRITE);
    junction.addAttribute("D");
    std::vector<DM::View> views;
    views.push_back(landuse);
    views.push_back(population);
    views.push_back(topology);


    std::vector<DM::View> viewsvec;
    viewsvec.push_back(mainSewer);
    viewsvec.push_back(wwtp);
    viewsvec.push_back(conduit);
    viewsvec.push_back(junction);
    viewsvec.push_back(globals);

    this->addData("City_RasterData", views);

    this->addData("City", viewsvec);

}
void DMVIBe2::run()
{

    //Init Logger
    ostream *out = &cout;
    vibens::Log::init(new vibens::OStreamLogSink(*out), vibens::Debug);
    vibens::Logger(vibens::Debug) << "Start";
    vibens::Simulation * s1 = new vibens::Simulation();
    vibens::DataManagement::init();
    vibens::DataBase * db = new vibens::DataBase();
    vibens::DataManagement::getInstance().registerDataBase(db);

    vibens::PythonEnv::getInstance()->addPythonPath("/usr/local/lib");
    s1->registerDataBase(db);
    s1->loadSimulation(FileInitalVIBeCity);

    s1->getModuleByName("Height")->setParameterValue("Value", QString::number(height).toStdString());
    s1->getModuleByName("Width")->setParameterValue("Value", QString::number(width).toStdString());

    vibens::Module * popgrowth = s1->getModuleByName("PopulationGrowthModel_1");
    popgrowth->setParameterValue("maxCPopDensity",  QString::number(maxCPopDensity).toStdString());
    popgrowth->setParameterValue("maxDCPopDensity",  QString::number(maxDCPopDensity).toStdString());
    popgrowth->setParameterValue("maxOBPopDensity",  QString::number(maxOBPopDensity).toStdString());
    popgrowth->setParameterValue("popChangeLow", QString::number(popChangeLow).toStdString());
    popgrowth->setParameterValue("popCUFRand", QString::number(popCUFRand).toStdString());
    popgrowth->setParameterValue("popDCUFRand",  QString::number(popDCUFRand).toStdString());
    popgrowth->setParameterValue("popAGRIRand",  QString::number(popAGRIRand).toStdString());


    vibens::Module * landusegrowth = s1->getModuleByName("LandUseGrowthModel_2");
    landusegrowth->setParameterValue("maxCPopDensity",  QString::number(maxCPopDensity).toStdString());
    landusegrowth->setParameterValue("maxDCPopDensity",  QString::number(maxDCPopDensity).toStdString());
    landusegrowth->setParameterValue("maxOBPopDensity",  QString::number(maxOBPopDensity).toStdString());
    landusegrowth->setParameterValue("popChangeLow", QString::number(popChangeLow).toStdString());
    landusegrowth->setParameterValue("popCUFRand", QString::number(popCUFRand).toStdString());
    landusegrowth->setParameterValue("popDCUFRand",  QString::number(popDCUFRand).toStdString());
    landusegrowth->setParameterValue("popAGRIRand",  QString::number(popAGRIRand).toStdString());

    vibens::Module * landusegrowthgroup = s1->getModuleByName("PopulationGrowth");
    landusegrowthgroup->setParameterValue("Runs",  QString::number(PopSteps).toStdString());

    vibens::Module * growth= s1->getModuleByName("Growthmodel");
    growth->setParameterValue("Runs",  QString::number(Steps).toStdString());

    vibens::Module * initcenters = s1->getModuleByName("InitialCityCenters_0");
    initcenters->setParameterValue("numberCenters",  QString::number(InitialCityCenter).toStdString());



    //Start Simulation
    s1->run();



    vibens::Group * g = (vibens::Group *) s1->getModuleByName("VIBe");

    //Copy Landuse

    RasterData r = g->getRasterData("Landuse");
    DM::RasterData * rland = this->getRasterData("City_RasterData", landuse);
    rland->setSize(r.getWidth(), r.getHeight(), r.getCellSize());
    for (int x = 0; x < r.getWidth(); x++) {
        for (int y = 0; y < r.getHeight(); y++) {
            rland->setValue(x,y, r.getValue(x,y));
        }
    }

    r = g->getRasterData("Population");
    DM::RasterData * rpop = this->getRasterData("City_RasterData", population);
    rpop->setSize(r.getWidth(), r.getHeight(), r.getCellSize());
    for (int x = 0; x < r.getWidth(); x++) {
        for (int y = 0; y < r.getHeight(); y++) {
            rpop->setValue(x,y, r.getValue(x,y));
        }
    }

    r = g->getRasterData("Topology");
    DM::RasterData * rtop = this->getRasterData("City_RasterData", topology);
    rtop->setSize(r.getWidth(), r.getHeight(), r.getCellSize());
    for (int x = 0; x < r.getWidth(); x++) {
        for (int y = 0; y < r.getHeight(); y++) {
            rtop->setValue(x,y, r.getValue(x,y));
        }
    }




    DM::System * sys = this->getData("City");

    DM::Node * gl = sys->addNode(0,0,0,globals);
    gl->addAttribute("Population", g->getDoubleData("Double_Pop"));
    gl->addAttribute("Area", g->getDoubleData("Double_Area"));



    g = (vibens::Group *) s1->getModuleByName("InitialCity");
    VectorData vec = g->getVectorData("MainSewer");
    DM::View empty;
    std::vector<std::string> conduits = VectorDataHelper::findElementsWithIdentifier("Conduit_", vec.getEdgeNames());
    foreach (std::string con, conduits) {
        std::vector<Edge> edges = vec.getEdges(con);
        std::vector<Point> points = vec.getPoints(con);
        foreach (Edge e, edges) {
            Point p1 = points[e.getID1()];
            Point p2 = points[e.getID2()];

            DM::Node * n2 = TBVectorData::addNodeToSystem2D(sys, empty, DM::Node(p1.getX(), p1.getY(), p1.getZ()), 0.01);
            DM::Node * n1 = TBVectorData::addNodeToSystem2D(sys, empty, DM::Node(p2.getX(), p2.getY(), p2.getZ()), 0.01);
            DM::Edge * ed = sys->addEdge(n1, n2, mainSewer);

            sys->addComponentToView(ed, conduit);

        }
    }

    std::vector<std::string> shafts = VectorDataHelper::findElementsWithIdentifier("Shaft_", vec.getPointsNames());
    foreach (std::string sh, shafts) {
        std::vector<Point> points = vec.getPoints(sh);
        foreach (Point p, points) {
            DM::Node * n = TBVectorData::addNodeToSystem2D(sys, empty, DM::Node(p.getX(), p.getY(), p.getZ()), 0.01);
            sys->addComponentToView(n, junction);
            n->addAttribute("D", 4);
        }
    }



    DM::Node * wwtp1;
    VectorData wwtpv = g->getVectorData("WWTP");
    shafts = VectorDataHelper::findElementsWithIdentifier("WWTP", wwtpv.getPointsNames());
    foreach (std::string sh, shafts) {
        std::vector<Point> points = wwtpv.getPoints(sh);
        foreach (Point p, points) {
            DM::Node * n = TBVectorData::addNodeToSystem2D(sys, empty, DM::Node(p.getX()+20, p.getY()+20, p.getZ()), 0.01);
            sys->addComponentToView(n, wwtp);
            wwtp1 = n;
            n->addAttribute("D", 4);
        }
    }

    foreach (std::string s, sys->getNamesOfComponentsInView(mainSewer)) {
        DM::Edge * e = sys->getEdge(s);
        if (e->getStartpointName().compare(wwtp1->getName()) == 0) {
            DM::Logger(DM::Debug) << "Found";
        }
        if (e->getEndpointName().compare(wwtp1->getName()) == 0) {
            DM::Logger(DM::Debug) << "Found";
        }

    }



}




