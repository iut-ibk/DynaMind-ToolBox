

#include <QCoreApplication>
#include <QTimer>
#include "task.h"

int main(int argc, char *argv[], char *envp[]) 
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("IUT");
    QCoreApplication::setApplicationName("DYNAMIND");


    // Task parented to the application so that it
    // will be deleted by the application.
    Task *task = new Task(argc, argv, envp, &a);

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));

    // This will run the task from the application event loop.
    QTimer::singleShot(0, task, SLOT(run()));

	//QThreadPool::globalInstance()->setMaxThreadCount(1);


    return a.exec();
}
