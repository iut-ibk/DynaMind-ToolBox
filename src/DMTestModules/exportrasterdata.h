#ifndef EXPORTRASTERDATA_H
#define EXPORTRASTERDATA_H

#include <dmmodule.h>
#include <dm.h>
/**
 * @ingroup Modules
 * @brief Exports Rasterdata to a file
 */

class DM_HELPER_DLL_EXPORT ExportRasterData : public DM::Module
{
    DM_DECLARE_NODE(ExportRasterData)
    private:
        DM::System * sys_in;
    std::string NameOfExistingView;

    std::vector<DM::View> data;
    std::string FileName;
    bool flip_h;
public:
    ExportRasterData();
    void run();
    void init();

    bool createInputDialog();
    DM::System * getSystemIn();
    void addView(DM::View view);

};

#endif // EXPORTRASTERDATA_H
