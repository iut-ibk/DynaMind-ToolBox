#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <dmview.h>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

namespace DM {
class DM_HELPER_DLL_EXPORT ISystem {
public:
	virtual ~ISystem() {}
	virtual void _moveToDb(){}
	virtual void _importViewElementsFromDB(){}
	virtual ISystem * createSuccessor() = 0;
	virtual void updateViews(const std::vector<View>& views) = 0;
	virtual ISystem *getPredecessor() const = 0;
	virtual std::vector<Component*> getAllComponentsInView(const DM::View &view) = 0;
};
}

#endif // ISYSTEM_H


