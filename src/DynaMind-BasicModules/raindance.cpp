#include "raindance.h"
#include <QDateTime>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

DM_DECLARE_NODE_NAME(RainDance, Modules)

RainDance::RainDance()
{
    this->ViewRainAdd = "";
    this->length = 0;
    this->addParameter("View", DM::STRING, &this->ViewRainAdd);
    this->addParameter("length", DM::INT, &this->length);

    sys_in = 0;
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );


    this->addData("Data", data);
}

void RainDance::init() {
    sys_in = this->getData("Data");

    if (!sys_in)
        return;
    if (ViewRainAdd.empty())
        return;

    DM::View * v = sys_in->getViewDefinition(this->ViewRainAdd);
    if (!v)
        return;

    v_rain = DM::View(v->getName(), v->getType(), READ);
    v_rain.addAttribute("RainSeries");
    v_rain.setAttributeType("RainSeries", ATTR::TIMESERIES);

    std::vector<DM::View> data;
    data.push_back(v_rain);
    this->addData("Data", data);
}

void RainDance::run() {
    sys_in = this->getData("Data");

    QDateTime tstart = QDateTime::currentDateTime();
    srand ( time(NULL) );
    std::vector<std::string> uuids = sys_in->getUUIDsOfComponentsInView(v_rain);
    foreach (std::string  uuid, uuids) {
        Component * cmp = sys_in->getComponent(uuid);
        std::vector<std::string> time;
        std::vector<double> vals;
        int r_val =  rand() % 10 + 1;
        for (long i = 0; i < length; i++) {
            time.push_back(tstart.addSecs(i*60).toString().toStdString());
            vals.push_back(i*r_val);
        }

        Attribute attr("RainSeries");
        attr.addTimeSeries(time, vals);
        cmp->addAttribute(attr);

    }
    int j;


}
