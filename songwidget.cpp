// songwidget.cpp
#include "songwidget.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

SongWidget::SongWidget(const Cancion& cancion, QWidget *parent)
    : QWidget(parent), m_cancion(cancion) {

    // Configurar el widget
    setFixedSize(180, 220);
    setStyleSheet("background: transparent;");

    // Crear layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    // Portada de la canción
    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(160, 160);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet("QLabel {"
                              "border-radius: 8px;"
                              "background: #333;"
                              "}");

    QPixmap cover(m_cancion.getRutaPortada());
    if(cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png").scaled(160, 160, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    } else {
        cover = cover.scaled(160, 160, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    coverLabel->setPixmap(cover);

    // Título de la canción
    titleLabel = new QLabel(m_cancion.getTitulo(), this);
    titleLabel->setStyleSheet("QLabel {"
                              "color: white;"
                              "font-weight: bold;"
                              "font-size: 14px;"
                              "}");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    titleLabel->setMaximumWidth(160);

    // Artista
    artistLabel = new QLabel(m_cancion.getArtista(), this);
    artistLabel->setStyleSheet("QLabel {"
                               "color: #aaa;"
                               "font-size: 12px;"
                               "}");
    artistLabel->setAlignment(Qt::AlignCenter);
    artistLabel->setWordWrap(true);
    artistLabel->setMaximumWidth(160);

    // Añadir al layout
    layout->addWidget(coverLabel);
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
