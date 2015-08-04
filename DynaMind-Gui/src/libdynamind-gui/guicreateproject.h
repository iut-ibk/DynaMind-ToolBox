#ifndef GUICREATEPROJECT_H
#define GUICREATEPROJECT_H

#include <dmcompilersettings.h>
#include <QDialog>

namespace Ui {
class GUICreateProject;
}


class GUISimulation;

class DM_HELPER_DLL_EXPORT GUICreateProject : public QDialog
{
	Q_OBJECT

public:
	explicit GUICreateProject(GUISimulation *sim, QWidget *parent = 0);
	~GUICreateProject();

public slots:
	void accept();
	void reject();
private:
	Ui::GUICreateProject *ui;
	GUISimulation * sim;
};

#endif // GUICREATEPROJECT_H
