#ifndef GUIWELCOME_H
#define GUIWELCOME_H

#include <QWidget>
#include <guisimulation.h>

namespace Ui {
class GUIWelcome;
}

class GUIWelcome : public QWidget
{
	Q_OBJECT

public:
	explicit GUIWelcome(GUISimulation * sim, QWidget *parent = 0);
	~GUIWelcome();

private:
	Ui::GUIWelcome *ui;
	GUISimulation * sim;

public slots:
	void on_pushButton_new_project_clicked();
	void on_pushButton_load_project_clicked();
};

#endif // GUIWELCOME_H
