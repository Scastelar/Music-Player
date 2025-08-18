#include "mainwindow.h"

#include <QApplication>

//yt-dlp -x --audio-format flac --embed-metadata ""

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Cuentas manejo;
    MainWindow w(nullptr, manejo);
    w.show();

    return a.exec();
}
