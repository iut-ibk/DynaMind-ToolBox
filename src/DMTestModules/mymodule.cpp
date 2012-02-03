#include "mymodule.h"

DM_DECLARE_NODE_NAME( MyModule,Modules )

MyModule::MyModule() {

   /* //Add Parameter to Module
    this->addParameter("Parameter1", VIBe2::INT, &this->MyParameter1);
    this->addParameter("Parameter2", VIBe2::STRING, &this->MyParameter2);

    //View Definitions
    DM::View inlets = DM::View("Inlets");
    inlets.addComponent(DM::NODE);
    inlets.addAttributes("A");
    inlets.addAttributes("B");
    DM::View conduits = DM::View("Conduits");
    conduits.addComponent(DM::EDGE);

    DM::View SomeRandomInformation = DM::View("SomeRandomInformation");
    SomeRandomInformation.addComponent(DM::EDGE);

    std::vector<DM::View> views;

    views.push_back(inlets);
    views.push_back(conduits);
    views.push_back(SomeRandomInformation);

    value = 10;




    this->addParameter("Value", VIBe2::DOUBLE, &value);

    ////////Datasets
    this->addData("Sewer",views);*/
}



void MyModule::run() {
    /*outputData = this->getData("Sewer");
    DM::Node * n1 = outputData->addNode(0,0,0, "Inlets");
    DM::Node * n2 = outputData->addNode(0,0,1, "Inlets");

    outputData->addEdge(n1, n2, "Conduits");

    outputData->addEdge(n1, n2, "SomeRandomInformation");

    Logger(Debug) << "Run Testmodule";*/



}


