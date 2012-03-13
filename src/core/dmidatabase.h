#ifndef IDATABASE_H
#define IDATABASE_H


#include "dmcompilersettings.h"
#include <iostream>
#include <dmrasterdata.h>
//#include <vectordata.h>
#include <map>
#include <dmdataobserver.h>

namespace DM {
class Module;

class DM_HELPER_DLL_EXPORT  IDataBase
{


public:

    virtual void setRasterData(std::string UUID, std::string Name,  RasterData & r) = 0;
    virtual RasterData & getRasterData(std::string UUID, std::string Name, bool read = true, bool fromBack = false) = 0;
    virtual RasterData & createRasterData(std::string UUID, std::string Name) = 0;

    //virtual void setVectorData(std::string UUID, std::string Name, VectorData & v) = 0;
    /*virtual VectorData & getVectorData(std::string UUID, std::string Name, bool read = true, bool fromBack = false) = 0;
    virtual VectorData & createVectorData(std::string UUID, std::string Name) = 0;
    virtual VectorData & getVectorData(std::string Name) = 0;*/
    virtual void setDoubleData(std::string UUID, std::string Name, double v) = 0;
    virtual double getDoubleData(std::string UUID, std::string Name, bool read = true, bool fromBack = false) = 0;
    virtual  void createDoubleData(std::string UUID, std::string Name) = 0;

    virtual  void resetDataBase() = 0;
    virtual  void registerDataObserver(DataObserver * ob)  = 0;

    virtual  std::vector<std::string> getRegisteredDatasets() const = 0;


    virtual  std::vector<std::string> getRegisteredDatasetForModule(std::string) const = 0;

};
}


#endif // IDATABASE_H
