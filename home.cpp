#include "home.h"
#include <QIcon>
#include <QPixmap>
#include <QFrame>

Home::Home(QWidget *parent)
     : QMainWindow(parent) {

    setWindowTitle("Reproductor");
    resize(1000, 600);

    // Widget central
    central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    central->setLayout(mainLayout);

    // --- Barra lateral ---
    QWidget *barraLateral = new QWidget();
    barraLateral->setFixedWidth(60);
    QVBoxLayout *layoutLateral = new QVBoxLayout(barraLateral);
    layoutLateral->setAlignment(Qt::AlignTop);

    btnInicio = new QToolButton();
    btnInicio->setIcon(QIcon(":/icons/home.png"));
    btnInicio->setIconSize(QSize(32, 32));
    btnInicio->setToolTip("Inicio");

    btnFavoritos = new QToolButton();
    btnFavoritos->setIcon(QIcon(":/icons/heart.png"));
    btnFavoritos->setIconSize(QSize(32, 32));
    btnFavoritos->setToolTip("Favoritos");

    btnPlaylist = new QToolButton();
    btnPlaylist->setIcon(QIcon(":/icons/music.png"));
    btnPlaylist->setIconSize(QSize(32, 32));
    btnPlaylist->setToolTip("Playlists");

    btnConfig = new QToolButton();
    btnConfig->setIcon(QIcon(":/icons/settings.png"));
    btnConfig->setIconSize(QSize(32, 32));
    btnConfig->setToolTip("Configuración");

    for (auto btn : {btnInicio, btnFavoritos, btnPlaylist, btnConfig}) {
        btn->setAutoRaise(true);
        layoutLateral->addWidget(btn);
    }

    // --- Área principal con barra superior ---
    QWidget *contenido = new QWidget();
    QVBoxLayout *layoutContenido = new QVBoxLayout(contenido);

    // Barra superior
    QWidget *barraSuperior = new QWidget();
    QHBoxLayout *layoutSuperior = new QHBoxLayout(barraSuperior);
    layoutSuperior->setContentsMargins(0, 0, 0, 0);

    buscador = new QLineEdit();
    buscador->setPlaceholderText("Buscar canciones, artistas...");
    buscador->setFixedHeight(30);

    btnPerfil = new QToolButton();
    btnPerfil->setIcon(QIcon(":/icons/profile.png"));
    btnPerfil->setIconSize(QSize(24, 24));
    btnPerfil->setAutoRaise(true);
    btnPerfil->setToolTip("Perfil");

    btnAjustes = new QToolButton();
    btnAjustes->setIcon(QIcon(":/icons/config.png"));
    btnAjustes->setIconSize(QSize(24, 24));
    btnAjustes->setAutoRaise(true);
    btnAjustes->setToolTip("Ajustes");

    layoutSuperior->addWidget(buscador);
    layoutSuperior->addStretch();
    layoutSuperior->addWidget(btnPerfil);
    layoutSuperior->addWidget(btnAjustes);

    // Área con scroll vertical
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // Agrega elementos de ejemplo
    for (int i = 1; i <= 10; ++i) {
        QLabel *label = new QLabel("Contenido #" + QString::number(i));
        label->setFixedHeight(50);
        scrollLayout->addWidget(label);
    }

    scrollArea->setWidget(scrollContent);

    // Ensamblar layout
    layoutContenido->addWidget(barraSuperior);
    layoutContenido->addWidget(scrollArea);

    // Añadir al layout principal
    mainLayout->addWidget(barraLateral);
    mainLayout->addWidget(contenido);

    // --- Estilos ---
    this->setStyleSheet(R"(
        QWidget {
            background-color: #F2F2F2;
            font-family: 'Segoe UI';
        }
        QLineEdit {
            background-color: white;
            border: 1px solid #ccc;
            border-radius: 10px;
            padding: 5px 10px;
        }
        QToolButton {
            border: none;
            padding: 6px;
        }
        QToolButton:hover {
            background-color: #DDD;
            border-radius: 8px;
        }
    )");
}
