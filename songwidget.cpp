#include "songwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QToolButton>
#include <QMenu>

SongWidget::SongWidget(const Cancion& cancion, QWidget *parent)
    : QWidget(parent), m_cancion(cancion) {

    setFixedSize(180, 230);
    setStyleSheet("background: transparent;");

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(14); // más separación real

    // Contenedor para portada y botón
    QWidget *coverContainer = new QWidget(this);
    coverContainer->setFixedSize(160, 160);
    QHBoxLayout *coverLayout = new QHBoxLayout(coverContainer);
    coverLayout->setContentsMargins(0, 0, 0, 0);
    coverLayout->setSpacing(0);

    // Portada
    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(160, 160);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet("QLabel { border-radius: 8px; background: #333; }");

    QPixmap cover(m_cancion.getRutaPortada());
    if (cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png")
        .scaled(160, 160, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    } else {
        cover = cover.scaled(160, 160, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    coverLabel->setPixmap(cover);

    // Botón de menú sobre la portada (esquina superior derecha)
    QToolButton *menuButton = new QToolButton(this);
    menuButton->setIcon(QIcon(":/images/menu_icon.png"));
    menuButton->setIconSize(QSize(16, 16));
    menuButton->setStyleSheet(
        "QToolButton { border: none; background: rgba(0,0,0,0.4); border-radius: 4px; padding: 2px; }"
        "QToolButton:hover { background: rgba(255,255,255,0.2); }"
        );
    menuButton->setFixedSize(20, 20);

    QMenu *menu = new QMenu(this);

    // Fondo sólido y texto visible
    menu->setStyleSheet(
        "QMenu { "
        "   background-color: #222; "       /* fondo oscuro sólido */
        "   color: white; "                 /* texto blanco */
        "   border: 1px solid #444; "        /* borde sutil */
        "} "
        "QMenu::item { "
        "   padding: 6px 20px; "
        "} "
        "QMenu::item:selected { "
        "   background-color: #444; "        /* fondo al pasar el mouse */
        "}"
        );

    menu->addAction("Descargar");
    menu->addAction("Favorito");
    menu->addAction("Calificar");
    menuButton->setMenu(menu);
    menuButton->setPopupMode(QToolButton::InstantPopup);

    // Posicionar portada y botón en el contenedor
    QGridLayout *gridCover = new QGridLayout(coverContainer);
    gridCover->setContentsMargins(0, 0, 0, 0);
    gridCover->addWidget(coverLabel, 0, 0);
    gridCover->addWidget(menuButton, 0, 0, Qt::AlignTop | Qt::AlignRight);

    // Título
    titleLabel = new QLabel(m_cancion.getTitulo(), this);
    titleLabel->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 14px; }");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    titleLabel->setMaximumWidth(160);

    // Artista
    artistLabel = new QLabel(m_cancion.getArtista(), this);
    artistLabel->setStyleSheet("QLabel { color: #aaa; font-size: 12px; }");
    artistLabel->setAlignment(Qt::AlignCenter);
    artistLabel->setWordWrap(true);
    artistLabel->setMaximumWidth(160);

    // Añadir al layout principal
    layout->addWidget(coverContainer, 0, Qt::AlignCenter);
    layout->addWidget(titleLabel);
    layout->addWidget(artistLabel);

    setLayout(layout);
}

void SongWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit songClicked(m_cancion);
    }
    QWidget::mousePressEvent(event);
}

void SongWidget::enterEvent(QEnterEvent *event) {
    setStyleSheet("background: #2a2a2a; border-radius: 8px;");
    QWidget::enterEvent(event);
}

void SongWidget::leaveEvent(QEvent *event) {
    setStyleSheet("background: transparent;");
    QWidget::leaveEvent(event);
}
