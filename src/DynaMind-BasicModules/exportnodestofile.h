#ifndef EXPORTNODESTOFILE_H
#define EXPORTNODESTOFILE_H

#include <dmmodule.h>
#include <dm.h>
class DM_HELPER_DLL_EXPORT ExportNodesToFile : public DM::Module
{
    DM_DECLARE_NODE(ExportNodesToFile)
private:
    std::string FileName;
    std::string ViewName;
public:


    ExportNodesToFile();
    void run();
    std::string getHelpUrl();
};

#endif // EXPORTNODESTOFILE_H
