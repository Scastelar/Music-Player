#include "AlbumWidget.h"
#include "Album.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDebug>

AlbumWidget::AlbumWidget(const Album& album, Cuentas* cuentas, QWidget* parent)
    : QWidget(parent), m_album(album), m_cuentas(cuentas) {
    setupUI();
}

void AlbumWidget::setupUI() {
    // Layout principal con márgenes y espaciado
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // Más espacio interno
    mainLayout->setSpacing(20);  // Espacio entre elementos

    // 1. Portada (usando tu código que funciona)
    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(50, 50);  // Tamaño más pequeño para lista
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet(
        "QLabel {"
        "   border-radius: 4px;"
        "   background: #333333;"
        "}"
        );

    QPixmap cover(m_album.getPortada());
    if (cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png")
        .scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    } else {
        cover = cover.scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    coverLabel->setPixmap(cover);

    // 2. Información del álbum
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(6);  // Espacio entre textos

    QLabel* lblNombre = new QLabel(m_album.getNombre());
    QLabel* lblTipo = new QLabel(m_album.getTipoString());

    // Estilo para texto blanco
    QString textStyle =
        "QLabel {"
        "   color: white;"
        "   font-size: 10pt;"
        "   background: transparent;"
        "}";

    lblNombre->setStyleSheet(textStyle + "font-weight: bold;");
    lblTipo->setStyleSheet(textStyle);

    infoLayout->addWidget(lblNombre);
    infoLayout->addWidget(lblTipo);
    infoLayout->addStretch();  // Empuja el texto hacia arriba

    // Organizar en layout principal
    mainLayout->addWidget(coverLabel);
    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();  // Espacio flexible a la derecha

    // Estilo del widget contenedor
    this->setStyleSheet(
        "AlbumWidget {"
        "   background: transparent;"
        "   border-bottom: 1px solid #444;"
        "}"
        "AlbumWidget:hover {"
        "   background: #333333;"
        "}"
        );
}

void AlbumWidget::mousePressEvent(QMouseEvent*) {
    emit clicked(m_album);
}
