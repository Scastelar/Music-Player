#include "artistawindow.h"
#include "ui_artistawindow.h"
#include "mainwindow.h"
#include "songwidget.h"
#include "albumwidget.h"
#include "albumdetailwindow.h"
#include "halbumwidget.h"

#include <QFile>
#include <QMenu>
#include <QFontDatabase>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>
#include <QMediaMetaData>


ArtistaWindow::ArtistaWindow(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::ArtistaWindow)
    ,  manejo(&manejo)
{
    ui->setupUi(this);

    int fontId = QFontDatabase::addApplicationFont(":/Montserrat-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (!families.isEmpty()) {
        Montserrat = families.at(0);  // ahora sí tienes el nombre real
    }

    int iconFontId = QFontDatabase::addApplicationFont(":/MaterialIcons-Regular.ttf");
    MaterialIcons = QFontDatabase::applicationFontFamilies(iconFontId).at(0);

    usuario = manejo.getIdUsuarioActual();
    admin = dynamic_cast<Administrador*>(usuario);
    rutaImagen = usuario->getRutaImagen();

    // Estado inicial
    isPaused = true;
    m_autoPlayPending =  false;
    isMuted = false;
    setIcono(ui->toolButton_play, 0xe037, 20); // Icono de play inicial

    loadSongs();
    initComponents();
    cargarAlbumesUsuario("");

    //Actualizar Archivos en componentes
    fileWatcher = new QFileSystemWatcher(this);
    QString archivoCanciones = "canciones.dat";
    fileWatcher->addPath(archivoCanciones);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &ArtistaWindow::onCancionesFileChanged);

    media = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    media->setAudioOutput(audioOutput);

    // Configuración de volumen
    audioOutput->setMuted(false);
    audioOutput->setVolume(ui->horizontalSlider_Audio_Volume->value() / 100.0);
    ui->horizontalSlider_Audio_Volume->setRange(1, 100);
    ui->horizontalSlider_Audio_Volume->setValue(30);

    isRandom = false;
    isLoop = false;
    updateRandomTooltip();
    updateLoopTooltip();

    media->stop();


    connect(media, &QMediaPlayer::positionChanged, this, &ArtistaWindow::positionChanged);
    connect(media, &QMediaPlayer::mediaStatusChanged, this, &ArtistaWindow::onMediaStatusChanged);
    connect(media, &QMediaPlayer::durationChanged, this, &ArtistaWindow::onDurationChanged);

   // Nueva conexión para manejar el cambio de posición del slider
   connect(ui->horizontalSlider_Audio_File_Duration, &QSlider::sliderPressed,
           this, &ArtistaWindow::on_horizontalSlider_Audio_File_Duration_sliderPressed);
   connect(ui->horizontalSlider_Audio_File_Duration, &QSlider::sliderReleased,
           this, &ArtistaWindow::on_horizontalSlider_Audio_File_Duration_sliderReleased);
   connect(ui->horizontalSlider_Audio_File_Duration, &QSlider::valueChanged,
           this, &ArtistaWindow::on_horizontalSlider_Audio_File_Duration_valueChanged);



}



ArtistaWindow::~ArtistaWindow()
{
    delete ui;
}

void ArtistaWindow::playSong(Cancion& cancion) {
    ui->label_3->setText(cancion.getTitulo());
    ui->label_2->setText(cancion.getArtista());

    // Cargar portada
    QPixmap cover(cancion.getRutaPortada());
    if(cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png");
    }
    ui->label->setPixmap(cover.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // Detener la reproducción actual y resetear
    //media->stop();

    // Buscar el índice actual en m_cancionesAlbumActual
    for (int i = 0; i < m_cancionesAlbumActual.size(); ++i) {
        if (m_cancionesAlbumActual.at(i)->getId() == cancion.getId()) {
            m_indiceAlbumActual = i;
            break;
        }
    }

    // Configurar la nueva fuente
    media->setSource(QUrl::fromLocalFile(cancion.getRutaAudio()));

    // Preparar para reproducir
    isPaused = false;
    m_autoPlayPending = true;
    setIcono(ui->toolButton_play, 0xe034, 20);

    media->play();
}


void ArtistaWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if ((status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia)
        && m_autoPlayPending && !isPaused) {
        qDebug() << "Media cargada -> reproduciendo automáticamente!";
        m_autoPlayPending = false;
        media->play();
    }
}



void ArtistaWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
    qDebug() << "playbackStateChanged:" << state;
    // Opcional: si por alguna razón vuelve a Stopped justo después de querer tocar,
    // reintenta una sola vez si aún estaba pendiente (defensivo).
    if (state == QMediaPlayer::StoppedState && m_autoPlayPending) {
        qDebug() << "Retry play (stopped while pending)";
        m_autoPlayPending = false;
        QMetaObject::invokeMethod(media, [this](){ media->play(); }, Qt::QueuedConnection);
    }
}

void ArtistaWindow::onDurationChanged(qint64 durationMs) {
    Mduration = durationMs / 1000;
    ui->horizontalSlider_Audio_File_Duration->setMaximum(Mduration);

    updateduration(0);


    if (m_autoPlayPending) {
        m_autoPlayPending = false;
        QMetaObject::invokeMethod(media, [this](){
            media->play();
        }, Qt::QueuedConnection);
    }

    if (ui->horizontalSlider_Audio_File_Duration->value()==Mduration){

    }
}

void ArtistaWindow::updateduration(qint64 currentSeconds) {
    if (Mduration < 0) return;

    QTime current(0,0); current = current.addSecs(static_cast<int>(currentSeconds));
    QTime total(0,0);   total   = total.addSecs(static_cast<int>(Mduration));

    const QString format = (Mduration > 3600) ? "hh:mm:ss" : "mm:ss";
    ui->label_Value_1->setText(current.toString(format));
    ui->label_Value_2->setText(total.toString(format));
}


void ArtistaWindow::on_horizontalSlider_Audio_Volume_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0);
}

void ArtistaWindow::on_toolButton_play_clicked() {
    if (media->playbackState() == QMediaPlayer::PlayingState) {
        media->pause();
        isPaused = true;
        setIcono(ui->toolButton_play, 0xe037, 20); // icono play
    } else {
        // Si hay autoplay pendiente, ignóralo; solo juega si no está pendiente
        if (!m_autoPlayPending) {
            media->play();
        }
        isPaused = false;
        setIcono(ui->toolButton_play, 0xe034, 20); // icono pausa
    }
}


void ArtistaWindow::on_toolButton_Volume_clicked()
{
    if(!isMuted){
        ui->toolButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        isMuted = true;
        audioOutput->setMuted(true);
    }else{
        ui->toolButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        isMuted = false;
        audioOutput->setMuted(false);
    }
}

void ArtistaWindow::durationChanged(qint64 duration) {
    Mduration = duration / 1000;
    ui->horizontalSlider_Audio_File_Duration->setMaximum(Mduration);
    updateduration(0); // Resetear el tiempo actual
}

void ArtistaWindow::positionChanged(qint64 progress)
{
    if (!isUserSeeking) {
        ui->horizontalSlider_Audio_File_Duration->setValue(progress / 1000);
        updateduration(progress / 1000);
    }
}



void ArtistaWindow::on_horizontalSlider_Audio_File_Duration_sliderPressed()
{
    isUserSeeking = true;
}


void ArtistaWindow::on_horizontalSlider_Audio_File_Duration_sliderReleased()
{
    isUserSeeking = false;
    media->setPosition(ui->horizontalSlider_Audio_File_Duration->value() * 1000);
}


void ArtistaWindow::on_horizontalSlider_Audio_File_Duration_valueChanged(int value)
{
    if (isUserSeeking) {
        updateduration(value);
    }
}


void ArtistaWindow::reproducirCancion(Cancion* cancion) {
    playSong(*cancion);
    qDebug() << "Reproduciendo:" << cancion->getTitulo();
}

void ArtistaWindow::reproducirAlbumCompleto(const QList<Cancion*>& canciones, int indiceInicial) {
    m_cancionesAlbumActual = canciones;
    m_indiceAlbumActual = indiceInicial;

    if (!m_cancionesAlbumActual.isEmpty() &&
        m_indiceAlbumActual >= 0 &&
        m_indiceAlbumActual < m_cancionesAlbumActual.size()) {
        playSong(*m_cancionesAlbumActual.at(m_indiceAlbumActual));
    }
}

void ArtistaWindow::loadSongs()
{
    clearGrid();

    canciones.clear();
    // Buscar canciones del artista actual usando el índice secundario
    QList<Cancion*> temp = manejo->buscarCancionesPorArtista(usuario->getNombreUsuario());

    if(temp.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No hay canciones disponibles");
        emptyLabel->setStyleSheet("color: #777; font-size: 16px;");
        ui->gridLayout_5->addWidget(emptyLabel, 0, 0, Qt::AlignCenter);
        return;
    }

    for(Cancion* ptr : temp) {
        canciones.append(*ptr);  // Hacemos una copia
    }

    // Calcular número de columnas basado en el ancho del gridWidget
    int columnCount = qMax(1, ui->scrollArea->width() / 200);

    // Añadir canciones al grid
    for(int i = 0; i < canciones.size(); ++i) {
        SongWidget *songWidget = new SongWidget(canciones[i],this);
        connect(songWidget, &SongWidget::songClicked, this, &ArtistaWindow::playSong);
        connect(songWidget, &SongWidget::editSongRequested, this, &ArtistaWindow::editarCancion);
        connect(songWidget, &SongWidget::deleteSongRequested, this, &ArtistaWindow::eliminarCancion);

        int row = i / columnCount;
        int col = i % columnCount;
        ui->gridLayout_5->addWidget(songWidget, row, col, Qt::AlignCenter);
    }
}


void ArtistaWindow::loadSongs(const QString &genero)
{
    clearGrid();

    QList<Cancion*> cancionesFiltradas;

    if (genero.isEmpty()) {
        // Sin filtro → mostrar todas del artista actual
        cancionesFiltradas = manejo->buscarCancionesPorArtista(usuario->getNombreUsuario());
    } else {
        // Buscar por género
        QList<Cancion*> cancionesGenero = manejo->buscarCancionesPorGenero(genero);

        // Filtrar por artista actual
        for (Cancion* c : cancionesGenero) {
            if (c->getArtista() == usuario->getNombreUsuario()) {
                cancionesFiltradas.append(c);
            }
        }
    }

    if (cancionesFiltradas.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No hay canciones disponibles");
        emptyLabel->setStyleSheet("color: #777; font-size: 16px;");
        ui->gridLayout_5->addWidget(emptyLabel, 0, 0, Qt::AlignCenter);
        return;
    }

    // Calcular columnas
    int columnCount = qMax(1, ui->scrollArea->width() / 200);

    // Añadir canciones al grid
    for (int i = 0; i < cancionesFiltradas.size(); ++i) {
        SongWidget *songWidget = new SongWidget(*cancionesFiltradas[i]);
        connect(songWidget, &SongWidget::songClicked, this, &ArtistaWindow::playSong);
        connect(songWidget, &SongWidget::editSongRequested, this, &ArtistaWindow::editarCancion);
        connect(songWidget, &SongWidget::deleteSongRequested, this, &ArtistaWindow::eliminarCancion);

        int row = i / columnCount;
        int col = i % columnCount;
        ui->gridLayout_5->addWidget(songWidget, row, col, Qt::AlignCenter);
    }
}

//Mostrar Albumes
void ArtistaWindow::cargarAlbumesUsuario(const QString& tipo) {
    ui->listWidget->clear();

    QList<Album*> albumesFiltrados;

    //miau
    if (tipo.isEmpty()) {
        // Sin filtro → mostrar todos del artista actual
        albumesFiltrados = manejo->buscarAlbumesPorArtista(usuario->getId());
    } else {
        // Buscar por género
        QList<Album*> albumesTipo = manejo->buscarAlbumesPorTipo(tipo);

        // Filtrar por artista actual
        for (Album* a : albumesTipo) {
            if (a->getIdArtista() == usuario->getId()) {
                albumesFiltrados.append(a);
            }
        }
    }

    for (Album* album : albumesFiltrados) {

        HAlbumWidget* albumWidget = new AlbumWidget(*album, nullptr);
        ui->horizontalLayout_32->addWidget(albumWidget,0,Qt::AlignCenter);

        AlbumWidget* albumWidget = new AlbumWidget(*album, manejo); // Pasamos cuentas
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(albumWidget->sizeHint());

        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, albumWidget);

        connect(albumWidget, &AlbumWidget::clicked, this, [this, album]() {
            mostrarDetalleAlbum(album);
        });
    }
}



//Agrega temporalmente canciones al list widget
// *restringir canciones repetidas/existentes del catalogo
void ArtistaWindow::actualizarListaCanciones() {
    if (!ui->listaCancionesWidget) {
        qWarning() << "listaCancionesWidget es null";
        return;
    }
    ui->listaCancionesWidget->clear();

    for (const auto& cancion : albumActual.canciones) {
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(QString("%1 - %2").arg(cancion.getTitulo(), cancion.getGenero()));

        // Guarda solo la ruta (que es lo único que probablemente necesites)
        item->setData(Qt::UserRole, cancion.getRutaAudio());

        ui->listaCancionesWidget->addItem(item);
    }
}


void ArtistaWindow::editarCancion(int cancionId) {
    Cancion* cancion = manejo->buscarCancionPorId(cancionId);
    if (!cancion) return;

    QString nuevoTitulo = QInputDialog::getText(
        this, "Editar canción", "Título:",
        QLineEdit::Normal, cancion->getTitulo()
        );
    if (nuevoTitulo.isEmpty()) return;

    QString nuevoGenero = QInputDialog::getText(
        this, "Editar canción", "Género:",
        QLineEdit::Normal, cancion->getGenero()
        );
    if (nuevoGenero.isEmpty()) return;

    QString nuevaRutaAudio = QInputDialog::getText(
        this, "Editar canción", "Ruta de audio:",
        QLineEdit::Normal, cancion->getRutaAudio()
        );
    if (nuevaRutaAudio.isEmpty()) return;

    if (manejo->actualizarCancion(cancionId, nuevoTitulo, nuevoGenero, nuevaRutaAudio)) {
        QMessageBox::information(this, "Éxito", "La canción ha sido actualizada.");
    } else {
        QMessageBox::warning(this, "Error", "No se pudo actualizar la canción.");
    }
}


void ArtistaWindow::eliminarCancion(int cancionId){
    manejo->eliminarCancion(cancionId);
}

void ArtistaWindow::clearGrid()
{
    QLayoutItem *child;
    while ((child = ui->gridLayout_5->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}


// Metodos para crear un album
void ArtistaWindow::seleccionarPortada() {
    rutaPortada = QFileDialog::getOpenFileName(this,"Seleccionar portada", "/Users/compu/Pictures/Albums", "Imágenes (*.png *.jpg *.jpeg)");

    if (ui->labelPortada) {
        QPixmap pixmap(rutaPortada);
        ui->labelPortada->setPixmap(pixmap.scaled(ui->labelPortada->size(),
                                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    verificarYCrearAlbum(); // Verificar si ya podemos crear el álbum

}
void ArtistaWindow::verificarYCrearAlbum() {
    QString titulo = ui->lineEditAlbum->text().trimmed();

    // Verificar que tenemos todos los datos necesarios
    if (!titulo.isEmpty() && !rutaPortada.isEmpty() && QFile::exists(rutaPortada)) {
        // Guardar datos básicos del álbum
        albumActual.titulo = titulo;
        albumActual.portada = rutaPortada;
        albumActual.canciones.clear();

        // Mostrar el botón para agregar canciones
        ui->toolButton_addSong->setEnabled(true);
        ui->toolButton_addSong->setStyleSheet(
            "QToolButton { color: #DE5D83; }"
            "QToolButton:hover { color: #A94064; }"
            );
        QMessageBox::information(this, "Álbum creado",QString("Álbum '%1' listo para agregar canciones").arg(titulo));
        ui->toolButton_upload->setEnabled(false);
        ui->lineEditAlbum->setEnabled(false);
    }
}
void ArtistaWindow::finalizarAlbum() {
    if (albumActual.canciones.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe agregar al menos una canción");
        return;
    }

    // Crear el álbum en el sistema de cuentas
    bool creado = manejo->crearAlbum(usuario->getId(), albumActual.titulo, albumActual.portada);
    if (!creado) {
        QMessageBox::warning(this, "Error", "No se pudo crear el álbum");
        return;
    }

    // Obtener el ID del álbum recién creado (necesitarás un método para esto)
    int albumId = manejo->getUltimoIdLista();

    // Procesar cada canción
    for (const auto& cancion : albumActual.canciones) {
        manejo->agregarCancionAlbum(albumId,cancion);
    }

    QMessageBox::information(this, "Exito","Tu album ahora esta disponible en el catalogo!");
    loadSongs();
    cargarAlbumesUsuario("");

    // Limpiar para el próximo álbum
    ui->labelPortada->clear();
    ui->lineEditAlbum->clear();
    albumActual = AlbumTemp();
    ui->listaCancionesWidget->clear();
    ui->toolButton_addSong->setEnabled(false);
    ui->lineEditAlbum->setEnabled(true);
    ui->toolButton_addSong->setStyleSheet(
        "QToolButton { color: #white; }"
        "QToolButton:hover { color: #white; }"
        );
}



void ArtistaWindow::agregarCancionAlAlbum(const Album* album) {
    QDialog* popup = new QDialog(this);
    popup->setWindowTitle("Registrar Canción");
    popup->setFixedSize(400, 330);
    popup->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QVBoxLayout* layout = new QVBoxLayout(popup);
    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* tituloEdit = new QLineEdit(popup);
    formLayout->addRow("Título:", tituloEdit);

    QComboBox* generoCombo = new QComboBox(popup);
    generoCombo->addItems({"Clásico", "Pop", "Electrónica", "Rock", "Reguetón", "Cristianos", "Corridos"});
    formLayout->addRow("Género:", generoCombo);

    QComboBox* categoriaCombo = new QComboBox(popup);
    categoriaCombo->addItems({"Recomendado", "Favorito", "Infantil", "Instrumental"});
    formLayout->addRow("Categoría:", categoriaCombo);

    QHBoxLayout* rutaLayout = new QHBoxLayout();
    QLineEdit* rutaEdit = new QLineEdit(popup);
    QPushButton* buscarBtn = new QPushButton("Buscar", popup);
    rutaLayout->addWidget(rutaEdit);
    rutaLayout->addWidget(buscarBtn);
    formLayout->addRow("Ruta del audio:", rutaLayout);

    layout->addLayout(formLayout);

    QHBoxLayout* botonesLayout = new QHBoxLayout();
    QPushButton* btnAceptar = new QPushButton("Aceptar", popup);
    QPushButton* btnCancelar = new QPushButton("Cancelar", popup);
    botonesLayout->addWidget(btnAceptar);
    botonesLayout->addWidget(btnCancelar);
    layout->addLayout(botonesLayout);

    connect(buscarBtn, &QPushButton::clicked, [rutaEdit]() {
        QString fileName = QFileDialog::getOpenFileName(nullptr,"Seleccionar archivo de audio", "", "Audio Files (*.mp3 *.wav *.ogg)");
        if (!fileName.isEmpty()) {
            rutaEdit->setText(fileName);
        }
    });

    connect(btnAceptar, &QPushButton::clicked, [this, popup, album, tituloEdit, generoCombo, categoriaCombo, rutaEdit]() {
        if (tituloEdit->text().isEmpty() || rutaEdit->text().isEmpty()) {
            QMessageBox::warning(popup, "Error", "Todos los campos son obligatorios");
            return;
        }

        Cancion* nuevaCancion = new Cancion(
            0, // ID temporal
            tituloEdit->text(),
            usuario->getNombreUsuario(),
            album->getNombre(),
            generoCombo->currentText(),
            categoriaCombo->currentText(),
            rutaEdit->text(),
            album->getPortada()
            );

        manejo->agregarCancionAlbum(album->getId(), *nuevaCancion);

        popup->accept();
    });

    connect(btnCancelar, &QPushButton::clicked, popup, &QDialog::reject);

    popup->exec();
}

void ArtistaWindow::on_toolButton_random_clicked()
{
    isRandom = !isRandom;

    // Si activamos random, desactivamos loop
    if (isRandom && isLoop) {
        isLoop = false;
        ui->toolButton_loop->setStyleSheet(QString("QToolButton { color: %1; }").arg(COLOR_INACTIVO.name()));
        updateLoopTooltip();
    }

    // Cambiar color según el estado
    ui->toolButton_random->setStyleSheet(QString("QToolButton { color: %1; }")
                                             .arg(isRandom ? COLOR_ACTIVO.name() : COLOR_INACTIVO.name()));

    updateRandomTooltip();
}

void ArtistaWindow::on_toolButton_loop_clicked()
{
    isLoop = !isLoop;

    // Si activamos loop, desactivamos random
    if (isLoop && isRandom) {
        isRandom = false;
        ui->toolButton_random->setStyleSheet(QString("QToolButton { color: %1; }").arg(COLOR_INACTIVO.name()));
        updateRandomTooltip();
    }

    // Cambiar color según el estado
    ui->toolButton_loop->setStyleSheet(QString("QToolButton { color: %1; }")
                                           .arg(isLoop ? COLOR_ACTIVO.name() : COLOR_INACTIVO.name()));

    updateLoopTooltip();
}

void ArtistaWindow::updateRandomTooltip()
{
    QString tooltip = isRandom ?
                          tr("Modo aleatorio activado\nClic para desactivar") :
                          tr("Modo aleatorio desactivado\nClic para activar");

    ui->toolButton_random->setToolTip(tooltip);
}

void ArtistaWindow::updateLoopTooltip()
{
    QString tooltip = isLoop ?
                          tr("Modo repetición activado\nClic para desactivar") :
                          tr("Modo repetición desactivado\nClic para activar");

    ui->toolButton_loop->setToolTip(tooltip);
}


void ArtistaWindow::onCancionTerminada()
{
    if (m_cancionesAlbumActual.isEmpty()) return;

    if (isLoop) {
        // Repetir la misma canción
        media->setPosition(0);
        media->play();
        return;
    }

    // Determinar la siguiente canción según el modo random
    int nextIndex;
    if (isRandom) {
        // Para evitar que se repita la misma canción si hay más de una
        if (m_cancionesAlbumActual.size() > 1) {
            do {
                nextIndex = QRandomGenerator::global()->bounded(m_cancionesAlbumActual.size());
            } while (nextIndex == m_indiceAlbumActual);
        } else {
            nextIndex = 0; // Solo hay una canción
        }
    } else {
        nextIndex = (m_indiceAlbumActual + 1) % m_cancionesAlbumActual.size();
    }

    // Actualizar el índice y reproducir
    m_indiceAlbumActual = nextIndex;
    playSong(*m_cancionesAlbumActual.at(m_indiceAlbumActual));
}


void ArtistaWindow::onRandomModeToggled(bool checked) {
    isRandom = checked;
    emit randomModeChanged(checked);
}

void ArtistaWindow::mostrarDetalleAlbum(Album* album) {
    AlbumDetailWindow* detailWindow = new AlbumDetailWindow(*album, manejo, nullptr);

    int pageIndex = ui->stackedWidget->addWidget(detailWindow);
    ui->stackedWidget->setCurrentIndex(pageIndex);

    detailWindow->setRandomMode(isRandom);

    connect(detailWindow, &AlbumDetailWindow::destroyed, this, [this, detailWindow]() {
        ui->stackedWidget->removeWidget(detailWindow);
    });

    connect(detailWindow, &AlbumDetailWindow::solicitarReproduccionCancion,
            this, &ArtistaWindow::reproducirCancion);


    // Modificar la conexión en mostrarDetalleAlbum
    connect(detailWindow, &AlbumDetailWindow::solicitarReproduccionAlbum,
            this, &ArtistaWindow::reproducirAlbumCompleto);

    // Conectar botones de navegación
    connect(ui->toolButton_next, &QToolButton::clicked, this, [this, detailWindow]() {
        if (isRandom) {
            // Forzar modo aleatorio incluso si la ventana de detalle no está sincronizada
            detailWindow->setRandomMode(true);
        }
        detailWindow->reproducirSiguiente();
    });

    connect(this, &ArtistaWindow::randomModeChanged, detailWindow, [detailWindow](bool enabled) {
        detailWindow->setRandomMode(enabled);
      });

    connect(ui->toolButton_prev, &QToolButton::clicked,
            detailWindow, &AlbumDetailWindow::reproducirAnterior);

    connect(detailWindow, &AlbumDetailWindow::agregarCancionAlAlbum,
            this, &ArtistaWindow::agregarCancionAlAlbum);

    // Conexión para manejar el final de la canción
    connect(media, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            onCancionTerminada();
        }
    });

}


//Editar Perfil
void ArtistaWindow::EditarPerfil(){

    QPixmap avatar(admin->getRutaImagen());
    QPixmap avatarEscalado = avatar.scaled(
        ui->editPerfilLbl->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    ui->editPerfilLbl->setPixmap(avatarEscalado);
    ui->editUserLbl->setText(admin->getNombreUsuario());
    ui->editNomLbl->setText(admin->getNombreReal());
    ui->editPaisLbl->setText(admin->getPais());
    ui->editGeneroCB->setCurrentText(admin->getGenero());
    ui->editDescLbl->setText(admin->getDescripcion());
    rutaImagen = admin->getRutaImagen();

    ui->editPassLbl->setEchoMode(QLineEdit::Password);
    ui->editPassLbl->setEnabled(false);
    ui->editPassLbl->setText("********");
}

//Vista previa del perfil
void ArtistaWindow::VistaPerfil(){
    QPixmap avatar(admin->getRutaImagen());
    QPixmap avatarEscalado = avatar.scaled(
        ui->PFPLabel->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    ui->PFPLabel->setPixmap(avatarEscalado);
    ui->USERlabel->setText(usuario->getNombreUsuario());
    ui->USERlabel_2->setText(usuario->getNombreReal() + " - " + admin->getPais() + " - " + admin->getGenero());
    ui->DESCtext->setText(admin->getDescripcion());
}



void ArtistaWindow::onCancionesFileChanged(const QString &path) {
    Q_UNUSED(path);

    loadSongs();

    if (QFile::exists(path) && !fileWatcher->files().contains(path)) {
        fileWatcher->addPath(path);
    }
}


//Metodos y Botones del UI
void ArtistaWindow::setIcono(QToolButton* boton, ushort unicode, int size){
    boton->setText(QChar(unicode));
    boton->setFont(QFont(MaterialIcons, size)); // Tamaño 20
    boton->setStyleSheet("color: white; border: none;");
}


void ArtistaWindow::on_toolButton_Playlist_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->toolButton_upload->setEnabled(true);
}

void ArtistaWindow::on_toolButton_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    loadSongs();
    cargarAlbumesUsuario("");

}

void ArtistaWindow::on_lineEdit_editingFinished()
{
    ui->stackedWidget->setCurrentIndex(4);

}

void ArtistaWindow::on_toolButton_limpiar_clicked()
{
    albumActual = AlbumTemp();
    ui->listaCancionesWidget->clear();
    ui->toolButton_addSong->setEnabled(false);
    ui->labelPortada->clear();
    ui->lineEditAlbum->clear();

    ui->toolButton_upload->setEnabled(true);
    ui->lineEditAlbum->setEnabled(true);
    ui->toolButton_addSong->setStyleSheet(
        "QToolButton { color: #white; }"
        "QToolButton:hover { color: #white; }"
        );
}


void ArtistaWindow::on_toolButton_addSong_clicked()
{
    // Abrir diálogo para seleccionar archivos WAV
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Seleccionar canciones"),
        QDir::homePath(),
        tr("Archivos de audio (*.wav *.WAV *.flac *.m4a)")
        );

    if (filePaths.isEmpty()) {
        return; // El usuario canceló
    }

    QMediaPlayer player;
    QAudioOutput audioOutput;
    player.setAudioOutput(&audioOutput);

    for (const QString &filePath : filePaths) {
        player.setSource(QUrl::fromLocalFile(filePath));

        // Esperar a que se carguen los metadatos (asíncrono)
        QEventLoop loop;
        QObject::connect(&player, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
        loop.exec();

        // Obtener metadatos
        QString titulo = player.metaData().value(QMediaMetaData::Title).toString();
        QString artista = player.metaData().value(QMediaMetaData::AlbumArtist).toString();
        QString album = player.metaData().value(QMediaMetaData::AlbumTitle).toString();
        QString genero = player.metaData().value(QMediaMetaData::Genre).toString();
        int duracion = player.metaData().value(QMediaMetaData::Duration).toInt();
        QDateTime fecha = player.metaData().value(QMediaMetaData::Date).toDateTime();

        QString categoria = "Recomendado";

        if (titulo.isEmpty()) titulo = QFileInfo(filePath).baseName();
        if (artista.isEmpty()) artista = "-";
        if (album.isEmpty()) album = "-";
        if (genero.isEmpty()) genero = "-";
        if (genero == "Clasica")categoria = "Instrumental";

            Cancion nuevaCancion(
            0,                           //Como es una cancion temporal no importa el ID
            titulo,
            usuario->getNombreUsuario(),
            album,
            genero,
            categoria,
            filePath,
            albumActual.portada
            );

            nuevaCancion.setfechaRegistro(fecha);
            nuevaCancion.setDuracion(duracion);
            albumActual.canciones.append(nuevaCancion);
    }
    // Aquí podrías emitir una señal o actualizar la UI para reflejar los cambios
    qDebug() << "Canciones agregadas al álbum:" << albumActual.titulo
             << "Total canciones:" << albumActual.canciones.size();
    actualizarListaCanciones();
}



void ArtistaWindow::initComponents()
{

    ui->toolButton_crear->setStyleSheet(
        "QToolButton { background-color: #DE5D83; color: white; padding: 6px 12px; border-radius: 4px; }"
        "QToolButton:hover { background-color: #A94064; }"
        );

    connect(ui->lineEditAlbum, &QLineEdit::editingFinished, this, &ArtistaWindow::verificarYCrearAlbum, Qt::UniqueConnection);
    connect(ui->toolButton_upload, &QToolButton::clicked, this, &ArtistaWindow::seleccionarPortada, Qt::UniqueConnection);
    //connect(ui->toolButton_addSong, &QToolButton::clicked, this, &ArtistaWindow::initPopup, Qt::UniqueConnection);
    connect(ui->toolButton_crear, &QToolButton::clicked, this, &ArtistaWindow::finalizarAlbum, Qt::UniqueConnection);
    // Poner el foco en el lineEdit del título
    ui->lineEditAlbum->setFocus();

    connect(ui->toolButton_clasico,     &QToolButton::clicked, this, [=]() { loadSongs("Clasico"); });
    connect(ui->toolButton_pop,         &QToolButton::clicked, [this]() { loadSongs("Pop"); });
    connect(ui->toolButton_elec,        &QToolButton::clicked, [this]() { loadSongs("Electronica"); });
    connect(ui->toolButton_rock,        &QToolButton::clicked, [this]() { loadSongs("Rock"); });
    connect(ui->toolButton_regueton,    &QToolButton::clicked, [this]() { loadSongs("Regueton"); });
    connect(ui->toolButton_cristianos,  &QToolButton::clicked, [this]() { loadSongs("Cristianos"); });
    connect(ui->toolButton_corridos,    &QToolButton::clicked, [this]() { loadSongs("Corridos"); });

    connect(ui->btnFilterAlbum,     &QToolButton::clicked, [this]() { cargarAlbumesUsuario("Album"); });
    connect(ui->btnFilterEP,        &QToolButton::clicked, [this]() { cargarAlbumesUsuario("EP"); });
    connect(ui->btnFilterSingle,    &QToolButton::clicked, [this]() { cargarAlbumesUsuario("Single"); });

    // Limpiar datos anteriores
    albumActual = AlbumTemp();
    ui->toolButton_addSong->setEnabled(false);



    //Componentes UI
    connect(ui->editUploadButton, &QToolButton::clicked, this, [this](){
        rutaImagen = QFileDialog::getOpenFileName(this, "Seleccionar imagen de perfil", "/Users/compu/Pictures/Albums", "Imagenes (*.jpg *.jpeg *.png)");

        if (!rutaImagen.isEmpty()) {
            QPixmap avatar(rutaImagen);

            // Escalar al tamaño del QLabel
            QPixmap avatarEscalado = avatar.scaled(
                ui->editPerfilLbl->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );

            ui->editPerfilLbl->setPixmap(avatarEscalado);
        }

    });


    connect(ui->editGuardarButton, &QToolButton::clicked, this, [=](){
        usuario->setRutaImagen(rutaImagen);
        QString user = ui->editUserLbl->text().toLower();
        QString nombre = ui->editNomLbl->text();
        QString pais = ui->editPaisLbl->text();
        QString genero = ui->editGeneroCB->currentText();
        QString desc = ui->editDescLbl->toPlainText();
        QString pass = ui->editPassLbl->text();

        QPixmap avatar(rutaImagen);

        if (ui->editPassLbl->text()=="********"){
            pass = usuario->getContrasena();
        }

        if (manejo->editarUsuario(usuario->getNombreUsuario(),user,pass,nombre,"",usuario->getRutaImagen(),pais,genero,desc)){
            QMessageBox::information(this, "Exito", "Su perfil ha sido editado!");
            QIcon icon(avatar.scaled(ui->btnPerfil->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation));
            ui->btnPerfil->setIcon(icon);
            ui->btnPerfil->setIconSize(ui->btnPerfil->size());
            ui->stackedWidget->setCurrentIndex(0);
        }else {
            QMessageBox::warning(this, "Error", "Su perfil no fue modificado");
        }
    });

    QPixmap logo;
    logo.load(":/imagenes/logoW.png");
    QPixmap logoEscalado = logo.scaled(ui->labelLogo->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    ui->labelLogo->setPixmap(logoEscalado);



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
            border: none; border-radius: 8px;
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
        QTextEdit {
            background-color: white;
            color: rgb(40,40,40);
        }
        QComboBox {
            background-color: white;
            color: rgb(40,40,40);
        }
        QToolTip {
               color: #ffffff;
               background-color: #333333;
               border: 1px solid #555555;
               padding: 3px;
               border-radius: 3px;
               opacity: 230;
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
    verPerfil = menu->addAction("Ver Perfil");
    menu->addSeparator();
    cerrarSesion = menu->addAction("Cerrar sesión");

    ui->btnPerfil->setMenu(menu);
    ui->btnPerfil->setPopupMode(QToolButton::InstantPopup);
    conectarMenu();

    // Conectar un botón que ya deberías tener en tu UI
    connect(ui->btnCambiarPass, &QPushButton::clicked, this, [this]() {
        bool ok;
        QString currentPassword = QInputDialog::getText(
            this,
            "Verificar contraseña",
            "Ingrese su contraseña actual:",
            QLineEdit::Password,
            "",
            &ok
            );

        if (ok && manejo->autenticar(usuario->getNombreUsuario(),currentPassword)!=nullptr) {
            ui->editPassLbl->setEnabled(true);
            ui->editPassLbl->setText("");
            ui->editPassLbl->setFocus();
        }
        else if (ok) {
            QMessageBox::warning(this, "Error", "Contraseña incorrecta");
        }
    });

    setIcono(ui->toolButton_Playlist,0xe03b,20); //0xe03b
    setIcono(ui->toolButton_home,0xe88a,20); //home 0xe88a
    //setIcono(ui->toolButton_play,0xe037,20);
    setIcono(ui->toolButton_prev,0xe045,20); //e045
    setIcono(ui->toolButton_next,0xe044,20);
    setIcono(ui->toolButton_random,0xe043,20);
    setIcono(ui->toolButton_loop,0xe040,20);
    //setIcono(ui->toolButton_fav,0xe87d,20);
    setIcono(ui->toolButton_upload,0xe413,20);
    setIcono(ui->editUploadButton,0xe413,20); //e43e
    setIcono(ui->toolButton_addSong,0xe7e5,20);
    setIcono(ui->toolButton_limpiar,0xe5d5,20);
    ui->toolButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

}

void ArtistaWindow::conectarMenu(){
    connect(editarDatos, &QAction::triggered, this, [=]() {
        qDebug() << "Editar datos";
        ui->stackedWidget->setCurrentIndex(2);
        EditarPerfil();
    });

    connect(verEstadisticas, &QAction::triggered, this, [this]() {
        qDebug() << "Ver estadísticas";
        ui->stackedWidget->setCurrentIndex(3);
    });


    connect(verPerfil, &QAction::triggered, this, [this]() {
        qDebug() << "Ver Perfil";
        ui->stackedWidget->setCurrentIndex(5);
        VistaPerfil();
    });

    connect(cerrarSesion, &QAction::triggered, this, [this]() {
        qDebug() << "Cerrar sesión";
        manejo->cerrarSesion();
        MainWindow* m = new MainWindow(nullptr,*manejo);
        m->show();
        this->close();
    });

}





