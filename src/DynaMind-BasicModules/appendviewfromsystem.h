#ifndef APPENDVIEWFROMSYSTEM_H
#define APPENDVIEWFROMSYSTEM_H

#include <dmmodule.h>
#include <dm.h>
/**
 * @ingroup Modules
 * @brief The AppendViewFromSystem class
 * @TODO: This module is a dirty hack at the moment!
 */

class DM_HELPER_DLL_EXPORT AppendViewFromSystem : public DM::Module
{
	DM_DECLARE_NODE(AppendViewFromSystem)

public:
	std::vector<std::string> Inports;

private:
	std::vector<DM::System*> Systems_Inport;
	std::vector<std::string> existingViews;
	std::vector<DM::View> views;
	std::string mainView;
	unsigned int sizeold;

public:
    AppendViewFromSystem();
    void init();
    void run();
    bool createInputDialog();
    void addSystem(std::string sys);
    std::string getHelpUrl();

};

#endif // APPENDVIEWFROMSYSTEM_H
