#include "artistawindow.h"
#include "ui_artistawindow.h"
#include "mainwindow.h"
#include "songwidget.h"
#include "albumwidget.h"
#include "albumdetailwindow.h"

#include <QFile>
#include <QMenu>
#include <QFontDatabase>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>



ArtistaWindow::ArtistaWindow(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::ArtistaWindow)
    ,  manejo(&manejo)
{
    ui->setupUi(this);

    usuario = manejo.getIdUsuarioActual();
    admin = dynamic_cast<Administrador*>(usuario);
    rutaImagen = usuario->getRutaImagen();

    //Actualizar Archivos en componentes
    fileWatcher = new QFileSystemWatcher(this);
    QString archivoCanciones = "canciones.dat";
    fileWatcher->addPath(archivoCanciones);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &ArtistaWindow::onCancionesFileChanged);

    connect(media, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            media->play();
        }
    });

    // En el constructor de ArtistaWindow
    media = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    media->setAudioOutput(audioOutput);

    // Configuración de buffers y tiempo de espera
    media->bufferProgress();
    media->setPlaybackRate(1.0); // Velocidad normal

    // Configuración de volumen
    audioOutput->setVolume(0.3);
    ui->horizontalSlider_Audio_Volume->setRange(1, 100);
    ui->horizontalSlider_Audio_Volume->setValue(30);

    // Conexiones esenciales
    connect(media, &QMediaPlayer::durationChanged, this, &ArtistaWindow::durationChanged);
    connect(media, &QMediaPlayer::positionChanged, this, &ArtistaWindow::positionChanged);
    connect(media, &QMediaPlayer::mediaStatusChanged, this, &ArtistaWindow::handleMediaStatusChanged);
    connect(media, &QMediaPlayer::errorOccurred, this, &ArtistaWindow::handleMediaError);

    // Estado inicial
    isPaused = true;
    isMuted = false;
    setIcono(ui->toolButton_play, 0xe037, 20); // Icono de play inicial

    loadSongs();
    cargarAlbumesUsuario();
    initComponents();
}

ArtistaWindow::~ArtistaWindow()
{
    delete ui;
}

void ArtistaWindow::playSong(const Cancion& cancion) {
    // 1. Actualizar la interfaz
    ui->label_3->setText(cancion.getTitulo());
    ui->label_2->setText(cancion.getArtista());

    // 2. Cargar portada
    QPixmap cover(cancion.getRutaPortada());
    if(cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png");
    }
    ui->label->setPixmap(cover.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 3. Detener reproducción actual si hay una
    media->stop();

    // 4. Cargar nueva canción
    currentSongPath = cancion.getRutaAudio();
    media->setSource(QUrl::fromLocalFile(currentSongPath));

    // 5. Actualizar botón (mostrar pausa porque se reproducirá automáticamente)
    setIcono(ui->toolButton_play, 0xe034, 20);
    isPaused = false;

    // La reproducción comenzará cuando el media esté cargado (handleMediaStatusChanged)
}

void ArtistaWindow::handleMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    qDebug() << "Estado del media:" << status;

    switch(status) {
    case QMediaPlayer::LoadedMedia:
        media->play();
        break;
    case QMediaPlayer::EndOfMedia:
        onCancionTerminada();
        break;
    case QMediaPlayer::InvalidMedia:
        qDebug() << "Error: Media inválido";
        break;
    default:
        break;
    }
}

void ArtistaWindow::handleMediaError(QMediaPlayer::Error error, const QString &errorString) {
    qDebug() << "Error en reproductor:" << error << "-" << errorString;
    QMessageBox::warning(this, "Error de reproducción",
                         QString("No se pudo reproducir la canción:\n%1").arg(errorString));
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
        item->setText(QString("%1 - %2").arg(cancion.titulo, cancion.genero));

        // Guarda solo la ruta (que es lo único que probablemente necesites)
        item->setData(Qt::UserRole, cancion.rutaAudio);

        ui->listaCancionesWidget->addItem(item);
    }
}

//Carga las canciones del catalogo
// *ignorar canciones eliminadas/ con ruta invalida
void ArtistaWindow::loadSongs()
{
    clearGrid();

    // Buscar canciones del artista actual usando el índice secundario
            QList<Cancion*> cancionesArtista = manejo->buscarCancionesPorArtista(usuario->getNombreUsuario());

    if(cancionesArtista.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No hay canciones disponibles");
        emptyLabel->setStyleSheet("color: #777; font-size: 16px;");
        ui->gridLayout_6->addWidget(emptyLabel, 0, 0, Qt::AlignCenter);
        return;
    }

    // Calcular número de columnas basado en el ancho del gridWidget
    int columnCount = qMax(1, ui->scrollArea->width() / 200);

    // Añadir canciones al grid
    for(int i = 0; i < cancionesArtista.size(); ++i) {
        // Asumiendo que SongWidget puede trabajar con Cancion* o necesitarás desreferenciar
        SongWidget *songWidget = new SongWidget(*cancionesArtista[i]);
        connect(songWidget, &SongWidget::songClicked, this, &ArtistaWindow::playSong);
        connect(songWidget, &SongWidget::editSongRequested, this, &ArtistaWindow::editarCancion);
        connect(songWidget, &SongWidget::deleteSongRequested, this, &ArtistaWindow::eliminarCancion);

        int row = i / columnCount;
        int col = i % columnCount;
        ui->gridLayout_6->addWidget(songWidget, row, col, Qt::AlignCenter);
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
    while ((child = ui->gridLayout_6->takeAt(0)) != nullptr) {
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
        ui->toolButton_upload->setEnabled(false);
        QMessageBox::information(this, "Álbum creado",QString("Álbum '%1' listo para agregar canciones").arg(titulo));


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
        Cancion* cancionReal = new Cancion(0,cancion.titulo,usuario->getNombreUsuario(),
                                           albumActual.titulo,cancion.genero,cancion.categoria,
                                           cancion.rutaAudio,albumActual.portada);
        manejo->agregarCancionAlbum(albumId,*cancionReal);
    }
    QMessageBox::information(this, "Exito","Tu album ahora esta disponible en el catalogo!");
    loadSongs();
    cargarAlbumesUsuario();

    // Limpiar para el próximo álbum
    ui->labelPortada->clear();
    ui->lineEditAlbum->clear();
    albumActual = AlbumTemp();
    ui->listaCancionesWidget->clear();
    ui->toolButton_addSong->setEnabled(false);
}

//Mostrar Albumes
void ArtistaWindow::cargarAlbumesUsuario() {
    ui->listWidget_6->clear();

    QList<Album*> albumes = manejo->buscarAlbumesPorArtista(usuario->getId());

    for (Album* album : albumes) {
        AlbumWidget* albumWidget = new AlbumWidget(*album, manejo); // Pasamos cuentas

        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(albumWidget->sizeHint());

        ui->listWidget_6->addItem(item);
        ui->listWidget_6->setItemWidget(item, albumWidget);

        connect(albumWidget, &AlbumWidget::clicked, this, [this, album]() {
            mostrarDetalleAlbum(album);
        });
    }
}

void ArtistaWindow::mostrarDetalleAlbum(Album* album) {
    AlbumDetailWindow* detailWindow = new AlbumDetailWindow(*album, manejo, this);

    int pageIndex = ui->stackedWidget->addWidget(detailWindow);
    ui->stackedWidget->setCurrentIndex(pageIndex);

    connect(detailWindow, &AlbumDetailWindow::destroyed, this, [this, pageIndex]() {
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(pageIndex));
    });

    connect(detailWindow, &AlbumDetailWindow::solicitarReproduccionCancion,
            this, &ArtistaWindow::reproducirCancion);

    connect(detailWindow, &AlbumDetailWindow::solicitarReproduccionAlbum,
            this, &ArtistaWindow::reproducirAlbumCompleto);

    // Conectar botones de navegación
    connect(ui->toolButton_next, &QToolButton::clicked,
            detailWindow, &AlbumDetailWindow::reproducirSiguiente);

    connect(ui->toolButton_prev, &QToolButton::clicked,
            detailWindow, &AlbumDetailWindow::reproducirAnterior);
}

void ArtistaWindow::reproducirCancion(Cancion* cancion) {
    playSong(*cancion);
    qDebug() << "Reproduciendo:" << cancion->getTitulo();
}

void ArtistaWindow::reproducirAlbumCompleto(Album* album, int indiceInicial) {
    m_cancionesAlbumActual.clear();
    for (int id : album->getCanciones()) {
        if (Cancion* cancion = manejo->buscarCancionPorId(id)) {
            m_cancionesAlbumActual.append(cancion);
        }
    }

    m_albumActual = album;
    m_indiceAlbumActual = indiceInicial;

    if (indiceInicial >= 0 && indiceInicial < m_cancionesAlbumActual.size()) {
        reproducirCancion(m_cancionesAlbumActual.at(indiceInicial));
    }
}

void ArtistaWindow::onCancionTerminada() {
    if (!m_cancionesAlbumActual.isEmpty()) {
        m_indiceAlbumActual = (m_indiceAlbumActual + 1) % m_cancionesAlbumActual.size();
        playSong(*m_cancionesAlbumActual.at(m_indiceAlbumActual));
    }
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


//Metodos y botones del MediaPlayer
void ArtistaWindow::updateduration(qint64 duration)
{
    QString timestr;
    if (duration > 0 && Mduration > 0)
    {
        QTime CurrentTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QTime totalTime((Mduration/3600)%60, (Mduration/60)%60, Mduration%60, (Mduration*1000)%1000);
        QString format = "mm:ss";
        if(Mduration > 3600){
            format = "hh:mm:ss";
        }
        ui->label_Value_1->setText(CurrentTime.toString(format));
        ui->label_Value_2->setText(totalTime.toString(format));
    }
}

void ArtistaWindow::on_horizontalSlider_Audio_Volume_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0);
}

void ArtistaWindow::on_toolButton_play_clicked() {
    if (media->playbackState() == QMediaPlayer::PlayingState) {
        media->pause();
        setIcono(ui->toolButton_play, 0xe037, 20); // Icono de play
    } else {
        // Si no hay fuente cargada pero hay canción actual
        if (media->source().isEmpty() && !currentSongPath.isEmpty()) {
            media->setSource(QUrl::fromLocalFile(currentSongPath));
        }
        media->play();
        setIcono(ui->toolButton_play, 0xe034, 20); // Icono de pausa
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

void ArtistaWindow::onCancionesFileChanged(const QString &path) {
    Q_UNUSED(path);

    loadSongs();

    if (QFile::exists(path) && !fileWatcher->files().contains(path)) {
        fileWatcher->addPath(path);
    }
}

void ArtistaWindow::durationChanged(qint64 duration) {
    Mduration = duration / 1000;
    ui->horizontalSlider_Audio_File_Duration->setMaximum(Mduration);
    updateduration(0); // Resetear el tiempo actual
}

void ArtistaWindow::positionChanged(qint64 progress) {
    if (!ui->horizontalSlider_Audio_File_Duration->isSliderDown()) {
        ui->horizontalSlider_Audio_File_Duration->setValue(progress / 1000);
    }
    updateduration(progress / 1000);
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
    cargarAlbumesUsuario();

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
}


void ArtistaWindow::initComponents()
{
    //Todo lo del mediaplayer
    audioOutput->setVolume(ui->horizontalSlider_Audio_Volume->value() / 100.0);


    ui->horizontalSlider_Audio_File_Duration->setRange(0, media->duration()/1000);

    connect(ui->lineEditAlbum, &QLineEdit::editingFinished, this, &ArtistaWindow::verificarYCrearAlbum, Qt::UniqueConnection);
    connect(ui->toolButton_upload, &QToolButton::clicked, this, &ArtistaWindow::seleccionarPortada, Qt::UniqueConnection);
    connect(ui->toolButton_addSong, &QToolButton::clicked, this, &ArtistaWindow::initPopup, Qt::UniqueConnection);
    connect(ui->toolButton_crear, &QToolButton::clicked, this, &ArtistaWindow::finalizarAlbum, Qt::UniqueConnection);
    // Poner el foco en el lineEdit del título
    ui->lineEditAlbum->setFocus();


    // Limpiar datos anteriores
    albumActual = AlbumTemp();
    ui->toolButton_addSong->setEnabled(false);


    int fontId = QFontDatabase::addApplicationFont(":/Montserrat-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (!families.isEmpty()) {
        Montserrat = families.at(0);  // ahora sí tienes el nombre real
    }

    int iconFontId = QFontDatabase::addApplicationFont(":/MaterialIcons-Regular.ttf");
    MaterialIcons = QFontDatabase::applicationFontFamilies(iconFontId).at(0);


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
            border: none;
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

    //Rating promedio
    /*
    int rating = 5;
    QString stars = "";
    for (int i = 0; i < rating; i++){
        stars +=   (QChar(0xf0ec));
    }
    ui->label_ratingProm->setText(stars); //Star unicode
    ui->label_ratingProm->setFont(QFont(MaterialIcons, 25));
    ui->label_ratingProm->setStyleSheet("color: yellow; border: none;");
    */
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
        if (!isPaused){
            media->pause();
        }
        manejo->cerrarSesion();
        MainWindow* m = new MainWindow(nullptr,*manejo);
        m->show();
        this->close();
    });

}

void ArtistaWindow::initPopup() {
    qDebug() << "Iniciando popup";

    QDialog* popup = new QDialog(this);
    popup->setWindowTitle("Registrar Canción");
    popup->setFixedSize(400, 330);
    popup->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    qDebug() << "Popup creado";

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

    qDebug() << "Widgets listos, aplicando estilo";

    popup->setStyleSheet("QDialog { background-color: white; }"); // simplificado

    connect(buscarBtn, &QPushButton::clicked, [rutaEdit]() {
        qDebug() << "Botón buscar presionado";
        QString fileName = QFileDialog::getOpenFileName(nullptr,"Seleccionar archivo de audio", "/Users/compu/Music","Audio Files (*.mp3 *.wav *.ogg)");
        if (!fileName.isEmpty()) {
            rutaEdit->setText(fileName);
            qDebug() << "Ruta seleccionada:" << fileName;
        } else {
            qDebug() << "No se seleccionó ningún archivo";
        }
    });

    connect(btnAceptar, &QPushButton::clicked, [this, popup, tituloEdit, generoCombo, categoriaCombo, rutaEdit]() {
        qDebug() << "Botón aceptar presionado";

        if (tituloEdit->text().isEmpty() || rutaEdit->text().isEmpty()) {
            QMessageBox::warning(popup, "Error", "Todos los campos son obligatorios");
            return;
        }

        qDebug() << "Creando canción";

        Song nuevaCancion;
        nuevaCancion.titulo=tituloEdit->text();
        nuevaCancion.genero=generoCombo->currentText();
        nuevaCancion.categoria=categoriaCombo->currentText();
        nuevaCancion.rutaAudio=rutaEdit->text();

        albumActual.canciones.append(nuevaCancion);
        qDebug() << "Canción agregada, actualizando lista";

        actualizarListaCanciones();  //

        popup->accept();
    });

    connect(btnCancelar, &QPushButton::clicked, popup, &QDialog::reject);

    qDebug() << "Ejecutando popup...";
    popup->exec();  // ← Si después de esto no hay logs, el programa muere aquí o en el botón aceptar
    qDebug() << "Popup cerrado correctamente";
}

