#ifndef DYNAMINDMAINWINDOW_H
#define DYNAMINDMAINWINDOW_H
#include <QWidget>
#include "dmcompilersettings.h"

class DMMainWindow;

class DM_HELPER_DLL_EXPORT DynaMindMainWindow
{
private:
	DMMainWindow * mw;
public:
	DynaMindMainWindow(QStringList args);
	void createMainWindow();
	void registerNativeDll(std::string fileName);
};

#endif // DYNAMINDMAINWINDOW_H
