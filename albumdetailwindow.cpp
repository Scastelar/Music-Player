#include "AlbumDetailWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QRandomGenerator>

AlbumDetailWindow::AlbumDetailWindow(const Album& album, Cuentas* cuentas, QWidget* parent)
    : QWidget(parent), m_album(album), m_cuentas(cuentas) {
    setWindowTitle(album.getNombre());
    setupUI();
    loadSongs();
}

void AlbumDetailWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // 1. Header compacto
    QWidget* header = new QWidget;
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(20);

    // Portada más pequeña
    QLabel* cover = new QLabel;
    QPixmap coverPixmap(m_album.getPortada());
    if (coverPixmap.isNull()) {
        coverPixmap = QPixmap(":/images/default_cover.png");
    }
    cover->setPixmap(coverPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    cover->setAlignment(Qt::AlignCenter);
    cover->setStyleSheet("border-radius: 4px;");

    // Detalles compactos
    QVBoxLayout* detailsLayout = new QVBoxLayout;
    detailsLayout->setSpacing(8);

    auto createDetailRow = [](const QString& title, const QString& value) {
        QWidget* row = new QWidget;
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        QLabel* titleLabel = new QLabel(title + ":");
        titleLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: white;");
        titleLabel->setMinimumWidth(60);

        QLabel* valueLabel = new QLabel(value);
        valueLabel->setStyleSheet("font-size: 11pt; color: white;");

        rowLayout->addWidget(titleLabel);
        rowLayout->addWidget(valueLabel);
        rowLayout->addStretch();

        return row;
    };

    detailsLayout->addWidget(createDetailRow("Título", m_album.getNombre()));
    detailsLayout->addWidget(createDetailRow("Tipo", m_album.getTipoString()));
    detailsLayout->addWidget(createDetailRow("Fecha", m_album.getFechaCreacion().date().toString("dd/MM/yyyy")));

    // Botón agregar canción
    QPushButton* btnAgregar = new QPushButton("Agregar Canción");
    btnAgregar->setStyleSheet(
        "QPushButton { background-color: #DE5D83; color: white; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #A94064; }"
        );
    connect(btnAgregar, &QPushButton::clicked, this, &AlbumDetailWindow::onAgregarCancionClicked);

    headerLayout->addStretch();

    QPushButton* btnPlayAlbum = new QPushButton("Reproducir");
    btnPlayAlbum->setStyleSheet(
        "QPushButton { background-color: #DE5D83; color: white; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #A94064; }"
        );
    btnPlayAlbum->setFixedWidth(130);
    connect(btnPlayAlbum, &QPushButton::clicked, this, [this]() {
        if (!m_cancionesAlbum.isEmpty()) {
            emit solicitarReproduccionAlbum(m_cancionesAlbum, 0);
        }
    });


    headerLayout->addWidget(cover);
    headerLayout->addLayout(detailsLayout, 1);
    headerLayout->addWidget(btnAgregar);

    // 2. Lista de canciones
    QLabel* songsTitle = new QLabel("CANCIONES");
    songsTitle->setStyleSheet("font-weight: bold; font-size: 12pt; color: white;");

    // Encabezados de columna
    QWidget* songsHeader = new QWidget;
    QHBoxLayout* headerSongsLayout = new QHBoxLayout(songsHeader);
    headerSongsLayout->setContentsMargins(15, 5, 15, 5);
    headerSongsLayout->setSpacing(20);

    QString headerStyle = "font-weight: bold; font-size: 10pt; color: #aaa;";

    QLabel* numHeader = new QLabel("#");
    QLabel* titleHeader = new QLabel("TÍTULO");
    QLabel* artistHeader = new QLabel("ARTISTA");
    QLabel* durationHeader = new QLabel("DURACIÓN");
    QLabel* dateHeader = new QLabel("FECHA");

    numHeader->setStyleSheet(headerStyle);
    titleHeader->setStyleSheet(headerStyle);
    artistHeader->setStyleSheet(headerStyle);
    durationHeader->setStyleSheet(headerStyle);
    dateHeader->setStyleSheet(headerStyle);

    numHeader->setFixedWidth(30);
    dateHeader->setFixedWidth(100);

    headerSongsLayout->addWidget(numHeader);
    headerSongsLayout->addWidget(titleHeader, 1);
    headerSongsLayout->addWidget(artistHeader, 1);
    headerSongsLayout->addWidget(durationHeader, 1);
    headerSongsLayout->addWidget(dateHeader);

    // Configuración de la lista
    m_songsList = new QListWidget;
    m_songsList->setMinimumHeight(200); // Altura mínima
    // Stylesheet ultra simple
    m_songsList->setStyleSheet(
        "QListWidget {"
        "   background-color: #2d2d2d;"  // Fondo oscuro
        "   color: white;"               // Texto blanco
        "   font-size: 11px;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #3d3d3d;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #3d3d3d;"
        "}"
        );

    mainLayout->addWidget(header);
    mainLayout->addWidget(btnPlayAlbum);
    mainLayout->addWidget(songsTitle);
    mainLayout->addWidget(songsHeader);
    mainLayout->addWidget(m_songsList, 1);
}

void AlbumDetailWindow::loadSongs() {
    m_songsList->clear();
    m_cancionesAlbum.clear();
    m_indiceActual = -1;

    int index = 0; // Índice visual (comienza desde 0)

    for (int cancionId : m_album.getCanciones()) {
        Cancion* cancion = m_cuentas->buscarCancionPorId(cancionId);
        if (cancion) {
            m_cancionesAlbum.append(cancion); // Guardar objeto canción

            QListWidgetItem* item = new QListWidgetItem;

            // Widget personalizado
            QWidget* songWidget = new QWidget;
            QHBoxLayout* songLayout = new QHBoxLayout(songWidget);
            songLayout->setContentsMargins(10, 5, 10, 5);
            songLayout->setSpacing(10);

            // Índice visual (1-based)
            QLabel* numLabel = new QLabel(QString::number(index + 1)); // Mostrar desde 1
            numLabel->setFixedWidth(25);
            numLabel->setMinimumHeight(40);

            // Título
            QLabel* titleLabel = new QLabel(cancion->getTitulo());
            titleLabel->setMinimumWidth(150);
            titleLabel->setMinimumHeight(40);

            // Artista
            QLabel* artistLabel = new QLabel(cancion->getArtista());
            artistLabel->setMinimumWidth(150);
            artistLabel->setMinimumHeight(40);

            // Duracion
            QTime current(0,0); current = current.addSecs((cancion->getDuracion()));
            const QString format = "mm:ss";
            QLabel* durationLabel = new QLabel();
            durationLabel->setText(current.toString(format));
            durationLabel->setFixedWidth(80);
            durationLabel->setMinimumHeight(40);

            // Fecha
            QLabel* dateLabel = new QLabel(cancion->getFechaRegistro().date().toString("dd/MM/yyyy"));
            dateLabel->setFixedWidth(80);
            dateLabel->setMinimumHeight(40);

            // Estilos
            QString labelStyle = "color: white; font-size: 11px;";
            numLabel->setStyleSheet(labelStyle);
            titleLabel->setStyleSheet(labelStyle + "font-weight: bold;");
            artistLabel->setStyleSheet(labelStyle);
            dateLabel->setStyleSheet(labelStyle);
            durationLabel->setStyleSheet(labelStyle);

            // Construir layout
            songLayout->addWidget(numLabel);
            songLayout->addWidget(titleLabel);
            songLayout->addWidget(artistLabel);
            songLayout->addWidget(durationLabel);
            songLayout->addWidget(dateLabel);

            // Configurar item
            item->setSizeHint(songWidget->sizeHint());
            m_songsList->addItem(item);
            m_songsList->setItemWidget(item, songWidget);

            // Guardar datos importantes (usando diferentes roles)
            item->setData(Qt::UserRole, cancionId); // ID para búsquedas
            item->setData(Qt::UserRole , index); // Índice en m_cancionesAlbum

            index++; // Incrementar índice
        }
    }

    connect(m_songsList, &QListWidget::itemClicked,
            this, &AlbumDetailWindow::onItemClicked);
}


void AlbumDetailWindow::onItemClicked(QListWidgetItem* item) {
    m_indiceActual = item->data(Qt::UserRole).toInt();
    if (m_indiceActual >= 0 && m_indiceActual < m_cancionesAlbum.size()) {
        // Emitir señal para reproducir el álbum completo comenzando por esta canción
        emit solicitarReproduccionAlbum(m_cancionesAlbum, m_indiceActual);
    }
}

void AlbumDetailWindow::reproducirSiguiente()
{
    if (m_cancionesAlbum.isEmpty()) return;

    int nextIndex;

    if (m_randomMode) {
        // Modo aleatorio
        if (m_cancionesAlbum.size() > 1) {
            do {
                nextIndex = QRandomGenerator::global()->bounded(m_cancionesAlbum.size());
            } while (nextIndex == m_indiceActual);
        } else {
            nextIndex = 0; // Solo hay una canción
        }
    } else {
        // Modo secuencial
        nextIndex = (m_indiceActual + 1) % m_cancionesAlbum.size();
    }

    m_indiceActual = nextIndex;
    emit solicitarReproduccionCancion(m_cancionesAlbum.at(m_indiceActual));
}

void AlbumDetailWindow::reproducirAnterior() {
    if (m_cancionesAlbum.isEmpty()) return;

    m_indiceActual = (m_indiceActual - 1 + m_cancionesAlbum.size()) % m_cancionesAlbum.size();
    m_songsList->setCurrentRow(m_indiceActual);
    emit solicitarReproduccionCancion(m_cancionesAlbum.at(m_indiceActual));
}

void AlbumDetailWindow::onAgregarCancionClicked() {
    emit agregarCancionAlAlbum(&m_album);
}

