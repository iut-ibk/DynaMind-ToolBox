#ifndef DM_CALCULATEDISTANCE_H
#define DM_CALCULATEDISTANCE_H

#include <dmmodule.h>
#include <dm.h>


class DM_HELPER_DLL_EXPORT DM_CalculateDistance : public DM::Module
{
	DM_DECLARE_NODE(DM_CalculateDistance)
public:
	DM_CalculateDistance();

	void init();
	void run();
private:
	bool properInit; // is true when init succeeded

	std::string lead_view_name;
	std::string distance_view_name;
	std::string attribute_name;

	DM::ViewContainer lead_view;
	DM::ViewContainer distance_view;

};

#endif // DM_CALCULATEDISTANCE_H
