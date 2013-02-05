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

#include <ostream>
#include <testsimulation.h>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmlog.h>
#include <dynamicinout.h>
//#include <grouptest.h>
//#include <dmporttuple.h>

#include <dmdbconnector.h>
#include <QSqlQuery>
#include <qelapsedtimer.h>
#include <dmsystem.h>
#include <dm.h>

#define SQLUNITTESTS
//#define SQLPROFILING
#define STDUNITTESTS


namespace {
	/*
void SeperateInsert(long nx, long ny)
{
    QSqlQuery q;
    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            if(!q.exec("INSERT INTO rasterfields VALUES (0,"
                        +QString::number(x)+","
                        +QString::number(y)+",0)"))
                    DM::PrintSqlError(&q);
        }
    }
}

void SeperateInsertWrapper(long nx, long ny)
{
    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            QSqlQuery *q = DM::DBConnector::getInstance()->getQuery(
                        "INSERT INTO rasterfields VALUES (0,"
                        +QString::number(x)+","
                        +QString::number(y)+",0)");
            DM::DBConnector::getInstance()->ExecuteQuery(q);
        }
    }
    DM::DBConnector::getInstance()->Synchronize();
}

void SeperateInsertParamWrapper(long nx, long ny)
{
    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            QSqlQuery *q = DM::DBConnector::getInstance()->getQuery("INSERT INTO rasterfields VALUES (0,?,?,0)");
            q->addBindValue(QVariant::fromValue(x));
            q->addBindValue(QVariant::fromValue(y));
            DM::DBConnector::getInstance()->ExecuteQuery(q);
        }
    }
    DM::DBConnector::getInstance()->Synchronize();
}

void TransactionInsert(long nx, long ny)
{
    QSqlQuery q;
    if(!q.exec("BEGIN TRANSACTION")) DM::PrintSqlError(&q);

    for(long x = 0; x < nx; x++)
        for(long y = 0; y < ny; y++)
            if(!q.exec("INSERT INTO rasterfields VALUES (0,"+QString::number(x)+","+QString::number(y)+",0)"))
                DM::PrintSqlError(&q);
    if(!q.exec("END TRANSACTION")) DM::PrintSqlError(&q);
}

void BLOBInsert(long nx, long ny)
{
    for(long x = 0; x < nx; x++)
    {
        QByteArray qba;
        QDataStream qds(&qba, QIODevice::WriteOnly);
        for(long y = 0; y < ny; y++)
            qds << (double)0.0;

        QSqlQuery *q = DM::DBConnector::getInstance()->getQuery("INSERT INTO rasterfields VALUES (0,?,?)");
        q->addBindValue(QVariant::fromValue(x));
        q->addBindValue(qba);
        DM::DBConnector::getInstance()->ExecuteQuery(q);
    }
    DM::DBConnector::getInstance()->Synchronize();
}
*/
/*
void TransactionInsert2(long nx, long ny)
{
    DM::DBConnector::getInstance()->BeginTransaction();
    QSqlQuery *q;

    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            q = DM::DBConnector::getInstance()->getQuery("INSERT INTO rasterfields VALUES (0,"+QString::number(x)+","+QString::number(y)+",0)");
            if(!q->exec())
                DM::PrintSqlError(q);
        }
    }
    DM::DBConnector::getInstance()->CommitTransaction();
}
void TransactionInsert3(long nx, long ny)
{
    DM::DBConnector::getInstance()->BeginTransaction();
    QSqlQuery *q;

    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            q = DM::DBConnector::getInstance()->getQuery("INSERT INTO rasterfields VALUES (0,?,?,0)");
            q->addBindValue(QVariant::fromValue(x));
            q->addBindValue(QVariant::fromValue(y));
            if(!q->exec())
                DM::PrintSqlError(q);
        }
    }
    DM::DBConnector::getInstance()->CommitTransaction();
}

void TransactionInsert4(long nx, long ny)
{
    //DM::DBConnector::getInstance()->BeginTransaction();
    QSqlQuery *q;

    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            q = DM::DBConnector::getInstance()->getQuery("INSERT INTO rasterfields VALUES (0,?,?,0)");
            q->addBindValue(QVariant::fromValue(x));
            q->addBindValue(QVariant::fromValue(y));
            DM::DBConnector::getInstance()->ExecuteQuery(q);
        }
    }
    DM::DBConnector::getInstance()->CommitTransaction();
}
*/
/*
QString CreateQuery(long firstx, long firsty)
{
    return "INSERT INTO rasterfields \n SELECT 0 AS datalink, "
            +QString::number(firstx)+" AS x, "
            +QString::number(firsty)+" AS y, 0 AS value\n";
}

void UnionInsert(long nx, long ny, long maxUnions)
{
    QString strQuery;
    QSqlQuery q;

    long counter = maxUnions;
    for(long x = 0; x < nx; x++)
    {
        for(long y = 0; y < ny; y++)
        {
            counter++;
            if(counter>=maxUnions)
            {
                if(x!=0 && y!=0)
                    if(!q.exec(strQuery)) DM::PrintSqlError(&q);

                strQuery = CreateQuery(x,y);
                counter = 0;
            }
            else
            {
                strQuery += "UNION SELECT 0,"+QString::number(x)+
                        ","+QString::number(y)+",0\n";
            }
        }
    }
    if(!q.exec(strQuery)) DM::PrintSqlError(&q);
}

double GetElapsedTime(QElapsedTimer *timer)
{
    return (double) timer->nsecsElapsed()*1e-6;
}
*/
/*
TEST_F(TestSimulation,BulkInsert)
{
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Write Test";

    long nx = 1;
    long ny = 1;
    int size = sizeof(int)+2*sizeof(long)+sizeof(double);

    QElapsedTimer timer;
    timer.start();
    //long t =0;

    DM::DBConnector::getInstance();
    QSqlQuery q;

    int iterations = 7;

    for(int k=0;k<18;k++)
    {
        qint64 rate[20];
        memset(rate,0,sizeof(qint64)*20);
        int p=0;
        for(int i=0;i<iterations;i++)
        {
            p=0;
            double absSize = nx*ny*size;

            if(!q.exec("CREATE TABLE rasterfields(datalink int NOT NULL, x bigint, y bigint, \
                        value DOUBLE PRECISION, PRIMARY KEY (datalink,x,y))"))
                DM::PrintSqlError(&q);

            timer.restart();
            SeperateInsert(nx,ny);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            timer.restart();
            SeperateInsertWrapper(nx,ny);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            timer.restart();
            SeperateInsertParamWrapper(nx,ny);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            timer.restart();
            TransactionInsert(nx,ny);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            timer.restart();
            UnionInsert(nx,ny,127);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            timer.restart();
            if(!q.exec("BEGIN TRANSACTION")) DM::PrintSqlError(&q);
            UnionInsert(nx,ny,127);
            if(!q.exec("END TRANSACTION")) DM::PrintSqlError(&q);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            if(!q.exec("DROP TABLE IF EXISTS rasterfields"))
                DM::PrintSqlError(&q);
            if(!q.exec("CREATE TABLE rasterfields(datalink int NOT NULL, x bigint, data BLOB, PRIMARY KEY (datalink,x))"))
                DM::PrintSqlError(&q);

            timer.restart();
            BLOBInsert(nx,ny);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            if(!q.exec("DROP TABLE IF EXISTS rasterfields"))
                DM::PrintSqlError(&q);
            if(!q.exec("CREATE TABLE rasterfields(datalink int NOT NULL, x bigint, data BLOB)"))
                DM::PrintSqlError(&q);

            timer.restart();
            BLOBInsert(nx,ny);
            rate[p++] += absSize/GetElapsedTime(&timer);
            if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);

            if(!q.exec("DROP TABLE IF EXISTS rasterfields"))
                DM::PrintSqlError(&q);

            timer.restart();
            double *data = new double[nx*ny];
            for(long x = 0; x < nx; x++)
                for(long y = 0; y < ny; y++)
                    data[x+nx*y] = 0.0;

            rate[p++] += absSize/GetElapsedTime(&timer);
            delete data;
        }

        QString str = "\t" + QString::number(nx*ny);
        for(int i=0;i<p;i++)
            str += "\t" + QString::number((long)(rate[i]/(double)iterations));
        DM::Logger(DM::Standard) << str;
        ny *= 2;
    }

    if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);
}
/*
TEST_F(TestSimulation,UnionInsert)
{
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Write Test";

    long nx = 1;
    long ny = 1;
    int size = sizeof(int)+2*sizeof(long)+sizeof(double);

    QElapsedTimer timer;
    timer.start();

    DM::DBConnector::getInstance();
    QSqlQuery q;

    int iterations = 10;

    for(int k=0;k<17;k++)
    {
        double absSize = (nx*ny)*size;
        long blockSize = 1;
        QString str = "";
        for(int j=0;j<6;j++)
        {
            double t = 0;

            for(int i=0;i<iterations;i++)
            {
                if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);
                timer.restart();
                UnionInsert(nx,ny,blockSize);

                t += GetElapsedTime(&timer);
            }
            long rate = absSize / t * iterations;

            str += "\t" + QString::number(rate);
            blockSize *= 4;
            if(blockSize>500)
                blockSize = 500;
        }

        DM::Logger(DM::Standard) << "\t" << (nx*ny) << str;
        ny *= 2;
    }

    if(!q.exec("DELETE FROM rasterfields WHERE datalink=0")) DM::PrintSqlError(&q);
}
/**/
/*
TEST_F(TestSimulation,validationtool) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test validation tool";

    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * mcreator = sim.addModule("CreateAllComponenets");
    //std::string uuid = mcreator->getUuid();
    ASSERT_TRUE(mcreator != 0);
    DM::Module * mcheck  = sim.addModule("CheckAllComponenets");
    ASSERT_TRUE(mcheck != 0);
    DM::ModuleLink * l2 = sim.addLink(mcreator->getOutPort("sys"), mcheck->getInPort("sys"));
    ASSERT_TRUE(l2 != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}*/

#ifdef SQLUNITTESTS

TEST_F(TestSimulation,cachetest) {
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
    ASSERT_TRUE(c.get(2)==NULL);
    ASSERT_TRUE(c.get(3)==three);
    ASSERT_TRUE(c.get(4)==four);
    ASSERT_TRUE(c.get(10)==NULL);
	/*
	int size = 4000;
	Cache<int,double> cache(size);
	for(int i=0;i<size;i++)
	{
		double* d = new double;
		*d = (double)i;
		cache.add(i,d);
	}
	
	QElapsedTimer time;
	time.start();
	for(int j=0;j<10;j++)
		for(int i=0;i<size;i++)
			cache.get(i);
	
	DM::Logger(Error) << "elapsed time for searching " << GetElapsedTime(&time);
	*/
	/*
	int size = 20000;
	//for(int i=0;i<size;i++)
	//	buf[i] = (double)rand();

	QElapsedTimer time;
	time.start();

	for(int i=0;i<10;i++)
	{
		double *buf = new double[size];
		QByteArray qba((char*)buf, size*sizeof(double));
	}

	DM::Logger(Error) << "elapsed time for 10x1 buffers: " << GetElapsedTime(&time);
	time.restart();
	
	double *buf = new double[size*10];
	double* p = buf;
	for(int i=0;i<10;i++)
	{
		QByteArray::fromRawData((char*)p,size*sizeof(double));
		p += size;
	}
	
	DM::Logger(Error) << "elapsed time for 1x10 buffers: " << GetElapsedTime(&time);
	delete buf;*/
}
/*
TEST_F(TestSimulation,simplesqltest) {
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
    ASSERT_TRUE(sim.addLink(mcreator, "sys", mallocator, "sys"));
    ASSERT_TRUE(sim.addLink(mallocator, "sys", mcheck, "sys"));
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

    DBConnector::getInstance()->Synchronize();
    // print cache statistics
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}
*/
/* DOES NOT WORK ANYMORE (VALIDATION TOOL ON SUCCESSORSTATE)
TEST_F(TestSimulation,sqlsuccessortest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Successor states (SQL)";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * mcreator = sim.addModule("CreateAllComponenets");
    ASSERT_TRUE(mcreator != 0);
    DM::Module * msucc  = sim.addModule("SuccessorCheck");
    ASSERT_TRUE(msucc != 0);
    DM::ModuleLink * l1 = sim.addLink(mcreator->getOutPort("sys"), msucc->getInPort("sys"));
    ASSERT_TRUE(l1 != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}*/

TEST_F(TestSimulation,sqlsuccessortest)
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

TEST_F(TestSimulation, SqlNodeTest)
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
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}

TEST_F(TestSimulation, SqlEdgeTest)
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
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}

TEST_F(TestSimulation, SqlFaceOrder)
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
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}
TEST_F(TestSimulation, SQLRasterdata)
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
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}

TEST_F(TestSimulation, SQLattributes)
{
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test attributes (SQL)";


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
    DM::LinkAttribute link0 = {"0","a"};
    DM::LinkAttribute link1 = {"1","b"};
    DM::LinkAttribute link2 = {"2","c"};

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
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}
TEST_F(TestSimulation, System)
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

TEST_F(TestSimulation,sqlprofiling) {
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

    delete a0;
    DM::DBConnector::getInstance()->Synchronize();
    DM::Logger(DM::Standard) << "copy " << n << " attributes " << (long)timer.elapsed();
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
    DM::Logger(DM::Standard) << "attache and copy " << n << "  nodes " << (long)timer.elapsed();

    delete baseNode;
    delete sys;
    DM::DBConnector::getInstance()->Synchronize();
    DM::Logger(DM::Standard) << "delete " << n << "  nodes with system " << (long)timer.elapsed();

    // print cache statistics
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}
/**/

TEST_F(TestSimulation,sqlRasterDataProfiling) {
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
    delete raster;
    DM::DBConnector::getInstance()->Synchronize();
    DM::Logger(DM::Standard) << "delete rasterdata(" << n << "x" << n << ") " << (long)timer.elapsed();

    DBConnector::getInstance()->Synchronize();
    // print cache statistics
    DM::Node::PrintStatistics();
    DM::Attribute::PrintStatistics();
    //DM::RasterData::PrintStatistics();
}

#endif
#ifdef STDUNITTESTS

TEST_F(TestSimulation,testMemory){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("CreateNodes");
    //std::string m_uuid = m->getUuid();
    ASSERT_TRUE(m != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    for (int i = 0; i < 5; i++) {
        sim.removeModule(m);
		DM::Logger(DM::Standard) << "#" << i;
        m = sim.addModule("CreateNodes");
        sim.run();
    }
}

TEST_F(TestSimulation,addModuleToSimulationTest){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
}

TEST_F(TestSimulation,loadModuleNativeTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Load Native Module";
    DM::Simulation sim;
    ASSERT_TRUE(sim.registerNativeModules("dynamind-testmodules") == true);
}

TEST_F(TestSimulation,repeatedRunTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Repeatet Run";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    for (long i = 0; i < 10; i++) {
        sim.run();
        ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    }

}

TEST_F(TestSimulation,linkedModulesTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");

    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");

    ASSERT_TRUE(inout != 0);

    ASSERT_TRUE(sim.addLink(m,"Sewer", inout,"Inport"));
    for (long i = 0; i < 10; i++){
        sim.run();
        ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    }
}

TEST_F(TestSimulation,linkedDynamicModules) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    ASSERT_TRUE(inout != 0);
    ASSERT_TRUE(sim.addLink(m,"Sewer", inout, "Inport"));
	DynamicInOut * dyinout  = (DynamicInOut*)sim.addModule("DynamicInOut");
    ASSERT_TRUE(dyinout != 0);
    dyinout->addAttribute("D");
    ASSERT_TRUE(sim.addLink(inout, "Inport", dyinout, "Inport"));
    DM::Module * inout2  = sim.addModule("InOut2");
    ASSERT_TRUE(inout2 != 0);
    ASSERT_TRUE(sim.addLink(dyinout,"Inport", inout2, "Inport"));
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

TEST_F(TestSimulation,branchTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test branching";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");

    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    ASSERT_TRUE(inout != 0);
    DM::Module * inout2  = sim.addModule("InOut");
    ASSERT_TRUE(inout2 != 0);
	
    ASSERT_TRUE(sim.addLink(m,"Sewer", inout, "Inport"));
    ASSERT_TRUE(sim.addLink(m,"Sewer", inout2, "Inport"));

    sim.run();

    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

TEST_F(TestSimulation,loopTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Standard) << "Test loops";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");

    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    ASSERT_TRUE(inout != 0);
    DM::Module * loop  = sim.addModule("Loop");


    ASSERT_TRUE(sim.addLink(m,"Sewer", inout, "Inport"));
	
	ASSERT_TRUE(sim.addLink(inout,"Inport", loop, "in"));
	ASSERT_TRUE(sim.addLink(loop,"true", inout, "Inport"));

    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

/*
TEST_F(TestSimulation,linkedDynamicModulesOverGroups) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    ASSERT_TRUE(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    ASSERT_TRUE(l != 0);
    //Here comes the group
    GroupTest * g = (GroupTest * ) sim.addModule("GroupTest");
    g->addInPort("In");
    DM::ModuleLink * l_in = sim.addLink(inout->getOutPort("Inport"),g->getInPortTuple("In")->getInPort());
    ASSERT_TRUE(l_in != 0);
    g->addOutPort("Out");
    DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut");
    ASSERT_TRUE(dyinout != 0);
    dyinout->setGroup(g);
    ASSERT_TRUE(dyinout != 0);
    dyinout->addAttribute("D");
    DM::ModuleLink * l1 = sim.addLink(g->getInPortTuple("In")->getOutPort(), dyinout->getInPort("Inport"));
    ASSERT_TRUE(l1 != 0);
    DM::ModuleLink * l_out = sim.addLink(dyinout->getOutPort("Inport"), g->getOutPortTuple("Out")->getInPort());
    ASSERT_TRUE(l_out != 0);
    DM::Module * inout2  = sim.addModule("InOut2");
    ASSERT_TRUE(inout2 != 0);
    DM::ModuleLink * l2 = sim.addLink(g->getOutPortTuple("Out")->getOutPort(), inout2->getInPort("Inport"));
    ASSERT_TRUE(l2 != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}
*/
#endif
/*
#ifndef PYTHON_EMBEDDING_DISABLED
    TEST_F(TestSimulation,loadPythonModule) {
        ostream *out = &cout;
        DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
        DM::Logger(DM::Standard) << "Add Module";
        DM::Simulation sim;
        sim.registerPythonModules("PythonModules/scripts/");
        DM::Module * m = sim.addModule("PythonTestModule");
        ASSERT_TRUE(m != 0);
    }

    TEST_F(TestSimulation,runSinglePythonModule) {
        ostream *out = &cout;
        DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
        DM::Logger(DM::Standard) << "Add Module";
        DM::Simulation sim;
        sim.registerPythonModules("PythonModules/scripts/");
        DM::Module * m = sim.addModule("PythonTestModule");
        ASSERT_TRUE(m != 0);
        sim.run();
        ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    }

    TEST_F(TestSimulation,runRepeatedSinglePythonModule) {
            ostream *out = &cout;
            DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
            DM::Logger(DM::Standard) << "Add Module";
            DM::Simulation sim;
            sim.registerPythonModules("PythonModules/scripts/");
            DM::Module * m = sim.addModule("PythonTestModule");
            ASSERT_TRUE(m != 0);
            for (int i = 0; i < 100; i++)
                sim.run();
            ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

        }
#endif
/**/
}
