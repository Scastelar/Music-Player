#include "mainwindow.h"
#include "homewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();

    HomeWindow h;
    h.show();
    return a.exec();
}
