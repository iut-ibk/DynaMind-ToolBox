#include "inoutmodule.h"



VIBe_DECLARE_NODE_NAME( InOut,Modules )
InOut::InOut() {

    Logger(Debug) << "Create InOut";
    this->addParameter("Inport", VIBe2::VECTORDATA_IN, & this->vec_in);
    this->addParameter("Outport", VIBe2::VECTORDATA_OUT, & this->vec_out);

}



void InOut::run() {
    Logger(Debug) << "Run InOut";

    std::vector<std::string> names = vec_in->getPointsNames();

    foreach (std::string name, names) {
        Logger(Debug) << name;
    }



}

InOut::~InOut() {
    Logger(Debug) << "Destructor InOut";
}
