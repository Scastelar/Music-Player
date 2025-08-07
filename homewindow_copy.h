#ifndef HOMEWINDOW_COPY_H
#define HOMEWINDOW_COPY_H

#include <QMainWindow>
#include <QToolButton>
#include "cuentas.h"

namespace Ui {
class HomeWindow_Copy;
}

class HomeWindow_Copy : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow_Copy(QWidget *parent, Cuentas& manejo);
    ~HomeWindow_Copy();

    void setIcono(QToolButton* boton, ushort unicode, int size);
    void conectarMenu();

private slots:
    void on_toolButton_Playlist_clicked();

    void on_toolButton_home_clicked();

    void on_lineEdit_editingFinished();

private:
    Ui::HomeWindow_Copy *ui;
    Cuentas* manejo;

    QString Montserrat = "Montserrat";
    QString MaterialIcons = "Material-Icons";



    Usuario* usuario;

    QAction* editarDatos;
    QAction* verEstadisticas;
    QAction* cerrarSesion;
};

#endif // HOMEWINDOW_COPY_H
