#include "mainwindow.h"

#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("1samer\n");
    QApplication a(argc, argv);
    printf("2wtf\n");
    MainWindow w;
    printf("3yes\n");
    w.show();
    printf("4Hello\n");
    return a.exec();
}
