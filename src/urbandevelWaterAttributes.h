#ifndef urbandevelWaterAttributes_H
#define urbandevelWaterAttributes_H

#include <dm.h>
#include <dmcompilersettings.h>

class urbandevelWaterAttributes : public DM::Module
{
    DM_DECLARE_NODE(urbandevelWaterAttributes);

private:
    DM::View city;
    DM::View buildingview;

    bool paramfromCity;
    double dwfpp;
    double wsdpp;

public:
    urbandevelWaterAttributes();
    void run();
    void init();
    std::string getHelpUrl();

};

#endif // CREATESINGLEFAMILYHOUSES_H
