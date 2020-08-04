#ifndef DMSELECTOR_H
#define DMSELECTOR_H

#include <dmmodule.h>
#include <dm.h>



class sqlite3;
class DM_HELPER_DLL_EXPORT DMSelector : public DM::Module
{
	DM_DECLARE_NODE(DMSelector)
public:
	DMSelector();
	void init();
	void run();

private:
	bool properInit; // is true when init succeeded
	std::string leadingViewName;
	std::string linkViewName;
	std::string attributeName;

	bool withCentroid;

	DM::ViewContainer leadingView;
	DM::ViewContainer linkView;

	std::string link_name;

	void execute_query1(sqlite3 *db, const char *sql);
	std::string get_filter(sqlite3 *db);
};


#endif // DMSELECTOR_H
