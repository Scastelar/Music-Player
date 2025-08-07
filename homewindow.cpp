#include "homewindow.h"
#include "mainwindow.h"
#include "ui_homewindow.h"

#include <QFile>
#include <QMenu>
#include <QFontDatabase>

QString Montserrat = "Montserrat";
QString MaterialIcons = "Material-Icons";



Usuario* usuario;

HomeWindow::HomeWindow(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow)
    ,  manejo(&manejo)
{
    ui->setupUi(this);

    int fontId = QFontDatabase::addApplicationFont(":/Montserrat-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (!families.isEmpty()) {
        Montserrat = families.at(0);  // ahora sí tienes el nombre real
    }

    int iconFontId = QFontDatabase::addApplicationFont(":/MaterialIcons-Regular.ttf");
    QString MaterialIcons = QFontDatabase::applicationFontFamilies(iconFontId).at(0);

    ui->verticalFrame->setFixedWidth(200);

    usuario = manejo.getIdUsuarioActual();

    QPixmap logo;
    logo.load(":/imagenes/logoW.png");
    QPixmap logoEscalado = logo.scaled(ui->label_5->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    ui->label_5->setPixmap(logoEscalado);

    QPixmap album;
    album.load(":/imagenes/album.png");
    QPixmap albumEscalado = album.scaled(ui->label_8->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    ui->label_8->setPixmap(albumEscalado);

    QPixmap avatar;
    if(!usuario->getRutaImagen().isEmpty() && QFile::exists(usuario->getRutaImagen())) {
        avatar.load(usuario->getRutaImagen());
    } else {
        avatar.load(":/images/default_avatar.png");
    }

    QIcon icon(avatar.scaled(ui->btnPerfil->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation));
    ui->btnPerfil->setIcon(icon);
    ui->btnPerfil->setIconSize(ui->btnPerfil->size());


    if (!Montserrat.isEmpty()) {
        QString style = R"(
        QWidget {
            background-color: rgb(40,40,40);
            color: white;
            font-family: 'FONT';
        }
        QLabel {
            font-size: 15px;

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
            color: black;
        }
    )";
        style.replace("FONT", Montserrat);
        this->setStyleSheet(style);
    } else {
        qDebug() << "Error: Montserrat no está cargado correctamente.";
    }


    ui->label_7->setText("Bienvenido, " + usuario->getNombreUsuario());

    QMenu *menu = new QMenu(this);
    menu->addAction("Editar datos");
    menu->addAction("Ver perfil");
    menu->addAction("Estadísticas");
    menu->addSeparator();
    menu->addAction("Cerrar sesión");

    ui->btnPerfil->setMenu(menu);
    ui->btnPerfil->setPopupMode(QToolButton::InstantPopup);


    setIcono(ui->toolButton_Playlist,0xe03b,20); //0xe03b
    setIcono(ui->toolButton_home,0xe88a,20); //home 0xe88a
    setIcono(ui->toolButton_play,0xe037,20);
    setIcono(ui->toolButton_prev,0xe045,20); //e045
    setIcono(ui->toolButton_next,0xe044,20);
    setIcono(ui->toolButton_random,0xe043,20);
    setIcono(ui->toolButton_loop,0xe040,20);
    setIcono(ui->toolButton_fav,0xe87d,20);



}

HomeWindow::~HomeWindow()
{
    delete ui;
}


void HomeWindow::setIcono(QToolButton* boton, ushort unicode, int size){
    boton->setText(QChar(unicode));
    boton->setFont(QFont(MaterialIcons, size)); // Tamaño 20
    boton->setStyleSheet("color: white; border: none;");
}

void HomeWindow::on_toolButton_Playlist_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void HomeWindow::on_toolButton_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}


void HomeWindow::on_lineEdit_editingFinished()
{
    ui->stackedWidget->setCurrentIndex(2);

}

