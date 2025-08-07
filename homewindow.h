#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include "cuentas.h"

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent, Cuentas& manejo);
    ~HomeWindow();

    void setIcono(QToolButton* boton, ushort unicode, int size);

private slots:
    void on_toolButton_Playlist_clicked();

    void on_toolButton_home_clicked();

    void on_lineEdit_editingFinished();

private:
    Ui::HomeWindow *ui;
    Cuentas* manejo;
};

#endif // HOMEWINDOW_H
