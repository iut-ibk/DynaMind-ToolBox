#ifndef DM_MICROCLIMATE_H
#define DM_MICROCLIMATE_H

#include <dmmodule.h>
#include <dm.h>



class DM_MicroClimate : public DM::Module
{
	DM_DECLARE_NODE(DM_MicroClimate)
public:
	DM_MicroClimate();
	void run();
private:
	DM::ViewContainer grid;
	double getTempForSurface(int surface, int percentile);
	int percentile;
};

#endif // DM_MICROCLIMATE_H
