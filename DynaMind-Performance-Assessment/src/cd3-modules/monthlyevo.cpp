#include "monthlyevo.h"

CD3_DECLARE_NODE_NAME(MonthlyEvo)

MonthlyEvo::MonthlyEvo()
{


	addOutPort(ADD_PARAMETERS(out));

}


int MonthlyEvo::f(ptime time, int dt) {

	std::vector<double> monthly_evo;


	monthly_evo.push_back(32.3/31./(60*60*24));
	monthly_evo.push_back(32.8/28./(60*60*24));
	monthly_evo.push_back(65.6/31./(60*60*24));
	monthly_evo.push_back(85.8/30./(60*60*24));
	monthly_evo.push_back(106.7/31./(60*60*24));
	monthly_evo.push_back(69.0/30./(60*60*24));
	monthly_evo.push_back(107.5/31./(60*60*24));
	monthly_evo.push_back(113.0/31./(60*60*24));
	monthly_evo.push_back(106.0/30./(60*60*24));
	monthly_evo.push_back(75.3/31./(60*60*24));
	monthly_evo.push_back(47.3/30./(60*60*24));
	monthly_evo.push_back(43.7/31./(60*60*24));

	int month = time.date().month();
	out[0] = monthly_evo[month-1] * dt;

	return dt;
}

bool MonthlyEvo::init(ptime start, ptime end, int dt) {
	return true;
}
