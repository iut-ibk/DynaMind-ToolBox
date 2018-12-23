#ifndef MONTHLYEVO_H
#define MONTHLYEVO_H
#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(MonthlyEvo)

public:
	MonthlyEvo();


	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	Flow out;

};

#endif // MONTHLYEVO_H
