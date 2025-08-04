#include "homewindow.h"
#include "mainwindow.h"
#include "ui_homewindow.h"

#include <QFile>

Usuario* usuario;

HomeWindow::HomeWindow(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow)
    ,  manejo(&manejo)
{
    ui->setupUi(this);

    ui->verticalFrame->setFixedWidth(200);

    usuario = manejo.getIdUsuarioActual();

    QPixmap logo;
    logo.load(":/imagenes/logoW.png");
    QPixmap logoEscalado = logo.scaled(ui->label_5->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    ui->label_5->setPixmap(logoEscalado);

    QPixmap avatar;
    if(!usuario->getRutaImagen().isEmpty() && QFile::exists(usuario->getRutaImagen())) {
        avatar.load(usuario->getRutaImagen());
    } else {
        avatar.load(":/images/default_avatar.png");
    }

    QIcon icon(avatar.scaled(ui->toolButton_5->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation));
    ui->toolButton_5->setIcon(icon);
    ui->toolButton_5->setIconSize(ui->toolButton_5->size());

    this->setStyleSheet(R"(
    QWidget {
        background-color: rgb(40,40,40);
        color: white;
        font-family: 'Arial';
    }
    QToolButton {
        border: none;
    }
    QToolButton:hover {
        background-color: #DDDDDD;
        border-radius: 10px;
    }
    QLineEdit {
        border: 1px solid #CCC;
        border-radius: 8px;
        padding: 5px 10px;
        background-color: white;
    }
)");


}

HomeWindow::~HomeWindow()
{
    delete ui;
}
