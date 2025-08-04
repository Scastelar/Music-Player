#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QMainWindow>
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

private:
    Ui::HomeWindow *ui;
    Cuentas* manejo;
};

#endif // HOMEWINDOW_H
