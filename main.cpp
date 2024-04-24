#include "mainwindow.h"
#include "parameters.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Lucas Placentino and Salman Houdaibi");
    QCoreApplication::setApplicationName("802.11ay Raytracing Simulator");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainWindow mainWin;
    mainWin.show();

    return app.exec();
}


