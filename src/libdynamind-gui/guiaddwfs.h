#ifndef GUIADDWFS_H
#define GUIADDWFS_H

#include <QDialog>

namespace Ui {
class GUIAddWFS;
}

class GUIAddWFS : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIAddWFS(QWidget *parent = 0);
    ~GUIAddWFS();
    
private:
    Ui::GUIAddWFS *ui;

public slots:
    void accept();
};

#endif // GUIADDWFS_H
