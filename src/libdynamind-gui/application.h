#ifndef APPLICATION_H
#define APPLICATION_H
#include <iostream>
#include <QApplication>
#include <iostream>
#include <QObject>

class Application: public QApplication {
   Q_OBJECT;
public:
   Application(int &c, char **v): QApplication(c, v) {}
   bool notify(QObject *rec, QEvent *ev) {
       // cDebug() << "Called Application::notify()" << endl;
       try {
           return QApplication::notify(rec, ev);
       }
       catch (char const *str) {
           std::cout << "EXCEPTION: " << str <<  std::endl;
           return false;
       }
       catch (...) {
            std::cout << "Unknown exception!" << std:: endl;
            return false;
           //abort();
       }
   }
};


#endif // APPLICATION_H
