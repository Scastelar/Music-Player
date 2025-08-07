#include "HomeWindow_Copy.h"
#include "mainwindow.h"
#include "ui_HomeWindow_Copy.h"

#include <QFile>
#include <QMenu>
#include <QFontDatabase>



HomeWindow_Copy::HomeWindow_Copy(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow_Copy)
    ,  manejo(&manejo)
{
    ui->setupUi(this);
    ui->verticalFrame->setFixedWidth(200);

    int fontId = QFontDatabase::addApplicationFont(":/Montserrat-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (!families.isEmpty()) {
        Montserrat = families.at(0);  // ahora sí tienes el nombre real
    }

    int iconFontId = QFontDatabase::addApplicationFont(":/MaterialIcons-Regular.ttf");
    QString MaterialIcons = QFontDatabase::applicationFontFamilies(iconFontId).at(0);


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
            font-size: 16px;

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

    editarDatos = menu->addAction("Editar datos");
    verEstadisticas = menu->addAction("Estadísticas");
    menu->addSeparator();
    cerrarSesion = menu->addAction("Cerrar sesión");

    ui->btnPerfil->setMenu(menu);
    ui->btnPerfil->setPopupMode(QToolButton::InstantPopup);
    conectarMenu();

    setIcono(ui->toolButton_Playlist,0xe03b,20); //0xe03b
    setIcono(ui->toolButton_home,0xe88a,20); //home 0xe88a
    setIcono(ui->toolButton_play,0xe037,20);
    setIcono(ui->toolButton_prev,0xe045,20); //e045
    setIcono(ui->toolButton_next,0xe044,20);
    setIcono(ui->toolButton_random,0xe043,20);
    setIcono(ui->toolButton_loop,0xe040,20);
    setIcono(ui->toolButton_fav,0xe87d,20);
    setIcono(ui->toolButton_upload,0xe413,20);
    setIcono(ui->toolButton_upload_2,0xe413,20); //e43e
    setIcono(ui->toolButton_addSong,0xe7e5,20);

    //Rating promedio
    /*
    int rating = 5;
    QString stars = "";
    for (int i = 0; i < rating; i++){
        stars +=   (QChar(0xf0ec));
    }
    ui->label_ratingProm->setText(stars); //Star unicode
    ui->label_ratingProm->setFont(QFont(MaterialIcons, 25));
    ui->label_ratingProm->setStyleSheet("color: yellow; border: none;");
    */


}

HomeWindow_Copy::~HomeWindow_Copy()
{
    delete ui;
}


void HomeWindow_Copy::setIcono(QToolButton* boton, ushort unicode, int size){
    boton->setText(QChar(unicode));
    boton->setFont(QFont(MaterialIcons, size)); // Tamaño 20
    boton->setStyleSheet("color: white; border: none;");
}

void HomeWindow_Copy::on_toolButton_Playlist_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void HomeWindow_Copy::on_toolButton_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}


void HomeWindow_Copy::on_lineEdit_editingFinished()
{
    ui->stackedWidget->setCurrentIndex(4);

}

void HomeWindow_Copy::conectarMenu(){
    connect(editarDatos, &QAction::triggered, this, [=]() {
        qDebug() << "Editar datos";
        ui->stackedWidget->setCurrentIndex(2);
    });

    connect(verEstadisticas, &QAction::triggered, this, [=]() {
        qDebug() << "Ver estadísticas";
        ui->stackedWidget->setCurrentIndex(3);
    });

    connect(cerrarSesion, &QAction::triggered, this, [=]() {
        qDebug() << "Cerrar sesión";
        manejo->cerrarSesion();
        MainWindow* m = new MainWindow(nullptr);
        m->show();
        this->close();
    });

}

