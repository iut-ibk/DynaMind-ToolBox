#ifndef GUIImport_H
#define GUIImport_H

#include <dmcompilersettings.h>
#include <QDialog>

namespace DM {
class Module;
}
namespace Ui {
class GUIImport;
}

class Import;

class DM_HELPER_DLL_EXPORT GUIImport : public QDialog
{
	Q_OBJECT

public:
	explicit GUIImport(DM::Module * m, QWidget *parent = 0);
	~GUIImport();

private:
	Ui::GUIImport *ui;
	Import * m;


public slots:
	void accept();
	void on_pushButton_wfs_pick_clicked();
	void on_pushButton_Filename_clicked();
};

#endif // GUIImport_H
