#include "menu.h"
#include "mainwindow.h"
#include "ui_menu.h"
#include <QFile>

#include <QPropertyAnimation>
#include <QScrollBar>

Usuario* usuario;

menu::menu(QWidget *parent, Cuentas& manejo)
    : QMainWindow(parent), ui(new Ui::menu), manejo(&manejo)
{
    ui->setupUi(this);

    usuario = manejo.getIdUsuarioActual();
    const bool esAdmin = (usuario->getTipo() == "ADMIN");

    if(!usuario) {
        qWarning() << "Error: No hay usuario autenticado";
        close();
        return;
    }

    // Configuración de la interfaz según tipo de usuario
    ui->stackedWidget->setCurrentIndex(esAdmin ? 1 : 0);

    // Carga de avatar
    QPixmap avatar;
    if(!usuario->getRutaImagen().isEmpty() && QFile::exists(usuario->getRutaImagen())) {
        avatar.load(usuario->getRutaImagen());
    } else {
        avatar.load(":/images/default_avatar.png");
    }

    QPixmap avatarEscalado = avatar.scaled(
        esAdmin ? ui->pfpADM->size() : ui->pfpSTD->size(),
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
        );

    if(esAdmin) {
        //Perfil
        ui->pfpADM->setPixmap(avatarEscalado);
        ui->userADM->setText(usuario->getNombreUsuario());
        ui->descADM->setText(usuario->getDescripcion());

        ui->editPfpADM->setPixmap(avatarEscalado);
        ui->editUserADM->setText(usuario->getNombreUsuario());
        ui->editNomADM->setText(usuario->getNombreReal());
        ui->editPassADM->setText("******");
        ui->descADM_3->setText(usuario->getDescripcion());

        //Editar Perfil
        ui->editPfpADM->setPixmap(avatarEscalado);
    } else {
        //Perfil
        ui->pfpSTD->setPixmap(avatarEscalado);
        ui->userSTD->setText(usuario->getNombreUsuario());
        ui->descSTD->setText(usuario->getDescripcion());

        //Editar Perfil
        ui->editPfpSTD->setPixmap(avatarEscalado);
        ui->editUserSTD->setText(usuario->getNombreUsuario());
        ui->editNomSTD->setText(usuario->getNombreReal());
        ui->editPassSTD->setText("******");
    }



}

menu::~menu()
{
    delete ui;
}



void menu::on_crearSTD_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void menu::on_crearADM_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void menu::on_editSTD_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void menu::on_editADM_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void menu::on_backButton_clicked()
{
    const bool esAdmin = (usuario->getTipo() == "ADMIN");
    ui->stackedWidget->setCurrentIndex(esAdmin ? 1 : 0);
}


void menu::on_backSTD_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void menu::on_backSTD_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void menu::on_pushButton_clicked()
{
    usuario = nullptr;
    MainWindow* m = new MainWindow(this);
    m->show();
    this->close();
}


void menu::on_pushButton_2_clicked()
{
    usuario = nullptr;
    MainWindow* m = new MainWindow(this);
    m->show();
    this->close();
}

