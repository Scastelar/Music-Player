#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include "cuentas.h"

namespace Ui {
class menu;
}

class menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent , Cuentas& manejo);
    ~menu();
    void slideTo(int positionY);

private slots:
    void on_crearSTD_clicked();

    void on_crearADM_clicked();

    void on_editSTD_clicked();

    void on_editADM_clicked();

    void on_backButton_clicked();

    void on_backSTD_2_clicked();

    void on_backSTD_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Cuentas* manejo;
    Ui::menu *ui;
};

#endif // MENU_H
