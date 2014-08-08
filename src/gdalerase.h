#ifndef GDALERASE_H
#define GDALERASE_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALErase : public DM::Module
{
	DM_DECLARE_NODE(GDALErase)
public:
	GDALErase();


	void run();
	void init();

private:
	std::string leadingViewName;
	std::string eraseViewName;

	DM::ViewContainer leadingView;
	DM::ViewContainer eraseView;

};

#endif // GDALERASE_H
