// HAlbumWidget.cpp
#include "halbumwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QToolButton>
#include <QMenu>
#include <QGridLayout>

HAlbumWidget::HAlbumWidget(Album& album, QWidget *parent)
    : QWidget(parent), m_album(album) {

    setFixedSize(270, 130);
    setStyleSheet("background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QWidget *coverContainer = new QWidget(this);
    coverContainer->setFixedSize(100, 100);

    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(100, 100);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet("QLabel { border-radius: 8px; background: #333; }");

    QPixmap cover(m_album.getPortada());
    if (cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png")
        .scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    } else {
        cover = cover.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    coverLabel->setPixmap(cover);


    QGridLayout *gridCover = new QGridLayout(coverContainer);
    gridCover->setContentsMargins(0, 0, 0, 0);
    gridCover->addWidget(coverLabel, 0, 0);

    titleLabel = new QLabel(m_album.getNombre(), this);
    titleLabel->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 11px; }");
    titleLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(coverContainer, 0, Qt::AlignCenter);
    layout->addWidget(titleLabel);
}



