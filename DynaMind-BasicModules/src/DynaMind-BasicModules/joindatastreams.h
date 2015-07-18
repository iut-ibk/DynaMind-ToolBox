#ifndef JOINDATASTREAMS_H
#define JOINDATASTREAMS_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT JoinDatastreams : public DM::Module
{
	DM_DECLARE_NODE(JoinDatastreams)

public:
	std::vector<std::string> Inports;
private:
	std::vector<DM::System*> Systems_Inport;
	std::vector<std::string> existingViews;
	std::vector<DM::View> views;
	unsigned int sizeold;

public:
	JoinDatastreams();
	void init();
	void run();
	bool createInputDialog();
	void addSystem(std::string sys);
	std::string getHelpUrl();
};

#endif // JOINDATASTREAMS_H
