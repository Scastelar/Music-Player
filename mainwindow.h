#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cuentas.h"


#include <QFileDialog>
#include <QStyle>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, Cuentas& manejo);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    Cuentas* manejo;
    void initPopup();
    void resetearCampos();
private slots:
    void on_regButton_clicked();

    void on_loadButton1_clicked();

    void on_loadButton2_clicked();

    void on_backButton_clicked();

    void on_backButton_2_clicked();

    void on_crearButton1_clicked();

    void on_loginButton_2_clicked();

    void on_loginButton_clicked();

    void on_crearButton2_clicked();

    void on_backButton_3_clicked();
};
#endif // MAINWINDOW_H
