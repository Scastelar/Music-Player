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

    setFixedSize(180, 230);
    setStyleSheet("background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(14);

    QWidget *coverContainer = new QWidget(this);
    coverContainer->setFixedSize(80, 80);

    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(80, 80);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet("QLabel { border-radius: 8px; background: #333; }");

    QPixmap cover(m_album.getPortada());
    if (cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png")
        .scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    } else {
        cover = cover.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    coverLabel->setPixmap(cover);


    QGridLayout *gridCover = new QGridLayout(coverContainer);
    gridCover->setContentsMargins(0, 0, 0, 0);
    gridCover->addWidget(coverLabel, 0, 0);

    titleLabel = new QLabel(m_album.getNombre(), this);
    titleLabel->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 14px; }");
    titleLabel->setAlignment(Qt::AlignCenter);

    artistLabel = new QLabel(this);
    artistLabel->setStyleSheet("QLabel { color: #aaa; font-size: 12px; }");
    artistLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(coverContainer, 0, Qt::AlignCenter);
    layout->addWidget(titleLabel);
    layout->addWidget(artistLabel);
}



