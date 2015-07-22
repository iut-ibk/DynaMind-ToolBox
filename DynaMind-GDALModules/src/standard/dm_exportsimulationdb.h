#ifndef DM_EXPORTSIMULATIONDB_H
#define DM_EXPORTSIMULATIONDB_H

#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>

class DM_ExportSimulationDB: public DM::Module
{
		DM_DECLARE_NODE(DM_ExportSimulationDB)
public:
	DM_ExportSimulationDB();
	void run();
	std::string getHelpUrl();
private:
	DM::ViewContainer dummy;
	int steps;
	int internal_counter;
	std::string file_name;
	bool overwrite;
};

#endif // DM_EXPORTSIMULATIONDB_H
