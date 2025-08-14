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

struct Song {
    QString titulo;
    QString genero;
    QString categoria;
    QString rutaAudio;

    // Constructor para facilitar la creación
    Song(const QString& t = "", const QString& g = "",
         const QString& c = "", const QString& r = "")
        : titulo(t), genero(g), categoria(c), rutaAudio(r) {}
};

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


private:
    Ui::ArtistaWindow *ui;
    Cuentas* manejo;
    QMediaPlayer *media;
    QAudioOutput* audioOutput;
    QString currentSongPath;

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

    QString rutaImagen = "";

    struct AlbumTemp {
        QString titulo;
        QString portada;
        QList<Song> canciones;
    };

    AlbumTemp albumActual; // Para guardar temporalmente los datos del álbum

    QList<Cancion*> m_cancionesAlbumActual;
    int m_indiceAlbumActual = -1;
    Album* m_albumActual = nullptr;

private slots:
    void on_toolButton_Playlist_clicked();

    void on_toolButton_home_clicked();

    void on_lineEdit_editingFinished();

    void loadSongs();

    void playSong(const Cancion& cancion);

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_horizontalSlider_Audio_Volume_valueChanged(int value);

    void on_toolButton_play_clicked();

    void on_toolButton_Volume_clicked();

    void onCancionesFileChanged(const QString &path);

    void on_toolButton_limpiar_clicked();

    void cargarAlbumesUsuario();

    void mostrarDetalleAlbum(Album* album);

    void onCancionSeleccionada(Cancion* cancion);

    void reproducirCancion(Cancion* cancion);

    void reproducirAlbumCompleto(Album* album, int indiceInicial);

    void onCancionTerminada();

    void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void handleMediaError(QMediaPlayer::Error error, const QString &errorString);

    void on_toolButton_next_clicked();

    void on_toolButton_prev_clicked();

    void handlePlaybackStateChanged(QMediaPlayer::PlaybackState state);

    void editarCancion(int cancionId);
    void eliminarCancion(int cancionId);



};

#endif // ARTISTAWINDOW_H
