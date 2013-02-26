#ifndef GUIPICKWFSDATASET_H
#define GUIPICKWFSDATASET_H

#include <QDialog>

namespace Ui {
class GUIPickWFSDataset;
}

class GUIPickWFSDataset : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIPickWFSDataset(QWidget *parent = 0);
    ~GUIPickWFSDataset();
    
private:
    Ui::GUIPickWFSDataset *ui;
signals:
    void WFSServerChanged(QString);
    void WFSUsernameChanged(QString);
    void WFSPasswordChanged(QString);
    void WFSDatasetChanged(QString);

public slots:
    void on_pushButton_connect_clicked();
    void accept();

};

#endif // GUIPICKWFSDATASET_H
