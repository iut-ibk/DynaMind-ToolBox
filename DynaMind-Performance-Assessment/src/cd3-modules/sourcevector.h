#ifndef SOURCEVECTOR_H
#define SOURCEVECTOR_H
#include <node.h>
#include <flow.h>

class SourceVector;

CD3_DECLARE_NODE(SourceVector)
public:
	SourceVector();
	virtual ~SourceVector();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	int counter;
	Flow out;
	std::vector<double> source;
};
#endif // SOURCEVECTOR_H
