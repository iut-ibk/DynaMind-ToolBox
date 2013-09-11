#ifndef READCSV_H
#define READCSV_H
#include <dmmodule.h>
#include <dm.h>
#include <QHash>
#include <QString>

using namespace DM;
class readCSV : public Module
{
    DM_DECLARE_NODE(readCSV);

private:
    std::string filestring;
    DM::View data;

public:
    readCSV();
    void run();

};
#endif
