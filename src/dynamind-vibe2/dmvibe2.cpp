

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

#include "dmvibe2.h"

DM_DECLARE_NODE_NAME(DMVibe2, Dynamind-Vibe2)

DMVibe2::DMVibe2()
{
    height = 250;
    width = 400;

    this->addParameter("Height", DM::LONG, &height);
    this->addParameter("Width", DM::LONG, &width);
}
void DMVibe2::run()
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

    s1->loadSimulation("/home/christian/Documents/test.vib");

    Module * m;
    s1->getModuleByName("Height")->setParameterValue("Value", QString::number(height).toStdString());
    s1->getModuleByName("Width")->setParameterValue("Value", QString::number(width).toStdString());

    s1->run();


    vibens::Group * g = (vibens::Group *)s1->getModuleByName("VIBe");

    RasterData r= g->getRasterData("Landuse");
    r;
    db->resetDataBase();

    std::cout << "Done" << std::endl;

}




