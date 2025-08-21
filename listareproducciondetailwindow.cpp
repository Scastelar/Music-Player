#include "ListaReproduccionDetailWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QRandomGenerator>
#include <QMenu>
#include <QAction>

ListaReproduccionDetailWindow::ListaReproduccionDetailWindow(ListaReproduccion& lista, Cuentas* cuentas, QWidget* parent)
    : QWidget(parent), m_lista(lista), m_cuentas(cuentas) {
    setWindowTitle(lista.getNombre());
    setupUI();
    loadSongs();
}

void ListaReproduccionDetailWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // 1. Header compacto
    QWidget* header = new QWidget;
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(20);

    // Portada más pequeña
    m_coverLabel = new QLabel;
    updateCoverImage();
    m_coverLabel->setAlignment(Qt::AlignCenter);
    m_coverLabel->setStyleSheet("border-radius: 4px;");

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

    m_titleLabel = new QLabel(m_lista.getNombre());
    m_titleLabel->setStyleSheet("font-size: 11pt; color: white;");

    QWidget* titleRow = new QWidget;
    QHBoxLayout* titleRowLayout = new QHBoxLayout(titleRow);
    titleRowLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* titleLabel = new QLabel("Título:");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: white;");
    titleLabel->setMinimumWidth(60);



    QPushButton* btnEditTitle = new QPushButton("Editar");
    btnEditTitle->setStyleSheet(
        "QPushButton { background-color: transparent; color: #DE5D83; padding: 2px 5px; border: 1px solid #DE5D83; border-radius: 2px; }"
        "QPushButton:hover { background-color: #DE5D83; color: white; }"
        );
    btnEditTitle->setFixedSize(50, 20);
    connect(btnEditTitle, &QPushButton::clicked, this, &ListaReproduccionDetailWindow::onEditTitleClicked);

    titleRowLayout->addWidget(titleLabel);
    titleRowLayout->addWidget(m_titleLabel);
    titleRowLayout->addWidget(btnEditTitle);
    titleRowLayout->addStretch();

    detailsLayout->addWidget(titleRow);
    detailsLayout->addWidget(createDetailRow("Tipo", m_lista.getTipoString()));
    detailsLayout->addWidget(createDetailRow("Fecha", m_lista.getFechaCreacion().date().toString("dd/MM/yyyy")));

    // Botón editar portada
    QPushButton* btnEditCover = new QPushButton("Cambiar Portada");
    btnEditCover->setStyleSheet(
        "QPushButton { background-color: transparent; color: #DE5D83; padding: 2px 5px; border: 1px solid #DE5D83; border-radius: 2px; }"
        "QPushButton:hover { background-color: #DE5D83; color: white; }"
        );
    btnEditCover->setFixedSize(120, 20);
    connect(btnEditCover, &QPushButton::clicked, this, &ListaReproduccionDetailWindow::onEditCoverClicked);

    // Botón agregar canción
    QPushButton* btnAgregar = new QPushButton("Agregar Canción");
    btnAgregar->setStyleSheet(
        "QPushButton { background-color: #DE5D83; color: white; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #A94064; }"
        );
    connect(btnAgregar, &QPushButton::clicked, this, &ListaReproduccionDetailWindow::onAgregarCancionClicked);

    headerLayout->addStretch();

    QPushButton* btnPlayAlbum = new QPushButton("Reproducir");
    btnPlayAlbum->setStyleSheet(
        "QPushButton { background-color: #DE5D83; color: white; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #A94064; }"
        );
    btnPlayAlbum->setFixedWidth(130);
    connect(btnPlayAlbum, &QPushButton::clicked, this, [this]() {
        if (!m_canciones.isEmpty()) {
            emit solicitarReproduccionAlbum(m_canciones, 0);
        }
    });

    // Layout para botones de edición
    QVBoxLayout* editButtonsLayout = new QVBoxLayout;
    editButtonsLayout->addWidget(btnEditCover);
    editButtonsLayout->addWidget(btnAgregar);
    editButtonsLayout->setSpacing(5);
    editButtonsLayout->setContentsMargins(0, 0, 0, 0);

    headerLayout->addWidget(m_coverLabel);
    headerLayout->addLayout(detailsLayout, 1);
    headerLayout->addLayout(editButtonsLayout);

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
    QLabel* durationHeader = new QLabel("DURACION");
    QLabel* dateHeader = new QLabel("FECHA");
    QLabel* actionsHeader = new QLabel("ACCIONES");

    numHeader->setStyleSheet(headerStyle);
    titleHeader->setStyleSheet(headerStyle);
    artistHeader->setStyleSheet(headerStyle);
    durationHeader->setStyleSheet(headerStyle);
    dateHeader->setStyleSheet(headerStyle);
    actionsHeader->setStyleSheet(headerStyle);

    numHeader->setFixedWidth(30);
    dateHeader->setFixedWidth(100);
    actionsHeader->setFixedWidth(80);

    headerSongsLayout->addWidget(numHeader);
    headerSongsLayout->addWidget(titleHeader, 1);
    headerSongsLayout->addWidget(artistHeader, 1);
    headerSongsLayout->addWidget(durationHeader, 1);
    //headerSongsLayout->addWidget(dateHeader);
    headerSongsLayout->addWidget(actionsHeader);

    // Configuración de la lista
    m_songsList = new QListWidget;
    m_songsList->setMinimumHeight(200);
    m_songsList->setStyleSheet(
        "QListWidget {"
        "   background-color: #2d2d2d;"
        "   color: white;"
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

void ListaReproduccionDetailWindow::loadSongs() {
    m_songsList->clear();
    m_canciones.clear();

    int index = 0;
    for (int cancionId : m_lista.getCanciones()) {
        Cancion* cancion = m_cuentas->buscarCancionPorId(cancionId);
        if (cancion) {
            m_canciones.append(cancion);

            QListWidgetItem* item = new QListWidgetItem;

            // Widget personalizado
            QWidget* songWidget = new QWidget;
            QHBoxLayout* songLayout = new QHBoxLayout(songWidget);
            songLayout->setContentsMargins(10, 5, 10, 5);
            songLayout->setSpacing(10);

            // Índice visual
            QLabel* numLabel = new QLabel(QString::number(index + 1));
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

            // Genero
            QLabel* durationLabel = new QLabel(cancion->getDuracion());
            durationLabel->setMinimumWidth(150);
            durationLabel->setMinimumHeight(40);

            // Fecha
            QLabel* dateLabel = new QLabel(cancion->getFechaRegistro().date().toString("dd/MM/yyyy"));
            dateLabel->setFixedWidth(80);
            dateLabel->setMinimumHeight(40);

            // Botón de menú
            QPushButton* menuButton = new QPushButton("⋮");
            menuButton->setFixedSize(30, 30);
            menuButton->setStyleSheet(
                "QPushButton { background-color: transparent; color: white; border: 1px solid #555; border-radius: 4px; }"
                "QPushButton:hover { background-color: #444; }"
                );

            // Menú contextual
            QMenu* songMenu = new QMenu(menuButton);
            QAction* editAction = new QAction("Editar canción", songMenu);
            QAction* deleteAction = new QAction("Eliminar canción", songMenu);

            songMenu->addAction(editAction);
            songMenu->addAction(deleteAction);

            menuButton->setMenu(songMenu);

            // Conectar acciones
            connect(editAction, &QAction::triggered, this, [this, cancionId]() {
                emit editarCancionClicked(cancionId);
            });

            connect(deleteAction, &QAction::triggered, this, [this, cancionId]() {
                emit eliminarCancionClicked(cancionId, m_lista.getId());
            });

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
            //songLayout->addWidget(dateLabel);
            songLayout->addWidget(menuButton);

            // Configurar item
            item->setSizeHint(songWidget->sizeHint());
            m_songsList->addItem(item);
            m_songsList->setItemWidget(item, songWidget);

            // Guardar datos importantes

            item->setData(Qt::UserRole, cancionId);
            item->setData(Qt::UserRole+1, index);

            index++;
        }
    }

    connect(m_songsList, &QListWidget::itemClicked, this, &ListaReproduccionDetailWindow::onItemClicked);
}

void ListaReproduccionDetailWindow::updateCoverImage() {
    QPixmap coverPixmap(m_lista.getPortada());
    if (coverPixmap.isNull()) {
        coverPixmap = QPixmap(":/images/default_cover.png");
    }
    m_coverLabel->setPixmap(coverPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ListaReproduccionDetailWindow::onEditTitleClicked() {
    bool ok;
    QString newTitle = QInputDialog::getText(this, "Editar título",
                                             "Nuevo título:", QLineEdit::Normal,
                                             m_lista.getNombre(), &ok);
    if (ok && !newTitle.isEmpty()) {
        m_lista.setNombre(newTitle);
        m_titleLabel->setText(newTitle);
        setWindowTitle(newTitle);
        emit listaModificada(m_lista);
    }
}

void ListaReproduccionDetailWindow::onEditCoverClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Seleccionar nueva portada",
                                                    QDir::homePath(),
                                                    "Imágenes (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        m_lista.setPortada(fileName);
        for (const auto& cancion: m_lista.getCanciones()){
            m_cuentas->buscarCancionPorId(cancion)->setRutaPortada(fileName);
        }
        updateCoverImage();
        emit listaModificada(m_lista);
    }
}

void ListaReproduccionDetailWindow::onItemClicked(QListWidgetItem* item) {
    int cancionId = item->data(Qt::UserRole).toInt();
    for (int i = 0; i < m_canciones.size(); i++) {
        if (m_canciones[i]->getId() == cancionId) {
            m_indiceActual = i;
            emit solicitarReproduccionAlbum(m_canciones, i);
            break;
        }
    }
}

void ListaReproduccionDetailWindow::onAgregarCancionClicked() {
    emit agregarCancionALista(&m_lista);
}

void ListaReproduccionDetailWindow::reproducirSiguiente() {
    if (m_canciones.isEmpty()) return;
    int nextIndex = (m_indiceActual + 1) % m_canciones.size();
    m_indiceActual = nextIndex;
    emit solicitarReproduccionCancion(m_canciones.at(m_indiceActual));
}

void ListaReproduccionDetailWindow::reproducirAnterior() {
    if (m_canciones.isEmpty()) return;
    m_indiceActual = (m_indiceActual - 1 + m_canciones.size()) % m_canciones.size();
    emit solicitarReproduccionCancion(m_canciones.at(m_indiceActual));
}
