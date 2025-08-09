#include "mainwindow.h"
#include "homewindow.h"
#include "homewindow_copy.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Cuentas manejo;
    //Usuario* userActual = manejo.autenticar("mila","123");
    //manejo.setIdUsuarioActual(userActual->getId());

    // Ventana principal
    //HomeWindow_Copy c(nullptr, manejo);
    //c.show();

    //HomeWindow h;
    //h.show();
    return a.exec();
}
