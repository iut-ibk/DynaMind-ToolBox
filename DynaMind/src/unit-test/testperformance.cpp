#include "testperformance.h"

#include <dmsimulation.h>
#include <dmdbconnector.h>
#include <dm.h>
#include <dmcache.h>
#include <math.h>
#include <QSqlQuery>
#include <QElapsedTimer>

using namespace DM;

#if 0

TEST_F(TestPerformance,node_one_attri_view) {

    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Test db I/O";

    DBConnectorConfig cfg = DBConnector::getInstance()->getConfig();
    // adjust configuration of cache
    DBConnectorConfig cfgNew = cfg;
    cfgNew.queryStackSize = 100;
    cfgNew.cacheBlockwritingSize = 100;
    cfgNew.attributeCacheSize = 1000;
    cfgNew.nodeCacheSize = 1000;
    DBConnector::getInstance()->setConfig(cfgNew);

    DM::View node_view("TestNode", DM::WRITE, DM::READ);
    for (long n = 1e3; n < 1e7; n*=10) 
	{
		std::list<DM::Node*> nodeList;
		DM::System sys;
        QElapsedTimer timer;
        QElapsedTimer timer2;
        long counter;

        Logger(Debug) << "Start Writing nodes and attributes";

        timer.start();
        timer2.start();
		
        for(long i=0;i<n;i++) {
			nodeList.push_back(sys.addNode(0,0,0, node_view));
            if (n%10000 == 0 && i>0) {
                DM::Logger(Debug)  <<  "write \t 10000 nodes | " << (long)timer2.elapsed() << " ms";
                timer2.restart();
            }
        }

		timer2.restart();
		counter = 0;
		foreach(DM::Node* node, nodeList){
			counter++;
			node->addAttribute("test", counter);
            if (n%10000 == 0 && counter>0) {
                DM::Logger(Debug)  <<  "write \t 10000 attributes | " << (long)timer2.elapsed() << " ms";
                timer2.restart();
            }
        }

        DM::Logger(Debug) <<  "write \t" << n << " nodes with one attribute | " << (long)timer.elapsed() << " ms";
        Logger(Standard) << "write\t" << n << "\t | "<< (double) timer.elapsed()/n << "/element\t" << "ms";

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
    // reset config
    DBConnector::getInstance()->setConfig(cfg);
}


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


