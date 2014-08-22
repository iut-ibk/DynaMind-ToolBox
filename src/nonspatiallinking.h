#ifndef NONSPATIALLINKING_H
#define NONSPATIALLINKING_H


#include <dmmodule.h>
#include <dm.h>


class DM_HELPER_DLL_EXPORT NonSpatialLinking : public DM::Module
{
	DM_DECLARE_NODE(NonSpatialLinking)
private:
	std::string leadingViewName;
	std::string leadingattributeName;
	std::string joinViewName;
	std::string joinAttributeName;

	DM::ViewContainer leadingView;
	DM::ViewContainer joinView;

	bool fullJoin;

	std::vector<std::string> attribute_names;

public:
	NonSpatialLinking();
	void run();
	void init();
};

#endif // NONSPATIALLINKING_H
