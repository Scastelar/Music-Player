#include "HomeWindow_Copy.h"
#include "mainwindow.h"
#include "ui_HomeWindow_Copy.h"
#include "songwidget.h"

#include <QFile>
#include <QMenu>
#include <QFontDatabase>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>



// Conectar señales solo una vez
//static bool connected = false;


HomeWindow_Copy::HomeWindow_Copy(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow_Copy)
    ,  manejo(&manejo)
{
    ui->setupUi(this);

    qRegisterMetaType<Song>("Song");
    usuario = manejo.getIdUsuarioActual();
    admin = dynamic_cast<Administrador*>(usuario);


    //Actualizar Archivos en componentes
    fileWatcher = new QFileSystemWatcher(this);
    QString archivoCanciones = "canciones.dat";
    fileWatcher->addPath(archivoCanciones);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &HomeWindow_Copy::onCancionesFileChanged);

    loadSongs();
    initComponents();
    EditarPerfil();

}

HomeWindow_Copy::~HomeWindow_Copy()
{
    delete ui;
}

void HomeWindow_Copy::playSong(const Cancion& cancion) {
    // Implementa la lógica para reproducir la canción
    ui->label_3->setText(cancion.getTitulo());
    ui->label_2->setText(cancion.getArtista());

    // Cargar portada
    QPixmap cover(cancion.getRutaPortada());
    if(cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png");
    }
    ui->label->setPixmap(cover.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    media->setSource(QUrl::fromLocalFile(cancion.getRutaAudio()));
    setIcono(ui->toolButton_play,0xe034,20);
    media->play();
}


//Agrega temporalmente canciones al list widget
// *restringir canciones repetidas/existentes del catalogo
void HomeWindow_Copy::actualizarListaCanciones() {

    if (!ui->listaCancionesWidget) {
        qWarning() << "listaCancionesWidget es null, no se puede actualizar.";
        return;
    }
    ui->listaCancionesWidget->clear();

    for (const auto& cancion : albumActual.canciones) {
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(QString("%1 - %2").arg(cancion.titulo, cancion.genero));
        item->setData(Qt::UserRole, QVariant::fromValue(cancion)); // Guardamos el objeto completo
        ui->listaCancionesWidget->addItem(item);
    }
}


//Carga las canciones del catalogo
// *ignorar canciones eliminadas/ con ruta invalida
void HomeWindow_Copy::loadSongs()
{
    clearGrid();

    // Cargar canciones desde el archivo
    QList<Cancion> canciones = Cancion::cargarDesdeArchivo("canciones.dat");

    if(canciones.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No hay canciones disponibles");
        emptyLabel->setStyleSheet("color: #777; font-size: 16px;");
        ui->gridLayout->addWidget(emptyLabel, 0, 0, Qt::AlignCenter);
        return;
    }

    // Calcular número de columnas basado en el ancho del gridWidget
    int columnCount = qMax(1, ui->scrollArea->width() / 200);

    // Añadir canciones al grid
    for(int i = 0; i < canciones.size(); ++i) {
        SongWidget *songWidget = new SongWidget(canciones[i]);
        connect(songWidget, &SongWidget::songClicked, this, &HomeWindow_Copy::playSong);

        int row = i / columnCount;
        int col = i % columnCount;
        ui->gridLayout->addWidget(songWidget, row, col, Qt::AlignCenter);
    }
}
void HomeWindow_Copy::clearGrid()
{
    QLayoutItem *child;
    while ((child = ui->gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}


// Metodos para crear un album
void HomeWindow_Copy::seleccionarPortada() {
    rutaPortada = QFileDialog::getOpenFileName(this,"Seleccionar portada", "/Users/compu/Pictures/Albums", "Imágenes (*.png *.jpg *.jpeg)");

    if (ui->label_8) {
        QPixmap pixmap(rutaPortada);
        ui->label_8->setPixmap(pixmap.scaled(ui->label_8->size(),
                                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
        verificarYCrearAlbum(); // Verificar si ya podemos crear el álbum

}
void HomeWindow_Copy::verificarYCrearAlbum() {
    QString titulo = ui->lineEdit_2->text().trimmed();

    // Verificar que tenemos todos los datos necesarios
    if (!titulo.isEmpty() && !rutaPortada.isEmpty() && QFile::exists(rutaPortada)) {
        // Guardar datos básicos del álbum
        albumActual.titulo = titulo;
        albumActual.portada = rutaPortada;
        albumActual.canciones.clear();

        // Mostrar el botón para agregar canciones
        ui->toolButton_addSong->setEnabled(true);

        QMessageBox::information(this, "Álbum creado",QString("Álbum '%1' listo para agregar canciones").arg(titulo));


    }
}
void HomeWindow_Copy::finalizarAlbum() {
    if (albumActual.canciones.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe agregar al menos una canción");
        return;
    }

    // Procesar cada canción
    for (const auto& cancion : albumActual.canciones) {
        admin->agregarCancionCatalogo(
            cancion.titulo,
            cancion.genero,
            cancion.categoria,
            albumActual.portada,
            cancion.rutaAudio
            );
    }

    QMessageBox::information(this, "Exito","Tu album ahora esta disponible en el catalogo!");

    // Limpiar para el próximo álbum
    ui->label_8->clear();
    ui->lineEdit_2->clear();
    albumActual = Album();
    ui->listaCancionesWidget->clear();
    ui->toolButton_addSong->setEnabled(false);
}

//Editar Perfil
void HomeWindow_Copy::EditarPerfil(){

    QPixmap avatar(admin->getRutaImagen());
    QPixmap avatarEscalado = avatar.scaled(
        ui->editPerfilLbl->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    ui->editPerfilLbl->setPixmap(avatarEscalado);
    ui->editUserLbl->setText(admin->getNombreUsuario());
    ui->editNomLbl->setText(admin->getNombreReal());
    ui->editPassLbl->setText("*****");
    ui->editPaisLbl->setText(admin->getPais());
    ui->editGeneroCB->setCurrentText(admin->getGenero());
    ui->editDescLbl->setText(admin->getDescripcion());

    connect(ui->editUploadButton, &QToolButton::clicked, this, [this](){
    QString rutaImagen = QFileDialog::getOpenFileName(this, "Seleccionar imagen de perfil", "/Users/compu/Pictures/Albums", "Imagenes (*.jpg *.jpeg *.png)");

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



    });



}




//Metodos y botones del MediaPlayer
void HomeWindow_Copy::updateduration(qint64 duration)
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

void HomeWindow_Copy::on_horizontalSlider_Audio_Volume_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0);
}

void HomeWindow_Copy::on_toolButton_play_clicked()
{
    if (!isPaused){
        setIcono(ui->toolButton_play,0xe037,20);
        isPaused = true;
        media->pause();
    }else {
        setIcono(ui->toolButton_play,0xe034,20);
        isPaused = false;
            media->play();
    }
}

void HomeWindow_Copy::on_toolButton_Volume_clicked()
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

void HomeWindow_Copy::onCancionesFileChanged(const QString &path) {
    Q_UNUSED(path);

    loadSongs();

    if (QFile::exists(path) && !fileWatcher->files().contains(path)) {
        fileWatcher->addPath(path);
    }
}

void HomeWindow_Copy::durationChanged(qint64 duration)
{
    Mduration = duration/1000;
    ui->horizontalSlider_Audio_File_Duration->setMaximum(Mduration);
}

void HomeWindow_Copy::positionChanged(qint64 progress)
{
    if(!ui->horizontalSlider_Audio_File_Duration->isSliderDown())
    {
        ui->horizontalSlider_Audio_File_Duration->setValue(progress/1000);
    }
    updateduration(progress/1000);
}


//Metodos y Botones del UI
void HomeWindow_Copy::setIcono(QToolButton* boton, ushort unicode, int size){
    boton->setText(QChar(unicode));
    boton->setFont(QFont(MaterialIcons, size)); // Tamaño 20
    boton->setStyleSheet("color: white; border: none;");
}

void HomeWindow_Copy::on_toolButton_Playlist_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void HomeWindow_Copy::on_toolButton_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}

void HomeWindow_Copy::on_lineEdit_editingFinished()
{
    ui->stackedWidget->setCurrentIndex(4);

}

void HomeWindow_Copy::on_toolButton_limpiar_clicked()
{
    albumActual = Album();
    ui->listaCancionesWidget->clear();
    ui->toolButton_addSong->setEnabled(false);
}


void HomeWindow_Copy::initComponents()
{
    //Todo lo del mediaplayer
    media = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    media->setAudioOutput(audioOutput);

    audioOutput->setVolume(0.3);  // Set default volume (30%)


    ui->horizontalSlider_Audio_Volume->setMinimum(1);
    ui->horizontalSlider_Audio_Volume->setMaximum(100);
    ui->horizontalSlider_Audio_Volume->setValue(30);

    audioOutput->setVolume(ui->horizontalSlider_Audio_Volume->value() / 100.0);

    connect(media, &QMediaPlayer::durationChanged, this, &HomeWindow_Copy::durationChanged);
    connect(media, &QMediaPlayer::positionChanged, this, &HomeWindow_Copy::positionChanged);

    ui->horizontalSlider_Audio_File_Duration->setRange(0, media->duration()/1000);

    connect(ui->lineEdit_2, &QLineEdit::editingFinished, this, &HomeWindow_Copy::verificarYCrearAlbum, Qt::UniqueConnection);
    connect(ui->toolButton_upload, &QToolButton::clicked, this, &HomeWindow_Copy::seleccionarPortada, Qt::UniqueConnection);
    connect(ui->toolButton_addSong, &QToolButton::clicked, this, &HomeWindow_Copy::initPopup, Qt::UniqueConnection);
    connect(ui->toolButton_upload_3, &QToolButton::clicked, this, &HomeWindow_Copy::finalizarAlbum, Qt::UniqueConnection);
    // Poner el foco en el lineEdit del título
    ui->lineEdit_2->setFocus();


    // Limpiar datos anteriores
    albumActual = Album();
    ui->toolButton_addSong->setEnabled(false);


    int fontId = QFontDatabase::addApplicationFont(":/Montserrat-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (!families.isEmpty()) {
        Montserrat = families.at(0);  // ahora sí tienes el nombre real
    }

    int iconFontId = QFontDatabase::addApplicationFont(":/MaterialIcons-Regular.ttf");
    MaterialIcons = QFontDatabase::applicationFontFamilies(iconFontId).at(0);


    //Componentes UI
    ui->verticalFrame->setFixedWidth(200);

    QPixmap logo;
    logo.load(":/imagenes/logoW.png");
    QPixmap logoEscalado = logo.scaled(ui->label_5->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    ui->label_5->setPixmap(logoEscalado);



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

    setIcono(ui->toolButton_Playlist,0xe03b,20); //0xe03b
    setIcono(ui->toolButton_home,0xe88a,20); //home 0xe88a
    setIcono(ui->toolButton_play,0xe037,20);
    setIcono(ui->toolButton_prev,0xe045,20); //e045
    setIcono(ui->toolButton_next,0xe044,20);
    setIcono(ui->toolButton_random,0xe043,20);
    setIcono(ui->toolButton_loop,0xe040,20);
    setIcono(ui->toolButton_fav,0xe87d,20);
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

void HomeWindow_Copy::conectarMenu(){
    connect(editarDatos, &QAction::triggered, this, [=]() {
        qDebug() << "Editar datos";
        ui->stackedWidget->setCurrentIndex(2);
    });

    connect(verEstadisticas, &QAction::triggered, this, [this]() {
        qDebug() << "Ver estadísticas";
        ui->stackedWidget->setCurrentIndex(3);
    });

    connect(verPerfil, &QAction::triggered, this, [this]() {
        qDebug() << "Ver Perfil";
        ui->stackedWidget->setCurrentIndex(5);
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

void HomeWindow_Copy::initPopup() {
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
        nuevaCancion.titulo = tituloEdit->text();
        nuevaCancion.genero = generoCombo->currentText();
        nuevaCancion.categoria = categoriaCombo->currentText();
        nuevaCancion.rutaAudio = rutaEdit->text();

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
