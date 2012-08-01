#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QWidget>

namespace Ui {
class ViewerWindow;
}
namespace DM {

class System;

class ViewerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit ViewerWindow(System *system, QWidget *parent = 0);
    ~ViewerWindow();
    
private:
    Ui::ViewerWindow *ui;
};

}
#endif // VIEWERWINDOW_H
