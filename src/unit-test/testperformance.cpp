#include "testperformance.h"

#include <dmsimulation.h>
#include <dmdbconnector.h>
#include <dm.h>
#include <dmcache.h>
#include <math.h>
#include <QSqlQuery>
#include <QElapsedTimer>

using namespace DM;

TEST_F(TestPerformance,node_one_attri_view) {

    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Test Node";


    // reset config
    DBConnectorConfig cfg = DBConnector::getInstance()->getConfig();
    // prepare a new one
    DBConnectorConfig cfgNew = cfg;
    cfgNew.queryStackSize = 1234;
    cfgNew.cacheBlockwritingSize = 1234;
    cfgNew.attributeCacheSize = 1234;
    cfgNew.nodeCacheSize = 1234;

    DBConnector::getInstance()->setConfig(cfgNew);

    DM::System sys;
    DM::View node_view("TestNode", DM::WRITE, DM::READ);
    for (int m = 3; m < 7; m++) {
        Logger(Debug) << "Start Writing";
        //Write
        long n = pow(10.,m)+3;
        QElapsedTimer timer;
        timer.start();

        QElapsedTimer timer2;
        timer2.start();
        int counter = 0;

        for(long i=0;i<n;i++) {
            counter++;
            DM::Node * node = sys.addNode(0,0,0, node_view);
            node->addAttribute("test",i);
            if (counter == 10000) {
                DM::Logger(Debug)  <<  "write" << "\t"  <<  counter << "|" << (long)timer2.elapsed() << " ms";
                counter = 0;
                timer2.restart();
            }

        }

        DM::Logger(Debug) <<  "write" << "\t" << n-3 << "|" << (long)timer.elapsed() << " ms";
        Logger(Standard) << "Write access per element " << "\t" << n-3 << "\t"<< (double) timer.elapsed()/n << "\t" << "ms";
        //Read
        timer.restart();
        Logger(Debug) << "Start get UUID";
        std::vector<std::string> uuids = sys.getUUIDs(node_view);
        DM::Logger(Debug) <<  "getuuids" << "\t"  << (long)timer.elapsed();
        timer.restart();


        Logger(Debug) << "Start Reading";
        int touched = 0;
        timer2.restart();
        counter = 0;
        for (int e = 0; e < uuids.size(); e++) {
            std::string uuid = uuids[e];
            DM::Node * n = sys.getNode(uuid);
            n->getAttribute("test")->getDouble();
            counter++;
            if (counter == 100) {
                DM::Logger(Debug) <<  "read" << "\t" <<  counter << "|" << (long)timer2.elapsed() << " ms";
                if (e/counter == 15) {
                    touched = e;
                    break;
                }
                counter = 0;
                timer2.restart();
            }
            touched = e;
        }
        Logger(Standard) << "Read access per element" << "\t" << n-3 << "\t" << (long) timer.elapsed()/touched << "\t" << "ms";

    }
}

#if 0

TEST_F(TestPerformance,node_one_attri) {

    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Standard) << "Test Node";


    // reset config
    DBConnectorConfig cfg = DBConnector::getInstance()->getConfig();
    // prepare a new one
    DBConnectorConfig cfgNew = cfg;
    cfgNew.queryStackSize = 1234;
    cfgNew.cacheBlockwritingSize = 1234;
    cfgNew.attributeCacheSize = 1234;
    cfgNew.nodeCacheSize = 1234;

    DBConnector::getInstance()->setConfig(cfgNew);

    DM::System sys;
    for (int m = 4; m < 5; m++) {

        //Write
        long n = pow(10.,m)+3;
        QElapsedTimer timer;
        timer.start();

        QElapsedTimer timer2;
        timer2.start();
        int counter = 0;

        for(long i=0;i<n;i++) {
            counter++;
            DM::Node * node = new DM::Node(0,0,0);
            sys.addNode(node);
            std::stringstream name;
            name << i;
            node->addAttribute(name.str(),i);
            if (counter == 10000) {
                DM::Logger(Debug) <<  i << "|" << (long)timer2.elapsed() << " ms";
                counter = 0;
                timer2.restart();
            }

        }
        DM::Logger(Standard) <<  n-3 << "|" << (long)timer.elapsed() << " ms";
    }

}

TEST_F(TestPerformance,component_one_attri) {

    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Standard) << "Test component";


    // reset config
    DBConnectorConfig cfg = DBConnector::getInstance()->getConfig();
    // prepare a new one
    DBConnectorConfig cfgNew = cfg;
    cfgNew.queryStackSize = 1234;
    cfgNew.cacheBlockwritingSize = 1234;
    cfgNew.attributeCacheSize = 1234;
    cfgNew.nodeCacheSize = 1234;

    DBConnector::getInstance()->setConfig(cfgNew);

    //Causes Crach
    DM::System sys;
    for (int m = 4; m < 5; m++) {
        long n = pow(10.,m)+3;
        QElapsedTimer timer;
        timer.start();

        QElapsedTimer timer2;
        timer2.start();
        int counter = 0;

        for(long i=0;i<n;i++) {
            counter++;
            DM::Component * cmp = new DM::Component();
            sys.addComponent(cmp);
            std::stringstream name;
            name << i;
            cmp->addAttribute(name.str(),i);
            if (counter == 10000) {
                DM::Logger(Standard) <<  i << "|" << (long)timer2.elapsed() << " ms";
                counter = 0;
                timer2.restart();
            }

        }
        DM::Logger(Standard) <<  n-3 << "|" << (long)timer.elapsed() << " ms";
    }

}
#endif


