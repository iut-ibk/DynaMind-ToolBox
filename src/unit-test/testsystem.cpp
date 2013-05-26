/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#include "testsystem.h"
#include <dm.h>

#include <dmsimulation.h>
#include <dmdbconnector.h>
#include <dmcache.h>

#include <QSqlQuery>
#ifdef _OPENMP
#include <omp.h>
#endif

//#define SQLUNITTESTS
//#define SQLPROFILING
//#define BIGDATATEST

#ifdef _OPENMP
//#define OMPUNITTESTS
//#define OMPPROFILINGTESTS
#endif

namespace DM {
    TEST_F(TestSystem,memoryTest){
        /* TODO
        DM::System * sys = new DM::System();
        DM::Node * n = sys->addNode(new DM::Node(0,0,0));
        std::string uuid = n->getUUID();
        n->addAttribute("Attribute1", 1);
        DM::System * sys1 = sys->createSuccessor();
        n = sys1->addNode(new DM::Node(0,0,0));
        n->addAttribute("Attribute2", 2);
        DM::Component * cmp = sys1->getComponent(uuid);
        cmp->addAttribute("Attribute1", 3);
        cmp->addAttribute("Attribute2", 4);
        cmp->changeAttribute("Attribute1", 5);
        sys1->createSuccessor();
        delete sys;
        ASSERT_TRUE(true);*/
}
TEST_F(TestSystem, RasterData_Flipped_Tset) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Test RasterDat Flipped";
    for (int i = 1; i < 20; i++) {
        DM::Logger(DM::Standard) << i;
        DM::RasterData * plane = new RasterData();
        int width = 10*i;
        int height = 20*i;
        int cellsize = 20;
        plane->setSize(width, height, cellsize, cellsize, 0, 0);

        for (long x = 0; x < width; x++) {
            for (long y = 0; y < height; y++) {
                plane->setCell(x, y, x + width*y);
            }
        }

        DM::Logger(DM::Standard) << plane->getCell(0,0);
        DM::Logger(DM::Standard) << plane->getCell(0,height-1);
        DM::Logger(DM::Standard) << plane->getCell(width-1,0);
        DM::Logger(DM::Standard) << plane->getCell(width-1, height-1);

        for (long x = 0; x < width; x++) {
            for (long y = 0; y < height; y++) {
                EXPECT_DOUBLE_EQ(x + width*y,plane->getCell(x,y));
            }
        }

        delete plane;
    }
}

	
#ifdef SQLUNITTESTS

TEST_F(TestSystem,cachetest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test cache";

	float* one = new float(1.0f);
	float* two = new float(2.0f);
	float* three = new float(3.0f);
	float* four = new float(4.0f);

	Cache<int,float> c(3);
	c.add(1,one);
	c.add(2,two);
	float f = *c.get(1);
	c.add(3,three);
	c.add(4,four);

	ASSERT_TRUE(c.get(1)==one);

	if(c.getSize())
		ASSERT_TRUE(c.get(2)==NULL);
	else
		ASSERT_TRUE(c.get(2)==two);

	ASSERT_TRUE(c.get(3)==three);
	ASSERT_TRUE(c.get(4)==four);
	ASSERT_TRUE(c.get(10)==NULL);

	// get current config
	DBConnectorConfig cfg = DBConnector::getInstance()->getConfig();
	// prepare a new one
	DBConnectorConfig cfgNew = cfg;
	cfgNew.queryStackSize = 1234;
	cfgNew.cacheBlockwritingSize = 1234;
	cfgNew.attributeCacheSize = 1234;
	cfgNew.nodeCacheSize = 1234;
	DBConnector::getInstance()->setConfig(cfgNew);
	// check if config is applied correctly
	DBConnectorConfig cfgNewReturned = DBConnector::getInstance()->getConfig();
	ASSERT_TRUE(cfgNewReturned.queryStackSize == cfgNew.queryStackSize);
	ASSERT_TRUE(cfgNewReturned.cacheBlockwritingSize == cfgNew.cacheBlockwritingSize);
	ASSERT_TRUE(cfgNewReturned.attributeCacheSize == cfgNew.attributeCacheSize);
	ASSERT_TRUE(cfgNewReturned.nodeCacheSize == cfgNew.nodeCacheSize);
	ASSERT_TRUE(Attribute::GetCacheSize() == cfgNew.attributeCacheSize);
	ASSERT_TRUE(Node::GetCacheSize() == cfgNew.nodeCacheSize);

	// reset config
	/*
	cfgNew = cfg;
	cfgNew.attributeCacheSize = 3;

	DM::System sys;
	DM::Component cmp;
	sys.addComponent(&cmp);
	QElapsedTimer timer;
	timer.start();
	for(int i=0;i<1003;i++)
	{
		std::stringstream name;
		name << i;
		cmp.addAttribute(name.str(),i);
	}
	DM::Logger(Error) << "writing 10.000 attributes into db takes " << (long)timer.elapsed() << " ms";
	*/
	DBConnector::getInstance()->setConfig(cfg);
}

TEST_F(TestSystem,simplesqltest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Reallocation (SQL)";

	DM::Simulation sim;
	sim.registerNativeModules("dynamind-testmodules");
	DM::Module * mcreator = sim.addModule("CreateAllComponenets");
	ASSERT_TRUE(mcreator != 0);
	DM::Module * mallocator  = sim.addModule("Reallocator");
	ASSERT_TRUE(mallocator != 0);
	DM::Module * mcheck  = sim.addModule("CheckAllComponenets");
	ASSERT_TRUE(mcheck != 0);
	DM::ModuleLink * l1 = sim.addLink(mcreator->getOutPort("sys"), mallocator->getInPort("sys"));
	ASSERT_TRUE(l1 != 0);
	DM::ModuleLink * l2 = sim.addLink(mallocator->getOutPort("sys"), mcheck->getInPort("sys"));
	ASSERT_TRUE(l2 != 0);
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

	DBConnector::getInstance()->Synchronize();
	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}


TEST_F(TestSystem,sqlsuccessortest)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Successor states (SQL)";

	// check if uuids are created
	Component* c = new Component();
	ASSERT_TRUE(c->getUUID() != "");
	// check if attribute is added
	ASSERT_TRUE(c->getAttribute(UUID_ATTRIBUTE_NAME)->getString() != "");
	delete c;

	// check if no name is created if there is no element
	System *sys = new System();
	c = sys->getComponent((std::string)"test");
	ASSERT_TRUE(c == NULL);
	delete sys;

	// check if successor names conserved
	sys = new System();
	Node* n = new Node(1,2,3);
	sys->addNode(n);
	std::string name = n->getUUID();	// add, then init uuid
	Node* n2 = new Node(4,5,6);
	sys->addNode(n2);
	System *sys2 = sys->createSuccessor();
	std::string name2 = n2->getUUID();	// add, successor then get name

	Node* sysn = sys->getNode(name);
	Node* sysn2 = sys2->getNode(name);
	Node* sysn22 = sys2->getNode(name2);
	ASSERT_TRUE(*sysn2 == *n);
	ASSERT_TRUE(*sysn22 == *n2);
	ASSERT_TRUE(sysn2->getUUID() == n->getUUID());
	ASSERT_TRUE(sysn22->getUUID() == n2->getUUID());
	ASSERT_TRUE(sysn->getQUUID() == n->getQUUID());
	ASSERT_TRUE(sysn22->getQUUID() != n2->getQUUID());

	delete sys;
	//delete sys2;	successor states are deleted by sys
}

TEST_F(TestSystem, SqlNodeTest)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test face nodes order (SQL)";

	DM::Node *node = new DM::Node(1,2,3);
	ASSERT_TRUE(node->getX()==1);
	ASSERT_TRUE(node->getY()==2);
	ASSERT_TRUE(node->getZ()==3);

	DM::Node *copy = new DM::Node(*node);
	*copy = *node;
	DM::Node* copy2 = new DM::Node(*node);
	DM::Node *copy3 = new DM::Node();
	*copy3 = *node;
	delete node;

	ASSERT_TRUE(copy->getX()==1);
	ASSERT_TRUE(copy->getY()==2);
	ASSERT_TRUE(copy->getZ()==3);

	ASSERT_TRUE(copy2->getX()==1);
	ASSERT_TRUE(copy2->getY()==2);
	ASSERT_TRUE(copy2->getZ()==3);

	ASSERT_TRUE(copy3->getX()==1);
	ASSERT_TRUE(copy3->getY()==2);
	ASSERT_TRUE(copy3->getZ()==3);

	delete copy;
	delete copy2;
	delete copy3;

	DBConnector::getInstance()->Synchronize();
	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}

/** @brief Tests deleting accessing nodes with edge pointers
 *
 * This method is often used in dynamind modules to go through a directed graph.
 *
 * First a map of start nodes over all edges is created. As key the pointers of the start nodes are used.
 * To go through the graph you use a leaf. Now you use the start node map to get the edge. To get the next element
 * you now use the pointer of the end node of the edge as start node for the start node map.
 * The method is tested with 3 points and two edges. n1->n2->n3.
 */

TEST_F(TestSystem, EdgeTestTreeSearch)
{
	DM::System * sys = new DM::System();

	DM::View nv("nv", DM::NODE, DM::WRITE);
	DM::Node * n1 = sys->addNode(0,0,0,nv);
	DM::Node * n2 = sys->addNode(0,1,0,nv);
	DM::Node * n3 = sys->addNode(0,2,0,nv);

	DM::View ev("ev", DM::EDGE, DM::WRITE);
	sys->addEdge(n1, n2, ev);
	sys->addEdge(n2, n3, ev);

	std::vector<std::string> uuids = sys->getUUIDs(ev);
	std::map<DM::Node *, DM::Edge*> startNodeMap;
	foreach (std::string uuid, uuids) {
		DM::Edge * c = sys->getEdge(uuid);
		startNodeMap[sys->getNode(c->getStartpointName())] = c;
	}

	DM::Node * startNode = sys->getNode(n1->getUUID());
	DM::Edge * se1 = startNodeMap[startNode];
	DM::Node * nextNode = sys->getNode(se1->getEndpointName());
	DM::Edge * se2 = startNodeMap[nextNode];

	ASSERT_TRUE(nextNode->getUUID() == n2->getUUID());
	ASSERT_TRUE(se2 != NULL);

	delete sys;
}

/** @brief Similar to EdgeTestTreeSearch, but with successor state*/
TEST_F(TestSystem, PredecessorEdgeTestTreeSearch)
{
	DM::System * sys = new DM::System();

	DM::View nv("nv", DM::NODE, DM::WRITE);
	DM::Node * n1 = sys->addNode(0,0,0,nv);
	DM::Node * n2 = sys->addNode(0,1,0,nv);
	DM::Node * n3 = sys->addNode(0,2,0,nv);

	DM::View ev("ev", DM::EDGE, DM::WRITE);
	sys->addEdge(n1, n2, ev);
	sys->addEdge(n2, n3, ev);
	
	DM::System * sys_succ = sys->createSuccessor();

	std::vector<std::string> uuids = sys_succ->getUUIDs(ev);

	std::map<std::string, Component*> uuidmap = sys_succ->getAllComponentsInView(ev);

	std::map<DM::Node *, DM::Edge*> startNodeMap;
	foreach (std::string uuid, uuids) {
		DM::Edge * c = sys_succ->getEdge(uuid);
		startNodeMap[sys_succ->getNode(c->getStartpointName())] = c;
	}

	DM::Node * startNode = sys_succ->getNode(n1->getUUID());
	DM::Edge * se1 = startNodeMap[startNode];
	DM::Node * nextNode = sys_succ->getNode(se1->getEndpointName());
	DM::Edge * se2 = startNodeMap[nextNode];
	
	// view update
	ASSERT_TRUE(uuidmap[se1->getUUID()] == se1);
	ASSERT_TRUE(uuidmap[se2->getUUID()] == se2);

	ASSERT_TRUE(nextNode->getUUID() == n2->getUUID());

	ASSERT_TRUE(se2 != NULL);

	ASSERT_TRUE(sys_succ->getChilds().size() == 5);	// 3 nodes + 2 edges

	delete sys;
}

TEST_F(TestSystem, SqlEdgeTest)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test face nodes order (SQL)";

	DM::Node *n0 = new DM::Node(1,2,3);
	DM::Node *n1 = new DM::Node(1,2,3);

	DM::Edge *edge = new DM::Edge(n0, n1);

	ASSERT_TRUE(edge->getStartpointName()==n0->getUUID());
	ASSERT_TRUE(edge->getEndpointName()==n1->getUUID());

	std::vector<Edge*> list = n0->getEdges();
	ASSERT_TRUE(list[0] == edge);

	list = n1->getEdges();
	ASSERT_TRUE(list[0] == edge);

	delete n0;
	delete n1;
	delete edge;

	DBConnector::getInstance()->Synchronize();
	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}

TEST_F(TestSystem, SqlFaceOrder)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test face nodes order (SQL)";

	DM::Node n0(0,1,2);
	DM::Node n1(3,4,5);
	DM::Node n2(6,7,8);
	std::vector<DM::Node*> nodes;
	nodes.push_back(&n0);
	nodes.push_back(&n1);
	nodes.push_back(&n2);

	DM::Face f(nodes);
	DM::Face h(f);
	f.addHole(&h);

	ASSERT_TRUE(f.getNodePointers().size()==3);
	ASSERT_TRUE(f.getHolePointers().size()==1);
	ASSERT_TRUE(f.getHolePointers()[0]->getNodePointers().size()==3);

	ASSERT_TRUE(*f.getNodePointers()[0]==n0);
	ASSERT_TRUE(*f.getNodePointers()[1]==n1);
	ASSERT_TRUE(*f.getNodePointers()[2]==n2);

	ASSERT_TRUE(*f.getHolePointers()[0]->getNodePointers()[0]==n0);
	ASSERT_TRUE(*f.getHolePointers()[0]->getNodePointers()[1]==n1);
	ASSERT_TRUE(*f.getHolePointers()[0]->getNodePointers()[2]==n2);

	DBConnector::getInstance()->Synchronize();
	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}
TEST_F(TestSystem, SQLRasterdata)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test raster data (SQL)";

	int size = 128;
	DM::RasterData* raster = new DM::RasterData(size,size,1,1,0,0);
	// check no value
	DM::Logger(DM::Debug) << "checking default values";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			ASSERT_TRUE(raster->getCell(x,y) == raster->getNoValue());
	// insert
	DM::Logger(DM::Debug) << "inserting values";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			raster->setCell(x,y,x*1000+y);
	// check values
	DM::Logger(DM::Debug) << "checking values";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
		{
			DM::Logger(DM::Debug) << "checking " << x << "/" << y;
			ASSERT_TRUE(raster->getCell(x,y) == x*1000+y);
		}
		delete raster;

		raster = new DM::RasterData();
		raster->setSize(size,size,1,1,0,0);
		// check no value
		DM::Logger(DM::Debug) << "checking default values with seperatly initialized grid";
		for(long x=0;x<size;x++)
			for(long y=0;y<size;y++)
				ASSERT_TRUE(raster->getCell(x,y) == raster->getNoValue());
		// insert
		DM::Logger(DM::Debug) << "inserting values with seperatly initialized grid";
		for(long x=0;x<size;x++)
			for(long y=0;y<size;y++)
				raster->setCell(x,y,x*1000+y);
		// check values
		DM::Logger(DM::Debug) << "checking values with seperatly initialized grid";
		for(long x=0;x<size;x++)
			for(long y=0;y<size;y++)
				ASSERT_TRUE(raster->getCell(x,y) == x*1000+y);

		delete raster;

		DBConnector::getInstance()->Synchronize();
		// print cache statistics
		DM::Node::PrintCacheStatistics();
		DM::Attribute::PrintCacheStatistics();
		//DM::RasterData::PrintCacheStatistics();
}

TEST_F(TestSystem, SQLattributes)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test attributes (SQL)";

	DM::Logger(DM::Standard) << "Test attribute cache";

	{
		// test attribute cache
		// generate new component, as cache wont be used if attribute is not owned
		Component* c = new Component;
		// resize cache, so we dont have to wait too long for reaching the limits
		unsigned int cacheBefore = Attribute::GetCacheSize();
		Attribute::ResizeCache(7);
		// add
		for(int i=0;i<10;i++)
		{
			std::stringstream name;
			name << "name " << i;
			c->addAttribute(name.str(), i+1);
		}
		// check
		for(int i=0;i<10;i++)
		{
			std::stringstream name;
			name << "name " << i;
			ASSERT_TRUE(c->getAttribute(name.str())->getDouble() == i+1.0);
		}
		// change attributes
		for(int i=0;i<10;i++)
		{
			std::stringstream name, newname;
			name << "name " << i;
			//newname << "changed name " << i;
			Attribute newatt(name.str(), i+20.0);
			c->getAttribute(name.str())->Change(newatt);
		}
		//DBConnector::getInstance()->Synchronize();
		// check
		for(int i=0;i<10;i++)
		{
			std::stringstream name;
			name << "name " << i;
			ASSERT_TRUE(c->getAttribute(name.str())->getDouble() == i+20.0);
			//Logger(Error) << name.str() << ": " << c->getAttribute(name.str())->getDouble();
		}
		// reset cache
		Attribute::ResizeCache(cacheBefore);
		delete c;
	}


	DM::Logger(DM::Debug) << "checking add attributes";
	// DOUBLE
	DM::Component *c = new DM::Component();
	DM::Attribute attr("hint", 50);
	bool isNew = c->addAttribute(attr);
	DM::Attribute *pa = c->getAttribute("hint");
	ASSERT_TRUE(pa->getDouble()==50);
	delete c;

	// check getUuid, assignment operators
	c = new DM::Component();
	std::string name = c->getUUID();
	ASSERT_TRUE(c->getUUID() == name);

	DM::Component* c2 = new DM::Component(*c);
	DM::Component* c3 = new DM::Component();
	*c3 = *c;
	delete c;

	ASSERT_TRUE(c2->getUUID() == name);
	ASSERT_TRUE(c3->getUUID() == name);
	delete c2;
	delete c3;
	//

	double dbl = 13.0;
	std::vector<double> vecDbl;
	vecDbl.push_back(dbl);
	vecDbl.push_back(dbl*2);
	vecDbl.push_back(dbl*3);

	std::string str = "peter thermometer";
	std::vector<std::string> vecStr;
	vecStr.push_back(str);
	vecStr.push_back(str+str);
	vecStr.push_back(str+str+str);

	DM::Logger(DM::Debug) << "checking doubles";
	// DOUBLE
	DM::Attribute* a = new DM::Attribute("fuzzi");
	a->setDouble(dbl);
	ASSERT_TRUE(a->getDouble() == dbl);
	delete a;

	DM::Logger(DM::Debug) << "checking double vectors";
	// DOUBLE VECTOR
	a = new DM::Attribute("fuzzi");
	a->setDoubleVector(vecDbl);
	ASSERT_TRUE(a->getDoubleVector() == vecDbl);
	delete a;

	DM::Logger(DM::Debug) << "checking strings";
	// STRING
	a = new DM::Attribute("fuzzi");
	a->setString(str);
	ASSERT_TRUE(a->getString() == str);
	delete a;

	DM::Logger(DM::Debug) << "checking string vectors";
	// STRING VECTOR
	a = new DM::Attribute("fuzzi");
	a->setStringVector(vecStr);
	ASSERT_TRUE(a->getStringVector() == vecStr);
	delete a;

	DM::Logger(DM::Debug) << "checking time series";
	// TIME SERIES
	a = new DM::Attribute("fuzzi");
	a->addTimeSeries(vecStr, vecDbl);
	std::vector<double> vecDblOut;
	std::vector<std::string> vecStrOut;
	a->getTimeSeries(&vecStrOut, &vecDblOut);
	ASSERT_TRUE(vecDblOut == vecDbl);
	ASSERT_TRUE(vecStrOut == vecStr);
	delete a;

	DM::Logger(DM::Debug) << "checking links";
	// LINKS
	a = new DM::Attribute("fuzzi");

	std::vector<DM::LinkAttribute> links;
    DM::LinkAttribute link0 ("0","a");
    DM::LinkAttribute link1 ("1","b");
    DM::LinkAttribute link2 ("2","c");

	links.push_back(link0);
	links.push_back(link1);
	a->setLinks(links);
	links.push_back(link2);
	a->setLink(link2.viewname, link2.uuid);
	std::vector<DM::LinkAttribute> linksOut = a->getLinks();

	for(unsigned int i=0;i<links.size();i++)
	{
		ASSERT_TRUE(linksOut[i].uuid == links[i].uuid);
		ASSERT_TRUE(linksOut[i].viewname == links[i].viewname);
	}
	delete a;
	DM::Logger(DM::Debug) << "checking copy constructor";
	a = new DM::Attribute("fuzzi", 5.0);
	DM::Attribute *b = new Attribute(*a);
	ASSERT_TRUE(a->getDouble() == 5.0);
	ASSERT_TRUE(b->getDouble() == 5.0);
	delete a;
	delete b;

	DBConnector::getInstance()->Synchronize();
	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}
TEST_F(TestSystem, System)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test GetEdges";


	System sys;
	Node* n0 = sys.addNode(1,2,3);
	Node* n1 = sys.addNode(4,5,6);
	Edge *e = sys.addEdge(n0,n1);
	ASSERT_TRUE(e==sys.getEdge(n0->getUUID(), n1->getUUID()));
}

#endif


#ifdef SQLPROFILING
/*
TEST_F(TestSystem,sqlprofiling) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Test Profiling (SQL)";

	const int n = 1000;

	QElapsedTimer timer;
	timer.start();
	// attribute profiling
	timer.restart();
	DM::Attribute* abuffer = new DM::Attribute[n];
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "create " << n << " attributes " << (long)timer.elapsed();

	timer.restart();
	delete[] abuffer;
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "delete " << n << " attributes " << (long)timer.elapsed();

	timer.restart();
	DM::Attribute * a0 = new DM::Attribute("name", 10.0);
	DM::Attribute *aptBuffer[n];
	for(int i=0;i<n;i++)
		aptBuffer[i] = new DM::Attribute(*a0);

	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "copy " << n << " attributes " << (long)timer.elapsed();
	delete a0;
	for(int i=0;i<n;i++)
		delete aptBuffer[i];

	// component stuff
	DM::Component* buffer = new DM::Component[n];
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "create " << n << " components " << (long)timer.elapsed();

	timer.restart();
	for(int i=0;i<n;i++)
		buffer[i].addAttribute("thedouble", 24.0);
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "add " << n << " attributes " << (long)timer.elapsed();

	timer.restart();
	for(int i=0;i<n;i++)
		buffer[i].changeAttribute("thedouble", 48.0);
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "change " << n << " attributes " << (long)timer.elapsed();

	timer.restart();
	for(int i=0;i<n;i++)
	{
		DM::Attribute attr("thestring", "blubberdiblub");
		buffer[i].addAttribute(attr);
	}
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "copyadd " << n << " attributes " << (long)timer.elapsed();

	timer.restart();
	delete[] buffer;
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "delete " << n << " components " << (long)timer.elapsed();

	// nodes
	timer.restart();
	for(int i=0;i<n;i++)
	{
		DM::Node* node = new DM::Node(0,0,0);
		node->setX(1.0);
		delete node;
	}
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "create and change " << n << " single nodes " << (long)timer.elapsed();

	timer.restart();
	DM::Node* baseNode = new DM::Node(0,0,0);
	DM::System* sys = new System();
	for(int i=0;i<n;i++)
	{
		DM::Node node(*baseNode);
		sys->addNode(node);
	}
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "attach and copy " << n << "  nodes " << (long)timer.elapsed();
	
	delete sys;
	sys = new System();
	baseNode->addAttribute("d_att",40);
	
	timer.restart();
	for(int i=0;i<n;i++)
		sys->addNode(*baseNode);

	std::vector<Component*> comps = sys->getChilds();
	foreach(Component* c, comps)
		sys->addNode(*(Node*)c);

	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "create and copy " << n << " attached nodes " << (long)timer.elapsed();
	
	delete baseNode;
	delete sys;
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "delete " << n << "  nodes with system " << (long)timer.elapsed();

	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}
/**/
/*
TEST_F(TestSystem,sqlRasterDataProfiling) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Test raster data profiling";

	const int n = 1000;
	QElapsedTimer timer;
	timer.restart();
	DM::RasterData* raster = new DM::RasterData(n,n,1.0,1.0,0.0,0.0);
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "create rasterdata(" << n << "x" << n << ") " << (long)timer.elapsed();

	timer.restart();
	for(int y=0;y<n;y++)
		for(int x=0;x<n;x++)
			raster->getValue(x,y);

	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "get each rasterdata entry(" << n << "x" << n << ") " << (long)timer.elapsed();

	timer.restart();
	for(int y=0;y<n;y++)
		for(int x=0;x<n;x++)
			raster->setValue(x,y,x*1000+y);

	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "change each rasterdata entry(" << n << "x" << n << ") " << (long)timer.elapsed();

	
	timer.restart();
	
	for(int y=0;y<n;y++)
	{
		for(int x=0;x<n;x++)
		{
			std::vector<double> neigh;
			neigh.resize(9,0.0);
			raster->getMoorNeighbourhood(neigh, x , y);
		}
	}
	
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "get neightboorhoods(" << n << "x" << n << ") " << (long)timer.elapsed();



	timer.restart();
	delete raster;
	DM::DBConnector::getInstance()->Synchronize();
	DM::Logger(DM::Standard) << "delete rasterdata(" << n << "x" << n << ") " << (long)timer.elapsed();

	DBConnector::getInstance()->Synchronize();
	// print cache statistics
	DM::Node::PrintCacheStatistics();
	DM::Attribute::PrintCacheStatistics();
	//DM::RasterData::PrintCacheStatistics();
}
*/

//#define SELECT_TEST
#ifdef SELECT_TEST

void insert(int numelements) 
{
    QSqlQuery query;
    QElapsedTimer timer;
    timer.start();
    query.exec("BEGIN");
    query.prepare("INSERT INTO t3 (key, x, y, z) "
                  "VALUES (?, ?, ?, ?)");
    for (int i = 0; i < numelements; i++) 
	{
        query.addBindValue(i);
        query.addBindValue((double)i*2);
        query.addBindValue((double)i*3);
        query.addBindValue((double)i*5);
        if (!query.exec())
		{
			PrintSqlError(&query);
			return;
		}
    }
    query.exec("COMMIT");
	std::cout << "inserted " << numelements << " elements in " << timer.elapsed() << " ms" << std::endl;
}

void iterative_select(int numelements) 
{
    QElapsedTimer timer;
    timer.start();
    
    int counter = 0;
	QSqlQuery query;
        query.prepare("SELECT key,x,y,z FROM t3 WHERE key = ?");

	for (int i = 0; i < numelements; i++) 
	{
		query.addBindValue(i);
        if (!query.exec())
		{
			PrintSqlError(&query);
			return;
		}
		if(!query.next())
            std::cout << "no entry found" << std::endl;

		int key = query.value(0).toInt();
		ASSERT_TRUE(query.value(1).toDouble() == key*2.0);
		ASSERT_TRUE(query.value(2).toDouble() == key*3.0);
		ASSERT_TRUE(query.value(3).toDouble() == key*5.0);
		counter++;
	}
	
	if(counter != numelements)
		std::cout << "error: could not read all elements (" << counter << "/" << numelements << ")" << std::endl;

    long t = timer.elapsed();
	std::cout << "iterative select of " << numelements << " elements took " << t << " ms / " << t/(double)numelements << " ms per element" << std::endl;
}

void range_select(int numelements, int blocksize) 
{
    QElapsedTimer timer;
    timer.start();

    int counter = 0;
	QSqlQuery query;
	query.prepare("SELECT key,x,y,z FROM t3 WHERE key >= ? AND key < ?");

    for (int i = 0; i < numelements/blocksize; i++) 
	{
        query.addBindValue( i*blocksize );
        query.addBindValue( (i+1)*blocksize );
        if (!query.exec())
		{
			PrintSqlError(&query);
			return;
		}

        while (query.next()) 
		{
			int key = query.value(0).toInt();
			ASSERT_TRUE(query.value(1).toDouble() == key*2.0);
			ASSERT_TRUE(query.value(2).toDouble() == key*3.0);
			ASSERT_TRUE(query.value(3).toDouble() == key*5.0);
            counter++;
        }
    }
	if(counter != numelements)
		std::cout << "error: could not read all elements (" << counter << "/" << numelements << ")" << std::endl;
	
    long t = timer.elapsed();
	std::cout << "range select of " << numelements << " elements with blocksize " << blocksize << " took " << t << " ms / " << t/(double)numelements << " ms per element" << std::endl;
}

void combined_select(int numelements, int blocksize) 
{
    QElapsedTimer timer;
    timer.start();

    int counter = 0;
	QString queryString = "SELECT key,x,y,z FROM t3  WHERE key = ?";
	for(int j = 1; j < blocksize; j++)
		queryString.append(" OR key = ?");
	
	QSqlQuery query;
		query.prepare(queryString);

    for (int i = 0; i < numelements/blocksize; i++) 
	{
		for(int j = 0; j < blocksize; j++)
			query.addBindValue( i*blocksize+j );

        if (!query.exec())
		{
			PrintSqlError(&query);
			return;
		}

        while (query.next()) 
		{
			int key = query.value(0).toInt();
			ASSERT_TRUE(query.value(1).toDouble() == key*2.0);
			ASSERT_TRUE(query.value(2).toDouble() == key*3.0);
			ASSERT_TRUE(query.value(3).toDouble() == key*5.0);
            counter++;
        }
    }

	if(counter != numelements)
		std::cout << "error: could not read all elements (" << counter << "/" << numelements << ")" << std::endl;
	
    long t = timer.elapsed();
	std::cout << "combined select of " << numelements << " elements with blocksize " << blocksize << " took " << t << " ms / " << t/(double)numelements << " ms per element" << std::endl;
}

#include <QSqlError>

TEST_F(TestSystem,selectTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Profiling sql select";

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    if(!db.open())
	{
        std::cout << "error opening db" << std::endl<< std::endl;
		return;
	}

    int N = 1e5;
    for(long numelements = 64; numelements <= N; numelements *= 4)
	{	
		QSqlQuery query;
        std::cout << "START N " << numelements << std::endl;
		query.exec("DROP TABLE IF EXISTS t3");
        if (!query.exec("create table t3 (key INTEGER, x DOUBLE PRECISION, y DOUBLE PRECISION, z DOUBLE PRECISION)"))
		{
			PrintSqlError(&query);
			return;
		}

        	insert(numelements);
		
		iterative_select(numelements);
		range_select(numelements, 1);
        	range_select(numelements, 4);
        	range_select(numelements, 16);
        	range_select(numelements, 64);
		combined_select(numelements, 1);
		combined_select(numelements, 4);
		combined_select(numelements, 16);
		combined_select(numelements, 64);

        if (!query.exec("DROP TABLE t3"))
            std::cout << "Error in droping" << std::endl;

        std::cout << "END" << std::endl<< std::endl;
    }

    std::cout << "End test" << std::endl;
    db.close(); // for close connection
}

#endif // SELECT_TEST

TEST_F(TestSystem,getComponentsInViewProfiling) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Profiling getAllComponentsInView";

	long n = 1e4;
	DM::View v("testview", DM::NODE, DM::MODIFY);
	DM::View va("testview2", DM::COMPONENT, DM::MODIFY);
	va.addAttribute("test_dbl");

	DM::System sys, sysa;
	sys.addView(v);
	sysa.addView(va);
	
    DM::Logger(DM::Standard) << "initializing test system";
	for(long i=0;i<n;i++)
		sys.addComponent(new DM::Node, v);
	DM::Node::ClearCache();

    DM::Logger(DM::Standard) << "retrieving " << n << " nodes from view";
	QElapsedTimer timer;
	timer.start();
	std::map<std::string, Component*> cmps = sys.getAllComponentsInView(v);
	DM::Logger(DM::Standard) << "took " << (long)timer.elapsed() << " ms";
	ASSERT_TRUE(cmps.size() == n);
	
    DM::Logger(DM::Standard) << "initializing successor test system";
	System* suc = sys.createSuccessor();
	DM::Node::ClearCache();
	
    DM::Logger(DM::Standard) << "retrieving " << n << " nodes from view";
	timer.restart();
	cmps = suc->getAllComponentsInView(v);
	DM::Logger(DM::Standard) << "took " << (long)timer.elapsed() << " ms";
	ASSERT_TRUE(cmps.size() == n);
	/*
    DM::Logger(DM::Standard) << "initializing attribute test system";
	for(long i=0;i<n;i++)
	{
		DM::Component *c = new DM::Component();
		c->addAttribute("test_dbl", 1.0);
		sysa.addComponent(c, va);
	}
	DM::Attribute::ClearCache();
	
    DM::Logger(DM::Standard) << "retrieving " << n << " components with attributes from view";
	timer.restart();
	cmps = sysa.getAllComponentsInView(va);
	DM::Logger(DM::Standard) << "took " << (long)timer.elapsed() << " ms";
	ASSERT_TRUE(cmps.size() == n);
	
    DM::Logger(DM::Standard) << "initializing successor test system";
	System* suca = sysa.createSuccessor();
	DM::Attribute::ClearCache();

    DM::Logger(DM::Standard) << "retrieving " << n << " components with attributes from view";
	timer.restart();
	cmps = suca->getAllComponentsInView(va);
	DM::Logger(DM::Standard) << "took " << (long)timer.elapsed() << " ms";
	ASSERT_TRUE(cmps.size() == n);*/
}

#endif

#ifdef BIGDATATEST
long randlong()
{
	long q = (2*rand()+rand()%2);
	q = (q<<16)+(2*rand()+rand()%2);
	//q = (q<<16)+(2*rand()+rand()%2);
	//q = (q<<16)+(2*rand()+rand()%2);
	return q;
}

TEST_F(TestSystem,standardBigDataTest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "standard big data test";
	
	// get current config
	DBConnectorConfig cfg = DBConnector::getInstance()->getConfig();
	// prepare a new one
	DBConnectorConfig cfgNew = cfg;
	
	QElapsedTimer timer;
	const long N = 1e7;
	
	DM::Logger(Standard) << "cachesize\tnumber of elements\toperation\ttime";
	
	for(long cachesize = 100; cachesize <= N; cachesize *= 10)
	{
		cfgNew.nodeCacheSize = cachesize;
		DBConnector::getInstance()->setConfig(cfgNew);

		for(long numelements = 100; numelements <= N; numelements *= 10)
		{
			long addTime, getTime, setTime;
			DM::Node::ClearCache();
			DM::System sys;
			std::vector<DM::Node*> nodes;
			std::vector<long> rndNumbers;

			// measure addNode
			timer.start();
			for(long i = 0; i < numelements; i++)
				nodes.push_back(sys.addNode(0,0,0));
			addTime = timer.elapsed();
			
			// generate a new set of random numbers
			rndNumbers.clear();
			for(long i = 0; i < numelements; i++)
				rndNumbers.push_back(abs(randlong()%numelements));
			
			// measure getX
			timer.restart();
			foreach(long l, rndNumbers)
				nodes[l]->getX();
			getTime = timer.elapsed();
			
			// generate a new set of random numbers
			rndNumbers.clear();
			for(long i = 0; i < numelements; i++)
				rndNumbers.push_back(abs(randlong()%numelements));
			
			// measure setX
			timer.restart();
			foreach(long l, rndNumbers)
				nodes[l]->setX(1.0);
			setTime = timer.elapsed();
			
			DM::Logger(Standard) << cachesize<< "\t\t" << numelements << "\t\t\tadd\t\t" << addTime;
			DM::Logger(Standard) << cachesize<< "\t\t" << numelements << "\t\t\tget\t\t" << getTime;
			DM::Logger(Standard) << cachesize<< "\t\t" << numelements << "\t\t\tset\t\t" << setTime;
		}
	}
	
	// reset configs
	DBConnector::getInstance()->setConfig(cfg);
}

#endif

#ifdef OMPUNITTESTS

TEST_F(TestSystem,OMP) 
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "testing omp";

	int n = 1000;
	omp_set_num_threads(8);

	DM::Component c;
#pragma omp parallel for
	for(int i=0;i<n;i++)
		c.addAttribute(QString::number(i).toStdString(), (double)i);
	
	for(int i=0;i<n;i++)
	{
		double d = c.getAttribute(QString::number(i).toStdString())->getDouble();
		ASSERT_TRUE(d == i);
		//DM::Logger(Debug) << "successfully added double attribute #" << i << " with value = " << d;
	}
	
#pragma omp parallel for
	for(int i=0;i<n;i++)
		c.removeAttribute(QString::number(i).toStdString());

	//DM::Logger(Debug) << "elements left: " << c.getAllAttributes().size();
	ASSERT_TRUE(c.getAllAttributes().size() == 0);
}
#endif

#ifdef OMPPROFILINGTESTS
void InsertRemoveComponentTestOMP(DM::Component& c,unsigned long n)
{
	#pragma omp parallel for
	for(int i=0;i<n;i++)
		c.addAttribute(QString::number(i).toStdString(), (double)i);

	#pragma omp parallel for
	for(int i=0;i<n;i++)
		c.removeAttribute(QString::number(i).toStdString());
}
void InsertRemoveComponentTest(DM::Component& c,unsigned long n)
{
	for(int i=0;i<n;i++)
		c.addAttribute(QString::number(i).toStdString(), (double)i);

	for(int i=0;i<n;i++)
		c.removeAttribute(QString::number(i).toStdString());
}

TEST_F(TestSystem,profilingOMP) 
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "profiling omp";
	
	DM::Component c;
	
	unsigned long maxN = 1e6;
#if !defined _DEBUG
	maxN = 1e7;
#endif
	for(unsigned long n = 10; n<maxN;n*=10)
	{
		QElapsedTimer timer;
		timer.start();
		InsertRemoveComponentTest(c,n);
		long noompThreadTime = timer.elapsed();

		omp_set_num_threads(1);
		timer.start();
		InsertRemoveComponentTestOMP(c,n);
		long singleThreadTime = timer.elapsed();
	
		omp_set_num_threads(2);
		timer.restart();
		InsertRemoveComponentTestOMP(c,n);
		long dualThreadTime = timer.elapsed();

		omp_set_num_threads(4);
		timer.restart();
		InsertRemoveComponentTestOMP(c,n);
		long quadThreadTime = timer.elapsed();
	
		DM::Logger(DM::Standard) << "results for n = "<<(long)n<<" time[ms](threadcount)<speedup>: "
			<< noompThreadTime <<"(no omp 1)<>\t" << singleThreadTime <<"(1)<1>\t"
			<< dualThreadTime <<"(2)<"<<singleThreadTime/(float)dualThreadTime<<">\t"
			<< quadThreadTime <<"(4)<"<<singleThreadTime/(float)quadThreadTime<<">\t";
	}
}
#endif

TEST_F(TestSystem,AttributesInSystem) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Attributes in System";
    DM::System * sys = new System();
    sys->addAttribute("year", 2010);
    DM::System * sys_next = sys->createSuccessor();

    double year = sys_next->getAttribute("year")->getDouble() + 1;

    sys_next->addAttribute("year",year);

    ASSERT_DOUBLE_EQ(sys->getAttribute("year")->getDouble(), 2010);
    ASSERT_DOUBLE_EQ(sys_next->getAttribute("year")->getDouble(), 2011);

    delete sys;
}

}
