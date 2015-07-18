#ifndef GUIEXPORTRASTERDATA_H
#define GUIEXPORTRASTERDATA_H

#include <dmcompilersettings.h>

#include <QDialog>

namespace DM{
	class Module;
}

namespace Ui {
class GUIExportRasterData;
}

class ExportRasterData;

class DM_HELPER_DLL_EXPORT GUIExportRasterData : public QDialog
{
	Q_OBJECT

public:
	explicit GUIExportRasterData(DM::Module * m, QWidget *parent = 0);
	~GUIExportRasterData();

private:
	Ui::GUIExportRasterData *ui;
	ExportRasterData * m;

public slots:
	void accept();
};

#endif // GUIEXPORTRASTERDATA_H
