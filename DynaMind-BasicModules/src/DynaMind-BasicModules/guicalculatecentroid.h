#ifndef GUICALCULATECENTROID_H
#define GUICALCULATECENTROID_H
#include <dmcompilersettings.h>
#include <QDialog>

namespace DM{
	class Module;
}
class CalculateCentroid;

namespace Ui {
class GUICalculateCentroid;
}

class DM_HELPER_DLL_EXPORT GUICalculateCentroid : public QDialog
{
	Q_OBJECT

public:
	explicit GUICalculateCentroid(DM::Module * m, QWidget *parent = 0);

	~GUICalculateCentroid();

private:
	Ui::GUICalculateCentroid *ui;
	CalculateCentroid * m;
public slots:
	void accept();
};

#endif // GUICALCULATECENTROID_H
