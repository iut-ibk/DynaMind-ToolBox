#ifndef GUIExport_H
#define GUIExport_H

#include <dmcompilersettings.h>
#include <QDialog>
#include <QTreeWidget>
#include <QSignalMapper>

namespace DM {
class Module;
}
namespace Ui {
class GUIExport;
}

class Export;

class DM_HELPER_DLL_EXPORT GUIExport : public QDialog
{
	Q_OBJECT

public:
	explicit GUIExport(DM::Module * m, QWidget *parent = 0);
	~GUIExport();

	void updateTree();
	void updateViewConfig();

private:
	Ui::GUIExport *ui;
	Export * m;
	QSignalMapper* treeCheckMapper;

public slots:
	void accept();
	void on_pushButton_Filename_clicked();
	//void on_wfsUpdateButton_clicked();
	void on_viewTree_itemDoubleClicked(QTreeWidgetItem * item, int column);
	void updateTreeChecks(QObject* obj);
	void on_selectAllButton_clicked();
	void on_selectNothingButton_clicked();
};

#endif // GUIExport_H
