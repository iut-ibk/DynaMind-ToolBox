#ifndef GUIWELCOME_H
#define GUIWELCOME_H

#include <QWidget>

namespace Ui {
class GUIWelcome;
}

class GUIWelcome : public QWidget
{
	Q_OBJECT

public:
	explicit GUIWelcome(QWidget *parent = 0);
	~GUIWelcome();

private:
	Ui::GUIWelcome *ui;
};

#endif // GUIWELCOME_H
