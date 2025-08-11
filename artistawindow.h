#ifndef ARTISTAWINDOW_H
#define ARTISTAWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QMediaPlayer>
#include <QAudioOutput>   // For Qt 6
#include <QGridLayout>
#include <QMetaType>
#include <QLineEdit>
#include <QFileSystemWatcher>

#include "cuentas.h"
#include "cancion.h"

namespace Ui {
class ArtistaWindow;
}

struct Song {
    QString titulo;
    QString genero;
    QString categoria;
    QString rutaAudio;
};
Q_DECLARE_METATYPE(Song)

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

private:
    Ui::ArtistaWindow *ui;
    Cuentas* manejo;
    QMediaPlayer *media;
    QAudioOutput* audioOutput;

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

    struct Album {
        QString titulo;
        QString portada;
        QList<Song> canciones;
    };

    Album albumActual; // Para guardar temporalmente los datos del álbum

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
};

#endif // ARTISTAWINDOW_H
