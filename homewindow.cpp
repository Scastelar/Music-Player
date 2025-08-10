#include "homewindow.h"
#include "mainwindow.h"
#include "ui_homewindow.h"
#include "songwidget.h"

#include <QFile>
#include <QMenu>
#include <QFontDatabase>
#include <QMessageBox>

HomeWindow::HomeWindow(QWidget *parent,Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow)
    ,  manejo(&manejo)
{
    ui->setupUi(this);
    ui->verticalFrame->setFixedWidth(200);

    loadSongs();
    usuario = manejo.getIdUsuarioActual();

    //Todo lo del media player
    media = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    media->setAudioOutput(audioOutput);

    audioOutput->setVolume(0.3);  // Set default volume (30%)


    ui->horizontalSlider_Audio_Volume->setMinimum(1);
    ui->horizontalSlider_Audio_Volume->setMaximum(100);
    ui->horizontalSlider_Audio_Volume->setValue(30);

    audioOutput->setVolume(ui->horizontalSlider_Audio_Volume->value() / 100.0);

    connect(media, &QMediaPlayer::durationChanged, this, &HomeWindow::durationChanged);
    connect(media, &QMediaPlayer::positionChanged, this, &HomeWindow::positionChanged);

    ui->horizontalSlider_Audio_File_Duration->setRange(0, media->duration()/1000);

    //Fonts usadas
    int fontId = QFontDatabase::addApplicationFont(":/Montserrat-Regular.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (!families.isEmpty()) {
        Montserrat = families.at(0);
    }

    int iconFontId = QFontDatabase::addApplicationFont(":/MaterialIcons-Regular.ttf");
    MaterialIcons = QFontDatabase::applicationFontFamilies(iconFontId).at(0);

    //Actualizar Archivos en componentes
    fileWatcher = new QFileSystemWatcher(this);
    QString archivoCanciones = "canciones.dat";
    fileWatcher->addPath(archivoCanciones);

    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &HomeWindow::onCancionesFileChanged);

    // UI components
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
    )";
        style.replace("FONT", Montserrat);
        this->setStyleSheet(style);
    } else {
        qDebug() << "Error: Montserrat no está cargado correctamente.";
    }


    ui->label_7->setText("Bienvenido, " + usuario->getNombreUsuario());

    //Menu del perfil
    QMenu *menu = new QMenu(this);

    editarDatos = menu->addAction("Editar datos");
    verEstadisticas = menu->addAction("Estadísticas");
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
    setIcono(ui->toolButton_upload_2,0xe413,20); //e43e
    setIcono(ui->toolButton_addSong,0xe7e5,20);
    ui->toolButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    //Rating promedio
    int rating = 5;
    QString stars = "";
    for (int i = 0; i < rating; i++){
        stars +=   (QChar(0xf0ec));
    }
    ui->label_ratingProm->setText(stars); //Star unicode
    ui->label_ratingProm->setFont(QFont(MaterialIcons, 25));
    ui->label_ratingProm->setStyleSheet("color: yellow; border: none;");
}

HomeWindow::~HomeWindow()
{
    delete ui;
}

void HomeWindow::durationChanged(qint64 duration)
{
    Mduration = duration/1000;
    ui->horizontalSlider_Audio_File_Duration->setMaximum(Mduration);
}

void HomeWindow::positionChanged(qint64 progress)
{
    if(!ui->horizontalSlider_Audio_File_Duration->isSliderDown())
    {
        ui->horizontalSlider_Audio_File_Duration->setValue(progress/1000);
    }
    updateduration(progress/1000);
}

void HomeWindow::loadSongs() {
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
        connect(songWidget, &SongWidget::songClicked, this, &HomeWindow::playSong);

        int row = i / columnCount;
        int col = i % columnCount;
        ui->gridLayout->addWidget(songWidget, row, col, Qt::AlignCenter);
    }
}


void HomeWindow::clearGrid() {
    QLayoutItem *child;
    while ((child = ui->gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void HomeWindow::playSong(const Cancion& cancion) {
    // Implementa la lógica para reproducir la canción
    ui->labelSong->setText(cancion.getTitulo());
    ui->labelArtist->setText(cancion.getArtista());

    // Cargar portada
    QPixmap cover(cancion.getRutaPortada());
    if(cover.isNull()) {
        cover = QPixmap(":/images/default_cover.png");
    }
    ui->labelP->setPixmap(cover.scaled(ui->labelP->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // Aquí iría el código para cargar y reproducir el archivo de audio
    // usando cancion.getRutaAudio()
    media->setSource(QUrl::fromLocalFile(cancion.getRutaAudio()));
    setIcono(ui->toolButton_play,0xe034,20);
    media->play();

}

void HomeWindow::setIcono(QToolButton* boton, ushort unicode, int size){
    boton->setText(QChar(unicode));
    boton->setFont(QFont(MaterialIcons, size)); // Tamaño 20
    boton->setStyleSheet("color: white; border: none;");
}

void HomeWindow::on_toolButton_Playlist_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void HomeWindow::on_toolButton_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}


void HomeWindow::on_lineEdit_editingFinished()
{
    ui->stackedWidget->setCurrentIndex(4);

}

void HomeWindow::conectarMenu(){
    connect(editarDatos, &QAction::triggered, this, [=]() {
        qDebug() << "Editar datos";
        ui->stackedWidget->setCurrentIndex(2);
    });

    connect(verEstadisticas, &QAction::triggered, this, [=]() {
        qDebug() << "Ver estadísticas";
        ui->stackedWidget->setCurrentIndex(3);
    });

    connect(cerrarSesion, &QAction::triggered, this, [=]() {
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



void HomeWindow::updateduration(qint64 duration)
{
    QString timestr;
    if(duration || Mduration)
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

void HomeWindow::on_toolButton_play_clicked()
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

void HomeWindow::on_toolButton_Volume_clicked()
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


void HomeWindow::on_horizontalSlider_Audio_Volume_valueChanged(int value)
{
     audioOutput->setVolume(value / 100.0);
}

//Desactivar Cuenta

void HomeWindow::on_toolButton_upload_5_clicked()
{
    QMessageBox warning;
    warning.setIcon(QMessageBox::Warning);
    warning.setInformativeText("Deseas desactivar tu cuenta permanentemente?");
    warning.setText("Se eliminara toda informacion y musica creada.");
    warning.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    warning.setDefaultButton(QMessageBox::Cancel);
    int opc = warning.exec();

    if (opc == QMessageBox::Cancel) {
        return; // Sale de la función sin hacer nada más
    }
    else if (opc == QMessageBox::Ok) {
        manejo->desactivarCuenta(usuario->getNombreUsuario());
        manejo->cerrarSesion();
        MainWindow* m = new MainWindow(nullptr,*manejo);
        m->show();
        this->close();
    }
}

void HomeWindow::onCancionesFileChanged(const QString &path) {
    Q_UNUSED(path);

    loadSongs();
    if (QFile::exists(path) && !fileWatcher->files().contains(path)) {
        fileWatcher->addPath(path);
    }
}


