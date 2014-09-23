#ifndef GUIFILTER_H
#define GUIFILTER_H

#include <QDialog>

namespace DM {
class Module;
}
namespace Ui {
class GUIFilter;
}

class GUIFilter : public QDialog
{
	Q_OBJECT
public:
	explicit GUIFilter(DM::Module *m, QWidget *parent = 0);
	~GUIFilter();

private:
	Ui::GUIFilter *ui;
	DM::Module *m;

public slots:
	void accept();
};

#endif // GUIFILTER_H
