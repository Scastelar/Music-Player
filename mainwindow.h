#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMediaPlayer>
#include <QAudioOutput>   // For Qt 6

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


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

private:
    Ui::MainWindow *ui;
    QMediaPlayer *M_Player;
    void initPopup();
    void resetearCampos();
};
#endif // MAINWINDOW_H
