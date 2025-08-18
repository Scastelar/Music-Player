#ifndef ARTISTAWINDOW_H
#define ARTISTAWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QMediaPlayer>
#include <QAudioOutput>   // For Qt 6
#include <QGridLayout>
#include <QLineEdit>
#include <QFileSystemWatcher>

#include "cuentas.h"

namespace Ui {
class ArtistaWindow;
}

class ArtistaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArtistaWindow(QWidget *parent,Cuentas& manejo);
    ~ArtistaWindow();

    void setIcono(QToolButton* boton, ushort unicode, int size);
    void setupPasswordLineEdit(QLineEdit *passwordEdit);
    void conectarMenu();
    void initPopup();
    void actualizarListaCanciones();
    void seleccionarPortada();
    void verificarYCrearAlbum();
    void finalizarAlbum();
    void EditarPerfil();
    void VistaPerfil();

signals:
    void randomModeChanged(bool enabled);


private:
    Ui::ArtistaWindow *ui;
    Cuentas* manejo;
    QMediaPlayer *media;
    QAudioOutput* audioOutput;
    QString currentSongPath;
    Cancion currentSong;
    QList<Cancion> canciones;

    const QColor COLOR_ACTIVO = QColor(255, 50, 222);  // Azul activo
    const QColor COLOR_INACTIVO = QColor(255, 255, 255); // Gris inactivo

    bool m_autoPlayPending = false;

    QFileSystemWatcher *fileWatcher;

    void initComponents();

    QString Montserrat = "Montserrat";
    QString MaterialIcons = "Material-Icons";
    Usuario* usuario;
    Administrador* admin;

    void setupGrid();
    void clearGrid();
    QGridLayout *m_gridLayout;

    QAction* editarDatos;
    QAction* verEstadisticas;
    QAction* cerrarSesion;
    QAction* verPerfil;

    bool isMuted = false;
    bool isPaused = false;
    void updateduration(qint64 duration);
    qint64 Mduration;
    QString rutaPortada = "";

    //Completando el player
    bool isRandom = false;
    bool isLoop = false;
    bool isUserSeeking = false;
    qint64 seekPosition = 0;

    QString rutaImagen = "";

    struct AlbumTemp {
        QString titulo;
        QString portada;
        QList<Cancion> canciones;
    };

    AlbumTemp albumActual; // Para guardar temporalmente los datos del álbum

    QList<Cancion*> m_cancionesAlbumActual;
    int m_indiceAlbumActual = -1;
    Album* m_albumActual = nullptr;

private slots:
    void on_toolButton_Playlist_clicked();

    void on_toolButton_home_clicked();

    void on_lineEdit_editingFinished();

    void loadSongs(const QString &genero);

    void loadSongs();

    void playSong(Cancion& cancion);

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_horizontalSlider_Audio_Volume_valueChanged(int value);

    void on_toolButton_play_clicked();

    void on_toolButton_Volume_clicked();

    void onCancionesFileChanged(const QString &path);

    void on_toolButton_limpiar_clicked();

    void cargarAlbumesUsuario(const QString& tipo);

    void mostrarDetalleAlbum(Album* album);

    void reproducirCancion(Cancion* cancion);

    void onCancionTerminada();

    void editarCancion(int cancionId);

    void eliminarCancion(int cancionId);

    void agregarCancionAlAlbum(const Album* album);

    void reproducirAlbumCompleto(const QList<Cancion*>& canciones, int indiceInicial);

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

    void onDurationChanged(qint64 durationMs);

    void on_toolButton_addSong_clicked();

    void on_toolButton_loop_clicked();

    void on_toolButton_random_clicked();

    void updateRandomTooltip();
    void updateLoopTooltip();

    void onRandomModeToggled(bool checked);

    void on_horizontalSlider_Audio_File_Duration_sliderPressed();
    void on_horizontalSlider_Audio_File_Duration_sliderReleased();
    void on_horizontalSlider_Audio_File_Duration_valueChanged(int value);
};

#endif // ARTISTAWINDOW_H
