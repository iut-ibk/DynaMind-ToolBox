#ifndef UPDATEALLCOMPONENTS_H
#define UPDATEALLCOMPONENTS_H


#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>


class DM_HELPER_DLL_EXPORT UpdateAllComponents : public DM::Module
{
	DM_DECLARE_NODE(UpdateAllComponents)
public:
	UpdateAllComponents();
	void run();
	void init();
	int elements;
private:
	DM::ViewContainer components;
};


#endif // UPDATEALLCOMPONENTS_H
