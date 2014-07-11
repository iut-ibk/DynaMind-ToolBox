#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <dmview.h>

namespace DM {
class ISystem {
public:
	virtual ~ISystem() {}
	virtual void _moveToDb(){}
	virtual void _importViewElementsFromDB(){};
	virtual ISystem * createSuccessor() = 0;
	virtual void updateViews(const std::vector<View>& views) = 0;
	virtual ISystem *getPredecessor() const = 0;
};
}

#endif // ISYSTEM_H


