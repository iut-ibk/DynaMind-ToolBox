#ifndef MULTIPLYER_H
#define MULTIPLYER_H
#include <node.h>
#include <flow.h>

class Node;

CD3_DECLARE_NODE(Multiplyer)
public:
	Multiplyer();
	virtual ~Multiplyer();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	Flow in, out1, out2;
	double multiplyer;
};
#endif // MULTIPLYER_H
