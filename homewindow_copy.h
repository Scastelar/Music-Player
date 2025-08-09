#ifndef HOMEWINDOW_COPY_H
#define HOMEWINDOW_COPY_H

#include <QMainWindow>
#include <QToolButton>
#include <QMediaPlayer>
#include <QAudioOutput>   // For Qt 6
#include <QGridLayout>
#include <QMetaType>
#include "cuentas.h"
#include "cancion.h"

namespace Ui {
class HomeWindow_Copy;
}

struct Song {
    QString titulo;
    QString genero;
    QString categoria;
    QString rutaAudio;
};
Q_DECLARE_METATYPE(Song)

class HomeWindow_Copy : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow_Copy(QWidget *parent, Cuentas& manejo);
    ~HomeWindow_Copy();

    void setIcono(QToolButton* boton, ushort unicode, int size);
    void conectarMenu();
    void initPopup();
    void actualizarListaCanciones();
    void seleccionarPortada();
    void verificarYCrearAlbum();
    void finalizarAlbum();

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

private:
    Ui::HomeWindow_Copy *ui;
    Cuentas* manejo;
    QMediaPlayer *media;
    QAudioOutput* audioOutput;


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

};

#endif // HOMEWINDOW_COPY_H

