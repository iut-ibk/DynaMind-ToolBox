#ifndef GUISQLCALCULATOR_H
#define GUISQLCALCULATOR_H

#include <QDialog>
#include <dmmodule.h>

namespace Ui {
class GUISQLCalculator;
}

class CustomTextEdit;

class GUISQLCalculator : public QDialog
{
	Q_OBJECT

public:
	explicit GUISQLCalculator(DM::Module *m, QWidget *parent = 0);
	~GUISQLCalculator();
private:
	Ui::GUISQLCalculator *ui;
	DM::Module * m;
     CustomTextEdit * ct;

protected slots:
	void accept();
};

#endif // GUISQLCALCULATOR_H
