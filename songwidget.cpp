// songwidget.cpp
#include "songwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QToolButton>
#include <QMenu>
#include <QGridLayout>

SongWidget::SongWidget(const Cancion& cancion, QWidget *parent)
    : QWidget(parent), m_cancion(cancion) {

    setFixedSize(180, 230);
    setStyleSheet("background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(14);

    QWidget *coverContainer = new QWidget(this);
    coverContainer->setFixedSize(160, 160);

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

    // Botón y menú
    QToolButton *menuButton = new QToolButton(this);
    menuButton->setIcon(QIcon(":/images/menu_icon.png"));
    menuButton->setIconSize(QSize(16, 16));
    menuButton->setFixedSize(20, 20);

    QMenu *menu = new QMenu(this);
    menu->setStyleSheet(
        "QMenu { background-color: #222; color: white; border: 1px solid #444; }"
        "QMenu::item { padding: 6px 20px; }"
        "QMenu::item:selected { background-color: #444; }"
        );

    QAction *editAction = menu->addAction("Editar");
    QAction *deleteAction = menu->addAction("Eliminar");

    // Conectar acciones a slots
    connect(editAction, &QAction::triggered, this, &SongWidget::onEditSong);
    connect(deleteAction, &QAction::triggered, this, &SongWidget::onDeleteSong);

    menuButton->setMenu(menu);
    menuButton->setPopupMode(QToolButton::InstantPopup);

    QGridLayout *gridCover = new QGridLayout(coverContainer);
    gridCover->setContentsMargins(0, 0, 0, 0);
    gridCover->addWidget(coverLabel, 0, 0);
    gridCover->addWidget(menuButton, 0, 0, Qt::AlignTop | Qt::AlignRight);

    titleLabel = new QLabel(m_cancion.getTitulo(), this);
    titleLabel->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 14px; }");
    titleLabel->setAlignment(Qt::AlignCenter);

    artistLabel = new QLabel(m_cancion.getArtista(), this);
    artistLabel->setStyleSheet("QLabel { color: #aaa; font-size: 12px; }");
    artistLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(coverContainer, 0, Qt::AlignCenter);
    layout->addWidget(titleLabel);
    layout->addWidget(artistLabel);
}

void SongWidget::onEditSong() {
    emit editSongRequested(m_cancion.getId()); // o el método que tengas para obtener ID
}

void SongWidget::onDeleteSong() {
    emit deleteSongRequested(m_cancion.getId());
}


void SongWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        emit songClicked(m_cancion);
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
