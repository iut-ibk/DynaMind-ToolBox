#ifndef GUICLEARVIEW_H
#define GUICLEARVIEW_H

#include <dmcompilersettings.h>

#include <QDialog>

namespace DM{
	class Module;
}

class ClearView;

namespace Ui {
class GUIClearView;
}

class DM_HELPER_DLL_EXPORT GUIClearView : public QDialog
{
	Q_OBJECT

public:
	explicit GUIClearView(DM::Module * m, QWidget *parent = 0);
	~GUIClearView();
	ClearView * m;

private:
	Ui::GUIClearView *ui;

public slots:
	void accept();
};

#endif // GUICLEARVIEW_H
