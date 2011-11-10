#include "compilersettings.h"

#include <iostream>
#include <simulation.h>
#include <database.h>
#include <vibe_log.h>
#include <datamanagement.h>
#include <QThreadPool>
using namespace std;
using namespace vibens;

int main(int argc, char *argv[], char *envp[]) {

    //Init Logger
    ostream *out = &cout;
    vibens::Log::init(new OStreamLogSink(*out), vibens::Debug);
    vibens::Logger(vibens::Debug) << "Start";
    QThreadPool::globalInstance()->setMaxThreadCount(1);
    DataManagement::init();
    DataBase * db = new DataBase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dmtestmodule");
    sim->addModule("TestModule");
    sim->run();

    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    vibens::Logger(vibens::Debug) << "End";

}
