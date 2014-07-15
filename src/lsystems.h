#ifndef LSYSTEMS_H
#define LSYSTEMS_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>
#include <ogrsf_frmts.h>

class Extend {
private:
	OGRPoint start;
public:
	Extend(OGRPoint pt);
	void run();

};

class DM_HELPER_DLL_EXPORT LSystems: public DM::Module
{
	DM_DECLARE_NODE(LSystems)

private:
	DM::ViewContainer view;
public:
	LSystems();
	void run();
};

#endif // LSYSTEMS_H
