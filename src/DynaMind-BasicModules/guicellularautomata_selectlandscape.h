#ifndef GUICELLULARAUTOMATA_SELECTLANDSCAPE_H
#define GUICELLULARAUTOMATA_SELECTLANDSCAPE_H

#include <QDialog>
#include <QStringList>
namespace Ui {
class GUICellularAutomata_SelectLandscape;
}

class GUICellularAutomata_SelectLandscape : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUICellularAutomata_SelectLandscape(QStringList landscapes, QWidget *parent = 0);
    ~GUICellularAutomata_SelectLandscape();
    
private:
    Ui::GUICellularAutomata_SelectLandscape *ui;

signals:
    void selected(QString s);

public slots:
    void accept();

};

#endif // GUICELLULARAUTOMATA_SELECTLANDSCAPE_H
